// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayEffectComponents/AdditionalEffectsGameplayEffectComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "Misc/DataValidation.h"

#define LOCTEXT_NAMESPACE "AdditionalEffectsGameplayEffectComponent"

namespace UE::AbilitySystem::Private
{
	int32 CVarCheckAuthorityWithOnRemoveGEsForSafetyValue = 0;

	// This is really not necessary but there are some edge cases that probably can use
	// this to be a bit safer and still be able to apply the OnRemoveGEs 
	FAutoConsoleVariableRef CVarCheckAuthorityWithOnRemoveGEsForSafety(TEXT("AbilitySystem.AdditionalEffects.CheckAuthorityWithForSafety"), CVarCheckAuthorityWithOnRemoveGEsForSafetyValue,
		TEXT("Should we check if we still have authority in case we got an invalid prediction key for the application of the OnRemove Gameplay Effects. 0 = No. 1 = Yes."));
}

bool UAdditionalEffectsGameplayEffectComponent::OnActiveGameplayEffectAdded(FActiveGameplayEffectsContainer& ActiveGEContainer, FActiveGameplayEffect& ActiveGE) const
{
	// We finally predict OnRemove GEs 
	ActiveGE.EventSet.OnEffectRemoved.AddUObject(this, &UAdditionalEffectsGameplayEffectComponent::OnActiveGameplayEffectRemoved, &ActiveGEContainer);
	return true;
}

void UAdditionalEffectsGameplayEffectComponent::OnGameplayEffectApplied(FActiveGameplayEffectsContainer& ActiveGEContainer, FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const
{
	if (!ensureMsgf(ActiveGEContainer.Owner, TEXT("OnGameplayEffectApplied is passed an ActiveGEContainer which lives within an ASC but that ASC was somehow null")))
	{
		return;
	}

	const float GELevel = GESpec.GetLevel();
	const FGameplayEffectContextHandle& GEContextHandle = GESpec.GetEffectContext();

	/** other effects that need to be applied to the target if this effect is successful */
	TArray<FGameplayEffectSpecHandle> TargetEffectSpecs;
	for (const FConditionalGameplayEffect& ConditionalEffect : OnApplicationGameplayEffects)
	{
		const UGameplayEffect* GameplayEffectDef = ConditionalEffect.EffectClass.GetDefaultObject();
		if (!GameplayEffectDef)
		{
			continue;
		}

		if (ConditionalEffect.CanApply(GESpec.CapturedSourceTags.GetActorTags(), GELevel))
		{
			FGameplayEffectSpecHandle SpecHandle;
			if (bOnApplicationCopyDataFromOriginalSpec)
			{
				SpecHandle = FGameplayEffectSpecHandle(new FGameplayEffectSpec());
				SpecHandle.Data->InitializeFromLinkedSpec(GameplayEffectDef, GESpec);
			}
			else
			{
				SpecHandle = ConditionalEffect.CreateSpec(GEContextHandle, GELevel);
			}

			if (ensure(SpecHandle.IsValid()))
			{
				TargetEffectSpecs.Add(SpecHandle);
			}
		}
	}

	// Add all of the dynamically linked ones that don't have any conditions applied to them
	// Note: I believe this functionality should be removed as its usage is dynamic and thus not 
	// synchronized between client/server.  I've deprecated the variable to help enforce this.
PRAGMA_DISABLE_DEPRECATION_WARNINGS
	TargetEffectSpecs.Append(GESpec.TargetEffectSpecs);
PRAGMA_ENABLE_DEPRECATION_WARNINGS

	UAbilitySystemComponent& AppliedToASC = *ActiveGEContainer.Owner;
	for (const FGameplayEffectSpecHandle& TargetSpec : TargetEffectSpecs)
	{
		if (TargetSpec.IsValid())
		{
			AppliedToASC.ApplyGameplayEffectSpecToSelf(*TargetSpec.Data.Get(), PredictionKey);
		}
	}
}

void UAdditionalEffectsGameplayEffectComponent::OnActiveGameplayEffectRemoved(const FGameplayEffectRemovalInfo& RemovalInfo, FActiveGameplayEffectsContainer* ActiveGEContainer) const
{
	FScopedActiveGameplayEffectLock ActiveScopeLock(*ActiveGEContainer);
	
	const FActiveGameplayEffect* ActiveGE = RemovalInfo.ActiveEffect;
	if (!ensure(ActiveGE))
	{
		UE_LOG(LogGameplayEffects, Error, TEXT("FGameplayEffectRemovalInfo::ActiveEffect was not populated in OnActiveGameplayEffectRemoved"));
		return;
	}

	UAbilitySystemComponent* ASC = ActiveGEContainer->Owner;
	if (!ensure(IsValid(ASC)))
	{
		UE_LOG(LogGameplayEffects, Error, TEXT("ActiveGEContainer was invalid in OnActiveGameplayEffectRemoved"));
		return;
	}

	// This is true on server/client. We must have a synced key in order to apply GEs properly
	if (ASC->CanPredict() == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("We tried to add OnRemove GEs without a valid prediction key"));
		if ((UE::AbilitySystem::Private::CVarCheckAuthorityWithOnRemoveGEsForSafetyValue & 1) == 0)
		{
			return;
		}

		// This does not really make sense but it might be a safer solution
		UE_LOG(LogTemp, Warning, TEXT("We are still going to apply them with a prediction key so we won't CaughtUp/reject any clients key"))
	}

	// Determine the appropriate type of effect to apply depending on whether the effect is being prematurely removed or not
	const TArray<TSubclassOf<UGameplayEffect>>& ExpiryEffects = (RemovalInfo.bPrematureRemoval ? OnCompletePrematurely : OnCompleteNormal);

	// Mix-in the always-executing GameplayEffects
	TArray<TSubclassOf<UGameplayEffect>> AllGameplayEffects{ ExpiryEffects };
	AllGameplayEffects.Append(OnCompleteAlways);


	for (const TSubclassOf<UGameplayEffect>& CurExpiryEffect : AllGameplayEffects)
	{
		if (const UGameplayEffect* CurExpiryCDO = CurExpiryEffect.GetDefaultObject())
		{
			FGameplayEffectSpec NewSpec;
			NewSpec.InitializeFromLinkedSpec(CurExpiryCDO, ActiveGE->Spec);

			// Apply it with the scoped prediction key (sync between server/client)
			FActiveGameplayEffectHandle Handle = 
				ASC->ApplyGameplayEffectSpecToSelf(NewSpec, ASC->ScopedPredictionKey);
		}
	}
}

#if WITH_EDITOR
EDataValidationResult UAdditionalEffectsGameplayEffectComponent::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (GetOwner()->DurationPolicy == EGameplayEffectDurationType::Instant)
	{
		const bool bHasOnCompleteEffects = (OnCompleteAlways.Num() + OnCompleteNormal.Num() + OnCompletePrematurely.Num() > 0);
		if (bHasOnCompleteEffects)
		{
			Context.AddError(FText::FormatOrdered(LOCTEXT("InstantDoesNotWorkWithOnComplete", "Instant GE will never receive OnComplete for {0}."), FText::FromString(GetClass()->GetName())));
			Result = EDataValidationResult::Invalid;
		}
	}
	else if (GetOwner()->Period.Value > 0.0f)
	{
		if (OnApplicationGameplayEffects.Num() > 0)
		{
			Context.AddWarning(LOCTEXT("IsPeriodicAndHasOnApplication", "Periodic GE has OnApplicationGameplayEffects. Those GE's will only be applied once."));
		}
	}

	return Result;
}
#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE
