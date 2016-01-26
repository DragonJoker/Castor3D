/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#include <SquareMatrix.hpp>
#include <StringUtils.hpp>

namespace Castor3D
{
	using Castor::real;
}

#include "Castor3DPrerequisites_Animation.hpp"
#include "Castor3DPrerequisites_Camera.hpp"
#include "Castor3DPrerequisites_Geometry.hpp"
#include "Castor3DPrerequisites_Light.hpp"
#include "Castor3DPrerequisites_Material.hpp"
#include "Castor3DPrerequisites_Overlay.hpp"
#include "Castor3DPrerequisites_Render.hpp"
#include "Castor3DPrerequisites_Scene.hpp"
#include "Castor3DPrerequisites_Shader.hpp"

namespace Castor3D
{
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
	CASTOR_TYPE( uint8_t )
	{
		eMOVABLE_TYPE_CAMERA,
		eMOVABLE_TYPE_GEOMETRY,
		eMOVABLE_TYPE_LIGHT,
		eMOVABLE_TYPE_BILLBOARD,
		eMOVABLE_TYPE_COUNT,
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
	CASTOR_TYPE( uint8_t )
	{
		eTARGET_TYPE_WINDOW,
		eTARGET_TYPE_TEXTURE,
		eTARGET_TYPE_COUNT,
	}	eTARGET_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Plugin types enumeration
	\~french
	\brief		Enumération des types de plugins
	*/
	typedef enum ePLUGIN_TYPE
	CASTOR_TYPE( uint8_t )
	{
		ePLUGIN_TYPE_RENDERER,	//!< Renderer plugin (OpenGl2, 3 or Direct3D)
		ePLUGIN_TYPE_IMPORTER,	//!< Importer plugin
		ePLUGIN_TYPE_DIVIDER,	//!< Mesh Divider plugin
		ePLUGIN_TYPE_GENERATOR,	//!< Procedural generator
		ePLUGIN_TYPE_TECHNIQUE,	//!< Render technique plugin
		ePLUGIN_TYPE_POSTFX,	//!< Post effect plugin
		ePLUGIN_TYPE_GENERIC,	//!< Generic plugin
		ePLUGIN_TYPE_COUNT,
	}	ePLUGIN_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		01/04/2011
	\~english
	\brief		Supported renderers enumeration
	\~french
	\brief		Enumération des renderers supportés
	*/
	typedef enum eRENDERER_TYPE
	CASTOR_TYPE( int8_t )
	{
		eRENDERER_TYPE_UNDEFINED = -1,
		eRENDERER_TYPE_OPENGL = 0,		//!< OpenGl Renderer
		eRENDERER_TYPE_COUNT = 2,
	}	eRENDERER_TYPE;
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
	CASTOR_TYPE( uint8_t )
	{
		eBUFFER_COMPONENT_COLOUR = 1,
		eBUFFER_COMPONENT_DEPTH = 2,
		eBUFFER_COMPONENT_STENCIL = 4,
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
	CASTOR_TYPE( uint8_t )
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
		eBUFFER_COUNT,
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
	CASTOR_TYPE( uint8_t )
	{
		eFRAMEBUFFER_TARGET_DRAW,	//!<\~english Frame buffer is bound as a target for draws	\~french Le tampon d'image est activé en tant que cible pour les rendus
		eFRAMEBUFFER_TARGET_READ,	//!<\~english Frame buffer is bound as a target for reads	\~french Le tampon d'image est activé en tant que cible pour les lectures
		eFRAMEBUFFER_TARGET_BOTH,	//!<\~english Frame buffer is bound as a target for reads and draws	\~french Le tampon d'image est activé en tant que cible pour les lectures et les rendus
		eFRAMEBUFFER_TARGET_COUNT,
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
	CASTOR_TYPE( uint8_t )
	{
		eFRAMEBUFFER_MODE_CONFIG,		//!<\~english Frame buffer is bound for configuration	\~french Le tampon d'image est activé pour configuration
		eFRAMEBUFFER_MODE_AUTOMATIC,	//!<\~english Frame buffer is bound and FrameBuffer::SetDrawBuffers is called automatically	\~french Le tampon d'image est activé et FrameBuffer::SetDrawBuffers est appelée automatiquement
		eFRAMEBUFFER_MODE_MANUAL,		//!<\~english Frame buffer is bound and user must call FrameBuffer::SetDrawBuffers if he wants	\~french Le tampon d'image est activé et l'utilisateur doit appeler FrameBuffer::SetDrawBuffers s'il veut
		eFRAMEBUFFER_MODE_COUNT,
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
	CASTOR_TYPE( uint8_t )
	{
		eATTACHMENT_POINT_NONE,
		eATTACHMENT_POINT_COLOUR,
		eATTACHMENT_POINT_DEPTH,
		eATTACHMENT_POINT_STENCIL,
		eATTACHMENT_POINT_COUNT,
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
	CASTOR_TYPE( uint8_t )
	{
		eATTACHMENT_TYPE_TEXTURE,
		eATTACHMENT_TYPE_BUFFER,
		eATTACHMENT_TYPE_COUNT,
	}	eATTACHMENT_TYPE;
	/*!
	\author		Sylvain DOREMUS
	\~english
	\brief		Projection Directions enumeration
	\~french
	\brief		Enumération des directions de projection
	*/
	typedef enum ePROJECTION_DIRECTION
	CASTOR_TYPE( uint8_t )
	{
		ePROJECTION_DIRECTION_FRONT,	//!< camera looks at the scene from the front
		ePROJECTION_DIRECTION_BACK,		//!< camera looks at the scene from the back
		ePROJECTION_DIRECTION_LEFT,		//!< camera looks at the scene from the left
		ePROJECTION_DIRECTION_RIGHT,	//!< camera looks at the scene from the right
		ePROJECTION_DIRECTION_TOP,		//!< camera looks at the scene from the top
		ePROJECTION_DIRECTION_BOTTOM,	//!< camera looks at the scene from the bottom
		ePROJECTION_DIRECTION_COUNT,
	}	ePROJECTION_DIRECTION;
	/*!
	\author		Sylvain DOREMUS
	\~english
	\brief		The  viewport projection types enumeration
	\~french
	\brief		Enumération des types de projection de viewport
	*/
	typedef enum eVIEWPORT_TYPE
	CASTOR_TYPE( uint8_t )
	{
		eVIEWPORT_TYPE_ORTHO,
		eVIEWPORT_TYPE_PERSPECTIVE,
		eVIEWPORT_TYPE_FRUSTUM,
		eVIEWPORT_TYPE_COUNT,
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
	CASTOR_TYPE( uint8_t )
	{
		eFRUSTUM_PLANE_NEAR,	//!<\~english Near plane	\~french Plan lointain
		eFRUSTUM_PLANE_FAR,		//!<\~english Far plane	\~french Plan proche
		eFRUSTUM_PLANE_LEFT,	//!<\~english Left plane	\~french Plan gauche
		eFRUSTUM_PLANE_RIGHT,	//!<\~english Right plane	\~french Plan droit
		eFRUSTUM_PLANE_TOP,		//!<\~english Top plane	\~french Plan haut
		eFRUSTUM_PLANE_BOTTOM,	//!<\~english Bottom plane	\~french Plan bas
		eFRUSTUM_PLANE_COUNT,	//!<\~english Planes count	\~french Compte des plans
	}	eFRUSTUM_PLANE;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Frame Event Type enumeration
	\~french
	\brief		Enumération des types d'évènement de frame
	*/
	typedef enum eEVENT_TYPE
	CASTOR_TYPE( uint8_t )
	{
		eEVENT_TYPE_PRE_RENDER,		//!<\~english This kind of event happens before any render, device context is active (so be fast !!)	\~french Ce type d'évènement est traité avant le rendu, le contexte de rendu est actif (donc soyez rapide !!)
		eEVENT_TYPE_POST_RENDER,	//!<\~english This kind of event happens after the buffer' swap	\~french Ce type d'évènement est traité après l'échange des tampons
		eEVENT_TYPE_COUNT,
	}	eEVENT_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date		29/11/2012
	\~english
	\brief		Tweakable options enumeration
	\~french
	\brief		Enumération des options réglables dans le contexte de dessin
	*/
	typedef enum eTWEAK
	CASTOR_TYPE( uint8_t )
	{
		eTWEAK_DEPTH_TEST,			//!<\~english Depth test	\~french Test de profondeur
		eTWEAK_STENCIL_TEST,		//!<\~english Stencil test	\~french Test de découpe
		eTWEAK_BLEND,				//!<\~english Colour and alpha blend	\~french Mélange des couleurs et de l'alpha
		eTWEAK_LIGHTING,			//!<\~english Lighting	\~french Eclairage
		eTWEAK_ALPHA_TEST,			//!<\~english Alpha testing	\~french Test d'alpha
		eTWEAK_CULLING,				//!<\~english Polygons culling	\~french Sélection des faces à dessiner
		eTWEAK_DITHERING,			//!<\~english Dithering	\~french Tramage
		eTWEAK_FOG,					//!<\~english Fog	\~french Brouillard
		eTWEAK_DEPTH_WRITE,			//!<\~english Depth write	\~french Ecriture dans le tampon de profondeur
		eTWEAK_ALPHA_TO_COVERAGE,	//!<\~english Alpha to coverage	\~french Alpha to coverage
		eTWEAK_COUNT,
	}	eTWEAK;
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
	CASTOR_TYPE( uint8_t )
	{
		eFACE_NONE,				//!<\~english No face	\~french Aucune face
		eFACE_FRONT,			//!<\~english Front face	\~french Face avant
		eFACE_BACK,				//!<\~english Back face	\~french Face arrière
		eFACE_FRONT_AND_BACK,	//!<\~english Back and front faces	\~french Faces avant et arrière
		eFACE_COUNT,			//!<\~english A count	\~french Un compte
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
	CASTOR_TYPE( uint8_t )
	{
		eFILL_MODE_POINT,	//!<\~english Polygon vertices that are marked as the start of a boundary edge are drawn as points	\~french Les vertices marquant les arêtes sont dessinés en tant que points
		eFILL_MODE_LINE,	//!<\~english Boundary edges of the polygon are drawn as line segments	\~french Les arêtes du polygone sont dessinées en tant que segments
		eFILL_MODE_SOLID,	//!<\~english The interior of the polygon is filled	\~french L'intérieur du polygone est rempli
		eFILL_MODE_COUNT,	//!<\~english Fill modes count	\~french Compte des modes de rastérisation
	}   eFILL_MODE;

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
	class RenderTechniqueBase;
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
	class FrameEvent;
	class FrameListener;
	class Version;
	class PostEffect;
	class Parameters;
	class BillboardList;
	class RenderLoop;
	class RenderLoopAsync;
	class RenderLoopSync;

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
	DECLARE_SMART_PTR( RenderTechniqueBase );
	DECLARE_SMART_PTR( PostEffect );
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

	class OverlayManager;
	class ShaderManager;
	class MaterialManager;
	class WindowManager;
	class MeshManager;
	class PluginManager;
	class SceneManager;
	class SamplerManager;
	class DepthStencilStateManager;
	class RasteriserStateManager;
	class BlendStateManager;
	class AnimationManager;
	class TargetManager;
	class ListenerManager;

	DECLARE_SMART_PTR( OverlayManager );
	DECLARE_SMART_PTR( ShaderManager );
	DECLARE_SMART_PTR( MaterialManager );
	DECLARE_SMART_PTR( WindowManager );
	DECLARE_SMART_PTR( MeshManager );
	DECLARE_SMART_PTR( PluginManager );
	DECLARE_SMART_PTR( SceneManager );
	DECLARE_SMART_PTR( SamplerManager );
	DECLARE_SMART_PTR( DepthStencilStateManager );
	DECLARE_SMART_PTR( RasteriserStateManager );
	DECLARE_SMART_PTR( BlendStateManager );
	DECLARE_SMART_PTR( AnimationManager );
	DECLARE_SMART_PTR( TargetManager );
	DECLARE_SMART_PTR( ListenerManager );

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

	DECLARE_ARRAY( RendererPluginSPtr, eRENDERER_TYPE_COUNT, RendererPtr );
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
	template<> struct is_cleanable< Overlay > : std::true_type {};
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
	template<> struct is_initialisable< Overlay > : std::true_type {};
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

	//@}
}

namespace Castor
{
	DECLARED_EXPORTED_OWNED_BY( C3D_API, Castor3D::Engine );
	DECLARED_EXPORTED_OWNED_BY( C3D_API, Castor3D::RenderSystem );
}

#endif
