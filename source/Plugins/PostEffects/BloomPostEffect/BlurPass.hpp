/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Bloom_BlurPass_HPP___
#define ___C3D_Bloom_BlurPass_HPP___

#include <Castor3D/Render/Passes/GaussianBlur.hpp>
#include <Castor3D/Render/PostEffect/PostEffect.hpp>

namespace Bloom
{
	using UboOffset = c3d::UniformBufferOffsetT< c3d::GaussianBlur::Configuration >;
	using UboOffsetArray = c3d::Vector< UboOffset >;

	class BlurPass
	{
	public:
		BlurPass( crg::FramePassGroup & graph
			, crg::FramePassArray const & previousPasses
			, c3d::RenderDevice const & device
			, crg::ImageViewIdArray const & srcImages
			, crg::ImageViewIdArray const & dstImages
			, c3d::Extent2D dimensions
			, uint32_t blurKernelSize
			, uint32_t blurPassesCount
			, bool isVertical
			, bool const * enabled );
		BlurPass( crg::FramePassGroup & graph
			, crg::FramePass const & previousPass
			, c3d::RenderDevice const & device
			, crg::ImageViewIdArray const & srcImages
			, crg::ImageViewIdArray const & dstImages
			, c3d::Extent2D dimensions
			, uint32_t blurKernelSize
			, uint32_t blurPassesCount
			, bool isVertical
			, bool const * enabled );
		~BlurPass()noexcept;
		void accept( c3d::ConfigurationVisitorBase & visitor )const;

		void update( uint32_t kernelSize );

		crg::FramePassArray const & getPasses()const
		{
			return m_passes;
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
				, c3d::UniformBufferOffsetT< c3d::GaussianBlur::Configuration > const & blurUbo
				, uint32_t index
				, bool isVertical
				, bool const * enabled );

			crg::FramePass & pass;
		};

	private:
		c3d::RenderDevice const & m_device;
		uint32_t m_blurPassesCount;
		UboOffsetArray m_blurUbo;
		c3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		crg::FramePassArray m_passes;
		c3d::Vector< Subpass > m_subpasses;
	};
}

#endif
