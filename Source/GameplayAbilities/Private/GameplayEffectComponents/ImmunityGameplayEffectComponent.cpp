// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayEffectComponents/ImmunityGameplayEffectComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "AbilitySystemStats.h"
#include "Misc/DataValidation.h"

#define LOCTEXT_NAMESPACE "ImmunityGameplayEffectComponent"

UImmunityGameplayEffectComponent::UImmunityGameplayEffectComponent()
{
#if WITH_EDITORONLY_DATA
	EditorFriendlyName = TEXT("Immunity (Prevent Other GEs)");
#endif // WITH_EDITORONLY_DATA
}

bool UImmunityGameplayEffectComponent::OnActiveGameplayEffectAdded(FActiveGameplayEffectsContainer& ActiveGEContainer, FActiveGameplayEffect& ActiveGE) const
{
	UE_LOG(LogTemp, Warning, TEXT("ActiveGE is the predictive or replicated version ? %d (1 if predictive)."), ActiveGE.PredictionKey.WasLocallyGenerated());
	RegisterGameplayEffectImmunity(ActiveGEContainer.Owner, ActiveGE);
	return true;
}

void UImmunityGameplayEffectComponent::RegisterGameplayEffectImmunity(UAbilitySystemComponent* OwnerASC, FActiveGameplayEffect& ActiveGE) const
{
	// Register our immunity query to potentially block applications of any Gameplay Effects
	FGameplayEffectApplicationQuery& BoundQuery = OwnerASC->GameplayEffectApplicationQueries.AddDefaulted_GetRef();
	BoundQuery.BindUObject(this, &UImmunityGameplayEffectComponent::AllowGameplayEffectApplication, ActiveGE.Handle);

	// Now that we've bound that function, let's unbind it when we're removed.  
	// This is safe because once we're removed, EventSet is gone.
	// lambda or regular delegate
	ActiveGE.EventSet.OnEffectRemoved.AddUObject(this, &ThisClass::OnGameplayEffectRemoved, OwnerASC, BoundQuery.GetHandle());
}

void UImmunityGameplayEffectComponent::OnGameplayEffectRemovalFailed(UAbilitySystemComponent* OwnerASC, FActiveGameplayEffect* ActiveGE) const
{
	if (ActiveGE)
	{
		UE_LOG(LogTemp, Warning, TEXT("We got enabled back after a mispredict of removal"));
		RegisterGameplayEffectImmunity(OwnerASC, *ActiveGE);
	}
	else
	{
		check(false);
	}
}

void UImmunityGameplayEffectComponent::OnGameplayEffectRemoved(const FGameplayEffectRemovalInfo& RemovalInfo, UAbilitySystemComponent* OwnerASC, FDelegateHandle QueryToRemove) const
{
	// Remove the immunity query
	if (ensure(IsValid(OwnerASC)))
	{
		TArray<FGameplayEffectApplicationQuery>& GEAppQueries = OwnerASC->GameplayEffectApplicationQueries;
		for (auto It = GEAppQueries.CreateIterator(); It; ++It)
		{
			if (It->GetHandle() == QueryToRemove)
			{
				It.RemoveCurrentSwap();
				break;
			}
		}
	}

	FActiveGameplayEffect* ActiveGE = const_cast<FActiveGameplayEffect*>(RemovalInfo.ActiveEffect);
	if (ActiveGE)
	{
		const bool bCanPredict = OwnerASC->CanPredict();
		const bool bAuthority = OwnerASC->IsOwnerActorAuthoritative();
		UE_LOG(LogTemp, Warning, TEXT("GE removed (Immunity one). This is the state : Can Predict [%d] and Authority [%d] and Scoped prediction key value : %s"), bCanPredict, bAuthority, *OwnerASC->ScopedPredictionKey.ToString());
		
		if (!bAuthority && bCanPredict)
		{
			// Bind to rejected delegate if we mispredicted removal
			FPredictionKey RemovalKey = ActiveGE->PredictiveRemovalKey;
			RemovalKey.NewRejectedDelegate().BindUObject(this, &UImmunityGameplayEffectComponent::OnGameplayEffectRemovalFailed, OwnerASC, ActiveGE);
			UE_LOG(LogTemp, Warning, TEXT("Removal key : %s"), *RemovalKey.ToString());
		}
	}
}

bool UImmunityGameplayEffectComponent::AllowGameplayEffectApplication(const FActiveGameplayEffectsContainer& ActiveGEContainer, const FGameplayEffectSpec& GESpecToConsider, FActiveGameplayEffectHandle ImmunityActiveGEHandle) const
{
	SCOPE_CYCLE_COUNTER(STAT_HasApplicationImmunityToSpec)

	const UAbilitySystemComponent* ASC = ActiveGEContainer.Owner;
	if (ASC != ImmunityActiveGEHandle.GetOwningAbilitySystemComponent())
	{
		ensureMsgf(false, TEXT("Something went wrong where an ActiveGameplayEffect jumped AbilitySystemComponents"));
		return false;
	}

	// ActiveGE can be null (IsPendingRemove) but we could have not yet received OnEffectRemoved. This can happen when applying more effects during its removal.
	const FActiveGameplayEffect* ActiveGE = ASC->GetActiveGameplayEffect(ImmunityActiveGEHandle);
	if (!ActiveGE || ActiveGE->bIsInhibited)
	{
		return true;
	}

	for (const FGameplayEffectQuery& ImmunityQuery : ImmunityQueries)
	{
		if (!ImmunityQuery.IsEmpty() && ImmunityQuery.Matches(GESpecToConsider))
		{
			ASC->OnImmunityBlockGameplayEffectDelegate.Broadcast(GESpecToConsider, ActiveGE);
			return false;
		}
	}

	return true;
}

#if WITH_EDITOR
EDataValidationResult UImmunityGameplayEffectComponent::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (GetOwner()->DurationPolicy == EGameplayEffectDurationType::Instant)
	{
		Context.AddError(FText::FormatOrdered(LOCTEXT("IsInstantEffectError", "GE is an Instant Effect and incompatible with {0}"), FText::FromString(GetClass()->GetName())));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE
