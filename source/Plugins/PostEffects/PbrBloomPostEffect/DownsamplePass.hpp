/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PbrBloom_DownsamplePass___
#define ___C3D_PbrBloom_DownsamplePass___

#include <Castor3D/Render/PostEffect/PostEffect.hpp>

namespace PbrBloom
{
	class DownsamplePass
	{
	public:
		DownsamplePass( crg::FramePassGroup & graph
			, crg::FramePass const & previousPass
			, c3d::RenderDevice const & device
			, crg::ImageViewIdArray const & sceneView
			, crg::ImageId const & resultImg
			, uint32_t passesCount
			, bool const * enabled
			, uint32_t const * passIndex );
		void accept( c3d::ConfigurationVisitorBase & visitor );

		crg::FramePass const & getPass()const
		{
			return *m_passes.back();
		}

	private:
		crg::ImageViewIdArray doCreateResultViews( crg::FramePassGroup & graph
			, crg::ImageId const & resultImg
			, uint32_t passesCount );
		c3d::Vector< crg::FramePass * > doCreatePasses( crg::FramePassGroup & graph
			, crg::FramePass const & previousPass
			, c3d::RenderDevice const & device
			, crg::ImageViewIdArray const & sceneView
			, uint32_t passesCount
			, bool const * enabled
			, uint32_t const * passIndex );

	private:
		crg::FramePassGroup & m_graph;
		c3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		crg::ImageViewId m_resultView;
		crg::ImageViewIdArray m_resultViews;
		c3d::Vector< crg::RenderQuad * > m_quads;
		c3d::Vector< crg::FramePass * > m_passes;
	};
}

#endif
