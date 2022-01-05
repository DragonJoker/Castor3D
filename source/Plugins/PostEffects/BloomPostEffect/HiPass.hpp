/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Bloom_HiPass___
#define ___C3D_Bloom_HiPass___

#include <Castor3D/Render/PostEffect/PostEffect.hpp>

#define Bloom_DebugHiPass 0

namespace Bloom
{
	class HiPass
	{
	public:
		HiPass( crg::FramePassGroup & graph
			, crg::FramePass const & previousPass
			, castor3d::RenderDevice const & device
			, crg::ImageViewId const & sceneView
			, VkExtent2D size
			, uint32_t blurPassesCount
			, bool const * enabled );
		void accept( castor3d::PipelineVisitorBase & visitor );

		crg::ImageViewIdArray const & getResult()const
		{
			return m_resultViews;
		}

		crg::FramePass const & getPass()const
		{
			return m_pass;
		}

	private:
		crg::FramePassGroup & m_graph;
		crg::ImageViewId const & m_sceneView;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		crg::ImageId m_resultImg;
		crg::ImageViewId m_resultView;
		crg::ImageViewIdArray m_resultViews;
		crg::FramePass & m_pass;
	};
}

#endif
