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
			, c3d::RenderDevice const & device
			, crg::ImageId const & resultImg
			, c3d::UniformBufferOffsetT< c3d::Point2f > const & ubo
			, uint32_t passesCount
			, bool const * enabled );
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
			, c3d::UniformBufferOffsetT< c3d::Point2f > const & ubo
			, uint32_t passesCount
			, bool const * enabled );

	private:
		crg::FramePassGroup & m_graph;
		c3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		crg::ImageViewIdArray m_resultViews;
		c3d::Vector< crg::FramePass * > m_passes;
	};
}

#endif
