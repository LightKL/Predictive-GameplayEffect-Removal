// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "GameplayAbilities/Public/GameplayPrediction.h"
#include "Net/Serialization/FastArraySerializerImplementation.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeGameplayPrediction() {}

// Begin Cross Module References
GAMEPLAYABILITIES_API UEnum* Z_Construct_UEnum_GameplayAbilities_EDependentChainBehavior();
GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FPredictionKey();
GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem();
GAMEPLAYABILITIES_API UScriptStruct* Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap();
NETCORE_API UScriptStruct* Z_Construct_UScriptStruct_FFastArraySerializer();
NETCORE_API UScriptStruct* Z_Construct_UScriptStruct_FFastArraySerializerItem();
UPackage* Z_Construct_UPackage__Script_GameplayAbilities();
// End Cross Module References

// Begin Enum EDependentChainBehavior
static FEnumRegistrationInfo Z_Registration_Info_UEnum_EDependentChainBehavior;
static UEnum* EDependentChainBehavior_StaticEnum()
{
	if (!Z_Registration_Info_UEnum_EDependentChainBehavior.OuterSingleton)
	{
		Z_Registration_Info_UEnum_EDependentChainBehavior.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_GameplayAbilities_EDependentChainBehavior, (UObject*)Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("EDependentChainBehavior"));
	}
	return Z_Registration_Info_UEnum_EDependentChainBehavior.OuterSingleton;
}
template<> GAMEPLAYABILITIES_API UEnum* StaticEnum<EDependentChainBehavior>()
{
	return EDependentChainBehavior_StaticEnum();
}
struct Z_Construct_UEnum_GameplayAbilities_EDependentChainBehavior_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** To me, its seems more logical that each key should have its own behavior\n\x09rather than one single way to deal with them. For instance, all the system \n\x09is actually on the 0 bitmask (EDependentChainBehavior::OlderAcceptRejectNewer)\n\x09where some keys would want to alter that default behavior \n*/" },
#endif
		{ "ModuleRelativePath", "Public/GameplayPrediction.h" },
		{ "NewerAcceptOlder.Name", "EDependentChainBehavior::NewerAcceptOlder" },
		{ "OlderAcceptRejectNewer.Name", "EDependentChainBehavior::OlderAcceptRejectNewer" },
		{ "OlderNotAcceptNewer.Name", "EDependentChainBehavior::OlderNotAcceptNewer" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "To me, its seems more logical that each key should have its own behavior\n      rather than one single way to deal with them. For instance, all the system\n      is actually on the 0 bitmask (EDependentChainBehavior::OlderAcceptRejectNewer)\n      where some keys would want to alter that default behavior" },
#endif
	};
#endif // WITH_METADATA
	static constexpr UECodeGen_Private::FEnumeratorParam Enumerators[] = {
		{ "EDependentChainBehavior::OlderAcceptRejectNewer", (int64)EDependentChainBehavior::OlderAcceptRejectNewer },
		{ "EDependentChainBehavior::OlderNotAcceptNewer", (int64)EDependentChainBehavior::OlderNotAcceptNewer },
		{ "EDependentChainBehavior::NewerAcceptOlder", (int64)EDependentChainBehavior::NewerAcceptOlder },
	};
	static const UECodeGen_Private::FEnumParams EnumParams;
};
const UECodeGen_Private::FEnumParams Z_Construct_UEnum_GameplayAbilities_EDependentChainBehavior_Statics::EnumParams = {
	(UObject*(*)())Z_Construct_UPackage__Script_GameplayAbilities,
	nullptr,
	"EDependentChainBehavior",
	"EDependentChainBehavior",
	Z_Construct_UEnum_GameplayAbilities_EDependentChainBehavior_Statics::Enumerators,
	RF_Public|RF_Transient|RF_MarkAsNative,
	UE_ARRAY_COUNT(Z_Construct_UEnum_GameplayAbilities_EDependentChainBehavior_Statics::Enumerators),
	EEnumFlags::None,
	(uint8)UEnum::ECppForm::EnumClass,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UEnum_GameplayAbilities_EDependentChainBehavior_Statics::Enum_MetaDataParams), Z_Construct_UEnum_GameplayAbilities_EDependentChainBehavior_Statics::Enum_MetaDataParams)
};
UEnum* Z_Construct_UEnum_GameplayAbilities_EDependentChainBehavior()
{
	if (!Z_Registration_Info_UEnum_EDependentChainBehavior.InnerSingleton)
	{
		UECodeGen_Private::ConstructUEnum(Z_Registration_Info_UEnum_EDependentChainBehavior.InnerSingleton, Z_Construct_UEnum_GameplayAbilities_EDependentChainBehavior_Statics::EnumParams);
	}
	return Z_Registration_Info_UEnum_EDependentChainBehavior.InnerSingleton;
}
// End Enum EDependentChainBehavior

// Begin ScriptStruct FPredictionKey
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_PredictionKey;
class UScriptStruct* FPredictionKey::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_PredictionKey.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_PredictionKey.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FPredictionKey, (UObject*)Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("PredictionKey"));
	}
	return Z_Registration_Info_UScriptStruct_PredictionKey.OuterSingleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FPredictionKey>()
{
	return FPredictionKey::StaticStruct();
}
struct Z_Construct_UScriptStruct_FPredictionKey_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "// PredictiveConnection\n" },
#endif
		{ "ModuleRelativePath", "Public/GameplayPrediction.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "PredictiveConnection" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Current_MetaData[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** The unique ID of this prediction key */" },
#endif
		{ "ModuleRelativePath", "Public/GameplayPrediction.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "The unique ID of this prediction key" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Base_MetaData[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** If non 0, the original prediction key this was created from (in a dependency chain) */" },
#endif
		{ "ModuleRelativePath", "Public/GameplayPrediction.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "If non 0, the original prediction key this was created from (in a dependency chain)" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_bIsServerInitiated_MetaData[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** True if this was created as a server initiated activation key, used to identify server activations but cannot be used for prediction */" },
#endif
		{ "ModuleRelativePath", "Public/GameplayPrediction.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "True if this was created as a server initiated activation key, used to identify server activations but cannot be used for prediction" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_DependentChainBehavior_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayPrediction.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FInt16PropertyParams NewProp_Current;
	static const UECodeGen_Private::FInt16PropertyParams NewProp_Base;
	static void NewProp_bIsServerInitiated_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_bIsServerInitiated;
	static const UECodeGen_Private::FBytePropertyParams NewProp_DependentChainBehavior_Underlying;
	static const UECodeGen_Private::FEnumPropertyParams NewProp_DependentChainBehavior;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FPredictionKey>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
};
const UECodeGen_Private::FInt16PropertyParams Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_Current = { "Current", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Int16, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FPredictionKey, Current), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Current_MetaData), NewProp_Current_MetaData) };
const UECodeGen_Private::FInt16PropertyParams Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_Base = { "Base", nullptr, (EPropertyFlags)0x0010000080000000, UECodeGen_Private::EPropertyGenFlags::Int16, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FPredictionKey, Base), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Base_MetaData), NewProp_Base_MetaData) };
void Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_bIsServerInitiated_SetBit(void* Obj)
{
	((FPredictionKey*)Obj)->bIsServerInitiated = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_bIsServerInitiated = { "bIsServerInitiated", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(FPredictionKey), &Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_bIsServerInitiated_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_bIsServerInitiated_MetaData), NewProp_bIsServerInitiated_MetaData) };
const UECodeGen_Private::FBytePropertyParams Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_DependentChainBehavior_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, nullptr, METADATA_PARAMS(0, nullptr) };
const UECodeGen_Private::FEnumPropertyParams Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_DependentChainBehavior = { "DependentChainBehavior", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FPredictionKey, DependentChainBehavior), Z_Construct_UEnum_GameplayAbilities_EDependentChainBehavior, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_DependentChainBehavior_MetaData), NewProp_DependentChainBehavior_MetaData) }; // 1338159315
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FPredictionKey_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_Current,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_Base,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_bIsServerInitiated,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_DependentChainBehavior_Underlying,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FPredictionKey_Statics::NewProp_DependentChainBehavior,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FPredictionKey_Statics::PropPointers) < 2048);
const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FPredictionKey_Statics::StructParams = {
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	nullptr,
	&NewStructOps,
	"PredictionKey",
	Z_Construct_UScriptStruct_FPredictionKey_Statics::PropPointers,
	UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FPredictionKey_Statics::PropPointers),
	sizeof(FPredictionKey),
	alignof(FPredictionKey),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000201),
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FPredictionKey_Statics::Struct_MetaDataParams), Z_Construct_UScriptStruct_FPredictionKey_Statics::Struct_MetaDataParams)
};
UScriptStruct* Z_Construct_UScriptStruct_FPredictionKey()
{
	if (!Z_Registration_Info_UScriptStruct_PredictionKey.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_PredictionKey.InnerSingleton, Z_Construct_UScriptStruct_FPredictionKey_Statics::StructParams);
	}
	return Z_Registration_Info_UScriptStruct_PredictionKey.InnerSingleton;
}
// End ScriptStruct FPredictionKey

// Begin ScriptStruct FReplicatedPredictionKeyItem
static_assert(std::is_polymorphic<FReplicatedPredictionKeyItem>() == std::is_polymorphic<FFastArraySerializerItem>(), "USTRUCT FReplicatedPredictionKeyItem cannot be polymorphic unless super FFastArraySerializerItem is polymorphic");
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_ReplicatedPredictionKeyItem;
class UScriptStruct* FReplicatedPredictionKeyItem::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_ReplicatedPredictionKeyItem.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_ReplicatedPredictionKeyItem.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem, (UObject*)Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("ReplicatedPredictionKeyItem"));
	}
	return Z_Registration_Info_UScriptStruct_ReplicatedPredictionKeyItem.OuterSingleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FReplicatedPredictionKeyItem>()
{
	return FReplicatedPredictionKeyItem::StaticStruct();
}
struct Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/GameplayPrediction.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_PredictionKey_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayPrediction.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FStructPropertyParams NewProp_PredictionKey;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FReplicatedPredictionKeyItem>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
};
const UECodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Statics::NewProp_PredictionKey = { "PredictionKey", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FReplicatedPredictionKeyItem, PredictionKey), Z_Construct_UScriptStruct_FPredictionKey, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_PredictionKey_MetaData), NewProp_PredictionKey_MetaData) }; // 2604158573
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Statics::NewProp_PredictionKey,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Statics::PropPointers) < 2048);
const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Statics::StructParams = {
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	Z_Construct_UScriptStruct_FFastArraySerializerItem,
	&NewStructOps,
	"ReplicatedPredictionKeyItem",
	Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Statics::PropPointers,
	UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Statics::PropPointers),
	sizeof(FReplicatedPredictionKeyItem),
	alignof(FReplicatedPredictionKeyItem),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Statics::Struct_MetaDataParams), Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Statics::Struct_MetaDataParams)
};
UScriptStruct* Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem()
{
	if (!Z_Registration_Info_UScriptStruct_ReplicatedPredictionKeyItem.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_ReplicatedPredictionKeyItem.InnerSingleton, Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Statics::StructParams);
	}
	return Z_Registration_Info_UScriptStruct_ReplicatedPredictionKeyItem.InnerSingleton;
}
// End ScriptStruct FReplicatedPredictionKeyItem

// Begin ScriptStruct FReplicatedPredictionKeyMap
static_assert(std::is_polymorphic<FReplicatedPredictionKeyMap>() == std::is_polymorphic<FFastArraySerializer>(), "USTRUCT FReplicatedPredictionKeyMap cannot be polymorphic unless super FFastArraySerializer is polymorphic");
static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_ReplicatedPredictionKeyMap;
class UScriptStruct* FReplicatedPredictionKeyMap::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_ReplicatedPredictionKeyMap.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_ReplicatedPredictionKeyMap.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap, (UObject*)Z_Construct_UPackage__Script_GameplayAbilities(), TEXT("ReplicatedPredictionKeyMap"));
	}
	return Z_Registration_Info_UScriptStruct_ReplicatedPredictionKeyMap.OuterSingleton;
}
template<> GAMEPLAYABILITIES_API UScriptStruct* StaticStruct<FReplicatedPredictionKeyMap>()
{
	return FReplicatedPredictionKeyMap::StaticStruct();
}
#if defined(UE_NET_HAS_IRIS_FASTARRAY_BINDING) && UE_NET_HAS_IRIS_FASTARRAY_BINDING
UE_NET_IMPLEMENT_FASTARRAY(FReplicatedPredictionKeyMap);
#else
UE_NET_IMPLEMENT_FASTARRAY_STUB(FReplicatedPredictionKeyMap);
#endif
struct Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[] = {
		{ "ModuleRelativePath", "Public/GameplayPrediction.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_PredictionKeys_MetaData[] = {
		{ "ModuleRelativePath", "Public/GameplayPrediction.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FStructPropertyParams NewProp_PredictionKeys_Inner;
	static const UECodeGen_Private::FArrayPropertyParams NewProp_PredictionKeys;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static void* NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FReplicatedPredictionKeyMap>();
	}
	static const UECodeGen_Private::FStructParams StructParams;
};
const UECodeGen_Private::FStructPropertyParams Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Statics::NewProp_PredictionKeys_Inner = { "PredictionKeys", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, 0, Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem, METADATA_PARAMS(0, nullptr) }; // 230195043
const UECodeGen_Private::FArrayPropertyParams Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Statics::NewProp_PredictionKeys = { "PredictionKeys", nullptr, (EPropertyFlags)0x0010000000000000, UECodeGen_Private::EPropertyGenFlags::Array, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(FReplicatedPredictionKeyMap, PredictionKeys), EArrayPropertyFlags::None, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_PredictionKeys_MetaData), NewProp_PredictionKeys_MetaData) }; // 230195043
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Statics::NewProp_PredictionKeys_Inner,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Statics::NewProp_PredictionKeys,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Statics::PropPointers) < 2048);
const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Statics::StructParams = {
	(UObject* (*)())Z_Construct_UPackage__Script_GameplayAbilities,
	Z_Construct_UScriptStruct_FFastArraySerializer,
	&NewStructOps,
	"ReplicatedPredictionKeyMap",
	Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Statics::PropPointers,
	UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Statics::PropPointers),
	sizeof(FReplicatedPredictionKeyMap),
	alignof(FReplicatedPredictionKeyMap),
	RF_Public|RF_Transient|RF_MarkAsNative,
	EStructFlags(0x00000001),
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Statics::Struct_MetaDataParams), Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Statics::Struct_MetaDataParams)
};
UScriptStruct* Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap()
{
	if (!Z_Registration_Info_UScriptStruct_ReplicatedPredictionKeyMap.InnerSingleton)
	{
		UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_ReplicatedPredictionKeyMap.InnerSingleton, Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Statics::StructParams);
	}
	return Z_Registration_Info_UScriptStruct_ReplicatedPredictionKeyMap.InnerSingleton;
}
// End ScriptStruct FReplicatedPredictionKeyMap

// Begin Registration
struct Z_CompiledInDeferFile_FID_Unreal_Projects_Test_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayPrediction_h_Statics
{
	static constexpr FEnumRegisterCompiledInInfo EnumInfo[] = {
		{ EDependentChainBehavior_StaticEnum, TEXT("EDependentChainBehavior"), &Z_Registration_Info_UEnum_EDependentChainBehavior, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 1338159315U) },
	};
	static constexpr FStructRegisterCompiledInInfo ScriptStructInfo[] = {
		{ FPredictionKey::StaticStruct, Z_Construct_UScriptStruct_FPredictionKey_Statics::NewStructOps, TEXT("PredictionKey"), &Z_Registration_Info_UScriptStruct_PredictionKey, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FPredictionKey), 2604158573U) },
		{ FReplicatedPredictionKeyItem::StaticStruct, Z_Construct_UScriptStruct_FReplicatedPredictionKeyItem_Statics::NewStructOps, TEXT("ReplicatedPredictionKeyItem"), &Z_Registration_Info_UScriptStruct_ReplicatedPredictionKeyItem, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FReplicatedPredictionKeyItem), 230195043U) },
		{ FReplicatedPredictionKeyMap::StaticStruct, Z_Construct_UScriptStruct_FReplicatedPredictionKeyMap_Statics::NewStructOps, TEXT("ReplicatedPredictionKeyMap"), &Z_Registration_Info_UScriptStruct_ReplicatedPredictionKeyMap, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FReplicatedPredictionKeyMap), 1310089831U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Unreal_Projects_Test_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayPrediction_h_934908050(TEXT("/Script/GameplayAbilities"),
	nullptr, 0,
	Z_CompiledInDeferFile_FID_Unreal_Projects_Test_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayPrediction_h_Statics::ScriptStructInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Unreal_Projects_Test_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayPrediction_h_Statics::ScriptStructInfo),
	Z_CompiledInDeferFile_FID_Unreal_Projects_Test_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayPrediction_h_Statics::EnumInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Unreal_Projects_Test_Plugins_GameplayAbilities_Source_GameplayAbilities_Public_GameplayPrediction_h_Statics::EnumInfo));
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
