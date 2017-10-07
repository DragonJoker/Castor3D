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
#ifndef ___C3D_SubsurfaceScatteringPass_H___
#define ___C3D_SubsurfaceScatteringPass_H___

#include "Render/Viewport.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "Texture/TextureUnit.hpp"
#include "Technique/Opaque/LightPass.hpp"

#include <Design/OwnedBy.hpp>

namespace castor3d
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
	class SubsurfaceScatteringPass
		: public castor::OwnedBy< Engine >
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
		C3D_API SubsurfaceScatteringPass( Engine & engine
			, GpInfoUbo & gpInfoUbo
			, SceneUbo & sceneUbo
			, castor::Size const & textureSize );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~SubsurfaceScatteringPass();
		/**
		 *\~english
		 *\brief		Applies Subsurface scattering.
		 *\param[in]	gp				The geometry pass result.
		 *\param[in]	lightDiffuse	The light pass diffuse result.
		 *\~french
		 *\brief		Applique le Subsurface scattering.
		 *\param[in]	gp				Le résultat de la geometry pass.
		 *\param[in]	lightDiffuse	Le résultat diffus de la light pass.
		 */
		C3D_API void render( GeometryPassResult const & gp
			, TextureUnit const & lightDiffuse )const;
		/**
		 *\~english
		 *\brief		Dumps the results on the screen.
		 *\param[in]	size	The dump dimensions.
		 *\~french
		 *\brief		Dumpe les résultats sur l'écran.
		 *\param[in]	size	Les dimensions d'affichage.
		 */
		C3D_API void debugDisplay( castor::Size const & size )const;

		inline TextureUnit const & getResult()const
		{
			return m_result;
		}

	private:
		void doBlur( GeometryPassResult const & gp
			, TextureUnit const & source
			, TextureUnit const & destination
			, TextureAttachmentSPtr attach
			, castor::Point2r const & direction )const;
		void doCombine( GeometryPassResult const & gp
			, TextureUnit const & source )const;

	public:
		static castor::String const Config;
		static castor::String const Step;
		static castor::String const Correction;
		static castor::String const PixelSize;

	private:
		//!\~english	The render size.
		//!\~french		La taille du rendu.
		castor::Size m_size;
		//!\~english	The render viewport.
		//!\~french		La viewport du rendu.
		Viewport m_viewport;
		//!\~english	The matrices uniform buffer.
		//!\~french		Le tampon d'uniformes contenant les matrices.
		MatrixUbo m_matrixUbo;
		//!\~english	The blur pass informations.
		//!\~french		Les informations de la passe de flou.
		UniformBuffer m_blurUbo;
		Uniform2fSPtr m_blurStep;
		Uniform1fSPtr m_blurCorrection;
		Uniform1fSPtr m_blurPixelSize;
		TextureUnit m_intermediate;
		TextureAttachmentSPtr m_intermediateAttach;
		std::array< TextureUnit, 3u > m_blurResults;
		std::array< TextureAttachmentSPtr, 3u > m_blurResultsAttaches;
		TextureUnit m_result;
		TextureAttachmentSPtr m_resultAttach;
		FrameBufferSPtr m_fbo;
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		VertexBufferSPtr m_vertexBuffer;
		//!\~english	The shader program.
		//!\~french		Le shader program.
		ShaderProgramSPtr m_blurProgram;
		//!\~english	The geometry buffers.
		//!\~french		Les tampons de géométrie.
		GeometryBuffersSPtr m_blurGeometryBuffers;
		//!\~english	The blur pipeline.
		//!\~french		Le pipeline de flou.
		RenderPipelineUPtr m_blurPipeline;
		//!\~english	The shader program.
		//!\~french		Le shader program.
		ShaderProgramSPtr m_combineProgram;
		//!\~english	The geometry buffers.
		//!\~french		Les tampons de géométrie.
		GeometryBuffersSPtr m_combineGeometryBuffers;
		//!\~english	The combine pipeline.
		//!\~french		Le pipeline de combinaison.
		RenderPipelineUPtr m_combinePipeline;


	};
}

#endif
