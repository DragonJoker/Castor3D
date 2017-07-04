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
#ifndef ___C3D_SkinningRenderNode_H___
#define ___C3D_SkinningRenderNode_H___

#include "ObjectRenderNode.hpp"
#include "Shader/SkinningUbo.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date		02/06/2016
	\~english
	\brief		Helper structure used to render animated submeshes.
	\~french
	\brief		Structure d'aide utilisée pour le dessin des sous-maillages animés.
	*/
	struct SkinningRenderNode
		: public SubmeshRenderNode
	{
		C3D_API SkinningRenderNode( RenderPipeline & p_pipeline
			, PassRenderNode && p_passNode
			, ModelMatrixUbo & p_modelMatrixBuffer
			, ModelUbo & p_modelBuffer
			, GeometryBuffers & p_buffers
			, SceneNode & p_sceneNode
			, DataType & p_data
			, InstanceType & p_instance
			, AnimatedSkeleton & p_skeleton
			, SkinningUbo & p_skinningUbo );

		//!\~english	The animated skeleton.
		//!\~french		Le squelette animé.
		AnimatedSkeleton & m_skeleton;
		//!\~english	The animation UBO.
		//!\~french		L'UBO d'animation.
		SkinningUbo & m_skinningUbo;
	};
}

#endif
