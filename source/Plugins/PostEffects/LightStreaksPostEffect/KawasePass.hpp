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
	using UboOffset = c3d::UniformBufferOffsetT< c3d::GaussianBlur::Configuration >;
	using UboOffsetArray = c3d::Vector< UboOffset >;

	class KawasePass
	{
	public:
		KawasePass( crg::FramePassGroup & graph
			, crg::FramePassArray const & previousPasses
			, c3d::RenderDevice const & device
			, crg::ImageViewIdArray const & hiViews
			, crg::ImageViewIdArray const & kawaseViews
			, KawaseUbo & kawaseUbo
			, c3d::Extent2D dimensions
			, bool const * enabled );
		void accept( c3d::ConfigurationVisitorBase & visitor );

		crg::FramePassArray const & getLastPasses()const
		{
			return m_lastPasses;
		}

	public:
		static constexpr uint32_t MaxCoefficients{ 64u };

		struct Subpass
		{
			Subpass( crg::FramePassGroup & graph
				, crg::FramePass const & previousPass
				, c3d::RenderDevice const & device
				, crg::ImageViewId const & srcView
				, crg::ImageViewId const & dstView
				, c3d::Extent2D dimensions
				, ashes::PipelineShaderStageCreateInfoArray const & stages
				, KawaseUbo const & kawaseUbo
				, uint32_t index
				, bool const * enabled );

			crg::FramePass & pass;
		};

	private:
		c3d::RenderDevice const & m_device;
		KawaseUbo & m_kawaseUbo;
		c3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		crg::FramePassArray m_lastPasses;
		c3d::Vector< Subpass > m_subpasses;
	};
}

#endif
