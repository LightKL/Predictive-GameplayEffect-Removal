// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayEffectComponents/TargetTagRequirementsGameplayEffectComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "Algo/Find.h"
#include "Misc/DataValidation.h"

#define LOCTEXT_NAMESPACE "TargetTagRequirementsGameplayEffectComponent"

namespace UE::AbilitySystem::Private
{
	int32 CVarDisableGEOnRemovalReplicationValue = 0;
	FAutoConsoleVariableRef CVarDisableGEOnRemovalReplication(TEXT("AbilitySystem.TargetTagsRequirement.DisableGEOnRemovalReplication"), CVarDisableGEOnRemovalReplicationValue,
		TEXT("How do we handle replication of the GE removing us ? 0 = Ignore. 1) Disable Gameplay Effect"));
}

#define DEPRECATE_PREDICTIVE_FLAG_AND_USE_PREDICTION_KEY 1

namespace GameplayEffectComponent::TargetTagRequirements
{
	// Quick method of appending a TArray to another TArray with no duplicates.
	auto AppendUnique = [](TArray<FGameplayTag>& Destination, const TArray<FGameplayTag>& Source)
		{
			// Make sure the array won't allocate during the loop
			if (Destination.GetSlack() < Source.Num())
			{
				Destination.Reserve(Destination.Num() + Source.Num());
			}
			const TConstArrayView<FGameplayTag> PreModifiedDestinationView{ Destination.GetData(), Destination.Num() };

			for (const FGameplayTag& Tag : Source)
			{
				if (!Algo::Find(PreModifiedDestinationView, Tag))
				{
					Destination.Emplace(Tag);
				}
			}
		};
}

UTargetTagRequirementsGameplayEffectComponent::UTargetTagRequirementsGameplayEffectComponent()
{
#if WITH_EDITORONLY_DATA
	EditorFriendlyName = TEXT("Target Tag Reqs (While GE is Active)");
#endif // WITH_EDITORONLY_DATA
}

// Maybe we'd need a PredictionKey with it to avoid the trick
bool UTargetTagRequirementsGameplayEffectComponent::CanGameplayEffectApply(const FActiveGameplayEffectsContainer& ActiveGEContainer, const FGameplayEffectSpec& GESpec, const FPredictionKey& PredictionKey) const
{
	FGameplayTagContainer Tags;
	ActiveGEContainer.Owner->GetOwnedGameplayTags(Tags);
	
	if (ApplicationTagRequirements.RequirementsMet(Tags) == false)
	{
		return false;
	}

	// Continue execution as we are a replicated version of the server GE. This means we
	// must be applied and local removal tags should be ignored at this stage. They will
	// be re-checked later on (in OnActiveGameplayEffectAdded)
	if (ActiveGEContainer.IsNetAuthority() == false && PredictionKey.IsValidKey() && PredictionKey.WasReceived())
	{
		return true;
	}

	if (!RemovalTagRequirements.IsEmpty() && RemovalTagRequirements.RequirementsMet(Tags) == true)
	{
		return false;
	}

	return true;
}

// UTargetTagRequirementsGameplayEffectComponent lives on an asset.  This doesn't get instanced at runtime, so this is NOT A UNIQUE INSTANCE (it is a shared instance for any GEContainer/ActiveGE that wants to use it).
bool UTargetTagRequirementsGameplayEffectComponent::OnActiveGameplayEffectAdded(FActiveGameplayEffectsContainer& GEContainer, FActiveGameplayEffect& ActiveGE) const
{
	UAbilitySystemComponent* ASC = GEContainer.Owner;
	if (!ensure(ASC))
	{
		return false;
	}

	FGameplayTagContainer OwnedTags;
	ASC->GetOwnedGameplayTags(OwnedTags);

	if (ActiveGE.IsPredictivelyRemoved())
	{
		// Why ? Because if GE_B is rejected -> We are OFF and we can't get back ON
		// New application, we still need to bind to Inhibition delegates
		const bool bRemovalRequirementsMet = !RemovalTagRequirements.IsEmpty() && RemovalTagRequirements.RequirementsMet(OwnedTags);
		if (bRemovalRequirementsMet)
		{
			using namespace GameplayEffectComponent::TargetTagRequirements;

			// We should gather a list of tags to listen on events for
			TArray<FGameplayTag> GameplayTagsToBind;
			AppendUnique(GameplayTagsToBind, RemovalTagRequirements.IgnoreTags.GetGameplayTagArray());
			AppendUnique(GameplayTagsToBind, RemovalTagRequirements.RequireTags.GetGameplayTagArray());
			AppendUnique(GameplayTagsToBind, RemovalTagRequirements.TagQuery.GetGameplayTagArray());

			// Add our tag requirements to the ASC's Callbacks map. This helps filter down the amount of callbacks we'll get due to tag changes
			// (rather than registering for the one callback whenever any tag changes).  We also need to keep track to remove those registered delegates in OnEffectRemoved.
			TArray<TTuple<FGameplayTag, FDelegateHandle>> AllBoundEvents;
			for (const FGameplayTag& Tag : GameplayTagsToBind)
			{
				FOnGameplayEffectTagCountChanged& OnTagEvent = ASC->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved);
				FDelegateHandle Handle = OnTagEvent.AddUObject(this, &UTargetTagRequirementsGameplayEffectComponent::OnGameplayEffectSomething, ASC, ActiveGE.Handle);
				AllBoundEvents.Emplace(Tag, Handle);
			}

			// Clear the array off the RemovalTags and 
			// just listen for the OngoingTags events
			GameplayTagsToBind.Reset();
			AppendUnique(GameplayTagsToBind, OngoingTagRequirements.IgnoreTags.GetGameplayTagArray());
			AppendUnique(GameplayTagsToBind, OngoingTagRequirements.RequireTags.GetGameplayTagArray());
			AppendUnique(GameplayTagsToBind, OngoingTagRequirements.TagQuery.GetGameplayTagArray());
			
			// New functionality. Just make a new callback for the inhibition 
			// for faster and better lisibility/readability of code
			for (const FGameplayTag& Tag : GameplayTagsToBind)
			{
				FOnGameplayEffectTagCountChanged& OnTagEvent = ASC->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved);
				FDelegateHandle Handle = OnTagEvent.AddUObject
				(this, &UTargetTagRequirementsGameplayEffectComponent::OnGameplayEffectInhibited, ActiveGE.Handle);
				AllBoundEvents.Emplace(Tag, Handle);
			}

			// Now when this Effect is removed, we should remove all of our registered callbacks.
			ActiveGE.EventSet.OnEffectRemoved.AddUObject(this, &UTargetTagRequirementsGameplayEffectComponent::OnGameplayEffectRemoved, ASC, MoveTemp(AllBoundEvents));
		}
		else
		{
			ASC->EnableGameplayEffect(ActiveGE);
		}

		return OngoingTagRequirements.RequirementsMet(OwnedTags);
	}

	RegisterGameplayEffectRequirements(ASC, ActiveGE.Handle);

	return OngoingTagRequirements.RequirementsMet(OwnedTags);
}

void UTargetTagRequirementsGameplayEffectComponent::RegisterGameplayEffectRequirements(UAbilitySystemComponent* ASC, FActiveGameplayEffectHandle ActiveGEHandle) const
{
	if (FActiveGameplayEffectEvents* EventSet = ASC->GetActiveEffectEventSet(ActiveGEHandle))
	{
		using namespace GameplayEffectComponent::TargetTagRequirements;

		// We should gather a list of tags to listen on events for
		TArray<FGameplayTag> GameplayTagsToBind;
		AppendUnique(GameplayTagsToBind, OngoingTagRequirements.IgnoreTags.GetGameplayTagArray());
		AppendUnique(GameplayTagsToBind, OngoingTagRequirements.RequireTags.GetGameplayTagArray());
		AppendUnique(GameplayTagsToBind, OngoingTagRequirements.TagQuery.GetGameplayTagArray());
		AppendUnique(GameplayTagsToBind, RemovalTagRequirements.IgnoreTags.GetGameplayTagArray());
		AppendUnique(GameplayTagsToBind, RemovalTagRequirements.RequireTags.GetGameplayTagArray());
		AppendUnique(GameplayTagsToBind, RemovalTagRequirements.TagQuery.GetGameplayTagArray());

		// Add our tag requirements to the ASC's Callbacks map. This helps filter down the amount of callbacks we'll get due to tag changes
		// (rather than registering for the one callback whenever any tag changes).  We also need to keep track to remove those registered delegates in OnEffectRemoved.
		TArray<TTuple<FGameplayTag, FDelegateHandle>> AllBoundEvents;
		for (const FGameplayTag& Tag : GameplayTagsToBind)
		{
			FOnGameplayEffectTagCountChanged& OnTagEvent = ASC->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved);
			FDelegateHandle Handle = OnTagEvent.AddUObject(this, &UTargetTagRequirementsGameplayEffectComponent::OnTagChanged, ActiveGEHandle);
			AllBoundEvents.Emplace(Tag, Handle);
		}

		// Now when this Effect is removed, we should remove all of our registered callbacks.
		EventSet->OnEffectRemoved.AddUObject(this, &UTargetTagRequirementsGameplayEffectComponent::OnGameplayEffectRemoved, ASC, MoveTemp(AllBoundEvents));
	}
	else
	{
		UE_LOG(LogGameplayEffects, Error, TEXT("UTargetTagRequirementsGameplayEffectComponent::OnGameplayEffectAdded called with an invalid FActiveGameplayEffectHandle [%s]."), *ActiveGEHandle.ToString());
	}
}

void UTargetTagRequirementsGameplayEffectComponent::OnGameplayEffectNotHere(const FGameplayEffectRemovalInfo& GERemovalInfo, UAbilitySystemComponent* ASC, TTuple<FGameplayTag, FDelegateHandle> BoundEvent) const
{
	if (ensure(BoundEvent.Key.IsValid() && BoundEvent.Value.IsValid() && ASC))
	{
		ASC->UnregisterGameplayTagEvent(BoundEvent.Value, BoundEvent.Key, EGameplayTagEventType::NewOrRemoved);
	}
}

void UTargetTagRequirementsGameplayEffectComponent::OnGameplayEffectRemovalFailed(FActiveGameplayEffectHandle ActiveGEHandle) const
{
	UAbilitySystemComponent* OwnerASC = ActiveGEHandle.GetOwningAbilitySystemComponent();
	if (OwnerASC)
	{
		// Register tag events back when removal fails. 
		// Gameplay effect applying it has been rejected.
		const FActiveGameplayEffect* ActiveGE = OwnerASC->GetActiveGameplayEffect(ActiveGEHandle);
		if (ActiveGE)
		{
			bool bBound = ActiveGE->EventSet.OnEffectRemoved.IsBoundToObject(this);
			UE_LOG(LogTemp, Warning, TEXT("Is the event set bound when removes fails : [%d] and the removal key is : [%s] (should be 0/0)"), bBound, *ActiveGE->PredictiveRemovalKey.ToString());
		}

		RegisterGameplayEffectRequirements(OwnerASC, ActiveGEHandle);
	}
}

void UTargetTagRequirementsGameplayEffectComponent::OnGameplayEffectInhibited(const FGameplayTag GameplayTag, int32 NewCount, FActiveGameplayEffectHandle ActiveGEHandle) const
{
	UAbilitySystemComponent* Owner = ActiveGEHandle.GetOwningAbilitySystemComponent();
	if (!Owner)
	{
		return;
	}

	FGameplayTagContainer OwnedTags;
	Owner->GetOwnedGameplayTags(OwnedTags);

	constexpr bool bInvokeCuesIfStateChanged = true;
	const bool bOngoingRequirementsMet = OngoingTagRequirements.IsEmpty() || OngoingTagRequirements.RequirementsMet(OwnedTags);
	Owner->SetActiveGameplayEffectInhibit(MoveTemp(ActiveGEHandle), !bOngoingRequirementsMet, bInvokeCuesIfStateChanged);

}

void UTargetTagRequirementsGameplayEffectComponent::OnGameplayEffectRemoved(const FGameplayEffectRemovalInfo& GERemovalInfo, UAbilitySystemComponent* ASC, TArray<TTuple<FGameplayTag, FDelegateHandle>> AllBoundEvents) const
{
	for (TTuple<FGameplayTag, FDelegateHandle>& Pair : AllBoundEvents)
	{
		const bool bSuccess = ASC->UnregisterGameplayTagEvent(Pair.Value, Pair.Key, EGameplayTagEventType::NewOrRemoved);
		UE_CLOG(!bSuccess, LogGameplayEffects, Error, TEXT("%s tried to unregister GameplayTagEvent '%s' on GameplayEffect '%s' but failed."), *GetName(), *Pair.Key.ToString(), *GetNameSafe(GetOwner()));
	}

	// This OnEffectRemoved can be caused by GE_B removing us locally.
	// we need to have a way, when getting enabled, in case GE_B is rejected
	// to re-listen for tag events locally.
	if (GERemovalInfo.ActiveEffect)
	{
		FPredictionKey Key = GERemovalInfo.ActiveEffect->PredictiveRemovalKey;
		if (Key.IsLocalClientKey())
		{
			Key.NewRejectedDelegate().BindUObject(this, &ThisClass::OnGameplayEffectRemovalFailed, GERemovalInfo.ActiveEffect->Handle);
		}
	}
}

void UTargetTagRequirementsGameplayEffectComponent::OnGameplayEffectSomething(const FGameplayTag GameplayTag, int32 NewCount, UAbilitySystemComponent* ASC, FActiveGameplayEffectHandle ActiveGEHandle) const
{
	if (NewCount == 0)
	{
		// GE_B is now removed. 
		// If predictive GE_B is removed, it would trigger tag delegates; 
		// If Rejected -> This is great because we get back ON and all good.
		// If CaughtUp -> Application will re-trigger this with non-null NewCount.
		if (ensure(ASC))
		{
			FActiveGameplayEffect* ActiveEffect = const_cast<FActiveGameplayEffect*>(ASC->GetActiveGameplayEffect(ActiveGEHandle));
			if (ActiveEffect)
			{
				// GE_B is removed. It will either be rejected, nothing happens, we 
				// successfully predicted removal
				// if GE_B is confirmed, we will get removed anyways. We could have a small
				// bump in attributes like Application has. 

				ASC->EnableGameplayEffect(*ActiveEffect);
				RegisterGameplayEffectRequirements(ASC, ActiveGEHandle);
			}
		}
	}
	else
	{
		// GE_B is re-applied due to replication. This means GE_B has been CaugthUp
		// And not rejected. This GE will be removed any soon.
		if (ensure(ASC))
		{
			FActiveGameplayEffect* ActiveEffect = const_cast<FActiveGameplayEffect*>(ASC->GetActiveGameplayEffect(ActiveGEHandle));
			if (ActiveEffect)
			{
				// We can disable it but it might unnecessary 
				// as the GE will be removed very soon
				if ((UE::AbilitySystem::Private::CVarDisableGEOnRemovalReplicationValue & 1) != 0)
				{
					ASC->DisableGameplayEffect(*ActiveEffect);
				}
			}
		}
	}
}

void UTargetTagRequirementsGameplayEffectComponent::OnTagChanged(const FGameplayTag GameplayTag, int32 NewCount, FActiveGameplayEffectHandle ActiveGEHandle) const
{
	// Note: This function can remove us (RemoveActiveGameplayEffect eventually calling OnGameplayEffectRemoved),
	// but we could be in the middle of a stack of OnTagChanged callbacks, wo we could get a stale OnTagChanged.
	UAbilitySystemComponent* Owner = ActiveGEHandle.GetOwningAbilitySystemComponent();
	if (!Owner)
	{
		return;
	}

	// It's possible for this to return nullptr if it was in the process of being removed (IsPendingRemove)
	const FActiveGameplayEffect* ActiveGE = Owner->GetActiveGameplayEffect(ActiveGEHandle);
	if (ActiveGE)
	{
		FGameplayTagContainer OwnedTags;
		Owner->GetOwnedGameplayTags(OwnedTags);

		const bool bRemovalRequirementsMet = !RemovalTagRequirements.IsEmpty() && RemovalTagRequirements.RequirementsMet(OwnedTags);
		if (bRemovalRequirementsMet)
		{
			// This is slightly different functionality from pre-UE5.3, we're calling RemoveActiveGameplayEffect rather than InternalRemoveActiveGameplayEffect.
			// The result is we set the calculated magnitudes back to zero.  This also used to only run on the Server.
			//Owner->RemoveActiveGameplayEffect(ActiveGEHandle);
			if (Owner->IsOwnerActorAuthoritative())
			{
				Owner->RemoveActiveGameplayEffect(ActiveGEHandle);
			}
			else if (Owner->CanPredict())
			{
				checkf(ActiveGE->PredictiveRemovalKey.IsValidKey() == false, TEXT("We should not be removed predictively if we did unregister our tag changes callbacks when initially predictively removed"));

				// This calls OnRemove. Thus unregistering delegates and waiting
				// for a EnableGE() call
				FActiveGameplayEffect& ActiveGERef = const_cast<FActiveGameplayEffect&>(*ActiveGE);
				Owner->DisableGameplayEffect(ActiveGERef);

				// Unbind OnEffectRemoved to avoid Unregistering stale delegates
				// Is it really needed ? Needs more debugging
				ActiveGERef.EventSet.OnEffectRemoved.RemoveAll(this);

				// Register GE_B tag events to handle different use cases
				FDelegateHandle DelegateHandle = Owner->RegisterGameplayTagEvent(GameplayTag).AddUObject(this, &ThisClass::OnGameplayEffectSomething, Owner, ActiveGEHandle);
				TTuple<FGameplayTag, FDelegateHandle> BoundEvent = { GameplayTag, DelegateHandle };

				// Unregister those when the effect is removed
				ActiveGERef.EventSet.OnEffectRemoved.AddUObject(this, &ThisClass::OnGameplayEffectNotHere, Owner, MoveTemp(BoundEvent));
			}
		}
		else
		{
			// See if we should be inhibiting the execution
			constexpr bool bInvokeCuesIfStateChanged = true;
			const bool bOngoingRequirementsMet = OngoingTagRequirements.IsEmpty() || OngoingTagRequirements.RequirementsMet(OwnedTags);
			Owner->SetActiveGameplayEffectInhibit(MoveTemp(ActiveGEHandle), !bOngoingRequirementsMet, bInvokeCuesIfStateChanged);
		}
	}
}

#if WITH_EDITOR
/**
 * Validate incompatible configurations
 */
EDataValidationResult UTargetTagRequirementsGameplayEffectComponent::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	const bool bInstantEffect = (GetOwner()->DurationPolicy == EGameplayEffectDurationType::Instant);
	if (bInstantEffect && !OngoingTagRequirements.IsEmpty())
	{
		Context.AddError(LOCTEXT("GEInstantAndOngoing", "GE is instant but has OngoingTagRequirements."));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}
#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE
