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
#ifndef ___C3D_PointLightPass_H___
#define ___C3D_PointLightPass_H___

#include "LightPass.hpp"

namespace deferred
{
	class PointLightPass
		: public LightPass
	{
	public:
		PointLightPass( Castor3D::Engine & p_engine
			, Castor3D::UniformBuffer & p_matrixUbo
			, Castor3D::UniformBuffer & p_sceneUbo );
		void Create( Castor3D::Scene const & p_scene );
		void Destroy();
		void Initialise();
		void Cleanup();
		void Render( Castor::Size const & p_size
			, GeometryPassResult const & p_gp
			, Castor3D::PointLight const & p_light
			, Castor3D::Camera const & p_camera
			, uint16_t p_fogType
			, bool p_first );

	private:
		Castor::String DoGetVertexShaderSource( Castor3D::SceneFlags const & p_sceneFlags )const;
		Castor::String DoGetPixelShaderSource( Castor3D::SceneFlags const & p_sceneFlags )const;

	private:
		struct Program
			: public LightPass::Program
		{
		public:
			void Create( Castor3D::Scene const & p_scene
				, Castor3D::UniformBuffer & p_matrixUbo
				, Castor3D::UniformBuffer & p_sceneUbo
				, Castor3D::UniformBuffer & p_modelMatrixUbo
				, Castor::String const & p_vtx
				, Castor::String const & p_pxl
				, uint16_t p_fogType );
			void Destroy();
			void Initialise( Castor3D::VertexBuffer & p_vbo
				, Castor3D::IndexBufferSPtr p_ibo
				, Castor3D::UniformBuffer & p_matrixUbo
				, Castor3D::UniformBuffer & p_sceneUbo
				, Castor3D::UniformBuffer & p_modelMatrixUbo );
			void Cleanup();
			void Render( Castor::Size const & p_size
				, Castor3D::PointLight const & p_light
				, Castor::Matrix4x4r const & p_projection
				, Castor::Matrix4x4r const & p_view
				, Castor3D::UniformBuffer & p_matrixUbo
				, Castor3D::UniformBuffer & p_sceneUbo
				, Castor3D::UniformBuffer & p_modelMatrixUbo
				, uint32_t p_count
				, bool p_first );

		public:
			//!\~english	The variable containing the light position.
			//!\~french		La variable contenant la position de la lumière.
			Castor3D::PushUniform3fSPtr m_lightPosition;
			//!\~english	The variable containing the light attenuation.
			//!\~french		La variable contenant l'atténuation de la lumière.
			Castor3D::PushUniform3fSPtr m_lightAttenuation;
			//!\~english	The uniform variable containing view matrix.
			//!\~french		La variable uniforme contenant la matrice vue.
			Castor3D::Uniform4x4fSPtr m_viewUniform;
			//!\~english	The uniform variable containing model matrix.
			//!\~french		La variable uniforme contenant la matrice modèle.
			Castor3D::Uniform4x4fSPtr m_modelUniform;
		};
		using Programs = std::array< Program, size_t( GLSL::FogType::eCount ) >;

	private:
		//!\~english	The uniform buffer containing the model data.
		//!\~french		Le tampon d'uniformes contenant les données de modèle.
		Castor3D::UniformBuffer m_modelMatrixUbo;
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		Castor3D::VertexBufferSPtr m_vertexBuffer;
		//!\~english	The index buffer.
		//!\~french		Le tampon d'indices.
		Castor3D::IndexBufferSPtr m_indexBuffer;
		//!\~english	The light pass' programs.
		//!\~french		Les programme de la passe de lumière.
		Programs m_programs;
	};
}

#endif
