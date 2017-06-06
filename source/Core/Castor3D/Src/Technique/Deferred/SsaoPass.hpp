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
#ifndef ___C3D_DeferredSsaoPass_H___
#define ___C3D_DeferredSsaoPass_H___

#include "LightPass.hpp"

#include "Miscellaneous/SsaoConfig.hpp"
#include "Shader/MatrixUbo.hpp"
#include "Texture/TextureUnit.hpp"

namespace Castor3D
{
	class SsaoPass
	{
	public:
		SsaoPass( Engine & p_engine
			, Castor::Size const & p_size
			, SsaoConfig const & p_config );
		~SsaoPass();
		void Render( GeometryPassResult const & p_gp
			, Camera const & p_camera
			, Castor::Matrix4x4r const & p_invViewProj
			, Castor::Matrix4x4r const & p_invView
			, Castor::Matrix4x4r const & p_invProj );

		inline TextureUnit const & GetResult()const
		{
			return m_blurResult;
		}

		inline TextureLayout const & GetRaw()const
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
		Engine & m_engine;
		SsaoConfig m_config;
		// Quad rendering
		Castor::Size m_size;
		MatrixUbo m_matrixUbo;
		Viewport m_viewport;
		// Raw SSAO pass
		VertexBufferSPtr m_ssaoVertexBuffer;
		GeometryBuffersSPtr m_ssaoGeometryBuffers;
		RenderPipelineUPtr m_ssaoPipeline;
		ShaderProgramSPtr m_ssaoProgram;
		Castor::Point3fArray m_ssaoKernel;
		TextureUnit m_ssaoNoise;
		TextureUnit m_ssaoResult;
		FrameBufferSPtr m_ssaoFbo;
		TextureAttachmentSPtr m_ssaoResultAttach;
		UniformBuffer m_ssaoConfig;
		Uniform3fSPtr m_kernelUniform;
		std::unique_ptr< GpInfo > m_gpInfo;
		// SSAO blur pass
		VertexBufferSPtr m_blurVertexBuffer;
		GeometryBuffersSPtr m_blurGeometryBuffers;
		RenderPipelineUPtr m_blurPipeline;
		ShaderProgramSPtr m_blurProgram;
		TextureUnit m_blurResult;
		FrameBufferSPtr m_blurFbo;
		TextureAttachmentSPtr m_blurResultAttach;

	};
}

#endif
