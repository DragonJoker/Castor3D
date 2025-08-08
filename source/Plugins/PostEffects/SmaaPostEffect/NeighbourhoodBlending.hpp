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
	class SmaaUbo;

	class NeighbourhoodBlending
	{
	public:
		NeighbourhoodBlending( crg::FramePassGroup & graph
			, c3d::RenderTarget & renderTarget
			, c3d::RenderDevice const & device
			, SmaaUbo const & ubo
			, c3d::Texture const & sourceView
			, c3d::Texture const & blendView
			, c3d::Texture const * velocityView
			, SmaaConfig const & config
			, bool const * enabled
			, uint32_t const * passIndex );
		~NeighbourhoodBlending();

		void accept( c3d::ConfigurationVisitorBase & visitor );

		crg::Attachment const * getResult()const
		{
			return m_result;
		}

		crg::ImageViewIdArray const & getViews()const
		{
			return m_imageViews;
		}

	private:
		c3d::RenderDevice const & m_device;
		crg::FramePassGroup & m_graph;
		c3d::Extent3D m_extent;
		c3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		c3d::TextureArray m_images;
		crg::ImageViewIdArray m_imageViews;
		crg::Attachment const * m_result{};
	};
}

#endif
