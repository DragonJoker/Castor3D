/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Bloom_BlurPass_HPP___
#define ___C3D_Bloom_BlurPass_HPP___

#include <Castor3D/Render/Passes/GaussianBlur.hpp>
#include <Castor3D/Render/PostEffect/PostEffect.hpp>

namespace Bloom
{
	using UboOffset = castor3d::UniformBufferOffsetT< castor3d::GaussianBlur::Configuration >;
	using UboOffsetArray = std::vector< UboOffset >;

	class BlurPass
	{
	public:
		BlurPass( crg::FrameGraph & graph
			, crg::FramePassArray const & previousPasses
			, castor3d::RenderDevice const & device
			, crg::ImageViewIdArray const & srcImages
			, crg::ImageViewIdArray const & dstImages
			, VkExtent2D dimensions
			, uint32_t blurKernelSize
			, uint32_t blurPassesCount
			, bool isVertical
			, bool const * enabled );
		BlurPass( crg::FrameGraph & graph
			, crg::FramePass const & previousPass
			, castor3d::RenderDevice const & device
			, crg::ImageViewIdArray const & srcImages
			, crg::ImageViewIdArray const & dstImages
			, VkExtent2D dimensions
			, uint32_t blurKernelSize
			, uint32_t blurPassesCount
			, bool isVertical
			, bool const * enabled );
		void accept( castor3d::PipelineVisitorBase & visitor );

		void update( uint32_t kernelSize );

		crg::FramePassArray const & getPasses()const
		{
			return m_passes;
		}

	public:
		static constexpr uint32_t MaxCoefficients{ 64u };

		struct Subpass
		{
			Subpass( crg::FrameGraph & graph
				, crg::FramePass const & previousPass
				, castor3d::RenderDevice const & device
				, crg::ImageViewId const & srcView
				, crg::ImageViewId const & dstView
				, VkExtent2D dimensions
				, ashes::PipelineShaderStageCreateInfoArray const & stages
				, castor3d::UniformBufferOffsetT< castor3d::GaussianBlur::Configuration > const & blurUbo
				, uint32_t index
				, bool isVertical
				, bool const * enabled );

			crg::FramePass & pass;
		};

	private:
		castor3d::RenderDevice const & m_device;
		uint32_t m_blurPassesCount;
		UboOffsetArray m_blurUbo;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		crg::FramePassArray m_passes;
		std::vector< Subpass > m_subpasses;
	};
}

#endif
