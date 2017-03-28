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
#ifndef ___C3D_SsaoPass_H___
#define ___C3D_SsaoPass_H___

#include "LightPass.hpp"

#include <RenderToTexture/RenderColourToTexture.hpp>

namespace deferred_common
{
	class SsaoPass
	{
	public:
		SsaoPass( Castor3D::Engine & p_engine
			, Castor::Size const & p_size );
		~SsaoPass();
		void Render( GeometryPassResult const & p_gp
			, Castor3D::Camera const & p_camera );

		inline Castor3D::TextureLayout const & GetResult()
		{
			return *m_blurResult;
		}

		inline Castor3D::TextureLayout const & GetRaw()
		{
			return *m_ssaoResult;
		}

	private:
		void DoInitialiseQuadRendering();
		void DoInitialiseSsaoPass();
		void DoInitialiseBlurPass();
		void DoCleanupQuadRendering();
		void DoCleanupSsaoPass();
		void DoCleanupBlurPass();
		void DoRenderSsao( GeometryPassResult const & p_gp );
		void DoRenderBlur();

	private:
		Castor3D::Engine & m_engine;
		// Quad rendering
		Castor::Size m_size;
		Castor3D::UniformBuffer m_matrixUbo;
		Castor3D::Uniform4x4fSPtr m_viewMatrix;
		Castor3D::Uniform4x4fSPtr m_projectionMatrix;
		Castor3D::Viewport m_viewport;
		std::array< Castor::real, 6 * (3 + 2) > m_bufferVertex;
		Castor3D::BufferDeclaration m_declaration;
		std::array< Castor3D::BufferElementGroupSPtr, 6 > m_arrayVertex;
		Castor3D::VertexBufferSPtr m_vertexBuffer;
		// Raw SSAO pass
		Castor3D::FrameBufferSPtr m_ssaoFbo;
		Castor3D::GeometryBuffersSPtr m_ssaoGeometryBuffers;
		Castor3D::RenderPipelineUPtr m_ssaoPipeline;
		Castor3D::ShaderProgramSPtr m_ssaoProgram;
		Castor::Point3fArray m_ssaoKernel;
		Castor3D::TextureUnit m_ssaoNoise;
		Castor3D::TextureLayoutSPtr m_ssaoResult;
		Castor3D::TextureAttachmentSPtr m_ssaoResultAttach;
		Castor3D::UniformBuffer m_ssaoConfig;
		Castor3D::Uniform3fSPtr m_kernelUniform;
		// SSAO blur pass
		Castor3D::FrameBufferSPtr m_blurFbo;
		Castor3D::GeometryBuffersSPtr m_blurGeometryBuffers;
		Castor3D::RenderPipelineUPtr m_blurPipeline;
		Castor3D::ShaderProgramSPtr m_blurProgram;
		Castor3D::TextureLayoutSPtr m_blurResult;
		Castor3D::TextureAttachmentSPtr m_blurResultAttach;

	};
}

#endif
