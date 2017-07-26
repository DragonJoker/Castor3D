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
#ifndef ___C3D_GaussianBlur_H___
#define ___C3D_GaussianBlur_H___

#include "Render/Viewport.hpp"
#include "Shader/MatrixUbo.hpp"
#include "Texture/TextureUnit.hpp"

#include <Design/OwnedBy.hpp>

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		26/07/2017
	\~english
	\brief		Gaussian blur pass.
	\~french
	\brief		Passe flou gaussien.
	*/
	class GaussianBlur
		: public Castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	textureSize	The render area dimensions.
		 *\param[in]	format		The pixel format for the textures to blur.
		 *\param[in]	kernelSize	The kernel coefficients count.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	textureSize	Les dimensions de la zone de rendu.
		 *\param[in]	format		Le format de pixel des textures à flouter.
		 *\param[in]	kernelSize	Le nombre de coefficients du kernel.
		 */
		C3D_API GaussianBlur( Engine & engine
			, Castor::Size const & textureSize
			, Castor::PixelFormat format
			, uint32_t kernelSize );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~GaussianBlur();
		/**
		 *\~english
		 *\brief		Blurs given texture.
		 *\param[in]	texture	The texture.
		 *\~french
		 *\brief		Applique le flou sur la texture donnée.
		 *\param[in]	texture	La texture.
		 */
		C3D_API void Blur( TextureLayoutSPtr texture );
		/**
		 *\~english
		 *\brief		Blurs given texture using given intermediate texture.
		 *\param[in]	texture			The texture to blur.
		 *\param[in]	intermediate	The intermediate texture.
		 *\~french
		 *\brief		Applique le flou sur la texture donnée en utilisant la texture intermédiaire donnée.
		 *\param[in]	texture			La texture à flouter.
		 *\param[in]	intermediate	La texture indtermédiaire.
		 */
		C3D_API void Blur( TextureLayoutSPtr texture
			, TextureLayoutSPtr intermediate );

	private:
		void DoBlur( TextureLayoutSPtr & source
			, TextureLayoutSPtr & destination
			, RenderPipeline & pipeline
			, UniformBuffer & ubo );
		bool DoInitialiseBlurXProgram( Engine & engine );
		bool DoInitialiseBlurYProgram( Engine & engine );

	public:
		static Castor::String const Config;
		static Castor::String const Coefficients;
		static Castor::String const CoefficientsCount;
		static constexpr uint32_t MaxCoefficients{ 64u };

	private:
		Castor::Size m_size;
		MatrixUbo m_matrixUbo;
		AttachmentPoint m_point;
		TextureUnit m_colour;
		FrameBufferSPtr m_fbo;
		TextureAttachmentSPtr m_colourAttach;
		std::vector< float > m_kernel;

		Castor3D::RenderPipelineUPtr m_blurXPipeline;
		Castor3D::PushUniform1sSPtr m_blurXMapDiffuse;
		Castor3D::UniformBuffer m_blurXUbo;
		Castor3D::Uniform1uiSPtr m_blurXCoeffCount;
		Castor3D::Uniform1fSPtr m_blurXCoeffs;

		Castor3D::RenderPipelineUPtr m_blurYPipeline;
		Castor3D::PushUniform1sSPtr m_blurYMapDiffuse;
		Castor3D::UniformBuffer m_blurYUbo;
		Castor3D::Uniform1uiSPtr m_blurYCoeffCount;
		Castor3D::Uniform1fSPtr m_blurYCoeffs;


	};
}

#endif
