/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Bloom_CombinePass_HPP___
#define ___C3D_Bloom_CombinePass_HPP___

#include <Castor3D/Render/PostEffect/PostEffect.hpp>

#include <ashespp/Image/ImageView.hpp>

namespace Bloom
{
	class CombinePass
	{
	public:
		CombinePass( crg::FrameGraph & graph
			, crg::FramePassArray const & previousPasses
			, castor3d::RenderDevice const & device
			, crg::ImageViewId const & sceneView
			, crg::ImageViewIdArray const & blurViews
			, VkExtent2D const & size
			, uint32_t blurPassesCount );
		void accept( castor3d::PipelineVisitorBase & visitor );

		crg::ImageViewId const & getResult()const
		{
			return m_resultView;
		}

		crg::FramePass const & getPass()const
		{
			return m_pass;
		}

	public:
		static castor::String const CombineMapPasses;
		static castor::String const CombineMapScene;

	private:
		castor3d::RenderDevice const & m_device;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		uint32_t m_blurPassesCount;
		crg::ImageId m_resultImg;
		crg::ImageViewId m_resultView;
		crg::FramePass & m_pass;
	};
}

#endif
