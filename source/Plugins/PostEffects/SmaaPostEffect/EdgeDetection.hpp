/*
See LICENSE file in root folder
*/
#ifndef ___C3DSMAA_EdgeDetection_H___
#define ___C3DSMAA_EdgeDetection_H___

#include "SmaaPostEffect/SmaaConfig.hpp"

#include <Castor3D/Render/PostEffect/PostEffect.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <ShaderAST/Shader.hpp>

namespace smaa
{
	class EdgeDetection
	{
	public:
		EdgeDetection( crg::FramePass const & previousPass
			, castor3d::RenderTarget & renderTarget
			, castor3d::RenderDevice const & device
			, SmaaConfig const & config
			, std::unique_ptr< ast::Shader > pixelShader
			, bool const * enabled );
		void accept( castor3d::PipelineVisitorBase & visitor );

		crg::ImageViewId const & getColourResult()const
		{
			return m_outColour.sampledViewId;
		}

		crg::ImageViewId const & getDepthResult()const
		{
			return m_outDepthStencilView;
		}

		crg::FramePass const & getPass()const
		{
			return m_pass;
		}

	protected:
		castor3d::RenderDevice const & m_device;
		crg::FrameGraph & m_graph;
		SmaaConfig const & m_config;
		VkExtent3D m_extent;
		castor3d::Texture m_outColour;
		castor3d::Texture m_outDepth;
		crg::ImageViewId m_outDepthStencilView;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		crg::FramePass & m_pass;
	};
}

#endif
