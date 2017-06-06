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
#ifndef ___C3D_DeferredCombinePass_H___
#define ___C3D_DeferredCombinePass_H___

#include "LightPass.hpp"
#include "SsaoPass.hpp"

#include "Shader/MatrixUbo.hpp"
#include "Shader/SceneUbo.hpp"

namespace Castor3D
{
	struct CombineProgram
	{
		CombineProgram( CombineProgram const & p_rhs ) = delete;
		CombineProgram & operator=( CombineProgram const & p_rhs ) = delete;
		CombineProgram( CombineProgram && p_rhs ) = default;
		CombineProgram & operator=( CombineProgram && p_rhs ) = default;

		CombineProgram( Engine & p_engine
			, VertexBuffer & p_vbo
			, MatrixUbo & p_matrixUbo
			, SceneUbo & p_sceneUbo
			, GpInfo & p_gpInfo
			, bool p_ssao
			, GLSL::FogType p_fogType );
		~CombineProgram();

		void Render()const;
		//!\~english	The shader program.
		//!\~french		Le shader program.
		ShaderProgramSPtr m_program;
		//!\~english	The geometry buffers.
		//!\~french		Les tampons de géométrie.
		GeometryBuffersSPtr m_geometryBuffers;
		//!\~english	The render pipeline.
		//!\~french		Le pipeline de rendu.
		RenderPipelineUPtr m_pipeline;
	};

	using CombinePrograms = std::array< CombineProgram, size_t( GLSL::FogType::eCount ) >;

	class CombinePass
	{
	public:
		CombinePass( Engine & p_engine
			, Castor::Size const & p_size
			, SsaoConfig const & p_config );
		~CombinePass();
		void Render( GeometryPassResult const & p_gp
			, TextureUnit const & p_lp
			, Camera const & p_camera
			, Castor::Matrix4x4r const & p_invViewProj
			, Castor::Matrix4x4r const & p_invView
			, Castor::Matrix4x4r const & p_invProj
			, Fog const & p_fog
			, FrameBuffer const & p_frameBuffer );

		inline TextureLayout const & GetSsao()const
		{
			return m_ssao.GetRaw();
		}

	private:
		//!\~english	The render size.
		//!\~french		La taille du rendu.
		Castor::Size m_size;
		//!\~english	The render viewport.
		//!\~french		La viewport du rendu.
		Viewport m_viewport;
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		VertexBufferSPtr m_vertexBuffer;
		//!\~english	The matrices uniform buffer.
		//!\~french		Le tampon d'uniformes contenant les matrices.
		MatrixUbo m_matrixUbo;
		//!\~english	The scene uniform buffer.
		//!\~french		Le tampon d'uniformes contenant les informations de la scène.
		SceneUbo m_sceneUbo;
		//!\~english	The geometry pass informations.
		//!\~french		Les informations de la passe de géométrie.
		GpInfo m_gpInfo;
		//!\~english	The shader program.
		//!\~french		Le shader program.
		CombinePrograms m_programs;
		//!\~english	Tells if SSAO is to be used in lighting pass.
		//!\~french		Dit si le SSAO doit être utilisé dans la light pass.
		bool m_ssaoEnabled{ false };
		//!\~english	The SSAO pass.
		//!\~french		La passe SSAO.
		SsaoPass m_ssao;
	};
}

#endif
