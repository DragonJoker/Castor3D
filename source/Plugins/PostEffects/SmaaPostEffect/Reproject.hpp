/*
See LICENSE file in root folder
*/
#ifndef ___C3DSMAA_Reproject_H___
#define ___C3DSMAA_Reproject_H___

#include "SmaaPostEffect/SmaaConfig.hpp"

#include <Castor3D/Render/PostEffect/PostEffect.hpp>

#include <ShaderAST/Shader.hpp>

namespace smaa
{
	class SmaaUbo;

	class Reproject
	{
	public:
		Reproject( crg::FramePass const & previousPass
			, castor3d::RenderTarget & renderTarget
			, castor3d::RenderDevice const & device
			, SmaaUbo const & ubo
			, crg::ImageViewIdArray const & currentColourViews
			, crg::ImageViewIdArray const & previousColourViews
			, crg::ImageViewId const * velocityView
			, SmaaConfig const & config
			, bool const * enabled );
		~Reproject();

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
		crg::ImageViewIdArray m_currentColourViews;
		crg::ImageViewIdArray m_previousColourViews;
		crg::ImageViewId const * m_velocityView;
		VkExtent3D m_extent;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		castor3d::Texture m_result;
		crg::FramePass & m_pass;
	};
}

#endif
