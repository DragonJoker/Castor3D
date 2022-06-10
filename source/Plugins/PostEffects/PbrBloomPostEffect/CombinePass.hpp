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
			, crg::FramePass const & previousPass
			, castor3d::RenderDevice const & device
			, crg::ImageViewId const & sceneView
			, crg::ImageId const & intermediate
			, castor3d::UniformBufferOffsetT< castor::Point2f > const & ubo
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
		static castor::String const CombineMapPasses;
		static castor::String const CombineMapScene;

	private:
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		crg::ImageId m_resultImg;
		crg::ImageViewId m_resultView;
		crg::FramePass & m_pass;
	};
}

#endif
