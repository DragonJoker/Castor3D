/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DistanceRenderNode_H___
#define ___C3D_DistanceRenderNode_H___

#include "Castor3D/Render/RenderNode/PassRenderNode.hpp"
#include "Castor3D/Render/RenderNode/SceneRenderNode.hpp"

namespace castor3d
{
	namespace details
	{
		uint32_t getPrimitiveCount( Submesh const & submesh );
		uint32_t getPrimitiveCount( BillboardBase const & instance );
		uint32_t getVertexCount( Submesh const & submesh );
		uint32_t getVertexCount( BillboardBase const & instance );
		SceneNode & getParentNode( Geometry & instance );
		SceneNode & getParentNode( BillboardBase & instance );
	}

	struct DistanceRenderNodeBase
	{
		virtual ~DistanceRenderNodeBase() = default;
		/**
		 *\~english
		 *\return		The render pipeline.
		 *\~french
		 *\return		Le pipeline de rendu.
		 */
		C3D_API virtual RenderPipeline & getPipeline() = 0;
		/**
		 *\~english
		 *\return		The pass render node.
		 *\~french
		 *\return		Le noeud de rendu de passe.
		 */
		C3D_API virtual PassRenderNode & getPassNode() = 0;
		/**
		 *\~english
		 *\return		The instance's scene node.
		 *\~french
		 *\return		Le noeud de scène de l'instance.
		 */
		C3D_API virtual SceneNode & getSceneNode() = 0;
		/**
		 *\~english
		 *\return		The instance's model UBO.
		 *\~french
		 *\return		L'UBO de modèle de l'instance.
		 */
		C3D_API virtual ModelUbo & getModelUbo() = 0;
		/**
		 *\~english
		 *\brief		Renders the node.
		 *\~french
		 *\brief		Dessine le noeud.
		 */
		C3D_API virtual void render() = 0;
	};

	template< typename NodeType >
	struct DistanceRenderNode
		: public DistanceRenderNodeBase
	{
		explicit DistanceRenderNode( NodeType & node )
			: node{ node }
		{
		}
		/**
		 *\copydoc		DistanceRenderNodeBase::getPipeline
		 */
		inline RenderPipeline & getPipeline()override
		{
			return node.m_pipeline;
		}
		/**
		 *\copydoc		DistanceRenderNodeBase::getPassNode
		 */
		inline PassRenderNode & getPassNode()override
		{
			return node.m_passNode;
		}
		/**
		 *\copydoc		DistanceRenderNodeBase::getSceneNode
		 */
		inline SceneNode & getSceneNode()override
		{
			return details::getParentNode( node.m_instance );
		}
		/**
		 *\copydoc		DistanceRenderNodeBase::getModelUbo
		 */
		inline ModelUbo & getModelUbo()override
		{
			return node.m_modelUbo;
		}
		/**
		 *\copydoc		DistanceRenderNodeBase::render
		 */
		inline void render()override
		{
			doUpdateNode( node );
		}

		//!\~english	The object node.
		//!\~french		Les noeud de l'objet.
		NodeType & node;
	};

	template< typename NodeType >
	std::unique_ptr< DistanceRenderNodeBase > makeDistanceNode( NodeType & node )
	{
		return std::make_unique< DistanceRenderNode< NodeType > >( node );
	}
}

#endif
