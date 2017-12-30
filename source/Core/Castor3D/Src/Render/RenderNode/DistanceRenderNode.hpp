/*
See LICENSE file in root folder
*/
#ifndef ___C3D_DistanceRenderNode_H___
#define ___C3D_DistanceRenderNode_H___

#include "PassRenderNode.hpp"
#include "SceneRenderNode.hpp"

namespace castor3d
{
	namespace details
	{
		uint32_t getPrimitiveCount( Geometry & p_instance );
		uint32_t getPrimitiveCount( BillboardBase & p_instance );
		uint32_t getVertexCount( Geometry & p_instance );
		uint32_t getVertexCount( BillboardBase & p_instance );
		SceneNode & getParentNode( Geometry & p_instance );
		SceneNode & getParentNode( BillboardBase & p_instance );
	}
	/*!
	\author 	Sylvain DOREMUS
	\date
	\~english
	\brief		Helper structure used to render billboards.
	\~french
	\brief		Structure d'aide utilisée pour le dessin des billboards.
	*/
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
	/*!
	\author 	Sylvain DOREMUS
	\date
	\~english
	\brief		Helper structure used to render objects.
	\~french
	\brief		Structure d'aide utilisée pour le dessin d'objets.
	*/
	template< typename NodeType >
	struct DistanceRenderNode
		: public DistanceRenderNodeBase
	{
		explicit DistanceRenderNode( NodeType & p_node )
			: m_node{ p_node }
		{
		}
		/**
		 *\copydoc		DistanceRenderNodeBase::getPipeline
		 */
		inline RenderPipeline & getPipeline()override
		{
			return m_node.m_pipeline;
		}
		/**
		 *\copydoc		DistanceRenderNodeBase::getPassNode
		 */
		inline PassRenderNode & getPassNode()override
		{
			return m_node.m_passNode;
		}
		/**
		 *\copydoc		DistanceRenderNodeBase::getSceneNode
		 */
		inline SceneNode & getSceneNode()override
		{
			return details::getParentNode( m_node.m_instance );
		}
		/**
		 *\copydoc		DistanceRenderNodeBase::getModelUbo
		 */
		inline ModelUbo & getModelUbo()override
		{
			return m_node.m_modelUbo;
		}
		/**
		 *\copydoc		DistanceRenderNodeBase::render
		 */
		inline void render()override
		{
			doRenderNode( m_node );
		}

		//!\~english	The object node.
		//!\~french		Les noeud de l'objet.
		NodeType & m_node;
	};
}

#endif
