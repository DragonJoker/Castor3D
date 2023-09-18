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
			, castor3d::RenderDevice const & device
			, crg::ImageViewIdArray const & sceneView
			, crg::ImageId const & resultImg
			, uint32_t passesCount
			, bool const * enabled
			, uint32_t const * passIndex );
		void accept( castor3d::ConfigurationVisitorBase & visitor );

		crg::FramePass const & getPass()const
		{
			return *m_passes.back();
		}

	private:
		crg::ImageViewIdArray doCreateResultViews( crg::FramePassGroup & graph
			, crg::ImageId const & resultImg
			, uint32_t passesCount );
		std::vector< crg::FramePass * > doCreatePasses( crg::FramePassGroup & graph
			, crg::FramePass const & previousPass
			, castor3d::RenderDevice const & device
			, crg::ImageViewIdArray const & sceneView
			, uint32_t passesCount
			, bool const * enabled
			, uint32_t const * passIndex );

	private:
		crg::FramePassGroup & m_graph;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		crg::ImageViewId m_resultView;
		crg::ImageViewIdArray m_resultViews;
		std::vector< crg::RenderQuad * > m_quads;
		std::vector< crg::FramePass * > m_passes;
	};
}

#endif
