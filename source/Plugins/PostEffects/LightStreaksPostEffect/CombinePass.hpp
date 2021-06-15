/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightStreaks_CombinePass_HPP___
#define ___C3D_LightStreaks_CombinePass_HPP___

#include <Castor3D/Render/PostEffect/PostEffect.hpp>

#include <ashespp/Image/ImageView.hpp>

namespace light_streaks
{
	class CombinePass
	{
	public:
		CombinePass( crg::FrameGraph & graph
			, crg::FramePass const & previousPass
			, castor3d::RenderDevice const & device
			, crg::ImageViewId const & sceneView
			, crg::ImageViewIdArray const & kawaseViews
			, VkExtent2D const & size
			, bool const * enabled );
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
		static castor::String const CombineMapScene;
		static castor::String const CombineMapKawase;

	private:
		castor3d::RenderDevice const & m_device;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		crg::ImageId m_resultImg;
		crg::ImageViewId m_resultView;
		crg::FramePass & m_pass;
	};
}

#endif
