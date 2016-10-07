/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
#include <Design/Collection.hpp>
#include <Design/OwnedBy.hpp>
#include <Math/Point.hpp>
#include <Graphics/Size.hpp>
#include <Math/Quaternion.hpp>
#include <Math/SquareMatrix.hpp>
#include <Miscellaneous/StringUtils.hpp>

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
	Castor::String const RENDERER_TYPE_UNDEFINED = cuT( "Undefined" );

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
	enum class MovableType
		: uint8_t
	{
		Camera,
		Geometry,
		Light,
		Billboard,
		CASTOR_ENUM_CLASS_BOUNDS( Camera )
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		RenderTarget supported types
	\~french
	\brief		Types de RenderTarget supportés
	*/
	enum class TargetType
		: uint8_t
	{
		Window,
		Texture,
		CASTOR_ENUM_CLASS_BOUNDS( Window )
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Plugin types enumeration
	\~french
	\brief		Enumération des types de plug-ins
	*/
	enum class PluginType
		: uint8_t
	{
		//!\~english	Renderer plug-in.
		//!\~french		Plug-in de rendu.
		Renderer,
		//!\~english	Importer plug-in.
		//!\~french		Plug-in d'importation.
		Importer,
		//!\~english	Mesh Divider plug-in.
		//!\~french		Plug-in de subdivision de maillage.
		Divider,
		//!\~english	Procedural generator.
		//!\~french		Plug-in de génération procédurale.
		Generator,
		//!\~english	Render technique plug-in.
		//!\~french		Plug-in de technique de rendu.
		Technique,
		//!\~english	Tone mapping plug-in.
		//!\~french		Plug-in d'effet de mappage de tons.
		ToneMapping,
		//!\~english	Post effect plug-in.
		//!\~french		Plug-in d'effet post-rendu.
		PostEffect,
		//!\~english	Generic plug-in.
		//!\~french		Plug-in générique.
		Generic,
		CASTOR_ENUM_CLASS_BOUNDS( Renderer )
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Frame buffer components enumeration
	\~french
	\brief		Enumération des composantes de tampon d'image
	*/
	enum class BufferComponent
		: uint8_t
	{
		None = 0,
		Colour = 1 << 0,
		Depth = 1 << 1,
		Stencil = 1 << 2,
	};
	/**
	 *\~english
	 *\brief		Bitwise OR on BufferComponent.
	 *\param[in]	p_lhs, p_rhs	The operands.
	 *\~french
	 *\brief		OU binaire sur des BufferComponent.
	 *\param[in]	p_lhs, p_rhs	Les opérandes.
	 */
	inline uint8_t operator|( BufferComponent p_lhs, BufferComponent p_rhs )
	{
		return uint8_t( p_lhs ) | uint8_t( p_rhs );
	}
	/**
	 *\~english
	 *\brief		Bitwise AND on BufferComponent.
	 *\param[in]	p_lhs, p_rhs	The operands.
	 *\~french
	 *\brief		ET binaire sur des BufferComponent.
	 *\param[in]	p_lhs, p_rhs	Les opérandes.
	 */
	inline uint8_t operator&( BufferComponent p_lhs, BufferComponent p_rhs )
	{
		return uint8_t( p_lhs ) & uint8_t( p_rhs );
	}
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Frame buffer components enumeration
	\~french
	\brief		Enumération des composantes de tampon d'image
	*/
	enum class WindowBuffer
		: uint8_t
	{
		None,
		FrontLeft,
		FrontRight,
		BackLeft,
		BackRight,
		Front,
		Back,
		Left,
		Right,
		FrontAndBack,
		CASTOR_ENUM_CLASS_BOUNDS( None )
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Frame buffer binding targets enumeration
	\~french
	\brief		Enumération des cibles d'activation de tampon d'image
	*/
	enum class FrameBufferTarget
		: uint8_t
	{
		//!\~english	Frame buffer is bound as a target for draws.
		//!\~french		Le tampon d'image est activé en tant que cible pour les rendus.
		Draw,
		//!\~english	Frame buffer is bound as a target for reads.
		//!\~french		Le tampon d'image est activé en tant que cible pour les lectures.
		Read,
		//!\~english	Frame buffer is bound as a target for reads and draws.
		//!\~french		Le tampon d'image est activé en tant que cible pour les lectures et les rendus.
		Both,
		CASTOR_ENUM_CLASS_BOUNDS( Draw )
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Frame buffer binding modes enumeration
	\~french
	\brief		Enumération des modes d'activation de tampon d'image
	*/
	enum class FrameBufferMode
		: uint8_t
	{
		//!\~english	Frame buffer is bound for configuration.
		//!\~french		Le tampon d'image est activé pour configuration.
		Config,
		//!\~english	Frame buffer is bound and FrameBuffer::SetDrawBuffers is called automatically.
		//!\~french		Le tampon d'image est activé et FrameBuffer::SetDrawBuffers est appelée automatiquement.
		Automatic,
		//!\~english	Frame buffer is bound and user must call FrameBuffer::SetDrawBuffers if he wants.
		//!\~french		Le tampon d'image est activé et l'utilisateur doit appeler FrameBuffer::SetDrawBuffers s'il veut.
		Manual,
		CASTOR_ENUM_CLASS_BOUNDS( Config )
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Frame buffer attachment points enumeration
	\~french
	\brief		Enumération des points d'attache pour un tampon d'image
	*/
	enum class AttachmentPoint
		: uint8_t
	{
		None,
		Colour,
		Depth,
		Stencil,
		CASTOR_ENUM_CLASS_BOUNDS( None )
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0.0
	\date		11/10/2015
	\~english
	\brief		Frame buffer attachment types enumeration.
	\~french
	\brief		Enumération des types d'attache pour un tampon d'image.
	*/
	enum class AttachmentType
		: uint8_t
	{
		Texture,
		Buffer,
		CASTOR_ENUM_CLASS_BOUNDS( Texture )
	};
	/*!
	\author		Sylvain DOREMUS
	\~english
	\brief		The  viewport projection types enumeration
	\~french
	\brief		Enumération des types de projection de viewport
	*/
	enum class ViewportType
		: uint8_t
	{
		Ortho,
		Perspective,
		Frustum,
		CASTOR_ENUM_CLASS_BOUNDS( Ortho )
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		10/06/2013
	\version	0.7.0
	\~english
	\brief		Frustum view planes enumeration
	\~french
	\brief		Enumération des plans du frustum de vue
	*/
	enum class FrustumPlane
	{
		//!\~english	Near plane.
		//!\~french		Plan éloigné.
		Near,
		//!\~english	Far plane.
		//!\~french		Plan proche.
		Far,
		//!\~english	Left plane.
		//!\~french		Plan gauche.
		Left,
		//!\~english	Right plane.
		//!\~french		Plan droit.
		Right,
		//!\~english	Top plane.
		//!\~french		Plan haut.
		Top,
		//!\~english	Bottom plane.
		//!\~french		Plan bas.
		Bottom,
		CASTOR_ENUM_CLASS_BOUNDS( Near )
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Frame Event Type enumeration
	\~french
	\brief		Enumération des types d'évènement de frame
	*/
	enum class EventType
		: uint8_t
	{
		//!\~english	This kind of event happens before any render, device context is active (so be fast !!).
		//!\~french		Ce type d'évènement est traité avant le rendu, le contexte de rendu est actif (donc soyez rapide !!)
		PreRender,
		//!\~english	This kind of event happens after the render, before buffers' swap.
		//!\~french		Ce type d'évènement est traité après le rendu, avant l'échange des tampons.
		QueueRender,
		//!\~english	This kind of event happens after the buffer' swap.
		//!\~french		Ce type d'évènement est traité après l'échange des tampons.
		PostRender,
		CASTOR_ENUM_CLASS_BOUNDS( PreRender )
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date		23/05/2013
	\~english
	\brief		Culled faces enumeration.
	\~french
	\brief		Enumération des faces supprimées.
	*/
	enum class Culling
		: uint8_t
	{
		//!\~english	No face culled.
		//!\~french		Aucune face supprimée.
		None,
		//!\~english	Front faces are culled.
		//!\~french		Faces avant supprimées.
		Front = 1 << 0,
		//!\~english	Back face are culled.
		//!\~french		Faces arrière supprimées.
		Back,
		//!\~english	Back and front faces are culled.
		//!\~french		Faces avant et arrière supprimées.
		FrontAndBack,
		CASTOR_ENUM_CLASS_BOUNDS( None )
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date		23/05/2013
	\~english
	\brief		Polygon rasterisation modes enumeration
	\~french
	\brief		Enumération des mode de rastérisation des polygones
	*/
	enum class FillMode
		: uint8_t
	{
		//!\~english	Polygon vertices that are marked as the start of a boundary edge are drawn as points.
		//!\~french		Les vertices marquant les arêtes sont dessinés en tant que points.
		Point,
		//!\~english	Boundary edges of the polygon are drawn as line segments.
		//!\~french		Les arêtes du polygone sont dessinées en tant que segments.
		Line,
		//!\~english	The interior of the polygon is filled.
		//!\~french		L'intérieur du polygone est rempli.
		Solid,
		CASTOR_ENUM_CLASS_BOUNDS( Point )
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.8.0
	\date		16/03/2016
	\~english
	\brief		GPU query supported types enumeration.
	\~french
	\brief		Enumération des types de requêtes GPU supportés.
	*/
	enum class QueryType
		: uint32_t
	{
		TimeElapsed,
		SamplesPassed,
		AnySamplesPassed,
		PrimitivesGenerated,
		TransformFeedbackPrimitivesWritten,
		AnySamplesPassedConservative,
		Timestamp,
		CASTOR_ENUM_CLASS_BOUNDS( TimeElapsed )
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.8.0
	\date		16/03/2016
	\~english
	\brief		GPU query supported informations enumeration.
	\~french
	\brief		Enumération des informations de requêtes GPU supportés.
	*/
	enum class QueryInfo
		: uint32_t
	{
		Result,
		ResultAvailable,
		ResultNoWait,
		CASTOR_ENUM_CLASS_BOUNDS( Result )
	};

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
	class Plugin;
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
	class PickingPass;

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
	DECLARE_SMART_PTR( Plugin );
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
	DECLARE_SMART_PTR( PickingPass );

	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to specialise a cache behaviour.
	*\remarks	Must hold:
				<ul>
				<li>Name: The element type name.</li>
				<li>Producer: The element creation function prototype.</li>
				<li>Merger: The prototype of the function use to merge a cache element into another cache.</li>
				</ul>
	\~french
	\brief		Structure permettant de spécialiser le comportement d'un cache.
	*\remarks	doit contenir:
				<ul>
				<li>Name: Le nom du type d'élément.</li>
				<li>Producer: Le prototype de la fonction de création d'un élément.</li>
				<li>Merger: Le prototype de la fonction pour fusionner un élément d'un cache dans un autre cache.</li>
				</ul>
	*/
	template< typename ElementType, typename KeyType >
	struct CacheTraits;

	template< typename ElementType, typename KeyType >
	class CacheBase;

	template< typename ElementType, typename KeyType >
	class Cache;

	template< typename ElementType, typename KeyType >
	struct ElementProducer;

	template< typename ElementType >
	using ElementInitialiser = std::function< void( std::shared_ptr< ElementType > ) >;

	template< typename ElementType >
	using ElementCleaner = std::function< void( std::shared_ptr< ElementType > ) >;

	template< typename ElementType, typename KeyType >
	using ElementMerger = std::function< void( CacheBase< ElementType, KeyType > const &, Castor::Collection< ElementType, KeyType > &, std::shared_ptr< ElementType > ) >;

	class ShaderProgramCache;
	class RenderTargetCache;

	using ListenerCache = Cache< FrameListener, Castor::String >;
	using MeshCache = Cache< Mesh, Castor::String >;
	using SamplerCache = Cache< Sampler, Castor::String >;
	using SceneCache = Cache< Scene, Castor::String >;
	using RenderTechniqueCache = Cache< RenderTechnique, Castor::String >;
	using RenderWindowCache = Cache< RenderWindow, Castor::String >;

	DECLARE_SMART_PTR( ListenerCache );
	DECLARE_SMART_PTR( MeshCache );
	DECLARE_SMART_PTR( SceneCache );
	DECLARE_SMART_PTR( SamplerCache );
	DECLARE_SMART_PTR( ShaderProgramCache );
	DECLARE_SMART_PTR( RenderTargetCache );
	DECLARE_SMART_PTR( RenderTechniqueCache );
	DECLARE_SMART_PTR( RenderWindowCache );

	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to specialise a cache behaviour.
	*\remarks	Must hold:
				<ul>
				<li>Name: The element type name.</li>
				<li>Producer: The element creation function prototype.</li>
				<li>Merger: The prototype of the function use to merge a cache element into another cache.</li>
				</ul>
	\~french
	\brief		Structure permettant de spécialiser le comportement d'un cache.
	*\remarks	doit contenir:
				<ul>
				<li>Name: Le nom du type d'élément.</li>
				<li>Producer: Le prototype de la fonction de création d'un élément.</li>
				<li>Merger: Le prototype de la fonction pour fusionner un élément d'un cache dans un autre cache.</li>
				</ul>
	*/
	template< typename ElementType, typename KeyType >
	struct ObjectCacheTraits;

	template< typename ElementType >
	using ElementAttacher = std::function< void( std::shared_ptr< ElementType >, SceneNodeSPtr, SceneNodeSPtr, SceneNodeSPtr, SceneNodeSPtr ) >;

	template< typename ElementType >
	using ElementDetacher = std::function< void( std::shared_ptr< ElementType > ) >;

	template< typename ElementType, typename KeyType >
	class ObjectCacheBase;

	template< typename ElementType, typename KeyType >
	class ObjectCache;

	using AnimatedObjectGroupCache = Cache< AnimatedObjectGroup, Castor::String >;
	using BillboardListCache = ObjectCache< BillboardList, Castor::String >;
	using CameraCache = ObjectCache< Camera, Castor::String >;

	DECLARE_SMART_PTR( AnimatedObjectGroupCache );
	DECLARE_SMART_PTR( BillboardListCache );
	DECLARE_SMART_PTR( CameraCache );

	template< typename ResourceType, typename CacheType, EventType EventType >
	class CacheView;

	//! real array
	DECLARE_VECTOR( real, Real );
	//! RenderWindow pointer array
	DECLARE_VECTOR( RenderWindowSPtr, RenderWindowPtr );
	//! RenderBuffer pointer array
	DECLARE_VECTOR( RenderBufferSPtr, RenderBufferPtr );
	//! FrameEvent pointer array
	DECLARE_VECTOR( FrameEventUPtr, FrameEventPtr );
	//! FrameListener pointer map, sorted by name
	DECLARE_MAP( Castor::String, FrameListenerSPtr, FrameListenerPtrStr );
	//! RenderWindow pointer map, sorted by index
	DECLARE_MAP( uint32_t, RenderWindowSPtr, RenderWindow );
	//! Plugin map, sorted by name
	DECLARE_MAP( Castor::String, PluginSPtr, PluginStr );
	DECLARE_MAP( int, Castor::String, StrInt );
	DECLARE_VECTOR( PostEffectSPtr, PostEffectPtr );
	DECLARE_VECTOR( BillboardListSPtr, BillboardList );

	DECLARE_MAP( Castor::String, RendererPluginSPtr, RendererPtr );
	DECLARE_ARRAY( PluginStrMap, size_t( PluginType::Count ), PluginStrMap );
	DECLARE_MAP( Castor::Path, Castor::DynamicLibrarySPtr, DynamicLibraryPtrPath );
	DECLARE_ARRAY( DynamicLibraryPtrPathMap, size_t( PluginType::Count ), DynamicLibraryPtrPathMap );
	DECLARE_MAP( Castor::Path, PluginType, PluginTypePath );
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
	template<> struct is_detachable< SceneNode >: std::true_type
	{
	};

	//@}

#define MAKE_CACHE_NAME( className )\
	Cache< className, Castor::String >

#define DECLARE_CACHE_MEMBER( memberName, className )\
	public:\
		inline MAKE_CACHE_NAME( className ) & Get##className##Cache()\
		{\
			return *m_##memberName##Cache;\
		}\
		inline MAKE_CACHE_NAME( className ) const & Get##className##Cache()const\
		{\
			return *m_##memberName##Cache;\
		}\
	private:\
		std::unique_ptr< MAKE_CACHE_NAME( className ) > m_##memberName##Cache

#define DECLARE_NAMED_CACHE_MEMBER( memberName, className )\
	public:\
		inline className##Cache & Get##className##Cache()\
		{\
			return *m_##memberName##Cache;\
		}\
		inline className##Cache const & Get##className##Cache()const\
		{\
			return *m_##memberName##Cache;\
		}\
	private:\
		std::unique_ptr< className##Cache > m_##memberName##Cache

#define MAKE_OBJECT_CACHE_NAME( className )\
	ObjectCache< className, Castor::String >

#define DECLARE_OBJECT_CACHE_MEMBER( memberName, className )\
	public:\
		inline MAKE_OBJECT_CACHE_NAME( className ) & Get##className##Cache()\
		{\
			return *m_##memberName##Cache;\
		}\
		inline MAKE_OBJECT_CACHE_NAME( className ) const & Get##className##Cache()const\
		{\
			return *m_##memberName##Cache;\
		}\
	private:\
		uint32_t m_on##className##Changed;\
		std::unique_ptr< MAKE_OBJECT_CACHE_NAME( className ) > m_##memberName##Cache

#define DECLARE_CACHE_VIEW_MEMBER( memberName, className, eventType )\
	public:\
		inline CacheView< className, MAKE_CACHE_NAME( className ), eventType > & Get##className##View()\
		{\
			return *m_##memberName##CacheView;\
		}\
		inline CacheView< className, MAKE_CACHE_NAME( className ), eventType > const & Get##className##View()const\
		{\
			return *m_##memberName##CacheView;\
		}\
	private:\
		std::unique_ptr< CacheView< className, MAKE_CACHE_NAME( className ), eventType > > m_##memberName##CacheView

#define DECLARE_CACHE_VIEW_MEMBER_CU( memberName, className, eventType )\
	public:\
		inline CacheView< Castor::className, Castor::className##Cache, eventType > & Get##className##View()\
		{\
			return *m_##memberName##CacheView;\
		}\
		inline CacheView< Castor::className, Castor::className##Cache, eventType > const & Get##className##View()const\
		{\
			return *m_##memberName##CacheView;\
		}\
	private:\
		std::unique_ptr< CacheView< Castor::className, Castor::className##Cache, eventType > > m_##memberName##CacheView

#define DECLARE_CACHE_VIEW_MEMBER_EX( memberName, mgrName, className, eventType )\
	public:\
		inline CacheView< MAKE_CACHE_NAME( className ), mgrName##Cache, eventType > & Get##className##View()\
		{\
			return *m_##memberName##CacheView;\
		}\
		inline CacheView< MAKE_CACHE_NAME( className ), mgrName##Cache, eventType > const & Get##className##View()const\
		{\
			return *m_##memberName##CacheView;\
		}\
	private:\
		std::unique_ptr< CacheView< MAKE_CACHE_NAME( className ), mgrName##Cache, eventType > > m_##memberName##CacheView

	C3D_API void ComputePreLightingMapContributions( GLSL::GlslWriter & p_writer
													 , GLSL::Vec3 & p_normal
													 , GLSL::Float & p_shininess
													 , uint16_t p_textureFlags
													 , uint8_t p_programFlags
													 , uint8_t p_sceneFlags );

	C3D_API void ComputePostLightingMapContributions( GLSL::GlslWriter & p_writer
													  , GLSL::Vec3 & p_ambient
													  , GLSL::Vec3 & p_diffuse
													  , GLSL::Vec3 & p_specular
													  , GLSL::Vec3 & p_emissive
													  , uint16_t p_textureFlags
													  , uint8_t p_programFlags
													  , uint8_t p_sceneFlags );
}

DECLARED_EXPORTED_OWNED_BY( C3D_API, Castor3D::Engine, Engine )
DECLARED_EXPORTED_OWNED_BY( C3D_API, Castor3D::RenderSystem, RenderSystem )
DECLARED_EXPORTED_OWNED_BY( C3D_API, Castor3D::Scene, Scene )

#endif
