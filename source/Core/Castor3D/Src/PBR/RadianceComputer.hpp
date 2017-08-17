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
#ifndef ___C3D_RadianceComputer_H___
#define ___C3D_RadianceComputer_H___

#include "Render/Viewport.hpp"

#include "Mesh/Buffer/BufferDeclaration.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"

#include <Design/OwnedBy.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date		02/03/2017
	\version	0.9.0
	\~english
	\brief		Class used to render colour equirectangular textures to cube maps.
	\~french
	\brief		Classe utilisée pour rendre les textures couleur équirectangulaires dans des cube maps.
	*/
	class RadianceComputer
		: public castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_context	The Context.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_context	Le Context.
		 */
		C3D_API explicit RadianceComputer( Engine & engine
			, castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~RadianceComputer();
		/**
		 *\~english
		 *\brief		Renders the wanted equirectangular 2D texture to given cube texture.
		 *\param[in]	p_size			La taille du viewport de rendu.
		 *\param[in]	p_srcTexture	The cube texture source.
		 *\param[in]	p_dstTexture	The cube texture destination.
		 *\~french
		 *\brief		Dessine a texture equirectangulaire 2D donnée dans la texture cube donnée.
		 *\param[in]	p_size			La taille du viewport de rendu.
		 *\param[in]	p_srcTexture	La texture cube source.
		 *\param[in]	p_dstTexture	La texture cube destination.
		 */
		C3D_API void render( TextureLayout const & p_srcTexture
			, TextureLayoutSPtr p_dstTexture );

	private:
		/**
		 *\~english
		 *\brief		Creates the render a 2D texture shader program.
		 *\return		The program.
		 *\~french
		 *\brief		Crée le programme shader de dessin de texture 2D.
		 *\return		Le programme.
		 */
		ShaderProgramSPtr doCreateProgram();

	private:
		//!\~english	The uniform buffer containing matrices data.
		//!\~french		Le tampon d'uniformes contenant les données de matrices.
		MatrixUbo m_matrixUbo;
		//!\~english	The resulting dimensions.
		//!\~french		Les dimensions du résultat.
		castor::Size m_size;
		//!\~english	The Viewport used when rendering a texture into to a frame buffer.
		//!\~french		Le Viewport utilisé lors du dessin d'une texture dans un tampon d'image.
		Viewport m_viewport;
		//!	6 (faces) * 6 (vertex) * 3 (vertex position)
		std::array< castor::real, 6 * 6 * 3 > m_bufferVertex;
		//!\~english	Buffer elements declaration.
		//!\~french		Déclaration des éléments d'un vertex.
		castor3d::BufferDeclaration m_declaration;
		//!\~english	Vertex array (quad definition).
		//!\~french		Tableau de vertex (définition du quad).
		std::array< castor3d::BufferElementGroupSPtr, 36 > m_arrayVertex;
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		VertexBufferSPtr m_vertexBuffer;
		//!\~english	The GeometryBuffers used when rendering a texture to the frame buffer.
		//!\~french		Le GeometryBuffers utilisé lors du dessin d'une texture dans le tampon d'image.
		GeometryBuffersSPtr m_geometryBuffers;
		//!\~english	The pipeline used to render a texture in the framebuffer.
		//!\~french		Le pipeline utilisé pour le rendu d'une texture dans le tampon d'image.
		RenderPipelineUPtr m_pipeline;
		//!\~english	The frame buffer.
		//!\~french		Le tampon d'image.
		FrameBufferSPtr m_frameBuffer;
		//!\~english	The depth buffer.
		//!\~french		Le tampon de profondeur.
		DepthStencilRenderBufferSPtr m_depthBuffer;
		//!\~english	The depth buffer attach.
		//!\~french		L'attache du tampon de profondeur.
		RenderBufferAttachmentSPtr m_depthAttach;
		//!\~english	The sampler for the texture.
		//!\~french		Le sampler pour la texture.
		SamplerSPtr m_sampler;
	};
}

#endif
