/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PbrBloom_UpsamplePass___
#define ___C3D_PbrBloom_UpsamplePass___

#include <Castor3D/Render/PostEffect/PostEffect.hpp>

namespace PbrBloom
{
	class UpsamplePass
	{
	public:
		UpsamplePass( crg::FramePassGroup & graph
			, crg::FramePass const & previousPass
			, castor3d::RenderDevice const & device
			, crg::ImageId const & resultImg
			, castor3d::UniformBufferOffsetT< castor::Point2f > const & ubo
			, uint32_t passesCount
			, bool const * enabled );
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
			, castor3d::UniformBufferOffsetT< castor::Point2f > const & ubo
			, uint32_t passesCount
			, bool const * enabled );

	private:
		crg::FramePassGroup & m_graph;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		crg::ImageViewIdArray m_resultViews;
		std::vector< crg::FramePass * > m_passes;
	};
}

#endif
