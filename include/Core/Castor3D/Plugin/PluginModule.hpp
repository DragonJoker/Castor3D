/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PluginModule_H___
#define ___C3D_PluginModule_H___

#include "Castor3D/Cache/CacheModule.hpp"

#include <CastorUtils/Data/Path.hpp>

namespace c3d
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
		eImporter = 0,
		//!\~english	Mesh Divider plug-in.
		//!\~french		Plug-in de subdivision de maillage.
		eDivider = 1,
		//!\~english	Procedural generator.
		//!\~french		Plug-in de génération procédurale.
		eGenerator = 2,
		//!\~english	Tone mapping plug-in.
		//!\~french		Plug-in d'effet de mappage de tons.
		eToneMapping = 3,
		//!\~english	Post effect plug-in.
		//!\~french		Plug-in d'effet post-rendu.
		ePostEffect = 4,
		//!\~english	Generic plug-in.
		//!\~french		Plug-in générique.
		eGeneric = 5,
		//!\~english	Particle plug-in.
		//!\~french		Plug-in de particule.
		eParticle = 6,
		CU_ScopedEnumBounds( eImporter, eParticle )
	};
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
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Specialisation for Plugin.
	*\~french
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Spécialisation pour Plugin.
	*/
	template<>
	struct PtrCacheTraitsT< Plugin, String >
		: PtrCacheTraitsBaseT< Plugin, String >
	{
		using ResT = Plugin;
		using KeyT = String;
		using Base = PtrCacheTraitsBaseT< ResT, KeyT >;
		using ElementT = typename Base::ElementT;
		using ElementPtrT = typename Base::ElementPtrT;

		C3D_API static const String Name;
	};

	using PluginCacheTraits = PtrCacheTraitsT< Plugin, String >;
	using PluginCache = ResourceCacheT< Plugin
		, String
		, PluginCacheTraits >;

	using PluginRes = PluginCacheTraits::ElementPtrT;
	using PluginResPtr = PluginCacheTraits::ElementObsT;

	/** @cond !Doxygen */
	CU_DeclareSmartPtr( c3d, Plugin, C3D_API );
	CU_DeclareSmartPtr( c3d, PluginCache, C3D_API );

	CU_DeclareMap( String, PluginUPtr, PluginStr );
	CU_DeclareArray( PluginStrMap, PluginType::eCount, PluginStrMap );
	CU_DeclareMap( Path, PluginType, PluginTypePath );
	/** @endcond */

	//@}
}

namespace c3d
{
	template<>
	struct ResourceCacheT< Plugin
		, String
		, PluginCacheTraits >;
}

#endif
