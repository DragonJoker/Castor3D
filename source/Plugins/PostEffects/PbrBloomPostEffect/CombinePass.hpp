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
			, crg::ImageViewIdArray const & lhs
			, crg::ImageId const & rhs
			, crg::ImageViewIdArray const & result
			, castor3d::UniformBufferOffsetT< castor::Point2f > const & ubo
			, bool const * enabled
			, uint32_t const * passIndex );
		void accept( castor3d::ConfigurationVisitorBase & visitor );

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
		crg::FramePass & m_pass;
	};
}

#endif
