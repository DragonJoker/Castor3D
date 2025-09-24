/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightStreaks_HiPass___
#define ___C3D_LightStreaks_HiPass___

#include <Castor3D/Render/PostEffect/PostEffect.hpp>

namespace light_streaks
{
	class HiPass
	{
	public:
		HiPass( crg::FramePassGroup & graph
			, c3d::RenderDevice const & device
			, c3d::Texture const & sceneView
			, c3d::Texture & result
			, c3d::Extent2D size
			, bool const * enabled
			, uint32_t const * passIndex );
		void accept( c3d::ConfigurationVisitorBase & visitor )const;

	private:
		c3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
	};
}

#endif
