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
			, crg::ImageViewId const & sceneView
			, crg::ImageId const & resultImg
			, uint32_t passesCount
			, bool const * enabled );
		void accept( castor3d::PipelineVisitorBase & visitor );

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
			, uint32_t passesCount
			, bool const * enabled );

	private:
		crg::FramePassGroup & m_graph;
		crg::ImageViewId const & m_sceneView;
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
