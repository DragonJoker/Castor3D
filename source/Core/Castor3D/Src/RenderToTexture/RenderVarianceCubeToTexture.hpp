﻿/*
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
#ifndef ___C3D_RenderVarianceCubeToTexture_H___
#define ___C3D_RenderVarianceCubeToTexture_H___

#include "Render/Viewport.hpp"

#include "Mesh/Buffer/BufferDeclaration.hpp"

#include <Design/OwnedBy.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date		02/03/2017
	\version	0.9.0
	\~english
	\brief		Class used to render a variance cube texture.
	\~french
	\brief		Classe utilisée pour rendre une texture cube variance.
	*/
	class RenderVarianceCubeToTexture
		: public castor::OwnedBy< Context >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	context		The Context.
		 *\param[in]	matrixUbo	The UBO containing matrix data.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	context		Le Context.
		 *\param[in]	matrixUbo	L'UBO contenant les données de matrices.
		 */
		C3D_API explicit RenderVarianceCubeToTexture( Context & context
			, MatrixUbo & matrixUbo );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~RenderVarianceCubeToTexture();
		/**
		 *\~english
		 *\brief		Initialises this render to texture.
		 *\~french
		 *\brief		Initialise le rendu en texture.
		 */
		C3D_API void initialise();
		/**
		 *\~english
		 *\brief		Cleans up the render to texture.
		 *\~french
		 *\brief		Nettoie le rendu en texture.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Renders the given cube texture to the currently draw-bound frame buffer.
		 *\param[in]	position	The render viewport position.
		 *\param[in]	size		The render viewport size.
		 *\param[in]	texture		The texture.
		 *\~french
		 *\brief		Rend la texture cube donnée dans le tampon d'image actuellement activé en dessin.
		 *\param[in]	position	La position du viewport de rendu.
		 *\param[in]	size		La taille du viewport de rendu.
		 *\param[in]	texture		La texture.
		 */
		C3D_API void render( castor::Position const & position
			, castor::Size const & size
			, TextureLayout const & texture );

	private:
		struct Pipeline
		{
			//!\~english	The GeometryBuffers used when rendering a texture to the current frame buffer.
			//!\~french		Le GeometryBuffers utilisé lors du dessin d'une texture dans le tampon d'image courant.
			GeometryBuffersSPtr m_geometryBuffers;
			//!\~english	The pipeline used to render a texture in the current draw-bound framebuffer.
			//!\~french		Le pipeline utilisé pour le rendu d'une texture dans le tampon d'image actuellement activé en dessin.
			RenderPipelineUPtr m_pipeline;
			//!\~english	The uniform variable holding the direction of the face to render.
			//!\~french		La variable uniforme contenant la direction de la face à dessiner.
			PushUniform3fSPtr m_faceUniform;
		};

	protected:
		/**
		 *\~english
		 *\brief		Renders the wanted face of given cube texture.
		 *\param[in]	position	The render viewport position.
		 *\param[in]	size		The render viewport size.
		 *\param[in]	texture		The texture.
		 *\param[in]	face		The cube face.
		 *\param[in]	pipeline	The render pipeline.
		 *\~french
		 *\brief		Dessine la face voulue de la texture cube donnée.
		 *\param[in]	position	La position du viewport de rendu.
		 *\param[in]	size		La taille du viewport de rendu.
		 *\param[in]	texture		La texture.
		 *\param[in]	face		La face du cube.
		 *\param[in]	pipeline	Le pipeline de rendu.
		 */
		C3D_API void doRender( castor::Point2i const & position
			, castor::Size const & size
			, TextureLayout const & texture
			, castor::Point3f const & face
			, Pipeline & pipeline );
		/**
		 *\~english
		 *\brief		Creates the render a 2D texture shader program.
		 *\return		The program.
		 *\~french
		 *\brief		Crée le programme shader de dessin de texture 2D.
		 *\return		Le programme.
		 */
		ShaderProgramSPtr doCreateDepthProgram();
		/**
		 *\~english
		 *\brief		Creates the render a 2D texture shader program.
		 *\return		The program.
		 *\~french
		 *\brief		Crée le programme shader de dessin de texture 2D.
		 *\return		Le programme.
		 */
		ShaderProgramSPtr doCreateVarianceProgram();
		void doInitialiseDepth();
		void doInitialiseVariance();

	private:
		//!\~english	The uniform buffer containing matrices data.
		//!\~french		Le tampon d'uniformes contenant les données de matrices.
		MatrixUbo & m_matrixUbo;
		//!\~english	The Viewport used when rendering a texture into to a frame buffer.
		//!\~french		Le Viewport utilisé lors du dessin d'une texture dans un tampon d'image.
		Viewport m_viewport;
		//!	6 * [2(vertex position) + 2(texture coordinates)]
		std::array< castor::real, 6 * ( 2 + 2 ) > m_bufferVertex;
		//!\~english	Buffer elements declaration.
		//!\~french		Déclaration des éléments d'un vertex.
		castor3d::BufferDeclaration m_declaration;
		//!\~english	Vertex array (quad definition).
		//!\~french		Tableau de vertex (définition du quad).
		std::array< castor3d::BufferElementGroupSPtr, 6 > m_arrayVertex;
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		VertexBufferSPtr m_vertexBuffer;
		//!\~english	The pipeline used to render a texture in the current draw-bound framebuffer.
		//!\~french		Le pipeline utilisé pour le rendu d'une texture dans le tampon d'image actuellement activé en dessin.
		Pipeline m_pipelineDepth;
		//!\~english	The pipeline used to render a texture in the current draw-bound framebuffer.
		//!\~french		Le pipeline utilisé pour le rendu d'une texture dans le tampon d'image actuellement activé en dessin.
		Pipeline m_pipelineVariance;
		//!\~english	The sampler for the texture.
		//!\~french		Le sampler pour la texture.
		SamplerSPtr m_sampler;
	};
}

#endif
