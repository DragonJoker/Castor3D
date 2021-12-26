/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderQueue_H___
#define ___C3D_RenderQueue_H___

#include "Castor3D/Event/Frame/FrameEventModule.hpp"
#include "Castor3D/Render/Node/RenderNodeModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/Multithreading/SpinMutex.hpp>

#pragma warning( push )
#pragma warning( disable:4365 )
#include <atomic>
#pragma warning( pop )

#if defined( CU_CompilerMSVC )
#	pragma warning( push )
#	pragma warning( disable:4503 )
#endif

namespace castor3d
{
	class RenderQueue
		: public castor::OwnedBy< SceneRenderPass >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	renderPass	The parent render pass.
		 *\param[in]	mode		The render mode for this render queue.
		 *\param[in]	ignored		The geometries attached to this node will be ignored in the render.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	renderPass	La passe de rendu parente.
		 *\param[in]	mode		Le mode de rendu de cette file de rendu.
		 *\param[in]	ignored		Les géométries attachées à ce noeud seront ignorées lors du rendu.
		 */
		C3D_API RenderQueue( SceneRenderPass & renderPass
			, RenderMode mode
			, SceneNode const * ignored );
		C3D_API ~RenderQueue();
		/**
		 *\~english
		 *\brief		Initialises the queue.
		 *\~french
		 *\brief		Initialise la file de rendu.
		 */
		C3D_API void initialise();
		/**
		 *\~english
		 *\brief		Cleans the queue up.
		 *\~french
		 *\brief		Nettoie la file de rendu.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief			Updates the render nodes.
		 *\param[in,out]	shadowMaps	Receives the shadow maps used in the render pass.
		 *\~french
		 *\brief			Met à jour les noeuds de rendu.
		 *\param[in,out]	shadowMaps	Reçoit les shadow maps utilisées par la passe de rendu.
		 */
		C3D_API void update( ShadowMapLightTypeArray & shadowMaps );
		/**
		 *\~english
		 *\brief			Updates the render nodes.
		 *\param[in,out]	shadowMaps	Receives the shadow maps used in the render pass.
		 *\param[in]		viewport	The viewport restraining the render pass.
		 *\param[in]		scissor		The scissors restraining the render pass.
		 *\~french
		 *\brief			Met à jour les noeuds de rendu.
		 *\param[in,out]	shadowMaps	Reçoit les shadow maps utilisées par la passe de rendu.
		 *\param[in]		viewport	Le viewport restreignant par la passe de rendu.
		 *\param[in]		scissor		Le scissor restreignant par la passe de rendu.
		 */
		C3D_API void update( ShadowMapLightTypeArray & shadowMaps
			, VkViewport const & viewport
			, VkRect2D const & scissor );
		/**
		 *\~english
		 *\brief			Updates the render nodes.
		 *\param[in,out]	shadowMaps	Receives the shadow maps used in the render pass.
		 *\param[in]		scissor		The scissors restraining the render pass.
		 *\~french
		 *\brief			Met à jour les noeuds de rendu.
		 *\param[in,out]	shadowMaps	Reçoit les shadow maps utilisées par la passe de rendu.
		 *\param[in]		scissor		Le scissor restreignant par la passe de rendu.
		 */
		C3D_API void update( ShadowMapLightTypeArray & shadowMaps
			, VkRect2D const & scissor );
		C3D_API void setIgnoredNode( SceneNode const & node );
		/**@}*/
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API bool hasNodes()const;
		C3D_API RenderMode getMode()const;
		C3D_API ashes::CommandBuffer const & initCommandBuffer();

		bool hasCommandBuffer()const
		{
			return m_commandBuffer != nullptr;
		}
		
		ashes::CommandBuffer const & getCommandBuffer()const
		{
			CU_Require( hasCommandBuffer() );
			return *m_commandBuffer;
		}

		QueueRenderNodes & getAllRenderNodes()const
		{
			CU_Require( m_renderNodes );
			return *m_renderNodes;
		}

		QueueCulledRenderNodes & getCulledRenderNodes()const
		{
			CU_Require( m_culledRenderNodes );
			return *m_culledRenderNodes;
		}

		SceneCuller const & getCuller()const
		{
			return m_culler;
		}

		SceneNode const * getIgnoredNode()const
		{
			return m_ignoredNode;
		}
		/**@}*/

	private:
		void doInitialise( QueueData const & queueData );
		void doPrepareCommandBuffer();
		void doParseAllRenderNodes();
		void doSortAllRenderNodes( ShadowMapLightTypeArray & shadowMaps );
		void doParseCulledRenderNodes();
		void doOnCullerCompute( SceneCuller const & culler );

	private:
		SceneCuller const & m_culler;
		SceneCullerSignalConnection m_onCullerCompute;
		SceneNode const * m_ignoredNode{ nullptr };
		QueueRenderNodesUPtr m_renderNodes;
		QueueCulledRenderNodesUPtr m_culledRenderNodes;
		castor::SpinMutex m_eventMutex;
		GpuFrameEvent * m_initEvent{};
		ashes::CommandBufferPtr m_commandBuffer;
		bool m_allChanged{};
		bool m_sortedChanged{};
		bool m_culledChanged{};
		bool m_commandsChanged{};
		castor::GroupChangeTracked< ashes::Optional< VkViewport > > m_viewport;
		castor::GroupChangeTracked< ashes::Optional< VkRect2D > > m_scissor;
		VkRenderPass m_renderPassAtInit{};
	};
}

#if defined( CU_CompilerMSVC )
#	pragma warning( pop )
#endif

#endif
