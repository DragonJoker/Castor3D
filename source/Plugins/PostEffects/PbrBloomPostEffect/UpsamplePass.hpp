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
			, c3d::RenderDevice const & device
			, c3d::Texture const & sourceImg
			, c3d::Vector< c3d::Texture > & resultImg
			, c3d::UniformBufferOffsetT< c3d::Point2f > const & ubo
			, uint32_t passesCount
			, bool const * enabled );
		void accept( c3d::ConfigurationVisitorBase & visitor )const;

	private:
		c3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
	};
}

#endif
