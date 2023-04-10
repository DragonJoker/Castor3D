/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PluginModule_H___
#define ___C3D_PluginModule_H___

#include "Castor3D/Cache/CacheModule.hpp"

#include <CastorUtils/Data/Path.hpp>

namespace castor3d
{
	/**@name Plugin */
	//@{

	/**
	*\~english
	*\brief
	*	Plugin types enumeration
	*\~french
	*\brief
	*	Enumération des types de plug-ins
	*/
	enum class PluginType
		: uint8_t
	{
		//!\~english	Importer plug-in.
		//!\~french		Plug-in d'importation.
		eImporter,
		//!\~english	Mesh Divider plug-in.
		//!\~french		Plug-in de subdivision de maillage.
		eDivider,
		//!\~english	Procedural generator.
		//!\~french		Plug-in de génération procédurale.
		eGenerator,
		//!\~english	Tone mapping plug-in.
		//!\~french		Plug-in d'effet de mappage de tons.
		eToneMapping,
		//!\~english	Post effect plug-in.
		//!\~french		Plug-in d'effet post-rendu.
		ePostEffect,
		//!\~english	Generic plug-in.
		//!\~french		Plug-in générique.
		eGeneric,
		//!\~english	Particle plug-in.
		//!\~french		Plug-in de particule.
		eParticle,
		CU_ScopedEnumBounds( eImporter )
	};

	/**
	*\~english
	*\brief
	*	Divider Plugin class
	*\~french
	*\brief
	*	Classe de plug-in de subdivision
	*/
	class DividerPlugin;
	/**
	*\~english
	*\brief
	*	Generator Plugin class
	*\~french
	*\brief
	*	Classe de plug-in de générateur procédural
	*/
	class GeneratorPlugin;
	/**
	*\~english
	*\brief
	*	Generic Plugin class.
	*\remarks
	*	Allows general specialisations for Castor3D.
	*\~french
	*\brief
	*	Classe de plug-in générique.
	*\remarks
	*	Permet des spécialisation générales pour Castor3D.
	*/
	class GenericPlugin;
	/**
	*\~english
	*\brief
	*	Importer Plugin class
	*\~french
	*\brief
	*	Classe de plug-in d'import
	*/
	class ImporterPlugin;
	/**
	*\~english
	*\brief
	*	Particle plug-in class.
	*\~french
	*\brief
	*	Classe de plug-in de particules.
	*/
	class ParticlePlugin;
	/**
	*\~english
	*\brief
	*	Plugin Base class
	*\remarks
	*	Manages the base plug-in functions, allows plug-ins to check versions and to register themselves
	*\~french
	*\brief
	*	Classe de base des plug-ins
	*\remarks
	*	Gère les fonctions de base d'un plug-in, permet aux plug-ins de faire des vérifications de version et  de s'enregistrer auprès du moteur
	*/
	class Plugin;
	/**
	*\~english
	*\brief
	*	Plugin loading exception
	*\~french
	*\brief
	*	Exception de chargement de plug-in
	*/
	class PluginException;
	/**
	*\~english
	*\brief
	*	Post effect plug-in class
	*\~french
	*\brief
	*	Classe de plug-in d'effets post rendu
	*/
	class PostFxPlugin;
	/**
	*\~english
	*\brief
	*	Tone mapping plug-in class.
	*\~french
	*\brief
	*	Classe de plug-in de mappage de tons.
	*/
	class ToneMappingPlugin;

	CU_DeclareSmartPtr( castor3d, Plugin, C3D_API );
	CU_DeclareSmartPtr( castor3d, ImporterPlugin, C3D_API );
	CU_DeclareSmartPtr( castor3d, DividerPlugin, C3D_API );
	CU_DeclareSmartPtr( castor3d, GeneratorPlugin, C3D_API );
	CU_DeclareSmartPtr( castor3d, GenericPlugin, C3D_API );
	CU_DeclareSmartPtr( castor3d, PostFxPlugin, C3D_API );
	CU_DeclareSmartPtr( castor3d, ParticlePlugin, C3D_API );
	CU_DeclareSmartPtr( castor3d, ToneMappingPlugin, C3D_API );

	CU_DeclareMap( castor::String, PluginUPtr, PluginStr );
	CU_DeclareArray( PluginStrMap, PluginType::eCount, PluginStrMap );
	CU_DeclareMap( castor::Path, PluginType, PluginTypePath );
	/**
	*\~english
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Specialisation for Plugin.
	*\~french
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Spécialisation pour Plugin.
	*/
	template<>
	struct PtrCacheTraitsT< Plugin, castor::String >
		: PtrCacheTraitsBaseT< Plugin, castor::String >
	{
		using ResT = Plugin;
		using KeyT = castor::String;
		using Base = PtrCacheTraitsBaseT< ResT, KeyT >;
		using ElementT = typename Base::ElementT;
		using ElementPtrT = typename Base::ElementPtrT;

		C3D_API static const castor::String Name;
	};

	using PluginCacheTraits = PtrCacheTraitsT< Plugin, castor::String >;
	using PluginCache = castor::ResourceCacheT< Plugin
		, castor::String
		, PluginCacheTraits >;

	using PluginRes = PluginCacheTraits::ElementPtrT;
	using PluginResPtr = PluginCacheTraits::ElementObsT;

	CU_DeclareSmartPtr( castor3d, PluginCache, C3D_API );

	//@}
}

namespace castor
{
	template<>
	struct ResourceCacheT< castor3d::Plugin
		, String
		, castor3d::PluginCacheTraits >;
}

#endif
