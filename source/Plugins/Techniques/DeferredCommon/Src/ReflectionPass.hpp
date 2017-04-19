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
#ifndef ___C3DDF_ReflectionPass_H___
#define ___C3DDF_ReflectionPass_H___

#include "LightPass.hpp"
#include <EnvironmentMap/EnvironmentMap.hpp>

namespace deferred_common
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		06/04/2017
	\~english
	\brief		The post lighting reflection pass.
	\~french
	\brief		La passe de réflexion post éclairage.
	*/
	class ReflectionPass
		: Castor::OwnedBy< Castor3D::Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 */
		ReflectionPass( Castor3D::Engine & p_engine
			, Castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~ReflectionPass();
		/**
		 *\~english
		 *\brief		Renders the reflection mapping.
		 *\~french
		 *\brief		Dessine le mapping de réflexion.
		 */
		void Render( GeometryPassResult & p_gp
			, Castor3D::TextureLayout const & p_lp
			, Castor3D::Scene const & p_scene
			, Castor3D::Camera const & p_camera
			, Castor::Matrix4x4r const & p_invViewProj
			, Castor::Matrix4x4r const & p_invView
			, Castor::Matrix4x4r const & p_invProj );

		inline Castor3D::TextureUnit const & GetResult()const
		{
			return m_result;
		}

	private:
		//!\~english	The render size.
		//!\~french		La taille du rendu.
		Castor::Size m_size;
		//!\~english	The light pass output.
		//!\~french		La sortie de la passe de lumières.
		Castor3D::TextureUnit m_result;
		//!\~english	The target FBO.
		//!\~french		Le FBO cible.
		Castor3D::FrameBufferSPtr m_frameBuffer;
		//!\~english	The attachments between textures and deferred shading frame buffer.
		//!\~french		Les attaches entre les textures et le tampon deferred shading.
		Castor3D::TextureAttachmentSPtr m_resultAttach;
		//!\~english	The render viewport.
		//!\~french		La viewport du rendu.
		Castor3D::Viewport m_viewport;
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		Castor3D::VertexBufferSPtr m_vertexBuffer;
		//!\~english	The shader program.
		//!\~french		Le shader program.
		Castor3D::ShaderProgramSPtr m_program;
		//!\~english	The geometry buffers.
		//!\~french		Les tampons de géométrie.
		Castor3D::GeometryBuffersSPtr m_geometryBuffers;
		//!\~english	The matrices uniform buffer.
		//!\~french		Le tampon d'uniformes contenant les matrices.
		Castor3D::MatrixUbo m_matrixUbo;
		//!\~english	The scene uniform buffer.
		//!\~french		Le tampon d'uniformes contenant les informations de la scène.
		Castor3D::SceneUbo m_sceneUbo;
		//!\~english	The geometry pass informations.
		//!\~french		Les informations de la passe de géométrie.
		GpInfo m_gpInfo;
		//!\~english	The HDR configuration.
		//!\~french		La configuration HDR.
		Castor3D::UniformBuffer m_configUbo;
		//!\~english	The render pipeline.
		//!\~french		Le pipeline de rendu.
		Castor3D::RenderPipelineUPtr m_pipeline;
		//!\~english	The exposure shader variable.
		//!\~french		La variable shader pour l'exposition.
		Castor3D::Uniform1fSPtr m_exposureUniform;
		//!\~english	The gamma correction shader variable.
		//!\~french		La variable shader pour la correction gamma.
		Castor3D::Uniform1fSPtr m_gammaUniform;
	};
}

#endif
