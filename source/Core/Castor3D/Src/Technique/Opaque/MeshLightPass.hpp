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
#ifndef ___C3D_DeferredMeshLightPass_H___
#define ___C3D_DeferredMeshLightPass_H___

#include "StencilPass.hpp"

#include <Shader/Ubos/ModelMatrixUbo.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		08/06/2017
	\~english
	\brief		Base class for light passes that need a mesh instead of a quad.
	\~french
	\brief		Classe de base pour les passes d'éclairage nécessitant un maillage plutôt qu'un quad.
	*/
	class MeshLightPass
		: public LightPass
	{
	protected:
		/*!
		\author		Sylvain DOREMUS
		\version	0.10.0
		\date		08/06/2017
		\~english
		\brief		Base class for light passe programs that need a mesh instead of a quad.
		\~french
		\brief		Classe de base pour les programmes de passe d'éclairage nécessitant un maillage plutôt qu'un quad.
		*/
		struct Program
			: public LightPass::Program
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\param[in]	engine	The engine.
			 *\param[in]	vtx		The vertex shader source.
			 *\param[in]	pxl		The fragment shader source.
			 *\~french
			 *\brief		Constructeur.
			 *\param[in]	engine	Le moteur.
			 *\param[in]	vtx		Le source du vertex shader.
			 *\param[in]	pxl		Le source du fagment shader.
			 */
			Program( Engine & engine
				, glsl::Shader const & vtx
				, glsl::Shader const & pxl );
			/**
			 *\~english
			 *\brief		Destructor.
			 *\~french
			 *\brief		Destructeur.
			 */
			virtual ~Program();

		private:
			/**
			 *\copydoc		castor3d::LightPass::Program::doCreatePipeline
			 */
			RenderPipelineUPtr doCreatePipeline( bool blend )override;

		protected:
			//!\~english	The variable containing the light position.
			//!\~french		La variable contenant la position de la lumière.
			PushUniform3fSPtr m_lightPosition;
			//!\~english	The variable containing the light attenuation.
			//!\~french		La variable contenant l'atténuation de la lumière.
			PushUniform3fSPtr m_lightAttenuation;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	frameBuffer	The target framebuffer.
		 *\param[in]	depthAttach	The depth buffer attach.
		 *\param[in]	gpInfoUbo	The geometry pass UBO.
		 *\param[in]	type		The light source type.
		 *\param[in]	hasShadows	Tells if shadows are enabled for this light pass.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	frameBuffer	Le tampon d'image cible.
		 *\param[in]	depthAttach	L'attache du tampon de profondeur.
		 *\param[in]	gpInfoUbo	L'UBO de la geometry pass.
		 *\param[in]	type		Le type de source lumineuse.
		 *\param[in]	hasShadows	Dit si les ombres sont activées pour cette passe d'éclairage.
		 */
		MeshLightPass( Engine & engine
			, FrameBuffer & frameBuffer
			, FrameBufferAttachment & depthAttach
			, GpInfoUbo & gpInfoUbo
			, LightType type
			, bool hasShadows );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~MeshLightPass();
		/**
		 *\~english
		 *\brief		Initialises the light pass.
		 *\param[in]	scene		The scene.
		 *\param[in]	sceneUbo	The scene UBO.
		 *\~french
		 *\brief		Initialise la passe d'éclairage.
		 *\param[in]	scene		La scène.
		 *\param[in]	sceneUbo	L'UBO de scène.
		 */
		void initialise( Scene const & scene
			, SceneUbo & sceneUbo )override;
		/**
		 *\~english
		 *\brief		Cleans up the light pass.
		 *\~french
		 *\brief		Nettoie la passe d'éclairage.
		 */
		void cleanup()override;
		/**
		 *\~english
		 *\return		The number of primitives to draw.
		 *\~french
		 *\return		Le nombre de primitives à dessiner.
		 */
		uint32_t getCount()const override;

	protected:
		/**
		 *\copydoc		castor3d::LightPass::doUpdate
		 */
		void doUpdate( castor::Size const & size
			, Light const & light
			, Camera const & camera )override;

	private:
		/**
		 *\copydoc		castor3d::LightPass::doGetVertexShaderSource
		 */
		glsl::Shader doGetVertexShaderSource( SceneFlags const & sceneFlags )const override;
		/**
		 *\~english
		 *\return		The vertices needed to draw the mesh.
		 *\~french
		 *\return		Les sommets nécessaires au dessin du maillage.
		 */
		virtual castor::Point3fArray doGenerateVertices()const = 0;
		/**
		 *\~english
		 *\brief		Computes the matrix used to render the model.
		 *\param[in]	light		The light.
		 *\param[in]	camera	The viewing camera.
		 *\return		The matrix.
		 *\~french
		 *\brief		Calcule La matrice utilisée pour dessiner le modèle.
		 *\param[in]	light		La source lumineuse.
		 *\param[in]	camera	La caméra.
		 *\return		La matrice.
		 */
		virtual castor::Matrix4x4r doComputeModelMatrix( Light const & light
			, Camera const & camera )const = 0;

	private:
		//!\~english	The uniform buffer containing the model data.
		//!\~french		Le tampon d'uniformes contenant les données de modèle.
		ModelMatrixUbo m_modelMatrixUbo;
		//!\~english	The light's stencil pass.
		//!\~french		La passe stencil de la lumière.
		StencilPass m_stencilPass;
		//!\~english	The light source type.
		//!\~french		Le type de source lumineuse.
		LightType m_type;
		//!\~english	The vertex count.
		//!\~french		Le nombre de sommets.
		uint32_t m_count;
	};
}

#endif
