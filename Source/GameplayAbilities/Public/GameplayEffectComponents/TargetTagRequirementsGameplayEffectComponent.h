// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayEffect.h"
#include "GameplayEffectComponent.h"
#include "TargetTagRequirementsGameplayEffectComponent.generated.h"

/*
 * Current issues with current GE predictive removal system :
 *
 * 1) External MMC dependencies
 * We need a way to disable these calls when disabled (predictively removed) and
 * add them in a pending list that would be called if mispredicted.
 * They are ignored for the moment as it would some kind of data structure
 *
 */

struct FGameplayEffectRemovalInfo;

/** Specifies tag requirements that the Target (owner of the Gameplay Effect) must have if this GE should apply or continue to execute */
UCLASS(DisplayName="Require Tags to Apply/Continue This Effect")
class GAMEPLAYABILITIES_API UTargetTagRequirementsGameplayEffectComponent : public UGameplayEffectComponent
{
	GENERATED_BODY()

public:
	/** Constructor to set EditorFriendlyName */
	UTargetTagRequirementsGameplayEffectComponent();

	/** Can we apply to the ActiveGEContainer? */
	virtual bool CanGameplayEffectApply(const FActiveGameplayEffectsContainer& ActiveGEContainer, const FGameplayEffectSpec& GESpec, const FPredictionKey& PredictionKey) const override;

	/** Once we've applied, we need to register for ongoing requirements */
	virtual bool OnActiveGameplayEffectAdded(FActiveGameplayEffectsContainer& GEContainer, FActiveGameplayEffect& ActiveGE) const override;

#if WITH_EDITOR
	/**
	 * Validate incompatible configurations
	 */
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif // WITH_EDITOR

private:

	/** We need to be notified when we're removed to unregister some callbacks */
	void OnGameplayEffectRemoved(const FGameplayEffectRemovalInfo& GERemovalInfo, UAbilitySystemComponent* ASC, TArray<TTuple<FGameplayTag, FDelegateHandle>> AllBoundEvents) const;

	/** We need to register a callback for when the owner changes its tags.  When that happens, we need to figure out if our GE should continue to execute */
	void OnTagChanged(const FGameplayTag GameplayTag, int32 NewCount, FActiveGameplayEffectHandle ActiveGEHandle) const;

	/** We need to register a callback for when the removal tags change locally to adjust local state */
	void OnRemovalGameplayTagChange(const FGameplayTag GameplayTag, int32 NewCount, UAbilitySystemComponent* ASC, FActiveGameplayEffectHandle ActiveGEHandle) const;

	/** Wrapper function to register the tag requirements */
	void RegisterGameplayEffectRequirements(UAbilitySystemComponent* ASC, FActiveGameplayEffectHandle ActiveGEHandle) const;
	
	/** When Removal Gameplay Effect is removed */
	void OnRemovalGameplayEffectRemoved(const FGameplayEffectRemovalInfo& GERemovalInfo, UAbilitySystemComponent* ASC, TTuple<FGameplayTag, FDelegateHandle> BoundEvent) const;

	/** We got removed externally and we need to register Tag Changes back properly */ 
	void OnGameplayEffectRemovalFailed(FActiveGameplayEffectHandle ActiveGEHandle) const;

	// Helper function to (un)inhibit gameplay effect, faster path when removal is not concerned
	void OnGameplayEffectInhibited(const FGameplayTag GameplayTag, int32 NewCount, FActiveGameplayEffectHandle ActiveGEHandle) const;

public:
	/** Tag requirements the target must have for this GameplayEffect to be applied. This is pass/fail at the time of application. If fail, this GE fails to apply. */
	UPROPERTY(EditDefaultsOnly, Category = Tags)
	FGameplayTagRequirements ApplicationTagRequirements;

	/** Once Applied, these tags requirements are used to determined if the GameplayEffect is "on" or "off". A GameplayEffect can be off and do nothing, but still applied. */
	UPROPERTY(EditDefaultsOnly, Category = Tags)
	FGameplayTagRequirements OngoingTagRequirements;

	/** Tag requirements that if met will remove this effect. Also prevents effect application. */
	UPROPERTY(EditDefaultsOnly, Category = Tags)
	FGameplayTagRequirements RemovalTagRequirements;
};
