﻿/*
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

#include <Shader/MatrixUbo.hpp>
#include <Texture/TextureUnit.hpp>

namespace deferred_common
{
	class SsaoPass
	{
	public:
		SsaoPass( Castor3D::Engine & p_engine
			, Castor::Size const & p_size );
		~SsaoPass();
		void Render( GeometryPassResult const & p_gp
			, Castor3D::Camera const & p_camera
			, Castor::Matrix4x4r const & p_invViewProj
			, Castor::Matrix4x4r const & p_invView
			, Castor::Matrix4x4r const & p_invProj );

		inline Castor3D::TextureUnit const & GetResult()const
		{
			return m_blurResult;
		}

		inline Castor3D::TextureLayout const & GetRaw()const
		{
			return *m_ssaoResult.GetTexture();
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
		Castor3D::MatrixUbo m_matrixUbo;
		Castor3D::Viewport m_viewport;
		// Raw SSAO pass
		Castor3D::FrameBufferSPtr m_ssaoFbo;
		Castor3D::VertexBufferSPtr m_ssaoVertexBuffer;
		Castor3D::GeometryBuffersSPtr m_ssaoGeometryBuffers;
		Castor3D::RenderPipelineUPtr m_ssaoPipeline;
		Castor3D::ShaderProgramSPtr m_ssaoProgram;
		Castor::Point3fArray m_ssaoKernel;
		Castor3D::TextureUnit m_ssaoNoise;
		Castor3D::TextureUnit m_ssaoResult;
		Castor3D::TextureAttachmentSPtr m_ssaoResultAttach;
		Castor3D::UniformBuffer m_ssaoConfig;
		Castor3D::Uniform3fSPtr m_kernelUniform;
		std::unique_ptr< GpInfo > m_gpInfo;
		// SSAO blur pass
		Castor3D::FrameBufferSPtr m_blurFbo;
		Castor3D::VertexBufferSPtr m_blurVertexBuffer;
		Castor3D::GeometryBuffersSPtr m_blurGeometryBuffers;
		Castor3D::RenderPipelineUPtr m_blurPipeline;
		Castor3D::ShaderProgramSPtr m_blurProgram;
		Castor3D::TextureUnit m_blurResult;
		Castor3D::TextureAttachmentSPtr m_blurResultAttach;

	};
}

#endif
