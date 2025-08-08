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
			, c3d::RenderTarget & renderTarget
			, c3d::RenderDevice const & device
			, SmaaUbo const & ubo
			, crg::Attachment const & neighbourResult
			, crg::ImageViewIdArray const & currentColourViews
			, crg::ImageViewIdArray const & previousColourViews
			, c3d::Texture const * velocityView
			, SmaaConfig const & config
			, bool const * enabled );
		~Reproject();

		void accept( c3d::ConfigurationVisitorBase & visitor );

		c3d::Texture const & getResult()const
		{
			return m_result;
		}

	private:
		crg::FramePassGroup & m_graph;
		c3d::Extent3D m_extent;
		c3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		c3d::Texture m_result;
	};
}

#endif
