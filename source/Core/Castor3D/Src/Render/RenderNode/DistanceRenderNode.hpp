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

namespace Castor3D
{
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
		C3D_API virtual RenderPipeline & GetPipeline() = 0;
		/**
		 *\~english
		 *\return		The pass render node.
		 *\~french
		 *\return		Le noeud de rendu de passe.
		 */
		C3D_API virtual PassRenderNode & GetPassNode() = 0;
		/**
		 *\~english
		 *\brief		Renders the node.
		 *\~french
		 *\brief		Dessine le noeud.
		 */
		C3D_API virtual void Render() = 0;
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
		 *\copydoc		DistanceRenderNodeBase::GetPipeline
		 */
		inline RenderPipeline & GetPipeline()override
		{
			return m_node.m_pipeline;
		};
		/**
		 *\copydoc		DistanceRenderNodeBase::GetPassNode
		 */
		inline PassRenderNode & GetPassNode()override
		{
			return m_node.m_passNode;
		}
		/**
		 *\copydoc		DistanceRenderNodeBase::Render
		 */
		inline void Render()override
		{
			DoRenderNode( m_node );
		}

		//!\~english	The object node.
		//!\~french		Les noeud de l'objet.
		NodeType & m_node;
	};
}

#endif
