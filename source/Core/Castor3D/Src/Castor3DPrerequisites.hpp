/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_PREREQUISITES_H___
#define ___C3D_PREREQUISITES_H___

#undef RGB

#if defined( __linux__ )
#	define CASTOR_X11
#	define C3D_API
#elif defined( _WIN32 )
#	define CASTOR_MSW
#	if defined( Castor3D_EXPORTS )
#		define C3D_API __declspec( dllexport )
#	else
#		define C3D_API __declspec( dllimport )
#	endif
#endif

#include <CastorUtilsPrerequisites.hpp>
#include <Collection.hpp>
#include <OwnedBy.hpp>
#include <Point.hpp>
#include <Quaternion.hpp>
#include <SquareMatrix.hpp>
#include <StringUtils.hpp>

#include <GlslWriterPrerequisites.hpp>

namespace Castor3D
{
	using Castor::real;
	using Castor::UIntStrMap;
	using Castor::UInt64StrMap;
}

#include "Castor3DPrerequisites_Animation.hpp"
#include "Castor3DPrerequisites_Camera.hpp"
#include "Castor3DPrerequisites_Geometry.hpp"
#include "Castor3DPrerequisites_Light.hpp"
#include "Castor3DPrerequisites_Material.hpp"
#include "Castor3DPrerequisites_Overlay.hpp"
#include "Castor3DPrerequisites_Scene.hpp"
#include "Castor3DPrerequisites_Shader.hpp"
#include "Castor3DPrerequisites_Render.hpp"
#include "Castor3DPrerequisites_Event.hpp"

namespace Castor3D
{
	namespace
	{
		Castor::String const RENDERER_TYPE_UNDEFINED = cuT( "Undefined" );
	}

	/**@name Importer */
	//@{

	class Importer;
	DECLARE_SMART_PTR( Importer );

	//@}
	/**@name General */
	//@{

	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Movable object types enumerator
	\remark		There are four movable object types : camera, geometry, light and billboard.
	\~french
	\brief		Enumération des types de MovableObject
	\remark		Il y a quatre types d'objets déplaçables : caméra, géométrie, lumière et billboard
	*/
	typedef enum eMOVABLE_TYPE
		: uint8_t
	{
		eMOVABLE_TYPE_CAMERA,
		eMOVABLE_TYPE_GEOMETRY,
		eMOVABLE_TYPE_LIGHT,
		eMOVABLE_TYPE_BILLBOARD,
		CASTOR_ENUM_BOUNDS( eMOVABLE_TYPE, eMOVABLE_TYPE_CAMERA )
	}	eMOVABLE_TYPE;
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		RenderTarget supported types
	\~french
	\brief		Types de RenderTarget supportés
	*/
	typedef enum eTARGET_TYPE
		: uint8_t
	{
		eTARGET_TYPE_WINDOW,
		eTARGET_TYPE_TEXTURE,
		CASTOR_ENUM_BOUNDS( eTARGET_TYPE, eTARGET_TYPE_WINDOW )
	}	eTARGET_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Plugin types enumeration
	\~french
	\brief		Enumération des types de plug-ins
	*/
	typedef enum ePLUGIN_TYPE
		: uint8_t
	{
		//!\~english	Renderer plug-in.
		//!\~french		Plug-in de rendu.
		ePLUGIN_TYPE_RENDERER,
		//!\~english	Importer plug-in.
		//!\~french		Plug-in d'importation.
		ePLUGIN_TYPE_IMPORTER,
		//!\~english	Mesh Divider plug-in.
		//!\~french		Plug-in de subdivision de maillage.
		ePLUGIN_TYPE_DIVIDER,
		//!\~english	Procedural generator.
		//!\~french		Plug-in de génération procédurale.
		ePLUGIN_TYPE_GENERATOR,
		//!\~english	Render technique plug-in.
		//!\~french		Plug-in de technique de rendu.
		ePLUGIN_TYPE_TECHNIQUE,
		//!\~english	Tone mapping plug-in.
		//!\~french		Plug-in d'effet de mappage de tons.
		ePLUGIN_TYPE_TONEMAPPING,
		//!\~english	Post effect plug-in.
		//!\~french		Plug-in d'effet post-rendu.
		ePLUGIN_TYPE_POSTFX,
		//!\~english	Generic plug-in.
		//!\~french		Plug-in générique.
		ePLUGIN_TYPE_GENERIC,
		CASTOR_ENUM_BOUNDS( ePLUGIN_TYPE, ePLUGIN_TYPE_RENDERER )
	}	ePLUGIN_TYPE;
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Frame buffer components enumeration
	\~french
	\brief		Enumération des composantes de tampon d'image
	*/
	typedef enum eBUFFER_COMPONENT
		: uint8_t
	{
		eBUFFER_COMPONENT_COLOUR = 0x01,
		eBUFFER_COMPONENT_DEPTH = 0x02,
		eBUFFER_COMPONENT_STENCIL = 0x04,
	}	eBUFFER_COMPONENT;
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Frame buffer components enumeration
	\~french
	\brief		Enumération des composantes de tampon d'image
	*/
	typedef enum eBUFFER
		: uint8_t
	{
		eBUFFER_NONE,
		eBUFFER_FRONT_LEFT,
		eBUFFER_FRONT_RIGHT,
		eBUFFER_BACK_LEFT,
		eBUFFER_BACK_RIGHT,
		eBUFFER_FRONT,
		eBUFFER_BACK,
		eBUFFER_LEFT,
		eBUFFER_RIGHT,
		eBUFFER_FRONT_AND_BACK,
		CASTOR_ENUM_BOUNDS( eBUFFER, eBUFFER_NONE )
	}	eBUFFER;
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Frame buffer binding targets enumeration
	\~french
	\brief		Enumération des cibles d'activation de tampon d'image
	*/
	typedef enum eFRAMEBUFFER_TARGET
		: uint8_t
	{
		//!\~english Frame buffer is bound as a target for draws	\~french Le tampon d'image est activé en tant que cible pour les rendus
		eFRAMEBUFFER_TARGET_DRAW,
		//!\~english Frame buffer is bound as a target for reads	\~french Le tampon d'image est activé en tant que cible pour les lectures
		eFRAMEBUFFER_TARGET_READ,
		//!\~english Frame buffer is bound as a target for reads and draws	\~french Le tampon d'image est activé en tant que cible pour les lectures et les rendus
		eFRAMEBUFFER_TARGET_BOTH,
		CASTOR_ENUM_BOUNDS( eFRAMEBUFFER_TARGET, eFRAMEBUFFER_TARGET_DRAW )
	}	eFRAMEBUFFER_TARGET;
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Frame buffer binding modes enumeration
	\~french
	\brief		Enumération des modes d'activation de tampon d'image
	*/
	typedef enum eFRAMEBUFFER_MODE
		: uint8_t
	{
		//!\~english Frame buffer is bound for configuration	\~french Le tampon d'image est activé pour configuration
		eFRAMEBUFFER_MODE_CONFIG,
		//!\~english Frame buffer is bound and FrameBuffer::SetDrawBuffers is called automatically	\~french Le tampon d'image est activé et FrameBuffer::SetDrawBuffers est appelée automatiquement
		eFRAMEBUFFER_MODE_AUTOMATIC,
		//!\~english Frame buffer is bound and user must call FrameBuffer::SetDrawBuffers if he wants	\~french Le tampon d'image est activé et l'utilisateur doit appeler FrameBuffer::SetDrawBuffers s'il veut
		eFRAMEBUFFER_MODE_MANUAL,
		CASTOR_ENUM_BOUNDS( eFRAMEBUFFER_MODE, eFRAMEBUFFER_MODE_CONFIG )
	}	eFRAMEBUFFER_MODE;
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Frame buffer attachment points enumeration
	\~french
	\brief		Enumération des points d'attache pour un tampon d'image
	*/
	typedef enum eATTACHMENT_POINT
		: uint8_t
	{
		eATTACHMENT_POINT_NONE,
		eATTACHMENT_POINT_COLOUR,
		eATTACHMENT_POINT_DEPTH,
		eATTACHMENT_POINT_STENCIL,
		CASTOR_ENUM_BOUNDS( eATTACHMENT_POINT, eATTACHMENT_POINT_NONE )
	}	eATTACHMENT_POINT;
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0.0
	\date		11/10/2015
	\~english
	\brief		Frame buffer attachment types enumeration.
	\~french
	\brief		Enumération des types d'attache pour un tampon d'image.
	*/
	typedef enum eATTACHMENT_TYPE
		: uint8_t
	{
		eATTACHMENT_TYPE_TEXTURE,
		eATTACHMENT_TYPE_BUFFER,
		CASTOR_ENUM_BOUNDS( eATTACHMENT_TYPE, eATTACHMENT_TYPE_TEXTURE )
	}	eATTACHMENT_TYPE;
	/*!
	\author		Sylvain DOREMUS
	\~english
	\brief		The  viewport projection types enumeration
	\~french
	\brief		Enumération des types de projection de viewport
	*/
	typedef enum eVIEWPORT_TYPE
		: uint8_t
	{
		eVIEWPORT_TYPE_ORTHO,
		eVIEWPORT_TYPE_PERSPECTIVE,
		eVIEWPORT_TYPE_FRUSTUM,
		CASTOR_ENUM_BOUNDS( eVIEWPORT_TYPE, eVIEWPORT_TYPE_ORTHO )
	}	eVIEWPORT_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\date 		10/06/2013
	\version	0.7.0
	\~english
	\brief		Frustum view planes enumeration
	\~french
	\brief		Enumération des plans du frustum de vue
	*/
	typedef enum eFRUSTUM_PLANE
		: uint8_t
	{
		//!\~english Near plane	\~french Plan éloigné
		eFRUSTUM_PLANE_NEAR,
		//!\~english Far plane	\~french Plan proche
		eFRUSTUM_PLANE_FAR,
		//!\~english Left plane	\~french Plan gauche
		eFRUSTUM_PLANE_LEFT,
		//!\~english Right plane	\~french Plan droit
		eFRUSTUM_PLANE_RIGHT,
		//!\~english Top plane	\~french Plan haut
		eFRUSTUM_PLANE_TOP,
		//!\~english Bottom plane	\~french Plan bas
		eFRUSTUM_PLANE_BOTTOM,
		CASTOR_ENUM_BOUNDS( eFRUSTUM_PLANE, eFRUSTUM_PLANE_NEAR )
	}	eFRUSTUM_PLANE;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Frame Event Type enumeration
	\~french
	\brief		Enumération des types d'évènement de frame
	*/
	typedef enum eEVENT_TYPE
		: uint8_t
	{
		//!\~english This kind of event happens before any render, device context is active (so be fast !!)	\~french Ce type d'évènement est traité avant le rendu, le contexte de rendu est actif (donc soyez rapide !!)
		eEVENT_TYPE_PRE_RENDER,
		//!\~english This kind of event happens after the buffer' swap	\~french Ce type d'évènement est traité après l'échange des tampons
		eEVENT_TYPE_POST_RENDER,
		CASTOR_ENUM_BOUNDS( eEVENT_TYPE, eEVENT_TYPE_PRE_RENDER )
	}	eEVENT_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date		23/05/2013
	\~english
	\brief		Faces orientations enumeration
	\~french
	\brief		Enumération des orientations des faces
	*/
	typedef enum eFACE
		: uint8_t
	{
		//!\~english No face	\~french Aucune face
		eFACE_NONE,
		//!\~english Front face	\~french Face avant
		eFACE_FRONT,
		//!\~english Back face	\~french Face arrière
		eFACE_BACK,
		//!\~english Back and front faces	\~french Faces avant et arrière
		eFACE_FRONT_AND_BACK,
		CASTOR_ENUM_BOUNDS( eFACE, eFACE_NONE )
	}   eFACE;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date		23/05/2013
	\~english
	\brief		Polygon rasterisation modes enumeration
	\~french
	\brief		Enumération des mode de rastérisation des polygones
	*/
	typedef enum eFILL_MODE
		: uint8_t
	{
		//!\~english Polygon vertices that are marked as the start of a boundary edge are drawn as points	\~french Les vertices marquant les arêtes sont dessinés en tant que points
		eFILL_MODE_POINT,
		//!\~english Boundary edges of the polygon are drawn as line segments	\~french Les arêtes du polygone sont dessinées en tant que segments
		eFILL_MODE_LINE,
		//!\~english The interior of the polygon is filled	\~french L'intérieur du polygone est rempli
		eFILL_MODE_SOLID,
		CASTOR_ENUM_BOUNDS( eFILL_MODE, eFILL_MODE_POINT )
	}   eFILL_MODE;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.8.0
	\date		16/03/2016
	\~english
	\brief		GPU query supported types enumeration.
	\~french
	\brief		Enumération des types de requêtes GPU supportés.
	*/
	typedef enum eQUERY_TYPE
		: uint32_t
	{
		eQUERY_TYPE_TIME_ELAPSED,
		eQUERY_TYPE_SAMPLES_PASSED,
		eQUERY_TYPE_ANY_SAMPLES_PASSED,
		eQUERY_TYPE_PRIMITIVES_GENERATED,
		eQUERY_TYPE_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN,
		eQUERY_TYPE_ANY_SAMPLES_PASSED_CONSERVATIVE,
		eQUERY_TYPE_TIMESTAMP,
		CASTOR_ENUM_BOUNDS( eQUERY_TYPE, eQUERY_TYPE_TIME_ELAPSED )
	}	eQUERY_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.8.0
	\date		16/03/2016
	\~english
	\brief		GPU query supported informations enumeration.
	\~french
	\brief		Enumération des informations de requêtes GPU supportés.
	*/
	typedef enum eQUERY_INFO
		: uint32_t
	{
		eQUERY_INFO_RESULT,
		eQUERY_INFO_RESULT_AVAILABLE,
		eQUERY_INFO_RESULT_NO_WAIT,
		CASTOR_ENUM_BOUNDS( eQUERY_INFO, eQUERY_INFO_RESULT )
	}	eQUERY_INFO;

	class WindowHandle;
	class RenderTarget;
	class RenderBuffer;
	class ColourRenderBuffer;
	class DepthStencilRenderBuffer;
	class FrameBufferAttachment;
	class RenderBufferAttachment;
	class TextureAttachment;
	class FrameBuffer;
	class BackBuffers;
	class RenderTechnique;
	class RenderWindow;
	class IWindowHandle;
	class DebugOverlays;
	class Engine;
	class RendererServer;
	class PluginBase;
	class RendererPlugin;
	class ImporterPlugin;
	class DividerPlugin;
	class GenericPlugin;
	class PostFxPlugin;
	class ToneMappingPlugin;
	class FrameEvent;
	class FrameListener;
	class Version;
	class Parameters;
	class BillboardList;
	class RenderLoop;
	class RenderLoopAsync;
	class RenderLoopSync;
	class GpuQuery;

	DECLARE_SMART_PTR( RenderWindow );
	DECLARE_SMART_PTR( RenderTarget );
	DECLARE_SMART_PTR( RenderBuffer );
	DECLARE_SMART_PTR( ColourRenderBuffer );
	DECLARE_SMART_PTR( DepthStencilRenderBuffer );
	DECLARE_SMART_PTR( FrameBufferAttachment );
	DECLARE_SMART_PTR( RenderBufferAttachment );
	DECLARE_SMART_PTR( TextureAttachment );
	DECLARE_SMART_PTR( FrameBuffer );
	DECLARE_SMART_PTR( BackBuffers );
	DECLARE_SMART_PTR( RenderTechnique );
	DECLARE_SMART_PTR( Engine );
	DECLARE_SMART_PTR( RendererServer );
	DECLARE_SMART_PTR( PluginBase );
	DECLARE_SMART_PTR( RendererPlugin );
	DECLARE_SMART_PTR( ImporterPlugin );
	DECLARE_SMART_PTR( DividerPlugin );
	DECLARE_SMART_PTR( GenericPlugin );
	DECLARE_SMART_PTR( PostFxPlugin );
	DECLARE_SMART_PTR( FrameEvent );
	DECLARE_SMART_PTR( FrameListener );
	DECLARE_SMART_PTR( IWindowHandle );
	DECLARE_SMART_PTR( BillboardList );
	DECLARE_SMART_PTR( RenderLoop );
	DECLARE_SMART_PTR( GpuQuery );

	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to get an object type name.
	\~french
	\brief		Structure permettant de récupérer le nom du type d'un objet.
	*/
	template< typename Elem >
	struct CachedObjectNamer;
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Helper structure to enable initialisation if a type supports it.
	\~french
	\brief		Structure permettant d'initialiser les éléments qui le supportent.
	*/
	template< typename Elem, typename Enable = void >
	struct ElementInitialiser;
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Helper structure to enable cleanup if a type supports it.
	\~french
	\brief		Structure permettant de nettoyer les éléments qui le supportent.
	*/
	template< typename Elem, typename Enable = void >
	struct ElementCleaner;
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/07/2016
	\version	0.9.0
	\~english
	\brief		Helper structure to create an element.
	\~french
	\brief		Structure permettant de créer un élément.
	*/
	template< typename Elem, typename Key, typename ... Parameters >
	struct ElementProducer;
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Helper structure to enable moving elements from a cache to another.
	\~french
	\brief		Structure permettant de déplacer les éléments d'un cache à l'autre.
	*/
	template< typename Elem, typename Key, typename Enable = void >
	struct ElementMerger;

	template< typename Elem, typename Key, typename ProducerType >
	class Cache;

	class ListenerCache;
	class MaterialCache;
	class OverlayCache;
	class PluginCache;
	class RenderTechniqueCache;
	class ShaderCache;
	class TargetCache;

	using BlendStateProducer = ElementProducer< BlendState, Castor::String >;
	using DepthStencilStateProducer = ElementProducer< DepthStencilState, Castor::String >;
	using ListenerProducer = ElementProducer< FrameListener, Castor::String >;
	using MaterialProducer = ElementProducer< Material, Castor::String, Engine >;
	using OverlayProducer = ElementProducer< Overlay, Castor::String, eOVERLAY_TYPE, OverlaySPtr, SceneSPtr >;
	using RasteriserStateProducer = ElementProducer< RasteriserState, Castor::String >;
	using RenderTechniqueProducer = ElementProducer< RenderTechnique, Castor::String, Castor::String, RenderTarget, RenderSystem, Parameters >;
	using SamplerProducer = ElementProducer< Sampler, Castor::String >;
	using SceneProducer = ElementProducer< Scene, Castor::String, Engine & >;
	using WindowProducer = ElementProducer< RenderWindow, Castor::String >;

	using BlendStateCache = Cache< BlendState, Castor::String, BlendStateProducer >;
	using DepthStencilStateCache = Cache< DepthStencilState, Castor::String, DepthStencilStateProducer >;
	using RasteriserStateCache = Cache< RasteriserState, Castor::String, RasteriserStateProducer >;
	using SamplerCache = Cache< Sampler, Castor::String, SamplerProducer >;
	using SceneCache = Cache< Scene, Castor::String, SceneProducer >;
	using WindowCache = Cache< RenderWindow, Castor::String, WindowProducer >;

	DECLARE_SMART_PTR( BlendStateCache );
	DECLARE_SMART_PTR( DepthStencilStateCache );
	DECLARE_SMART_PTR( ListenerCache );
	DECLARE_SMART_PTR( MaterialCache );
	DECLARE_SMART_PTR( OverlayCache );
	DECLARE_SMART_PTR( PluginCache );
	DECLARE_SMART_PTR( RasteriserStateCache );
	DECLARE_SMART_PTR( RenderTechniqueCache );
	DECLARE_SMART_PTR( SceneCache );
	DECLARE_SMART_PTR( SamplerCache );
	DECLARE_SMART_PTR( ShaderCache );
	DECLARE_SMART_PTR( TargetCache );
	DECLARE_SMART_PTR( WindowCache );

	/*!
	\author 	Sylvain DOREMUS
	\date 		29/01/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to enable attaching if a type supports it.
	\~french
	\brief		Structure permettant d'attacher les éléments qui le supportent.
	*/
	template< typename Elem, typename Enable = void >
	struct ElementAttacher;
	/*!
	\author 	Sylvain DOREMUS
	\date 		29/01/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to enable detaching if a type supports it.
	\~french
	\brief		Structure permettant de détacher les éléments qui le supportent.
	*/
	template< typename Elem, typename Enable = void >
	struct ElementDetacher;

	template< typename Elem, typename Key, typename ProducerType >
	class ObjectCache;

	class GeometryCache;
	class LightCache;

	using AnimatedObjectGroupProducer = ElementProducer< AnimatedObjectGroup, Castor::String >;
	using BillboardProducer = ElementProducer< BillboardList, Castor::String, Scene, SceneNodeSPtr, RenderSystem >;
	using CameraProducer = ElementProducer< Camera, Castor::String, Scene, SceneNodeSPtr, Viewport >;
	using GeometryProducer = ElementProducer< Geometry, Castor::String, Scene, SceneNodeSPtr, MeshSPtr >;
	using LightProducer = ElementProducer< Light, Castor::String, Scene, SceneNodeSPtr, eLIGHT_TYPE >;
	using MeshProducer = ElementProducer< Mesh, Castor::String, eMESH_TYPE >;
	using SceneNodeProducer = ElementProducer< SceneNode, Castor::String, Scene >;

	using AnimatedObjectGroupCache = Cache< AnimatedObjectGroup, Castor::String, AnimatedObjectGroupProducer >;
	using BillboardCache = ObjectCache< BillboardList, Castor::String, BillboardProducer >;
	using CameraCache = ObjectCache< Camera, Castor::String, CameraProducer >;
	using MeshCache = Cache< Mesh, Castor::String, MeshProducer >;
	using SceneNodeCache  = ObjectCache< SceneNode, Castor::String, SceneNodeProducer >;

	DECLARE_SMART_PTR( AnimatedObjectGroupCache );
	DECLARE_SMART_PTR( BillboardCache );
	DECLARE_SMART_PTR( CameraCache );
	DECLARE_SMART_PTR( GeometryCache );
	DECLARE_SMART_PTR( LightCache );
	DECLARE_SMART_PTR( MeshCache );
	DECLARE_SMART_PTR( SceneNodeCache );

	template< typename ResourceType, typename CacheType, eEVENT_TYPE EventType >
	class CacheView;

	class TechniqueFactory;
	DECLARE_SMART_PTR( TechniqueFactory );

	//! real array
	DECLARE_VECTOR( real, Real );
	//! RenderWindow pointer array
	DECLARE_VECTOR( RenderWindowSPtr, RenderWindowPtr );
	//! RenderBuffer pointer array
	DECLARE_VECTOR( RenderBufferSPtr, RenderBufferPtr );
	//! FrameEvent pointer array
	DECLARE_VECTOR( FrameEventSPtr, FrameEventPtr );
	//! FrameListener pointer map, sorted by name
	DECLARE_MAP( Castor::String, FrameListenerSPtr, FrameListenerPtrStr );
	//! RenderWindow pointer map, sorted by index
	DECLARE_MAP( uint32_t, RenderWindowSPtr, RenderWindow );
	//! Plugin map, sorted by name
	DECLARE_MAP( Castor::String, PluginBaseSPtr, PluginStr );
	DECLARE_MAP( int, Castor::String, StrInt );
	DECLARE_VECTOR( PostEffectSPtr, PostEffectPtr );
	DECLARE_VECTOR( BillboardListSPtr, BillboardList );

	DECLARE_MAP( Castor::String, RendererPluginSPtr, RendererPtr );
	DECLARE_ARRAY( PluginStrMap, ePLUGIN_TYPE_COUNT, PluginStrMap );
	DECLARE_MAP( Castor::Path, Castor::DynamicLibrarySPtr, DynamicLibraryPtrPath );
	DECLARE_ARRAY( DynamicLibraryPtrPathMap, ePLUGIN_TYPE_COUNT, DynamicLibraryPtrPathMap );
	DECLARE_MAP( Castor::Path, ePLUGIN_TYPE, PluginTypePath );
	DECLARE_MAP( Castor::String, BillboardListSPtr, BillboardListStr );

	typedef std::map< Castor::String, RenderWindowSPtr > WindowPtrStrMap;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.8.0
	\date		13/10/2015
	\~english
	\brief		Helper class, telling if the template parameter has a Cleanup(void) method.
	\~french
	\brief		Classe d'aide, dit si le paramètre template possède une méthode Cleanup(void).
	*/
	template< typename T > struct is_cleanable : std::false_type {};
	template<> struct is_cleanable< Material > : std::true_type {};
	template<> struct is_cleanable< Mesh > : std::true_type {};
	template<> struct is_cleanable< RenderWindow > : std::true_type {};
	template<> struct is_cleanable< Scene > : std::true_type {};
	template<> struct is_cleanable< Sampler > : std::true_type {};
	template<> struct is_cleanable< BlendState > : std::true_type {};
	template<> struct is_cleanable< RasteriserState > : std::true_type {};
	template<> struct is_cleanable< DepthStencilState > : std::true_type {};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.8.0
	\date		13/10/2015
	\~english
	\brief		Helper class, telling if the template parameter has an Initialise(void) method.
	\~french
	\brief		Classe d'aide, dit si le paramètre template possède une méthode Initialise(void).
	*/
	template< typename T > struct is_initialisable : std::false_type {};
	template<> struct is_initialisable< Material > : std::true_type {};
	template<> struct is_initialisable< Scene > : std::true_type {};
	template<> struct is_initialisable< Sampler > : std::true_type {};
	template<> struct is_initialisable< BlendState > : std::true_type {};
	template<> struct is_initialisable< RasteriserState > : std::true_type {};
	template<> struct is_initialisable< DepthStencilState > : std::true_type {};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.8.0
	\date		13/10/2015
	\~english
	\brief		Helper class, telling if the template parameter Initialise() and Cleanup() methods must be called instantly.
	\~french
	\brief		Classe d'aide, dit si les méthodes Initialise() et Cleanup() du paramètre template doivent s'exécuter immédiatement.
	*/
	template< typename T > struct is_instant : std::false_type {};
	template<> struct is_instant< Scene > : std::true_type {};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.8.0
	\date		29/01/2016
	\~english
	\brief		Helper class, telling if the template parameter has the method Detach.
	\~french
	\brief		Classe d'aide, dit si le paramètre template possède la méthode Detach.
	*/
	template< typename T > struct is_detachable : std::false_type {};
	template<> struct is_detachable< MovableObject > : std::true_type {};
	template<> struct is_detachable< Camera > : std::true_type {};
	template<> struct is_detachable< Geometry > : std::true_type {};
	template<> struct is_detachable< Light > : std::true_type {};
	template<> struct is_detachable< BillboardList > : std::true_type {};
	template<> struct is_detachable< SceneNode > : std::true_type {};

	//@}
}

namespace Castor
{
	DECLARED_EXPORTED_OWNED_BY( C3D_API, Castor3D::Engine, Engine );
	DECLARED_EXPORTED_OWNED_BY( C3D_API, Castor3D::RenderSystem, RenderSystem );
	DECLARED_EXPORTED_OWNED_BY( C3D_API, Castor3D::Scene, Scene );
}

#endif
