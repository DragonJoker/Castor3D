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
			, crg::FramePass const & previousPass
			, c3d::RenderDevice const & device
			, crg::ImageViewIdArray const & sceneView
			, crg::ImageViewIdArray const & resultViews
			, c3d::Extent2D size
			, bool const * enabled
			, uint32_t const * passIndex );
		void accept( c3d::ConfigurationVisitorBase & visitor );

		crg::FramePassArray const & getLastPasses()const
		{
			return m_lastPasses;
		}

	private:
		c3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		crg::FramePassArray m_lastPasses;
	};
}

#endif
