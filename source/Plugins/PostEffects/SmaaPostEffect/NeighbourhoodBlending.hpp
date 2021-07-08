/*
See LICENSE file in root folder
*/
#ifndef ___C3DSMAA_NeighbourhoodBlending_H___
#define ___C3DSMAA_NeighbourhoodBlending_H___

#include "SmaaPostEffect/SmaaConfig.hpp"

#include <Castor3D/Render/PostEffect/PostEffect.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <ShaderAST/Shader.hpp>

namespace smaa
{
	class NeighbourhoodBlending
	{
	public:
		NeighbourhoodBlending( crg::FramePass const & previousPass
			, castor3d::RenderTarget & renderTarget
			, castor3d::RenderDevice const & device
			, crg::ImageViewId const & sourceView
			, crg::ImageViewId const & blendView
			, crg::ImageViewId const * velocityView
			, SmaaConfig const & config );
		void accept( castor3d::PipelineVisitorBase & visitor );

		crg::ImageViewIdArray const & getResult()const
		{
			return m_imageViews;
		}

		crg::FramePass const & getPass()const
		{
			return m_pass;
		}

	private:
		castor3d::RenderDevice const & m_device;
		crg::FrameGraph & m_graph;
		crg::ImageViewId const & m_sourceView;
		crg::ImageViewId const & m_blendView;
		crg::ImageViewId const * m_velocityView;
		VkExtent3D m_extent;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		castor3d::TextureArray m_images;
		crg::ImageViewIdArray m_imageViews;
		crg::FramePass & m_pass;
	};
}

#endif
