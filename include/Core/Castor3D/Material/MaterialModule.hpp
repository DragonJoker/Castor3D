/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MaterialModule_H___
#define ___C3D_MaterialModule_H___

#include "Castor3D/Cache/CacheModule.hpp"

#include <CastorUtils/Design/Resource.hpp>
#include <CastorUtils/Design/Signal.hpp>

namespace castor3d
{
	/**@name Material */
	//@{

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

	constexpr std::pair< PassComponentID, TextureFlags > splitTextureFlag( PassComponentTextureFlag flag )noexcept
	{
		return { PassComponentID( uint32_t( flag ) >> 8u )
			, TextureFlags( uint32_t( flag ) & 0x000000FFu ) };
	}

	using TextureFlagsSet = std::set< PassComponentTextureFlag >;
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

	constexpr std::pair< PassComponentID, PassFlags > splitPassComponentFlag( PassComponentFlag flag )noexcept
	{
		return { PassComponentID( uint32_t( flag ) >> 8u )
			, PassFlags( uint32_t( flag ) & 0x000000FFu ) };
	}

	using PassComponentFlagsSet = std::set< PassComponentFlag >;
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

	CU_DeclareSmartPtr( castor3d, Material, C3D_API );
	CU_DeclareSmartPtr( castor3d, MaterialImporter, C3D_API );
	CU_DeclareSmartPtr( castor3d, MaterialImporterFactory, C3D_API );

	using PassTypeID = uint16_t;

	using OnMaterialChangedFunction = std::function< void( Material const & ) >;
	using OnMaterialChanged = castor::SignalT< OnMaterialChangedFunction >;
	using OnMaterialChangedConnection = OnMaterialChanged::connection;

	C3D_API VkFormat convert( castor::PixelFormat format );
	C3D_API castor::PixelFormat convert( VkFormat format );
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
	struct PtrCacheTraitsT< Material, castor::String >
		: PtrCacheTraitsBaseT< Material, castor::String >
	{
		using ResT = Material;
		using KeyT = castor::String;
		using Base = PtrCacheTraitsBaseT< ResT, KeyT >;
		using ElementT = typename Base::ElementT;
		using ElementPtrT = typename Base::ElementPtrT;

		C3D_API static const castor::String Name;
	};

	using MaterialCacheTraits = PtrCacheTraitsT< Material, castor::String >;
	using MaterialCache = castor::ResourceCacheT< Material
		, castor::String
		, MaterialCacheTraits >;

	using MaterialPtr = MaterialCacheTraits::ElementPtrT;
	using MaterialObs = MaterialCacheTraits::ElementObsT;

	CU_DeclareSmartPtr( castor3d, MaterialCache, C3D_API );

	//! Material pointer array
	CU_DeclareVector( MaterialObs, MaterialPtr );
	//! Material pointer map, sorted by name
	CU_DeclareMap( castor::String, MaterialObs, MaterialPtrStr );
	//! Material pointer map
	CU_DeclareMap( uint32_t, MaterialObs, MaterialPtrUInt );

	struct TextureCombine
	{
		TextureCombineID baseId{};
		uint32_t configCount{};
		TextureFlagsSet flags{};
	};

	C3D_API bool operator==( TextureCombine const & lhs, TextureCombine const & rhs );

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

namespace castor
{
	template<>
	struct ResourceCacheT< castor3d::Material
		, String
		, castor3d::MaterialCacheTraits >;
}

#endif
