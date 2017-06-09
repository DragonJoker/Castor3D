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
#ifndef ___C3D_DeferredStencilPass_H___
#define ___C3D_DeferredStencilPass_H___

#include "LightPass.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		08/06/2017
	\~english
	\brief		Stencil pre-pass for light passes needing a mesh.
	\~french
	\brief		Pré-passe de stencil pour les passes d'éclairage nécessitant un maillage.
	*/
	class StencilPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_frameBuffer		The target framebuffer.
		 *\param[in]	p_depthAttach		The depth buffer attach.
		 *\param[in]	p_matrixUbo			The matrix UBO.
		 *\param[in]	p_modelMatrixUbo	The model matrix UBO.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_frameBuffer		Le tampon d'image cible.
		 *\param[in]	p_depthAttach		L'attache du tampon de profondeur.
		 *\param[in]	p_matrixUbo			L'UBO des matrices.
		 *\param[in]	p_modelMatrixUbo	L'UBO des matrices modèle.
		 */
		StencilPass( FrameBuffer & p_frameBuffer
			, FrameBufferAttachment & p_depthAttach
			, MatrixUbo & p_matrixUbo
			, ModelMatrixUbo & p_modelMatrixUbo );
		/**
		 *\~english
		 *\brief		Initialises the program and its pipeline.
		 *\param[in]	p_vbo	The vertex buffer containing the object to render.
		 *\param[in]	p_ibo	An optional index buffer.
		 *\~french
		 *\brief		Initialise le programme et son pipeline.
		 *\param[in]	p_vbo	Le tampon de sommets contenant l'objet à dessiner.
		 *\param[in]	p_ibo	Un tampon d'indices optionnel.
		 */
		void Initialise( VertexBuffer & p_vbo
			, IndexBufferSPtr p_ibo );
		/**
		*\~english
		*\brief		Cleans up the program and its pipeline.
		*\~french
		*\brief		Nettoie le programme et son pipeline.
		*/
		void Cleanup();
		/**
		 *\~english
		 *\brief		Renders the stencil pass.
		 *\param[in]	p_count	The number of primitives to draw.
		 *\~french
		 *\brief		Dessine la passe de stencil.
		 *\param[in]	p_count	Le nombre de primitives à dessiner.
		 */
		void Render( uint32_t p_count );

	private:
		//!\~english	The target FBO.
		//!\~french		Le FBO cible.
		FrameBuffer & m_frameBuffer;
		//!\~english	The target RBO attach.
		//!\~french		L'attache de RBO cible.
		FrameBufferAttachment & m_depthAttach;
		//!\~english	The uniform buffer containing matrices data.
		//!\~french		Le tampon d'uniformes contenant les données de matrices.
		MatrixUbo & m_matrixUbo;
		//!\~english	The uniform buffer containing the model data.
		//!\~french		Le tampon d'uniformes contenant les données de modèle.
		ModelMatrixUbo & m_modelMatrixUbo;
		//!\~english	The shader program used to render lights.
		//!\~french		Le shader utilisé pour rendre les lumières.
		ShaderProgramSPtr m_program;
		//!\~english	Geometry buffers holder.
		//!\~french		Conteneur de buffers de géométries.
		GeometryBuffersSPtr m_geometryBuffers;
		//!\~english	The pipeline used by the light pass.
		//!\~french		Le pipeline utilisé par la passe lumières.
		RenderPipelineUPtr m_pipeline;
	};
}

#endif
