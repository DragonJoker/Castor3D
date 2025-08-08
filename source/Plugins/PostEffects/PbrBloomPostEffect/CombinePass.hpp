/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PbrBloom_CombinePass_HPP___
#define ___C3D_PbrBloom_CombinePass_HPP___

#include <Castor3D/Render/PostEffect/PostEffect.hpp>

#include <ashespp/Image/ImageView.hpp>

namespace PbrBloom
{
	class CombinePass
	{
	public:
		CombinePass( crg::FramePassGroup & graph
			, c3d::RenderDevice const & device
			, c3d::Texture const & blurredView
			, c3d::Texture const & sceneView
			, c3d::Texture & resultView
			, c3d::Extent2D const & size
			, c3d::UniformBufferOffsetT< c3d::Point2f > const & ubo
			, bool const * enabled
			, uint32_t const * passIndex );
		void accept( c3d::ConfigurationVisitorBase & visitor );

	public:
		static c3d::MbString const CombineMapPasses;
		static c3d::MbString const CombineMapScene;

	private:
		c3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
	};
}

#endif
