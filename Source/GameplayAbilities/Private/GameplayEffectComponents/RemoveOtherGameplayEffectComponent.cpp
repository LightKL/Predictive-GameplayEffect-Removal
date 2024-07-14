// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayEffectComponents/RemoveOtherGameplayEffectComponent.h"
#include "AbilitySystemComponent.h"
#include "Misc/DataValidation.h"

#define LOCTEXT_NAMESPACE "RemoveOtherGameplayEffectComponent"

namespace UE::AbilitySystem::Private
{
	// Wether we should also listen for the OnCaughtUp event when predictively removing 
	// other GEs ? This should not be necessary in classic gameplay 

	int32 CVarListenForOnCaughtUpTooValue = 0;
	FAutoConsoleVariableRef CVarListenForOnCaughtUpToo(TEXT("AbilitySystem.RemoveOther.ListenForOnCaughtUpToo"), CVarListenForOnCaughtUpTooValue,
		TEXT("Should we listen for the OnCaughtUp event when we predict removal ? 0 = No. 1 = Yes"));
}

URemoveOtherGameplayEffectComponent::URemoveOtherGameplayEffectComponent()
{
#if WITH_EDITORONLY_DATA
	EditorFriendlyName = TEXT("Remove Other Gameplay Effects");
#endif
}

void URemoveOtherGameplayEffectComponent::OnGameplayEffectApplied(FActiveGameplayEffectsContainer& ActiveGEContainer, FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const
{
	UAbilitySystemComponent* Owner = ActiveGEContainer.Owner;
	if (!ensure(Owner))
	{
		return;
	}

	FGameplayEffectQuery FindOwnerQuery;
	FindOwnerQuery.EffectDefinition = GetOwner() ? GetOwner()->GetClass() : nullptr;

	// We need to keep track to ensure we never remove ourselves
	TArray<FActiveGameplayEffectHandle> ActiveGEHandles = ActiveGEContainer.GetActiveEffects(FindOwnerQuery);
	TArray<FActiveGEHandleWithPredictionKey> ActiveGEsToEnableIfMispredict;

	const bool bCanPredict = Owner->CanPredict();
	const bool bIsNetAuthority = ActiveGEContainer.IsNetAuthority();
	constexpr int32 RemoveAllStacks = -1;

	for (const FGameplayEffectQuery& RemoveQuery : RemoveGameplayEffectQueries)
	{
		if (!RemoveQuery.IsEmpty())
		{
			// If we have an ActiveGEHandle, make sure we never remove ourselves.
			// If we don't, there's no need to make a copy.
			if (ActiveGEHandles.IsEmpty())
			{
				// Faster path: No copy needed
				if (bIsNetAuthority)
				{
					ActiveGEContainer.RemoveActiveEffects(RemoveQuery, RemoveAllStacks);
				}
				else if (bCanPredict)
				{
					// Gather effects needed to be removed and disable them
					TArray<FActiveGameplayEffectHandle> RemoveHandles = ActiveGEContainer.GetActiveEffects(RemoveQuery);
					for (FActiveGameplayEffectHandle Handle : RemoveHandles)
					{
						FActiveGameplayEffect* ActiveEffect = ActiveGEContainer.GetActiveGameplayEffect(Handle);
						if (ActiveEffect)
						{
							// For now, ignore disabled gameplay effects (predictively removed)
							if (ActiveEffect->IsPredictivelyRemoved())
							{
								UE_LOG(LogTemp, Warning, TEXT("We tried to predictively remove a GE that was already predictively removed in %hs. GE : %s"), __func__, *ActiveEffect->GetDebugString());
								continue;
							}

							// Cache a handle with its prediction key so it can be used later on
							// to remove a replicated GE which has its handle different than
							// the predictive one.
							FActiveGEHandleWithPredictionKey NewEntry(Handle, ActiveEffect->PredictionKey);
							ActiveGEsToEnableIfMispredict.Add(NewEntry);
							Owner->DisableGameplayEffect(*ActiveEffect);
						}
					}
				}
			}
			else
			{
				FGameplayEffectQuery MutableRemoveQuery = RemoveQuery;
				MutableRemoveQuery.IgnoreHandles = MoveTemp(ActiveGEHandles);

				if (bIsNetAuthority)
				{
					ActiveGEContainer.RemoveActiveEffects(MutableRemoveQuery, RemoveAllStacks);
				}
				else if (bCanPredict)
				{
					TArray<FActiveGameplayEffectHandle> RemoveHandles = ActiveGEContainer.GetActiveEffects(MutableRemoveQuery);
					for (FActiveGameplayEffectHandle Handle : RemoveHandles)
					{
						FActiveGameplayEffect* ActiveEffect = ActiveGEContainer.GetActiveGameplayEffect(Handle);
						if (ActiveEffect)
						{
							if (ActiveEffect->IsPredictivelyRemoved())
							{
								UE_LOG(LogTemp, Warning, TEXT("We tried to predictively remove a GE that was already predictively removed in %hs. GE : %s"), __func__, *ActiveEffect->GetDebugString());
								continue;
							}

							FActiveGEHandleWithPredictionKey NewEntry(Handle, ActiveEffect->PredictionKey);
							ActiveGEsToEnableIfMispredict.Add(NewEntry);
							Owner->DisableGameplayEffect(*ActiveEffect);
						}
					}
				}
			}
			if (bIsNetAuthority == false && bCanPredict)
			{
				if (ActiveGEsToEnableIfMispredict.IsEmpty() == false)
				{
					if ((UE::AbilitySystem::Private::CVarListenForOnCaughtUpTooValue & 1) == 0)
					{
						// For now, just use the rejected delegate as if we were CaughtUp,
						// it means that we are going to remove this GE server-side or 
						// server does not have this GE to remove so it'll be removed 
						// on client anyways too. This should work as expected
						PredictionKey.NewRejectedDelegate().BindUObject(this, &ThisClass::OnGameplayEffectRemovalFailed, Owner, MoveTemp(ActiveGEsToEnableIfMispredict));
					}
					else
					{
						// This can be the long-term value as it is safer to use
						FPredictionKeyEvent Event = FPredictionKeyEvent::CreateUObject(this, &ThisClass::OnGameplayEffectRemovalFailed, Owner, MoveTemp(ActiveGEsToEnableIfMispredict));
						PredictionKey.NewRejectOrCaughtUpDelegate(Event);
					}
				}
			}

			// What if we are predictively removed while predictively removing another GE ?
			// This is not supported yet. We'd need to find the active effect being applied
			// (maybe with its prediction key) and bind its OnEffectRemoved to re-enable 
			// the "ActiveGEsToEnableIfMispredict" handles
		}
	}
}

void URemoveOtherGameplayEffectComponent::OnGameplayEffectRemovalFailed(UAbilitySystemComponent* OwnerASC, TArray<FActiveGEHandleWithPredictionKey> RemoveThoseGEs) const
{
	// Application got rejected -> we need to enable locally removed GEs
	if (OwnerASC == nullptr || RemoveThoseGEs.IsEmpty())
	{
		return;
	}

	for (const FActiveGEHandleWithPredictionKey& ActiveGEHandle : RemoveThoseGEs)
	{
		// Try to find a matching handle to enable in case of mispredict
		FActiveGameplayEffect* ActiveEffect = const_cast<FActiveGameplayEffect*>(OwnerASC->GetActiveGameplayEffect(ActiveGEHandle.Handle));
		if (ActiveEffect)
		{
			OwnerASC->EnableGameplayEffect(*ActiveEffect);
		}
		else if (ActiveGEHandle.PredictionKey.IsValidKey())
		{
			// If we did not find this ActiveGE tied to that Handle. It means both GEs
			// are applied locally. So the predicted applied GE has been removed and got
			// re-applied via replication.
			ActiveEffect = OwnerASC->GetActiveEffectWithPredictionKey(ActiveGEHandle.PredictionKey);
			if (ActiveEffect)
			{
				OwnerASC->EnableGameplayEffect(*ActiveEffect);
			}
		}
	}
}

#if WITH_EDITOR
EDataValidationResult URemoveOtherGameplayEffectComponent::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (GetOwner()->DurationPolicy != EGameplayEffectDurationType::Instant)
	{
		if (GetOwner()->Period.Value > 0.0f)
		{
			Context.AddError(FText::FormatOrdered(LOCTEXT("PeriodicEffectError", "GE is Periodic. Remove {0} and use TagRequirements (Ongoing) instead."), FText::FromString(GetClass()->GetName())));
			Result = EDataValidationResult::Invalid;
		}
	}

	return Result;
}
#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE
