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
#ifndef ___C3D_DistanceRenderNode_H___
#define ___C3D_DistanceRenderNode_H___

#include "PassRenderNode.hpp"
#include "SceneRenderNode.hpp"

namespace castor3d
{
	namespace details
	{
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
		DistanceRenderNode( NodeType & p_node )
			: m_node{ p_node }
		{
		}
		/**
		 *\copydoc		DistanceRenderNodeBase::getPipeline
		 */
		inline RenderPipeline & getPipeline()override
		{
			return m_node.m_pipeline;
		};
		/**
		 *\copydoc		DistanceRenderNodeBase::getPassNode
		 */
		inline PassRenderNode & getPassNode()override
		{
			return m_node.m_passNode;
		}
		/**
		 *\~english
		 *\return		The instance's scene node.
		 *\~french
		 *\return		Le noeud de scène de l'instance.
		 */
		inline SceneNode & getSceneNode()override
		{
			return details::getParentNode( m_node.m_instance );
		}
		/**
		 *\~english
		 *\return		The instance's model UBO.
		 *\~french
		 *\return		L'UBO de modèle de l'instance.
		 */
		inline ModelUbo & getModelUbo()override
		{
			return m_node.m_modelUbo;
		}
		/**
		 *\copydoc		DistanceRenderNodeBase::Render
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
