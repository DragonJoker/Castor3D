/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderQueue_H___
#define ___C3D_RenderQueue_H___

#include "RenderModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"

#include "Castor3D/Render/Node/RenderNode.hpp"

#include <CastorUtils/Design/ChangeTracked.hpp>

#include <ashespp/Command/CommandBuffer.hpp>

#include <atomic>

#if defined( CU_CompilerMSVC )
#	pragma warning( push )
#	pragma warning( disable:4503 )
#endif

namespace castor3d
{
	template< typename NodeType, typename MapType >
	struct RenderNodesT
	{
		//!\~english	The geometries, sorted by shader program.
		//!\~french		Les géométries, triées par programme shader.
		MapType frontCulled;
		//!\~english	The geometries, sorted by shader program.
		//!\~french		Les géométries, triées par programme shader.
		MapType backCulled;
	};

	struct SceneRenderNodes
	{
		using StaticNodesMap = RenderNodesT< StaticRenderNode, StaticRenderNodesByPipelineMap >;
		using SkinnedNodesMap = RenderNodesT< SkinningRenderNode, SkinningRenderNodesByPipelineMap >;
		using InstantiatedStaticNodesMap = RenderNodesT< StaticRenderNode, SubmeshStaticRenderNodesByPipelineMap >;
		using InstantiatedSkinnedNodesMap = RenderNodesT< SkinningRenderNode, SubmeshSkinningRenderNodesByPipelineMap >;
		using MorphingNodesMap = RenderNodesT< MorphingRenderNode, MorphingRenderNodesByPipelineMap >;
		using BillboardNodesMap = RenderNodesT< BillboardRenderNode, BillboardRenderNodesByPipelineMap >;

		//!\~english	The scene.
		//!\~french		La scène.
		Scene const & scene;
		//!\~english	The static render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu statiques, triés par programme shader.
		StaticNodesMap staticNodes;
		//!\~english	The animated render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu animés, triés par programme shader.
		SkinnedNodesMap skinnedNodes;
		//!\~english	The instanced render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu instanciés, triés par programme shader.
		InstantiatedStaticNodesMap instancedStaticNodes;
		//!\~english	The animated render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu animés, triés par programme shader.
		InstantiatedSkinnedNodesMap instancedSkinnedNodes;
		//!\~english	The animated render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu animés, triés par programme shader.
		MorphingNodesMap morphingNodes;
		//!\~english	The billboards render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu de billboards, triés par programme shader.
		BillboardNodesMap billboardNodes;

		inline explicit SceneRenderNodes( Scene const & scene )
			: scene{ scene }
		{
		}

		inline bool hasNodes()const
		{
			return !staticNodes.backCulled.empty()
				|| !staticNodes.frontCulled.empty()
				|| !skinnedNodes.backCulled.empty()
				|| !skinnedNodes.frontCulled.empty()
				|| !instancedStaticNodes.backCulled.empty()
				|| !instancedStaticNodes.frontCulled.empty()
				|| !instancedSkinnedNodes.backCulled.empty()
				|| !instancedSkinnedNodes.frontCulled.empty()
				|| !morphingNodes.backCulled.empty()
				|| !morphingNodes.frontCulled.empty()
				|| !billboardNodes.backCulled.empty()
				|| !billboardNodes.frontCulled.empty();
		}
	};

	struct SceneCulledRenderNodes
	{
		using StaticNodesMap = RenderNodesT< StaticRenderNode, StaticRenderNodesPtrByPipelineMap >;
		using SkinnedNodesMap = RenderNodesT< SkinningRenderNode, SkinningRenderNodesPtrByPipelineMap >;
		using InstantiatedStaticNodesMap = RenderNodesT< StaticRenderNode, SubmeshStaticRenderNodesPtrByPipelineMap >;
		using InstantiatedSkinnedNodesMap = RenderNodesT< SkinningRenderNode, SubmeshSkinningRenderNodesPtrByPipelineMap >;
		using MorphingNodesMap = RenderNodesT< MorphingRenderNode, MorphingRenderNodesPtrByPipelineMap >;
		using BillboardNodesMap = RenderNodesT< BillboardRenderNode, BillboardRenderNodesPtrByPipelineMap >;

		//!\~english	The scene.
		//!\~french		La scène.
		Scene const & scene;
		//!\~english	The static render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu statiques, triés par programme shader.
		StaticNodesMap staticNodes;
		//!\~english	The animated render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu animés, triés par programme shader.
		SkinnedNodesMap skinnedNodes;
		//!\~english	The instanced render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu instanciés, triés par programme shader.
		InstantiatedStaticNodesMap instancedStaticNodes;
		//!\~english	The animated render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu animés, triés par programme shader.
		InstantiatedSkinnedNodesMap instancedSkinnedNodes;
		//!\~english	The animated render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu animés, triés par programme shader.
		MorphingNodesMap morphingNodes;
		//!\~english	The billboards render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu de billboards, triés par programme shader.
		BillboardNodesMap billboardNodes;

		inline explicit SceneCulledRenderNodes( Scene const & scene )
			: scene{ scene }
		{
		}

		inline bool hasNodes()const
		{
			return !staticNodes.backCulled.empty()
				|| !staticNodes.frontCulled.empty()
				|| !skinnedNodes.backCulled.empty()
				|| !skinnedNodes.frontCulled.empty()
				|| !instancedStaticNodes.backCulled.empty()
				|| !instancedStaticNodes.frontCulled.empty()
				|| !instancedSkinnedNodes.backCulled.empty()
				|| !instancedSkinnedNodes.frontCulled.empty()
				|| !morphingNodes.backCulled.empty()
				|| !morphingNodes.frontCulled.empty()
				|| !billboardNodes.backCulled.empty()
				|| !billboardNodes.frontCulled.empty();
		}
	};

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
			, bool opaque
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

		inline bool hasNodes()const
		{
			return ( m_renderNodes
				? m_renderNodes->hasNodes()
				: false );
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
		bool m_opaque;
		SceneNode const * m_ignoredNode{ nullptr };
		std::unique_ptr< SceneRenderNodes > m_renderNodes;
		std::unique_ptr< SceneCulledRenderNodes > m_culledRenderNodes;
		ashes::CommandBufferPtr m_commandBuffer;
		bool m_allChanged;
		bool m_culledChanged;
		castor::ChangeTracked< ashes::Optional< VkViewport > > m_viewport;
		castor::ChangeTracked< ashes::Optional< VkRect2D > > m_scissor;
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
