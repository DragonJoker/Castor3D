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
	*	Texture channels.
	*\~french
	*\brief
	*	Les canaux de texture.
	*/
	enum class TextureChannel
		: uint16_t
	{
		//!\~english	Height map.
		//!\~french		Map de hauteur.
		eHeight,
		//!\~english	Normal map.
		//!\~french		Map de normales.
		eNormal,
		//!\~english	Colour (raw, base, or diffuse) map.
		//!\~french		Map de couleur (brute, base ou diffuse).
		eColour,
		//!\~english	Opacity map.
		//!\~french		Map d'opacité.
		eOpacity,
		//!\~english	Emissive map.
		//!\~french		Map d'émissive.
		eEmissive,
		//!\~english	Occlusion map.
		//!\~french		Map d'occlusion.
		eOcclusion,
		//!\~english	Light transmittance map.
		//!\~french		Map de transmission de lumière.
		eTransmittance,
		//!\~english	Specular map.
		//!\~french		Map de spéculaire.
		eSpecular,
		//!\~english	Metalness map.
		//!\~french		Map de metalness.
		eMetalness,
		//!\~english	Glossiness/Shininess map.
		//!\~french		Map de glossiness/shininess.
		eGlossiness,
		//!\~english	Roughness map.
		//!\~french		Map de roughness.
		eRoughness,
		//!\~english	Count.
		//!\~french		Compte.
		eCount,
	};
	/**
	*\~english
	*\brief
	*	Texture channels flags.
	*\~french
	*\brief
	*	Indicateurs des canaux de texture.
	*/
	enum class TextureFlag
		: uint16_t
	{
		//!\~english	No texture.
		//!\~french		Pas de texture.
		eNone = 0x0000,
		//!\~english	Height map.
		//!\~french		Map de hauteur.
		eHeight = 0x0001 << uint16_t( TextureChannel::eHeight ),
		//!\~english	Normal map.
		//!\~french		Map de normales.
		eNormal = 0x0001 << uint16_t( TextureChannel::eNormal ),
		//!\~english	Opacity map.
		//!\~french		Map d'opacité.
		eOpacity = 0x0001 << uint16_t( TextureChannel::eOpacity ),
		//!\~english	Colour (raw, base, or diffuse) map.
		//!\~french		Map de couleur (brute, base ou diffuse).
		eColour = 0x0001 << uint16_t( TextureChannel::eColour ),
		//!\~english	Emissive map.
		//!\~french		Map d'émissive.
		eEmissive = 0x0001 << uint16_t( TextureChannel::eEmissive ),
		//!\~english	Occlusion map.
		//!\~french		Map d'occlusion.
		eOcclusion = 0x0001 << uint16_t( TextureChannel::eOcclusion ),
		//!\~english	Light transmittance map.
		//!\~french		Map de transmission de lumière.
		eTransmittance = 0x0001 << uint16_t( TextureChannel::eTransmittance ),
		//!\~english	Specular map.
		//!\~french		Map de spéculaire.
		eSpecular = 0x0001 << uint16_t( TextureChannel::eSpecular ),
		//!\~english	Metalness map.
		//!\~french		Map de metalness.
		eMetalness = 0x0001 << uint16_t( TextureChannel::eMetalness ),
		//!\~english	Glossiness map.
		//!\~french		Map de glossiness.
		eGlossiness = 0x0001 << uint16_t( TextureChannel::eGlossiness ),
		//!\~english	Roughness map.
		//!\~french		Map de roughness.
		eRoughness = 0x0001 << uint16_t( TextureChannel::eRoughness ),
		//!\~english	Mask for all the texture channels affecting geometry.
		//!\~french		Masque pour les canaux de texture affectant la géométrie.
		eGeometry = eOpacity | eHeight,
		//!\~english	Mask for all the texture channels.
		//!\~french		Masque pour les canaux de texture.
		eAll = ( 0x0001 << uint16_t( TextureChannel::eCount ) ) - 1u,
		//!\~english	Mask for all the texture channels except for opacity.
		//!\~french		Masque pour les canaux de texture sauf l'opacité.
		eAllButOpacity = eAll & ~( eOpacity ),
		//!\~english	Mask for all the texture channels except for opacity and colour.
		//!\~french		Masque pour les canaux de texture sauf l'opacité et la couleur.
		eAllButColourAndOpacity = eAllButOpacity & ~( eColour ),
		//!\~english	Mask for all the texture channels except for opacity and normal.
		//!\~french		Masque pour les canaux de texture sauf l'opacité et la normale.
		eAllButNormalAndOpacity = eAllButOpacity & ~( eNormal ),
		//!\~english	Mask for all the texture channels except for geometry related ones.
		//!\~french		Masque pour les canaux de texture sauf ceux liés à la géométrie.
		eAllButGeometry = eAll & ~( eGeometry ),
	};
	CU_ImplementFlags( TextureFlag )
	C3D_API castor::String getName( TextureFlag value
		, bool isPbr );
	struct TextureFlagsId
	{
		TextureFlags flags;
		uint32_t id;
	};
	C3D_API bool operator==( TextureFlagsId const & lhs, TextureFlagsId const & rhs );
	using TextureFlagsArray = std::vector< TextureFlagsId >;
	using FilteredTextureFlags = std::map< uint32_t, TextureFlagsId >;
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
