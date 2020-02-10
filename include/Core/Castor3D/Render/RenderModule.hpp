/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderModule_H___
#define ___C3D_RenderModule_H___

#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Material/MaterialModule.hpp"
#include "Castor3D/Texture/TextureModule.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>

namespace castor3d
{
	/**@name Render */
	//@{

	castor::String const RenderTypeUndefined = cuT( "Undefined" );
	/**
	*\~english
	*\brief
	*	RenderTarget supported types
	*\~french
	*\brief
	*	Types de RenderTarget supportés
	*/
	enum class TargetType
		: uint8_t
	{
		eWindow,
		eTexture,
		CU_ScopedEnumBounds( eWindow )
	};
	castor::String getName( TargetType value );
	/**
	*\~english
	*\brief
	*	The  viewport projection types enumeration
	*\~french
	*\brief
	*	Enumération des types de projection de viewport
	*/
	enum class ViewportType
		: uint8_t
	{
		eOrtho,
		ePerspective,
		eFrustum,
		CU_ScopedEnumBounds( eOrtho )
	};
	castor::String getName( ViewportType value );
	/**
	*\~english
	*\brief
	*	Pipeline flags.
	*\~french
	*\brief
	*	Indicateurs de pipeline.
	*/
	struct PipelineFlags
	{
		BlendMode colourBlendMode{ BlendMode::eNoBlend };
		BlendMode alphaBlendMode{ BlendMode::eNoBlend };
		PassFlags passFlags{ PassFlag::eNone };
		TextureFlags textures{ TextureFlag::eNone };
		uint32_t texturesCount{ 0u };
		uint32_t heightMapIndex{ InvalidIndex };
		ProgramFlags programFlags{ ProgramFlag::eNone };
		SceneFlags sceneFlags{ SceneFlag::eNone };
		VkPrimitiveTopology topology{ VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST };
		VkCompareOp alphaFunc{ VK_COMPARE_OP_ALWAYS };
	};
	C3D_API bool operator<( PipelineFlags const & lhs, PipelineFlags const & rhs );
	/**
	*\~english
	*\brief
	*	Holds data for device rendering.
	*\~french
	*\brief
	*	Contient les données pour le rendu sur device.
	*/
	class RenderDevice;
	/**
	*\~english
	*\brief
	*	Holds render informations.
	*\~french
	*\brief
	*	Contient les informations de rendu.
	*/
	struct RenderInfo;
	/**
	*\~english
	*\brief
	*	Render loop base class.
	*\~french
	*\brief
	*	Classe de base d'une boucle de rendu.
	*/
	class RenderLoop;
	/**
	*\~english
	*\brief
	*	Asynchronous render loop.
	*\~french
	*\brief
	*	Boucle de rendu asynchrone.
	*/
	class RenderLoopAsync;
	/**
	*\~english
	*\brief
	*	Synchronous render loop.
	*\~french
	*\brief
	*	Boucle de rendu synchrone.
	*/
	class RenderLoopSync;
	/**
	*\~english
	*\brief
	*	Render pass base class.
	*\~french
	*\brief
	*	Classe de base d'une passe de rendu.
	*/
	class RenderPass;
	/**
	*\~english
	*\brief
	*	Allows stopping a RenderPassTimer when an instance of this class goes out of scope.
	*\~french
	*\brief
	*	Permet d'arrêter un RenderPassTimer lorsqu'une instance de cette classe sort du scope courant.
	*/
	class RenderPassTimerBlock;
	/**
	*\~english
	*\brief
	*	Class that holds data needed to compute a render pass times.
	*\~french
	*\brief
	*	Classe englobant les données nécessaires au calcul des temps d'une passe de rendu.
	*/
	class RenderPassTimer;
	/**
	*\~english
	*\brief
	*	A pipeline to render specific nodes from a render pass.
	*\~french
	*\brief
	*	Un pipeline pour le rendu de noeuds d'une passe de rendu.
	*/
	class RenderPipeline;
	/**
	*\~english
	*\brief
	*	The render nodes for a specific scene.
	*\~french
	*\brief
	*	Les noeuds de rendu pour une scène spécifique.
	*/
	template< typename NodeType, typename MapType >
	struct RenderNodesT;
	/**
	*\~english
	*\brief
	*	The render nodes for a specific scene.
	*\~french
	*\brief
	*	Les noeuds de rendu pour une scène spécifique.
	*/
	struct SceneRenderNodes;
	/**
	*\~english
	*\brief
	*	The culled render nodes for a specific scene.
	*\~french
	*\brief
	*	Les noeuds de rendu culled pour une scène spécifique.
	*/
	struct SceneCulledRenderNodes;
	/**
	*\~english
	*\brief
	*	A render queue, processing render nodes.
	*\~french
	*\brief
	*	Une file de rendu, agissant sur des noeuds de rendu.
	*/
	class RenderQueue;
	/**
	*\~english
	*\brief
	*	Links Castor3D to the rendering API.
	*\~french
	*\brief
	*	Fait le lien entre Castor3D et l'API de rendu.
	*/
	class RenderSystem;
	/**
	*\~english
	*\brief
	*	Render target class
	*\remarks
	*	A render target draws a scene in a frame buffer that can then be used by a window to have a direct render, or a texture to have offscreen rendering
	*\~french
	*\brief
	*	Classe de cible de rendu (render target)
	*\remarks
	*	Une render target dessine une scène dans un tampon d'image qui peut ensuite être utilisé dans une fenêtre pour un rendu direct, ou une texture pour un rendu hors écran
	*/
	class RenderTarget;
	/**
	*\~english
	*\brief
	*	Render window representation.
	*\remark
	*	Manages a window where you can render a scene.
	*\~french
	*\brief
	*	Implémentation d'une fenêtre de rendu.
	*\remark
	*	Gère une fenêtre dans laquelle une scène peut être rendue
	*/
	class RenderWindow;
	/**
	*\~english
	*\brief
	*	A render viewport.
	*\~french
	*\brief
	*	Un viewport de rendu.
	*/
	class Viewport;

	CU_DeclareSmartPtr( RenderDevice );
	CU_DeclareSmartPtr( RenderLoop );
	CU_DeclareSmartPtr( RenderPassTimer );
	CU_DeclareSmartPtr( RenderPipeline );
	CU_DeclareSmartPtr( RenderSystem );
	CU_DeclareSmartPtr( RenderTarget );
	CU_DeclareSmartPtr( RenderWindow );

	//! RenderWindow pointer array.
	CU_DeclareVector( RenderWindowSPtr, RenderWindowPtr );
	//! RenderWindow pointer map, sorted by index.
	CU_DeclareMap( uint32_t, RenderWindowSPtr, RenderWindow );
	//! RenderWindow pointer map, sorted by name.
	CU_DeclareMap( castor::String, RenderWindowSPtr, RenderWindowPtrStr );

	using RenderQueueArray = std::vector< std::reference_wrapper< RenderQueue > >;
	//@}
}

CU_DeclareExportedOwnedBy( C3D_API, castor3d::RenderSystem, RenderSystem )
CU_DeclareExportedOwnedBy( C3D_API, castor3d::RenderDevice, RenderDevice )

namespace castor3d
{
	/**@name Render */
	//@{

	C3D_API RenderDevice const & getCurrentRenderDevice( RenderDevice const & obj );
	C3D_API RenderDevice const & getCurrentRenderDevice( RenderSystem const & obj );
	C3D_API RenderDevice const & getCurrentRenderDevice( Engine const & obj );
	C3D_API RenderDevice const & getCurrentRenderDevice( Scene const & obj );

	inline RenderDevice const & getCurrentRenderDevice( castor::OwnedBy< RenderSystem > const & obj )
	{
		return getCurrentRenderDevice( *obj.getRenderSystem() );
	}

	inline RenderDevice const & getCurrentRenderDevice( castor::OwnedBy< Engine > const & obj )
	{
		return getCurrentRenderDevice( *obj.getEngine() );
	}

	inline RenderDevice const & getCurrentRenderDevice( castor::OwnedBy< Scene > const & obj )
	{
		return getCurrentRenderDevice( *obj.getScene() );
	}

	//@}
}

#endif
