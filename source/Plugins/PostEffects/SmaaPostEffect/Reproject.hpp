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
		Reproject( crg::FramePassGroup & graph
			, crg::FramePass const & previousPass
			, c3d::RenderTarget & renderTarget
			, c3d::RenderDevice const & device
			, SmaaUbo const & ubo
			, crg::ImageViewIdArray const & currentColourViews
			, crg::ImageViewIdArray const & previousColourViews
			, crg::ImageViewId const * velocityView
			, SmaaConfig const & config
			, bool const * enabled );
		~Reproject();

		void accept( c3d::ConfigurationVisitorBase & visitor );

		crg::ImageViewId const & getResult()const
		{
			return m_result.sampledViewId;
		}

		crg::FramePass const & getPass()const
		{
			return m_pass;
		}

	private:
		c3d::RenderDevice const & m_device;
		crg::FramePassGroup & m_graph;
		crg::ImageViewIdArray m_currentColourViews;
		crg::ImageViewIdArray m_previousColourViews;
		crg::ImageViewId const * m_velocityView;
		c3d::Extent3D m_extent;
		c3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		c3d::Texture m_result;
		crg::FramePass & m_pass;
	};
}

#endif
