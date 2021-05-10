/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderQueue_H___
#define ___C3D_RenderQueue_H___

#include "Castor3D/Render/Node/RenderNodeModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>

#include <atomic>

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

		ashes::CommandBuffer const & getCommandBuffer()const
		{
			return *m_commandBuffer;
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
		void doPrepareCommandBuffer();
		void doParseAllRenderNodes( ShadowMapLightTypeArray & shadowMaps );
		void doParseCulledRenderNodes();
		void doOnCullerCompute( SceneCuller const & culler );

	private:
		SceneCuller const & m_culler;
		SceneCullerSignalConnection m_onCullerCompute;
		SceneNode const * m_ignoredNode{ nullptr };
		QueueRenderNodesUPtr m_renderNodes;
		QueueCulledRenderNodesUPtr m_culledRenderNodes;
		ashes::CommandBufferPtr m_commandBuffer;
		bool m_allChanged{};
		bool m_culledChanged{};
		castor::GroupChangeTracked< ashes::Optional< VkViewport > > m_viewport;
		castor::GroupChangeTracked< ashes::Optional< VkRect2D > > m_scissor;
		enum class Preparation
		{
			eWaiting,
			eRunning,
			eDone,
		};
		std::atomic< Preparation > m_preparation{ Preparation::eDone };
	};
}

#if defined( CU_CompilerMSVC )
#	pragma warning( pop )
#endif

#endif
