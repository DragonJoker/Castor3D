/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MaterialModule_H___
#define ___C3D_MaterialModule_H___

#include "Castor3D/Cache/CacheModule.hpp"

#include <CastorUtils/Design/Resource.hpp>
#include <CastorUtils/Design/Signal.hpp>

namespace c3d
{
	/**@name Material */
	//@{

	/**
	*\~english
	*\brief
	*	Compares a reference and a test value.
	*\~french
	*\brief
	*	Compare une valeur de référence et une valeur de test.
	*/
	enum class ComparisonFunc
	{
		//!\~english	Comparison always evaluates to false.
		//\~french		La comparaison est toujours évaluée à faux.
		eNever = 0,
		//!\~english	Comparison evaluates reference < test.
		//\~french		La comparaison évalue reference < test.
		eLess = 1,
		//!\~english	Comparison evaluates reference == test.
		//\~french		La comparaison évalue reference == test.
		eEqual = 2,
		//!\~english	Comparison evaluates reference <= test.
		//\~french		La comparaison évalue reference <= test.
		eLessOrEqual = 3,
		//!\~english	Comparison evaluates reference > test.
		//\~french		La comparaison évalue reference > test.
		eGreater = 4,
		//!\~english	Comparison evaluates reference != test.
		//\~french		La comparaison évalue reference != test.
		eNotEqual = 5,
		//!\~english	Comparison evaluates reference >= test.
		//\~french		La comparaison évalue reference >= test.
		eGreaterOrEqual = 6,
		//!\~english	Comparison always evaluates to true.
		//\~french		La comparaison est toujours évaluée à vrai.
		eAlways = 7,
		CU_ScopedEnumBounds( eNever, eAlways )
	};
	C3D_API String getName( ComparisonFunc value );

	constexpr VkCompareOp convert( ComparisonFunc v )
	{
		return VkCompareOp( v );
	}

	constexpr ComparisonFunc convert( VkCompareOp v )
	{
		return ComparisonFunc( v );
	}
	/**
	*\~english
	*\brief
	*	Texture flags.
	*\~french
	*\brief
	*	Indicateurs de texture.
	*/
	enum class TextureFlag
		: uint8_t
	{
		//!\~english	No flag.
		//\~french		Aucun indicateur.
		eNone = 0x00u,
		//!\~english	Mask for all the flags.
		//!\~french		Masque pour les indcateurs.
		eAll = 0xFFu,
	};
	CU_ImplementFlags( TextureFlag )

	using PassComponentID = uint16_t;
	using TextureCombineID = uint16_t;
	using PassComponentTextureFlag = uint32_t;

	constexpr PassComponentTextureFlag makeTextureFlag( PassComponentID componentId
		, TextureFlags componentTextureFlag )noexcept
	{
		return PassComponentTextureFlag{ uint32_t( uint32_t( componentId ) << 8u )
			| uint32_t( componentTextureFlag ) };
	}

	constexpr Pair< PassComponentID, TextureFlags > splitTextureFlag( PassComponentTextureFlag flag )noexcept
	{
		return { PassComponentID( uint32_t( flag ) >> 8u )
			, TextureFlags( uint32_t( flag ) & 0x000000FFu ) };
	}

	using TextureFlagsSet = Set< PassComponentTextureFlag >;
	/**
	*\~english
	*\brief
	*	Pass specific flags.
	*\~french
	*\brief
	*	Indicateurs spécifiques à la passe.
	*/
	enum class PassFlag
		: uint16_t
	{
		//!\~english	No flag.
		//\~french		Aucun indicateur.
		eNone = 0x0000,
		//!\~english	Mask for all the flags.
		//!\~french		Masque pour les indicateurs.
		eAll = 0xFFu,
	};
	CU_ImplementFlags( PassFlag )

	using PassComponentCombineID = uint16_t;
	using PassComponentFlag = uint32_t;

	constexpr PassComponentFlag makePassComponentFlag( PassComponentID componentId
		, PassFlags componentTextureFlag )noexcept
	{
		return PassComponentFlag{ uint32_t( uint32_t( componentId ) << 8u )
			| uint32_t( componentTextureFlag ) };
	}

	constexpr Pair< PassComponentID, PassFlags > splitPassComponentFlag( PassComponentFlag flag )noexcept
	{
		return { PassComponentID( uint32_t( flag ) >> 8u )
			, PassFlags( uint32_t( flag ) & 0x000000FFu ) };
	}

	using PassComponentFlagsSet = Set< PassComponentFlag >;
	/**
	*\~english
	*\brief
	*	Definition of a material
	*\remarks
	*	A material is composed of one or more passes.
	*\~french
	*\brief
	*	Définition d'un matériau
	*\remarks
	*	Un matériau est composé d'une ou plusieurs passes
	*/
	class Material;
	/**
	*\~english
	*\brief
	*	Base class for external file import
	*\~french
	*\brief
	*	Classe de base pour l'import de fichiers externes
	*/
	class MaterialImporter;
	/**
	*\~english
	*\brief
	*	The importer factory.
	*\~french
	*\brief
	*	La fabrique d'importeurs.
	*/
	class MaterialImporterFactory;

	using PassTypeID = uint16_t;

	using OnMaterialChangedFunction = Function< void( Material const & ) >;
	using OnMaterialChanged = SignalT< OnMaterialChangedFunction >;
	using OnMaterialChangedConnection = OnMaterialChanged::connection;

	/**
	*\~english
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Specialisation for Material.
	*\~french
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Spécialisation pour Material.
	*/
	template<>
	struct PtrCacheTraitsT< Material, String >
		: PtrCacheTraitsBaseT< Material, String >
	{
		using ResT = Material;
		using KeyT = String;
		using Base = PtrCacheTraitsBaseT< ResT, KeyT >;
		using ElementT = typename Base::ElementT;
		using ElementPtrT = typename Base::ElementPtrT;

		C3D_API static const String Name;
	};

	using MaterialCacheTraits = PtrCacheTraitsT< Material, String >;
	using MaterialCache = ResourceCacheT< Material
		, String
		, MaterialCacheTraits >;
	using MaterialPtr = MaterialCacheTraits::ElementPtrT;
	using MaterialObs = MaterialCacheTraits::ElementObsT;

	/** @cond !Doxygen */
	CU_DeclareSmartPtr( c3d, Material, C3D_API );
	CU_DeclareSmartPtr( c3d, MaterialImporter, C3D_API );
	CU_DeclareSmartPtr( c3d, MaterialImporterFactory, C3D_API );

	CU_DeclareSmartPtr( c3d, MaterialCache, C3D_API );

	//! Material pointer array
	CU_DeclareVector( MaterialObs, MaterialPtr );
	//! Material pointer map, sorted by name
	CU_DeclareMap( String, MaterialObs, MaterialPtrStr );
	//! Material pointer map
	CU_DeclareMap( uint32_t, MaterialObs, MaterialPtrUInt );
	/** @endcond */

	struct TextureCombine
	{
		TextureCombineID baseId{};
		uint32_t configCount{};
		TextureFlagsSet flags{};

	private:
		friend bool operator==( TextureCombine const & lhs, TextureCombine const & rhs )noexcept
		{
			return lhs.configCount == rhs.configCount
				&& lhs.flags == rhs.flags;
		}
	};

	C3D_API TextureFlagsSet::const_iterator checkFlag( TextureCombine const & lhs
		, PassComponentTextureFlag rhs );
	C3D_API bool hasAny( TextureFlagsSet const & lhs
		, PassComponentTextureFlag rhs );
	C3D_API bool hasAny( TextureCombine const & lhs
		, PassComponentTextureFlag rhs );
	C3D_API bool hasIntersect( PassComponentTextureFlag lhs
		, PassComponentTextureFlag rhs );
	C3D_API void remFlags( TextureCombine & lhs
		, PassComponentTextureFlag rhs );
	C3D_API void remFlags( TextureCombine & lhs
		, TextureFlagsSet const & rhs );
	C3D_API void addFlags( TextureCombine & lhs
		, PassComponentTextureFlag rhs );
	C3D_API void addFlags( TextureCombine & lhs
		, TextureFlagsSet const & rhs );
	C3D_API bool contains( TextureCombine const & cont
		, TextureCombine const & test );

	//@}
}

namespace c3d
{
	template<>
	struct ResourceCacheT< Material
		, String
		, MaterialCacheTraits >;
}

#endif
