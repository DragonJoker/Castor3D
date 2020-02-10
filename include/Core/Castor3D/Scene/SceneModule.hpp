/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneModule_H___
#define ___C3D_SceneModule_H___

#include "Castor3D/Castor3DModule.hpp"

#include <CastorUtils/Design/Signal.hpp>

namespace castor3d
{
	/**@name Scene */
	//@{

	/**
	*\~english
	*\brief
	*	Movable object types enumerator
	*\remarks
	*	There are four movable object types : camera, geometry, light and billboard.
	*\~french
	*\brief
	*	Enumération des types de MovableObject
	*\remarks
	*	Il y a quatre types d'objets déplaçables : caméra, géométrie, lumière et billboard
	*/
	enum class MovableType
		: uint8_t
	{
		eCamera,
		eGeometry,
		eLight,
		eBillboard,
		eParticleEmitter,
		CU_ScopedEnumBounds( eCamera )
	};
	castor::String getName( MovableType value );
	/**
	*\~english
	*\brief
	*	Shadow filter types enumeration.
	*\~french
	*\brief
	*	Enumération des types de filtrage des ombres.
	*/
	enum class ShadowType
	{
		//!\~english	No shadows at all.
		//!\~french		Pas d'ombres du tout.
		eNone,
		//!\~english	No filtering.
		//!\~french		Pas de filtrage.
		eRaw,
		//!\~english	PCF filtering.
		//!\~french		Filtrage PCF.
		ePCF,
		//!\~english	Variance shadow mapping.
		//!\~french		Filtrage par variance.
		eVariance,
		CU_ScopedEnumBounds( eNone )
	};
	castor::String getName( ShadowType value );
	/**
	*\~english
	*\brief
	*	Fog types enumeration.
	*\~french
	*\brief
	*	Enumération des types de brouillard.
	*/
	enum class FogType
	{
		//!\~english	No fog.
		//!\~french		Pas de brouillard
		eDisabled,
		//!\~english	Fog intensity increases linearly with distance to camera.
		//!\~french		L'intensité du brouillard augmente lin�airement avec la distance à la caméra.
		eLinear,
		//!\~english	Fog intensity increases exponentially with distance to camera.
		//!\~french		L'intensité du brouillard augmente exponentiellement avec la distance à la caméra.
		eExponential,
		//!\~english	Fog intensity increases even more with distance to camera.
		//!\~french		L'intensité du brouillard augmente encore plus avec la distance à la caméra.
		eSquaredExponential,
		CU_ScopedEnumBounds( eDisabled )
	};
	castor::String getName( FogType value );
	/**
	*\~english
	*\brief
	*	Scene flags enumeration.
	*\~french
	*\brief
	*	Enumération des indicateurs de scène.
	*/
	enum class SceneFlag
		: uint8_t
	{
		//!\~english	No flag.
		//!\~french		Aucun indicateur.
		eNone = 0x00,
		//!\~english	Linear fog.
		//!\~french		Brouillard linéaire.
		eFogLinear = 0x01,
		//!\~english	Exponential fog.
		//!\~french		Brouillard exponentiel.
		eFogExponential = 0x02,
		//!\~english	Squared exponential fog.
		//!\~french		Brouillard exponentiel au carré.
		eFogSquaredExponential = 0x04,
		//!\~english	All flags.
		//!\~french		Tous les indicateurs.
		eAll = 0x07,
	};
	CU_ImplementFlags( SceneFlag )
	/**
	*\~english
	*\brief
	*	Gives the fog type matching the given flags.
	*\param[in] flags
	*	The scene flags.
	*\return
	*	The fog type.
	*\~french
	*\brief
	*	Récupère le type de brouillard correspondant aux indicateurs donnés.
	*\param[in] flags
	*	Les indicateurs de scène.
	*\return
	*	Le type de brouillard.
	*/
	C3D_API FogType getFogType( SceneFlags const & flags );
	/**
	*\~english
	*\brief
	*	Billboard rendering types enumeration.
	*\~french
	*\brief
	*	Enumération des types de rendu des billboards.
	*/
	enum class BillboardType
	{
		//!\~english	Billboards always face the camera.
		//!\~french		Les billboards font toujours face à la caméra.
		eSpherical,
		//!\~english	Billboards rotate only on Y axis.
		//!\~french		Les billboards tournent uniquement sur l'axe Y.
		eCylindrical,
		CU_ScopedEnumBounds( eSpherical )
	};
	castor::String getName( BillboardType value );
	/**
	*\~english
	*\brief
	*	Billboard sizing types enumeration.
	*\~french
	*\brief
	*	Enumération des types de dimensions des billboards.
	*/
	enum class BillboardSize
	{
		//!\~english	Billboards size changes with camera position.
		//!\~french		Les dimensions des billboards changent en fonction de la position de la caméra.
		eDynamic,
		//!\~english	Billboards size doesn't change.
		//!\~french		Les dimensions des billboards ne changent pas.
		eFixed,
		CU_ScopedEnumBounds( eDynamic )
	};
	castor::String getName( BillboardSize value );
	/**
	*\~english
	*\brief
	*	Billboards list
	*\remarks
	*	All billboards from this list shares the same texture
	*\~french
	*\brief
	*	Liste de billboards
	*\remarks
	*	Tous les billboards de cette liste ont la meme texture
	*/
	class BillboardBase;
	/**
	*\~english
	*\brief
	*	Billboards list
	*\remarks
	*	All billboards from this list shares the same texture
	*\~french
	*\brief
	*	Liste de billboards
	*\remarks
	*	Tous les billboards de cette liste ont la meme texture
	*/
	class BillboardList;
	/**
	*\~english
	*\brief
	*	Class which represents a Camera
	*\remarks
	*	Gives its position, orientation, viewport ...
	*\~french
	*\brief
	*	Classe de représentation de Camera
	*\remarks
	*	donne la position, orientation, viewport ...
	*/
	class Camera;
	/**
	*\~english
	*\brief
	*	Fog implementation.
	*\~french
	*\brief
	*	Implémentation de brouillard.
	*/
	class Fog;
	/**
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\version	0.1
	\~english
	\brief		Geometry handler class
	\~french
	\brief		Classe de gestion d'une géométrie affichable
	*/
	class Geometry;
	/**
	*\~english
	*\brief
	*	Movable object class
	*\~french
	*\brief
	*	Classe d'objet déplaçable
	*/
	class MovableObject;
	/**
	*\~english
	*\brief
	*	Handles object's visibility, and behavior with shadows.
	*\~french
	*\brief
	*	Gère la visibilité de l'objet, ainsi que son comportement avec les ombres.
	*/
	class RenderedObject;
	/**
	*\~english
	*\brief
	*	Scene handler class
	*\remarks
	*	A scene is a collection of lights, scene nodes and geometries.
	*	<br />It has at least one camera to render it.
	*\~french
	*\brief
	*	Classe de gestion d'un scène
	*\remarks
	*	Une scène est une collection de lumières, noeuds et géométries.
	*	<br />Elle a au moins une caméra permettant son rendu.
	*/
	class Scene;
	/**
	*\~english
	*\brief
	*	The context used into parsing functions.
	*\~french
	*\brief
	*	Le contexte utilisé lors de l'analyse des fonctions.
	*/
	class SceneFileContext;
	/**
	*\~english
	*\brief
	*	CSCN file parser.
	*\remarks
	*	Reads CSCN files and extracts all 3D data from it.
	*\~french
	*\brief
	*	Analyseur de fichiers CSCN.
	*\remarks
	*	Lit les fichiers CSCN et en extrait toutes les données 3D.
	*/
	class SceneFileParser;
	/**
	*\~english
	*\brief
	*	The scene node handler class
	*\remarks
	*	A scene node is a parent for nearly every object in a scene : geometry, camera, ...
	*\~french
	*\brief
	*	La classe de gestion de noeud de scène
	*\remarks
	*	Un noeud de scène est un parent pour à peu près tous les objets d'une scène : géométrie, caméra, ...
	*/
	class SceneNode;
	/**
	*\~english
	*\brief
	*	Shadows configuration class.
	*\~french
	*\brief
	*	Classe de configuration des ombres.
	*/
	class Shadow;

	CU_DeclareSmartPtr( BillboardBase );
	CU_DeclareSmartPtr( BillboardList );
	CU_DeclareSmartPtr( Camera );
	CU_DeclareSmartPtr( Geometry );
	CU_DeclareSmartPtr( MovableObject );
	CU_DeclareSmartPtr( Scene );
	CU_DeclareSmartPtr( SceneFileContext );
	CU_DeclareSmartPtr( SceneFileParser );
	CU_DeclareSmartPtr( SceneNode );

	//! Camera pointer array
	CU_DeclareVector( CameraSPtr, CameraPtr );
	//! Geometry pointer array
	CU_DeclareVector( GeometrySPtr, GeometryPtr );
	//! SceneNode pointer array.
	CU_DeclareVector( SceneNodeSPtr, SceneNodePtr );
	//! BillboardList pointer array.
	CU_DeclareVector( BillboardListSPtr, BillboardList );
	//! Camera pointer map, sorted by name
	CU_DeclareMap( castor::String, CameraSPtr, CameraPtrStr );
	//! MovableObject pointer map, sorted by name.
	CU_DeclareMap( castor::String, MovableObjectSPtr, MovableObjectPtrStr );
	//! Geometry pointer map, sorted by name.
	CU_DeclareMap( castor::String, GeometrySPtr, GeometryPtrStr );
	//! Scene pointer map, sorted by name.
	CU_DeclareMap( castor::String, SceneSPtr, ScenePtrStr );
	//! SceneNode pointer map, sorted by name.
	CU_DeclareMap( castor::String, SceneNodeSPtr, SceneNodePtrStr );
	//! BillboardList pointer map, sorted by name.
	CU_DeclareMap( castor::String, BillboardListSPtr, BillboardListStr );

	using OnSceneChangedFunction = std::function< void( Scene const & ) >;
	using OnSceneChanged = castor::Signal< OnSceneChangedFunction >;
	using OnSceneChangedConnection = OnSceneChanged::connection;

	using OnSceneUpdateFunction = std::function< void( Scene const & ) >;
	using OnSceneUpdate = castor::Signal< OnSceneUpdateFunction >;
	using OnSceneUpdateConnection = OnSceneUpdate::connection;

	using OnCameraChangedFunction = std::function< void( Camera const & ) >;
	using OnCameraChanged = castor::Signal< OnCameraChangedFunction >;
	using OnCameraChangedConnection = OnCameraChanged::connection;

	using OnSceneNodeChangedFunction = std::function< void( SceneNode const & ) >;
	using OnSceneNodeChanged = castor::Signal< OnSceneNodeChangedFunction >;
	using OnSceneNodeChangedConnection = OnSceneNodeChanged::connection;

	using SubmeshMaterialMap = std::map< Submesh const *, MaterialWPtr >;
	using OnSubmeshMaterialChangedFunction = std::function< void( Geometry const &, Submesh const &, MaterialSPtr oldMaterial, MaterialSPtr newMaterial) >;
	using OnSubmeshMaterialChanged = castor::Signal< OnSubmeshMaterialChangedFunction >;
	using OnSubmeshMaterialChangedConnection = OnSubmeshMaterialChanged::connection;

	using OnBillboardMaterialChangedFunction = std::function< void( BillboardBase const &, MaterialSPtr oldMaterial, MaterialSPtr newMaterial ) >;
	using OnBillboardMaterialChanged = castor::Signal< OnBillboardMaterialChangedFunction >;
	using OnBillboardMaterialChangedConnection = OnBillboardMaterialChanged::connection;

	//@}
}

CU_DeclareExportedOwnedBy( C3D_API, castor3d::Scene, Scene )

#endif
