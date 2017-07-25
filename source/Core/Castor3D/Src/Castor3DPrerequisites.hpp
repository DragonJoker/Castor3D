﻿/*
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
#include <GlslWriterPrerequisites.hpp>

#if !defined( CASTOR_PLATFORM_WINDOWS )
#	define C3D_API
#else
#	ifdef MemoryBarrier
#		undef MemoryBarrier
#	endif
#	if defined( Castor3D_EXPORTS )
#		define C3D_API __declspec( dllexport )
#	else
#		define C3D_API __declspec( dllimport )
#	endif
#endif

#include <Design/Collection.hpp>
#include <Design/FlagCombination.hpp>
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

#include "Prerequisites/Castor3DPrerequisites_Animation.hpp"
#include "Prerequisites/Castor3DPrerequisites_Camera.hpp"
#include "Prerequisites/Castor3DPrerequisites_Geometry.hpp"
#include "Prerequisites/Castor3DPrerequisites_Light.hpp"
#include "Prerequisites/Castor3DPrerequisites_Material.hpp"
#include "Prerequisites/Castor3DPrerequisites_Overlay.hpp"
#include "Prerequisites/Castor3DPrerequisites_Scene.hpp"
#include "Prerequisites/Castor3DPrerequisites_Shader.hpp"
#include "Prerequisites/Castor3DPrerequisites_Render.hpp"
#include "Prerequisites/Castor3DPrerequisites_Event.hpp"

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
		eCamera,
		eGeometry,
		eLight,
		eBillboard,
		eParticleEmitter,
		CASTOR_SCOPED_ENUM_BOUNDS( eCamera )
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
		eWindow,
		eTexture,
		CASTOR_SCOPED_ENUM_BOUNDS( eWindow )
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
		eRenderer,
		//!\~english	Importer plug-in.
		//!\~french		Plug-in d'importation.
		eImporter,
		//!\~english	Mesh Divider plug-in.
		//!\~french		Plug-in de subdivision de maillage.
		eDivider,
		//!\~english	Procedural generator.
		//!\~french		Plug-in de génération procédurale.
		eGenerator,
		//!\~english	Render technique plug-in.
		//!\~french		Plug-in de technique de rendu.
		eTechnique,
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
		CASTOR_SCOPED_ENUM_BOUNDS( eRenderer )
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
		eNone = 0,
		eColour = 1 << 0,
		eDepth = 1 << 1,
		eStencil = 1 << 2,
	};
	IMPLEMENT_FLAGS( BufferComponent )
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
		eNone,
		eFrontLeft,
		eFrontRight,
		eBackLeft,
		eBackRight,
		eFront,
		eBack,
		eLeft,
		eRight,
		eFrontAndBack,
		CASTOR_SCOPED_ENUM_BOUNDS( eNone )
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
		eDraw,
		//!\~english	Frame buffer is bound as a target for reads.
		//!\~french		Le tampon d'image est activé en tant que cible pour les lectures.
		eRead,
		//!\~english	Frame buffer is bound as a target for reads and draws.
		//!\~french		Le tampon d'image est activé en tant que cible pour les lectures et les rendus.
		eBoth,
		CASTOR_SCOPED_ENUM_BOUNDS( eDraw )
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
		eConfig,
		//!\~english	Frame buffer is bound and FrameBuffer::SetDrawBuffers is called automatically.
		//!\~french		Le tampon d'image est activé et FrameBuffer::SetDrawBuffers est appelée automatiquement.
		eAutomatic,
		//!\~english	Frame buffer is bound and user must call FrameBuffer::SetDrawBuffers if he wants.
		//!\~french		Le tampon d'image est activé et l'utilisateur doit appeler FrameBuffer::SetDrawBuffers s'il veut.
		eManual,
		CASTOR_SCOPED_ENUM_BOUNDS( eConfig )
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
		eNone,
		eColour,
		eDepth,
		eStencil,
		eDepthStencil,
		CASTOR_SCOPED_ENUM_BOUNDS( eNone )
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
		eTexture,
		eBuffer,
		CASTOR_SCOPED_ENUM_BOUNDS( eTexture )
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
		eOrtho,
		ePerspective,
		eFrustum,
		CASTOR_SCOPED_ENUM_BOUNDS( eOrtho )
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
		eNear,
		//!\~english	Far plane.
		//!\~french		Plan proche.
		eFar,
		//!\~english	Left plane.
		//!\~french		Plan gauche.
		eLeft,
		//!\~english	Right plane.
		//!\~french		Plan droit.
		eRight,
		//!\~english	Top plane.
		//!\~french		Plan haut.
		eTop,
		//!\~english	Bottom plane.
		//!\~french		Plan bas.
		eBottom,
		CASTOR_SCOPED_ENUM_BOUNDS( eNear )
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
		ePreRender,
		//!\~english	This kind of event happens after the render, before buffers' swap.
		//!\~french		Ce type d'évènement est traité après le rendu, avant l'échange des tampons.
		eQueueRender,
		//!\~english	This kind of event happens after the buffer' swap.
		//!\~french		Ce type d'évènement est traité après l'échange des tampons.
		ePostRender,
		CASTOR_SCOPED_ENUM_BOUNDS( ePreRender )
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
		eNone,
		//!\~english	Front faces are culled.
		//!\~french		Faces avant supprimées.
		eFront,
		//!\~english	Back face are culled.
		//!\~french		Faces arrière supprimées.
		eBack,
		//!\~english	Back and front faces are culled.
		//!\~french		Faces avant et arrière supprimées.
		eFrontAndBack,
		CASTOR_SCOPED_ENUM_BOUNDS( eNone )
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
		ePoint,
		//!\~english	Boundary edges of the polygon are drawn as line segments.
		//!\~french		Les arêtes du polygone sont dessinées en tant que segments.
		eLine,
		//!\~english	The interior of the polygon is filled.
		//!\~french		L'intérieur du polygone est rempli.
		eSolid,
		CASTOR_SCOPED_ENUM_BOUNDS( ePoint )
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
		eTimeElapsed,
		eSamplesPassed,
		eAnySamplesPassed,
		ePrimitivesGenerated,
		eTransformFeedbackPrimitivesWritten,
		eAnySamplesPassedConservative,
		eTimestamp,
		CASTOR_SCOPED_ENUM_BOUNDS( eTimeElapsed )
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
		eResult,
		eResultAvailable,
		eResultNoWait,
		CASTOR_SCOPED_ENUM_BOUNDS( eResult )
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.8.0
	\date		16/03/2016
	\~english
	\brief		Supported memory barriers enumeration.
	\~french
	\brief		Enumération des types de barrières supportées.
	*/
	enum class MemoryBarrier
		: uint32_t
	{
		eVertexBuffer = 0x01 << 0,
		eIndexBuffer = 0x01 << 1,
		eUniformBuffer = 0x01 << 2,
		eAtomicCounterBuffer = 0x01 << 3,
		eQueryBuffer = 0x01 << 4,
		eShaderStorageBuffer = 0x01 << 5,
		eTextureFetch = 0x01 << 6,
		eTextureUpdate = 0x01 << 7,
		eAll = 0xFFFFFFFF,
	};
	IMPLEMENT_FLAGS( MemoryBarrier )

	class WindowHandle;
	class RenderBuffer;
	class ColourRenderBuffer;
	class DepthStencilRenderBuffer;
	class FrameBufferAttachment;
	class RenderBufferAttachment;
	class TextureAttachment;
	class FrameBuffer;
	class BackBuffers;
	class IWindowHandle;
	class DebugOverlays;
	class Engine;
	class Plugin;
	class RendererPlugin;
	class ImporterPlugin;
	class DividerPlugin;
	class GenericPlugin;
	class PostFxPlugin;
	class ToneMappingPlugin;
	class ParticlePlugin;
	class FrameEvent;
	class FrameListener;
	class Version;
	class Parameters;
	class GpuQuery;
	class PickingPass;
	class TransformFeedback;
	class ComputePipeline;
	struct SsaoConfig;
	class RenderPassTimer;

	DECLARE_SMART_PTR( RenderBuffer );
	DECLARE_SMART_PTR( ColourRenderBuffer );
	DECLARE_SMART_PTR( DepthStencilRenderBuffer );
	DECLARE_SMART_PTR( FrameBufferAttachment );
	DECLARE_SMART_PTR( RenderBufferAttachment );
	DECLARE_SMART_PTR( TextureAttachment );
	DECLARE_SMART_PTR( FrameBuffer );
	DECLARE_SMART_PTR( BackBuffers );
	DECLARE_SMART_PTR( Engine );
	DECLARE_SMART_PTR( Plugin );
	DECLARE_SMART_PTR( RendererPlugin );
	DECLARE_SMART_PTR( ImporterPlugin );
	DECLARE_SMART_PTR( DividerPlugin );
	DECLARE_SMART_PTR( GenericPlugin );
	DECLARE_SMART_PTR( PostFxPlugin );
	DECLARE_SMART_PTR( ParticlePlugin );
	DECLARE_SMART_PTR( FrameEvent );
	DECLARE_SMART_PTR( FrameListener );
	DECLARE_SMART_PTR( IWindowHandle );
	DECLARE_SMART_PTR( GpuQuery );
	DECLARE_SMART_PTR( PickingPass );
	DECLARE_SMART_PTR( TransformFeedback );
	DECLARE_SMART_PTR( ComputePipeline );
	DECLARE_SMART_PTR( RenderPassTimer );

	using ParticleFactory = Castor::Factory< CpuParticleSystem, Castor::String, CpuParticleSystemUPtr, std::function< CpuParticleSystemUPtr( ParticleSystem & ) > >;

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
	DECLARE_ARRAY( PluginStrMap, PluginType::eCount, PluginStrMap );
	DECLARE_MAP( Castor::Path, Castor::DynamicLibrarySPtr, DynamicLibraryPtrPath );
	DECLARE_ARRAY( DynamicLibraryPtrPathMap, PluginType::eCount, DynamicLibraryPtrPathMap );
	DECLARE_MAP( Castor::Path, PluginType, PluginTypePath );
	DECLARE_MAP( Castor::String, BillboardListSPtr, BillboardListStr );

	typedef std::map< Castor::String, RenderWindowSPtr > WindowPtrStrMap;

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
		Castor::Connection< MAKE_OBJECT_CACHE_NAME( className )::OnChanged > m_on##className##Changed;\
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

	namespace legacy
	{
		C3D_API void ComputePreLightingMapContributions( GLSL::GlslWriter & p_writer
			, GLSL::Vec3 & p_normal
			, GLSL::Float & p_shininess
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags );

		C3D_API void ComputePostLightingMapContributions( GLSL::GlslWriter & p_writer
			, GLSL::Vec3 & p_diffuse
			, GLSL::Vec3 & p_specular
			, GLSL::Vec3 & p_emissive
			, GLSL::Float const & p_gamma
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags );

		C3D_API std::shared_ptr< GLSL::PhongLightingModel > CreateLightingModel( GLSL::GlslWriter & p_writer
			, GLSL::ShadowType p_shadows );

		C3D_API std::shared_ptr< GLSL::PhongLightingModel > CreateLightingModel( GLSL::GlslWriter & p_writer
			, LightType p_light
			, GLSL::ShadowType p_shadows );
	}

	namespace pbr
	{
		namespace mr
		{
			C3D_API void ComputePreLightingMapContributions( GLSL::GlslWriter & p_writer
				, GLSL::Vec3 & p_normal
				, GLSL::Float & p_metallic
				, GLSL::Float & p_roughness
				, TextureChannels const & textureFlags
				, ProgramFlags const & programFlags
				, SceneFlags const & sceneFlags );

			C3D_API void ComputePostLightingMapContributions( GLSL::GlslWriter & p_writer
				, GLSL::Vec3 & p_albedo
				, GLSL::Vec3 & p_emissive
				, GLSL::Float const & p_gamma
				, TextureChannels const & textureFlags
				, ProgramFlags const & programFlags
				, SceneFlags const & sceneFlags );

			C3D_API std::shared_ptr< GLSL::MetallicBrdfLightingModel > CreateLightingModel( GLSL::GlslWriter & p_writer
				, GLSL::ShadowType p_shadows );

			C3D_API std::shared_ptr< GLSL::MetallicBrdfLightingModel > CreateLightingModel( GLSL::GlslWriter & p_writer
				, LightType p_light
				, GLSL::ShadowType p_shadows );
		}

		namespace sg
		{
			C3D_API void ComputePreLightingMapContributions( GLSL::GlslWriter & p_writer
				, GLSL::Vec3 & p_normal
				, GLSL::Vec3 & p_specular
				, GLSL::Float & p_glossiness
				, TextureChannels const & textureFlags
				, ProgramFlags const & programFlags
				, SceneFlags const & sceneFlags );

			C3D_API void ComputePostLightingMapContributions( GLSL::GlslWriter & p_writer
				, GLSL::Vec3 & p_diffuse
				, GLSL::Vec3 & p_emissive
				, GLSL::Float const & p_gamma
				, TextureChannels const & textureFlags
				, ProgramFlags const & programFlags
				, SceneFlags const & sceneFlags );

			C3D_API std::shared_ptr< GLSL::SpecularBrdfLightingModel > CreateLightingModel( GLSL::GlslWriter & p_writer
				, GLSL::ShadowType p_shadows );

			C3D_API std::shared_ptr< GLSL::SpecularBrdfLightingModel > CreateLightingModel( GLSL::GlslWriter & p_writer
				, LightType p_light
				, GLSL::ShadowType p_shadows );
		}
	}

	using ParallaxFunction = GLSL::Function< GLSL::Vec2, GLSL::InParam< GLSL::Vec2 >, GLSL::InParam< GLSL::Vec3 > >;
	using ParallaxShadowFunction = GLSL::Function< GLSL::Float, GLSL::InParam< GLSL::Vec3 >, GLSL::InParam< GLSL::Vec2 >, GLSL::InParam< GLSL::Float > >;

	C3D_API ParallaxFunction DeclareParallaxMappingFunc( GLSL::GlslWriter & p_writer
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags );

	C3D_API ParallaxShadowFunction DeclareParallaxShadowFunc( GLSL::GlslWriter & p_writer
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags );
}

DECLARED_EXPORTED_OWNED_BY( C3D_API, Castor3D::Engine, Engine )
DECLARED_EXPORTED_OWNED_BY( C3D_API, Castor3D::RenderSystem, RenderSystem )
DECLARED_EXPORTED_OWNED_BY( C3D_API, Castor3D::Scene, Scene )

#endif
