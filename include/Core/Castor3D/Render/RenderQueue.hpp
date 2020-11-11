/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderQueue_H___
#define ___C3D_RenderQueue_H___

#include "Castor3D/Render/Node/RenderNodeModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Render/Node/RenderNode.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>

#include <atomic>

#if defined( CU_CompilerMSVC )
#	pragma warning( push )
#	pragma warning( disable:4503 )
#endif

namespace castor3d
{
	class RenderQueue
		: public castor::OwnedBy< RenderPass >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	renderPass	The parent render pass.
		 *\param[in]	opaque		Tells if this render queue is for opaque nodes.
		 *\param[in]	ignored		The geometries attached to this node will be ignored in the render.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	renderPass	La passe de rendu parente.
		 *\param[in]	opaque		Dit si cette file de rendu est pour les noeuds opaques.
		 *\param[in]	ignored		Les géométries attachées à ce noeud seront ignorées lors du rendu.
		 */
		C3D_API RenderQueue( RenderPass & renderPass
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
		 *\brief		Updates the render nodes.
		 *\~french
		 *\brief		Met à jour les noeuds de rendu.
		 */
		C3D_API void update( ShadowMapLightTypeArray & shadowMaps );
		/**
		 *\~english
		 *\brief		Updates the render nodes.
		 *\~french
		 *\brief		Met à jour les noeuds de rendu.
		 */
		C3D_API void update( ShadowMapLightTypeArray & shadowMaps
			, VkViewport const & viewport
			, VkRect2D const & scissor );
		/**
		 *\~english
		 *\brief		Updates the render nodes.
		 *\~french
		 *\brief		Met à jour les noeuds de rendu.
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
		inline SceneRenderNodes & getAllRenderNodes()const
		{
			CU_Require( m_renderNodes );
			return *m_renderNodes;
		}

		inline SceneCulledRenderNodes & getCulledRenderNodes()const
		{
			CU_Require( m_culledRenderNodes );
			return *m_culledRenderNodes;
		}

		inline ashes::CommandBuffer const & getCommandBuffer()const
		{
			return *m_commandBuffer;
		}

		inline RenderMode getMode()const
		{
			return m_mode;
		}

		inline SceneNode const * getIgnoredNode()const
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
		RenderMode m_mode;
		SceneNode const * m_ignoredNode{ nullptr };
		SceneRenderNodesPtr m_renderNodes;
		SceneCulledRenderNodesPtr m_culledRenderNodes;
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
