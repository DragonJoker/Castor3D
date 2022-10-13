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
	*	Texture channels flags.
	*\~french
	*\brief
	*	Indicateurs des canaux de texture.
	*/
	enum class TextureFlag
		: uint8_t
	{
		//!\~english	No texture.
		//!\~french		Pas de texture.
		eNone = 0x00u,
		//!\~english	Mask for all the texture channels.
		//!\~french		Masque pour les canaux de texture.
		eAll = 0xFFu,
	};
	CU_ImplementFlags( TextureFlag )

	using PassComponentID = uint16_t;
	using PassComponentTextureFlag = uint32_t;

	inline PassComponentTextureFlag makeTextureFlag( PassComponentID componentId
		, TextureFlags componentTextureFlag )
	{
		return PassComponentTextureFlag{ uint32_t( uint32_t( componentId ) << 8u )
			| uint32_t( componentTextureFlag ) };
	}

	inline std::pair< PassComponentID, TextureFlags > splitTextureFlag( PassComponentTextureFlag flag )
	{
		return { PassComponentID( uint32_t( flag ) >> 8u )
			, TextureFlags( uint32_t( flag ) & 0x000000FFu ) };
	}

	using TextureFlagsArray = std::vector< PassComponentTextureFlag >;
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

	CU_DeclareSmartPtr( Material );
	CU_DeclareSmartPtr( MaterialImporter );

	CU_DeclareCUSmartPtr( castor3d, MaterialImporterFactory, C3D_API );

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
	struct ResourceCacheTraitsT< Material, castor::String >
		: castor::ResourceCacheTraitsBaseT< Material, castor::String, ResourceCacheTraitsT< Material, castor::String > >
	{
		using ResT = Material;
		using KeyT = castor::String;
		using TraitsT = ResourceCacheTraitsT< ResT, KeyT >;
		using Base = castor::ResourceCacheTraitsBaseT< ResT, KeyT, TraitsT >;
		using ElementT = typename Base::ElementT;
		using ElementPtrT = typename Base::ElementPtrT;

		C3D_API static const castor::String Name;
	};

	using MaterialCacheTraits = ResourceCacheTraitsT< Material, castor::String >;
	using MaterialCache = castor::ResourceCacheT< Material
		, castor::String
		, MaterialCacheTraits >;

	using MaterialRes = MaterialCacheTraits::ElementPtrT;
	using MaterialResPtr = MaterialCacheTraits::ElementObsT;

	CU_DeclareCUSmartPtr( castor3d, MaterialCache, C3D_API );

	//! Material pointer array
	CU_DeclareVector( MaterialRPtr, MaterialPtr );
	//! Material pointer map, sorted by name
	CU_DeclareMap( castor::String, MaterialRPtr, MaterialPtrStr );
	//! Material pointer map
	CU_DeclareMap( uint32_t, MaterialRPtr, MaterialPtrUInt );

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
