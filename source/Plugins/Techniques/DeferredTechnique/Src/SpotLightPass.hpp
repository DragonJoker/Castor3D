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
#ifndef ___C3D_SpotLightPass_H___
#define ___C3D_SpotLightPass_H___

#include "StencilPass.hpp"

namespace deferred
{
	class SpotLightPass
		: public LightPass
	{
	protected:
		struct Program
			: public LightPass::Program
		{
		public:
			Program( Castor3D::Scene const & p_scene
				, Castor::String const & p_vtx
				, Castor::String const & p_pxl );
			virtual ~Program();

		private:
			void DoBind( Castor3D::Light const & p_light )override;

		private:
			//!\~english	The variable containing the light position.
			//!\~french		La variable contenant la position de la lumière.
			Castor3D::PushUniform3fSPtr m_lightPosition;
			//!\~english	The variable containing the light attenuation.
			//!\~french		La variable contenant l'atténuation de la lumière.
			Castor3D::PushUniform3fSPtr m_lightAttenuation;
			//!\~english	The variable containing the light direction.
			//!\~french		La variable contenant la direction de la lumière.
			Castor3D::PushUniform3fSPtr m_lightDirection;
			//!\~english	The variable containing the light exponent.
			//!\~french		La variable contenant l'exposant de la lumière.
			Castor3D::PushUniform1fSPtr m_lightExponent;
			//!\~english	The variable containing the light cut off.
			//!\~french		La variable contenant l'angle du cône de la lumière.
			Castor3D::PushUniform1fSPtr m_lightCutOff;
			//!\~english	The variable containing the light space transformation matrix.
			//!\~french		La variable contenant la matrice de transformation de la lumière.
			Castor3D::PushUniform4x4fSPtr m_lightTransform;
		};
		using ProgramPtr = std::unique_ptr< Program >;
		using Programs = std::array< ProgramPtr, size_t( GLSL::FogType::eCount ) >;

	public:
		SpotLightPass( Castor3D::Engine & p_engine
			, Castor3D::FrameBuffer & p_frameBuffer
			, Castor3D::RenderBufferAttachment & p_depthAttach
			, bool p_shadows );
		~SpotLightPass();
		void Initialise( Castor3D::Scene const & p_scene
			, Castor3D::UniformBuffer & p_sceneUbo )override;
		void Cleanup()override;
		uint32_t GetCount()const override;

	protected:
		void DoUpdate( Castor::Size const & p_size
			, Castor3D::Light const & p_light
			, Castor3D::Camera const & p_camera )override;

	private:
		Castor::String DoGetVertexShaderSource( Castor3D::SceneFlags const & p_sceneFlags )const;
		LightPass::ProgramPtr DoCreateProgram( Castor3D::Scene const & p_scene
			, Castor::String const & p_vtx
			, Castor::String const & p_pxl )override;

	private:
		//!\~english	The uniform buffer containing the model data.
		//!\~french		Le tampon d'uniformes contenant les données de modèle.
		Castor3D::UniformBuffer m_modelMatrixUbo;
		//!\~english	The uniform variable containing projection matrix.
		//!\~french		La variable uniforme contenant la matrice projection.
		Castor3D::Uniform4x4fSPtr m_projectionUniform;
		//!\~english	The uniform variable containing view matrix.
		//!\~french		La variable uniforme contenant la matrice vue.
		Castor3D::Uniform4x4fSPtr m_viewUniform;
		//!\~english	The uniform variable containing model matrix.
		//!\~french		La variable uniforme contenant la matrice modèle.
		Castor3D::Uniform4x4fSPtr m_modelUniform;
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		Castor3D::VertexBufferSPtr m_vertexBuffer;
		//!\~english	The index buffer.
		//!\~french		Le tampon d'indices.
		Castor3D::IndexBufferSPtr m_indexBuffer;
		//!\~english	The light's stencil pass.
		//!\~french		La passe stencil de la lumière.
		StencilPass m_stencilPass;
	};
}

#endif
