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
#ifndef ___C3D_FogPass_H___
#define ___C3D_FogPass_H___

#include "LightPass.hpp"

#include <Shader/MatrixUbo.hpp>
#include <Shader/SceneUbo.hpp>

namespace deferred_common
{
	struct FogProgram
	{
		FogProgram( FogProgram const & p_rhs ) = delete;
		FogProgram & operator=( FogProgram const & p_rhs ) = delete;
		FogProgram( FogProgram && p_rhs ) = default;
		FogProgram & operator=( FogProgram && p_rhs ) = default;

		FogProgram( Castor3D::Engine & p_engine
			, Castor3D::VertexBuffer & p_vbo
			, Castor3D::MatrixUbo & p_matrixUbo
			, Castor3D::SceneUbo & p_sceneUbo
			, GpInfo & p_gpInfo
			, GLSL::FogType p_fogType );
		~FogProgram();
		void Render()const;
		//!\~english	The shader program.
		//!\~french		Le shader program.
		Castor3D::ShaderProgramSPtr m_program;
		//!\~english	The geometry buffers.
		//!\~french		Les tampons de géométrie.
		Castor3D::GeometryBuffersSPtr m_geometryBuffers;
		//!\~english	The render pipeline.
		//!\~french		Le pipeline de rendu.
		Castor3D::RenderPipelineUPtr m_pipeline;
	};

	using FogPrograms = std::array< FogProgram, size_t( GLSL::FogType::eCount ) >;

	class FogPass
	{
	public:
		FogPass( Castor3D::Engine & p_engine
			, Castor::Size const & p_size );
		~FogPass();
		void Render( GeometryPassResult const & p_gp
			, Castor3D::TextureUnit const & p_lp
			, Castor3D::Camera const & p_camera
			, Castor::Matrix4x4r const & p_invViewProj
			, Castor::Matrix4x4r const & p_invView
			, Castor::Matrix4x4r const & p_invProj
			, Castor3D::Fog const & p_fog );

	private:
		//!\~english	The render size.
		//!\~french		La taille du rendu.
		Castor::Size m_size;
		//!\~english	The render viewport.
		//!\~french		La viewport du rendu.
		Castor3D::Viewport m_viewport;
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		Castor3D::VertexBufferSPtr m_vertexBuffer;
		//!\~english	The matrices uniform buffer.
		//!\~french		Le tampon d'uniformes contenant les matrices.
		Castor3D::MatrixUbo m_matrixUbo;
		//!\~english	The scene uniform buffer.
		//!\~french		Le tampon d'uniformes contenant les informations de la scène.
		Castor3D::SceneUbo m_sceneUbo;
		//!\~english	The geometry pass informations.
		//!\~french		Les informations de la passe de géométrie.
		GpInfo m_gpInfo;
		//!\~english	The shader program.
		//!\~french		Le shader program.
		FogPrograms m_programs;

	};
}

#endif
