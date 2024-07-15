// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayEffect.h"
#include "GameplayEffectComponent.h"
#include "ImmunityGameplayEffectComponent.generated.h"

/** 
 * Immunity is blocking the application of _other_ GameplayEffectSpecs.
 * This registers a global handler on the ASC to block the application of other GameplayEffectSpecs.
 */
UCLASS(DisplayName="Immunity to Other Effects")
class GAMEPLAYABILITIES_API UImmunityGameplayEffectComponent : public UGameplayEffectComponent
{
	friend class UGameplayEffect; // Needed for upgrade path
	GENERATED_BODY()

public:
	/** Constructor to set EditorFriendlyName */
	UImmunityGameplayEffectComponent();

	/** We need to register our callback to check for Immunity */
	virtual bool OnActiveGameplayEffectAdded(FActiveGameplayEffectsContainer& ActiveGEContainer, FActiveGameplayEffect& ActiveGE) const override;

#if WITH_EDITOR
	/**
	 * Warn about instant Gameplay Effects as they do not persist over time (and therefore cannot grant immunity)
	 */
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif // WITH_EDITOR

protected:
	/** We register with the AbilitySystemComponent to try and block any GESpecs we think we should be immune to */
	bool AllowGameplayEffectApplication(const FActiveGameplayEffectsContainer& ActiveGEContainer, const FGameplayEffectSpec& GESpecToConsider, FActiveGameplayEffectHandle ImmunityActiveGE) const;

	/** Wrapper function to register immunity queries when needed */
	void RegisterGameplayEffectImmunity(UAbilitySystemComponent* OwnerASC, FActiveGameplayEffect& ActiveGE) const;
	
	/** We need to register a callback when removal is mispredicted */
	void OnGameplayEffectRemovalFailed(UAbilitySystemComponent* OwnerASC, FActiveGameplayEffect* ActiveGE) const;

	/** Callback for the removal of the GE. This is where we bind the mispredict. This used to be a lambda */
	void OnGameplayEffectRemoved(const FGameplayEffectRemovalInfo& RemovalInfo, UAbilitySystemComponent* OwnerASC, FDelegateHandle QueryToRemove) const;

public:
	/** Grants immunity to GameplayEffects that match this query. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = None)
	TArray<FGameplayEffectQuery> ImmunityQueries;
};
