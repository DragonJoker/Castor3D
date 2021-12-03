/*
See LICENSE file in root folder
*/
#ifndef ___C3DDE_ObjectIDEdgeDetection_H___
#define ___C3DDE_ObjectIDEdgeDetection_H___

#include <Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp>
#include <Castor3D/Render/PostEffect/PostEffect.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <ShaderAST/Shader.hpp>

namespace draw_edges
{
	class ObjectIDEdgeDetection
	{
	public:
		ObjectIDEdgeDetection( crg::FramePass const & previousPass
			, castor3d::RenderTarget & renderTarget
			, castor3d::RenderDevice const & device
			, castor3d::PassBuffer const & passBuffer
			, crg::ImageViewId const & data0 );
		void accept( castor3d::PipelineVisitorBase & visitor );

		crg::ImageViewId const & getResult()const
		{
			return m_result.sampledViewId;
		}

		crg::FramePass const & getPass()const
		{
			return m_pass;
		}

	private:
		castor3d::RenderDevice const & m_device;
		crg::FrameGraph & m_graph;
		VkExtent3D m_extent;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		castor3d::Texture m_result;
		crg::FramePass & m_pass;
	};
}

#endif
