/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightStreaks_KawasePass_HPP___
#define ___C3D_LightStreaks_KawasePass_HPP___

#include "KawaseUbo.hpp"

#include <Castor3D/Render/Passes/GaussianBlur.hpp>
#include <Castor3D/Render/PostEffect/PostEffect.hpp>

namespace light_streaks
{
	using UboOffset = castor3d::UniformBufferOffsetT< castor3d::GaussianBlur::Configuration >;
	using UboOffsetArray = std::vector< UboOffset >;

	class KawasePass
	{
	public:
		KawasePass( crg::FramePassGroup & graph
			, crg::FramePass const & previousPass
			, castor3d::RenderDevice const & device
			, crg::ImageViewIdArray const & hiViews
			, crg::ImageViewIdArray const & kawaseViews
			, KawaseUbo & kawaseUbo
			, VkExtent2D dimensions
			, bool const * enabled );
		void accept( castor3d::PipelineVisitorBase & visitor );

		crg::FramePass const & getLastPass()const
		{
			return *m_lastPass;
		}

	public:
		static constexpr uint32_t MaxCoefficients{ 64u };

		struct Subpass
		{
			Subpass( crg::FramePassGroup & graph
				, crg::FramePass const & previousPass
				, castor3d::RenderDevice const & device
				, crg::ImageViewId const & srcView
				, crg::ImageViewId const & dstView
				, VkExtent2D dimensions
				, ashes::PipelineShaderStageCreateInfoArray const & stages
				, KawaseUbo const & kawaseUbo
				, uint32_t index
				, bool const * enabled );

			crg::FramePass & pass;
		};

	private:
		castor3d::RenderDevice const & m_device;
		KawaseUbo & m_kawaseUbo;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		crg::FramePass const * m_lastPass;
		std::vector< Subpass > m_subpasses;
	};
}

#endif
