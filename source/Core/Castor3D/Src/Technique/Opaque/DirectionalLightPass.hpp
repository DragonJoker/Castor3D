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
#ifndef ___C3D_DeferredDirectionalLightPass_H___
#define ___C3D_DeferredDirectionalLightPass_H___

#include "LightPass.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		08/06/2017
	\~english
	\brief		Directional light pass.
	\~french
	\brief		Passe de lumière directionnelle.
	*/
	class DirectionalLightPass
		: public LightPass
	{
	protected:
		/*!
		\author		Sylvain DOREMUS
		\version	0.10.0
		\date		08/06/2017
		\~english
		\brief		Directional light pass program.
		\~french
		\brief		Programme de passe de lumière directionnelle.
		*/
		struct Program
			: public LightPass::Program
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\param[in]	p_engine	The engine.
			 *\param[in]	p_vtx		The vertex shader source.
			 *\param[in]	p_pxl		The fragment shader source.
			 *\~french
			 *\brief		Constructeur.
			 *\param[in]	p_engine	Le moteur.
			 *\param[in]	p_vtx		Le source du vertex shader.
			 *\param[in]	p_pxl		Le source du fagment shader.
			 */
			Program( Engine & p_engine
				, GLSL::Shader const & p_vtx
				, GLSL::Shader const & p_pxl );
			/**
			 *\~english
			 *\brief		Destructor.
			 *\~french
			 *\brief		Destructeur.
			 */
			virtual ~Program();

		private:
			/**
			 *\copydoc		Castor3D::LightPass::Program::DoCreatePipeline
			 */
			virtual RenderPipelineUPtr DoCreatePipeline( bool p_blend )override;
			/**
			 *\copydoc		Castor3D::LightPass::Program::DoBind
			 */
			void DoBind( Light const & p_light )override;

		private:
			//!\~english	The variable containing the light intensities.
			//!\~french		La variable contenant les intensités de la lumière.
			PushUniform2fSPtr m_lightIntensity;
			//!\~english	The variable containing the light direction.
			//!\~french		La variable contenant la direction de la lumière.
			PushUniform3fSPtr m_lightDirection;
			//!\~english	The variable containing the light space transformation matrix.
			//!\~french		La variable contenant la matrice de transformation de la lumière.
			PushUniform4x4fSPtr m_lightTransform;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine		The engine.
		 *\param[in]	p_frameBuffer	The target framebuffer.
		 *\param[in]	p_depthAttach	The depth buffer attach.
		 *\param[in]	p_shadows		Tells if shadows are enabled for this light pass.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine		Le moteur.
		 *\param[in]	p_frameBuffer	Le tampon d'image cible.
		 *\param[in]	p_depthAttach	L'attache du tampon de profondeur.
		 *\param[in]	p_shadows		Dit si les ombres sont activées pour cette passe d'éclairage.
		 */
		DirectionalLightPass( Engine & p_engine
			, FrameBuffer & p_frameBuffer
			, FrameBufferAttachment & p_depthAttach
			, bool p_shadows );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~DirectionalLightPass();
		/**
		 *\~english
		 *\brief		Initialises the light pass.
		 *\param[in]	p_scene				The scene.
		 *\param[in]	p_sceneUbo			The scene UBO.
		 *\~french
		 *\brief		Initialise la passe d'éclairage.
		 *\param[in]	p_scene				La scène.
		 *\param[in]	p_sceneUbo			L'UBO de scène.
		 */
		void Initialise( Scene const & p_scene
			, SceneUbo & p_sceneUbo )override;
		/**
		 *\~english
		 *\brief		Cleans up the light pass.
		 *\~french
		 *\brief		Nettoie la passe d'éclairage.
		 */
		void Cleanup()override;
		/**
		 *\~english
		 *\return		The number of primitives to draw.
		 *\~french
		 *\return		Le nombre de primitives à dessiner.
		 */
		uint32_t GetCount()const override;

	protected:
		/**
		 *\copydoc		Castor3D::LightPass::DoUpdate
		 */
		void DoUpdate( Castor::Size const & p_size
			, Light const & p_light
			, Camera const & p_camera )override;

	private:
		/**
		 *\copydoc		Castor3D::LightPass::DoGetVertexShaderSource
		 */
		GLSL::Shader DoGetVertexShaderSource( SceneFlags const & p_sceneFlags )const override;
		/**
		 *\copydoc		Castor3D::LightPass::DoCreateProgram
		 */
		LightPass::ProgramPtr DoCreateProgram( GLSL::Shader const & p_vtx
			, GLSL::Shader const & p_pxl )const override;

	private:
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		VertexBufferSPtr m_vertexBuffer;
		//!\~english	The viewport used when rendering is done.
		//!\~french		Le viewport utilisé pour rendre la cible sur sa cible (fenêtre ou texture).
		Viewport m_viewport;
	};
}

#endif
