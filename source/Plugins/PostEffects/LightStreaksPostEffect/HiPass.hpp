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
			, castor3d::RenderDevice const & device
			, crg::ImageViewId const & sceneView
			, crg::ImageViewIdArray const & resultViews
			, VkExtent2D size
			, bool const * enabled );
		void accept( castor3d::PipelineVisitorBase & visitor );

		crg::FramePass const & getLastPass()const
		{
			return *m_lastPass;
		}

	private:
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		crg::FramePass const * m_lastPass;
	};
}

#endif
