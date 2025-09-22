/*
See LICENSE file in root folder
*/
#ifndef ___C3DORFFT_BakeHeightGradientPass_H___
#define ___C3DORFFT_BakeHeightGradientPass_H___

#include "OceanFFTRenderingPrerequisites.hpp"

#include <Castor3D/Render/RenderModule.hpp>

#include <RenderGraph/RunnablePass.hpp>

namespace ocean_fft
{
	class BakeHeightGradientPass
		: public crg::RunnablePass
	{
	public:
		enum Bindings : uint32_t
		{
			eConfig,
			eHeight,
			eDisplacement,
			eHeightDisplacement,
			eGradientJacobian,
		};
		/**
		 *\~english
		 *\param[in]	device		The GPU device.
		 *\param[in]	vctConfig	The voxelizer configuration.
		 *\param[in]	voxels		The voxels buffer.
		 *\param[in]	result		The resulting texture.
		 *\~french
		 *\param[in]	device		Le device GPU.
		 *\param[in]	vctConfig	La configuration du voxelizer.
		 *\param[in]	voxels		Le tampon de voxels.
		 *\param[in]	result		La texture résultante.
		 */
		BakeHeightGradientPass( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, c3d::RenderDevice const & device
			, c3d::Extent2D const & extent
			, c3d::Point2f const & heightMapSize
			, uint32_t displacementDownsample
			, crg::RunnablePass::IsEnabledCallback isEnabled = crg::RunnablePass::IsEnabledCallback( [](){ return true; } ) );
		/**
		 *\copydoc		c3d::RenderTechniquePass::accept
		 */
		void accept( c3d::RenderTechniqueVisitor & visitor )const;

	public:
		static c3d::MbString const Name;

		struct Data
		{
			c3d::Point4f invSize;
			c3d::Point4f scale;
		};

	private:
		void doRecordInto( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t index );
		uint32_t doGetPassIndex()const;
		bool doIsComputePass()const;

	private:
		c3d::RenderDevice const & m_device;
		ashes::DescriptorSetLayoutPtr m_descriptorSetLayout;
		ashes::PipelineLayoutPtr m_pipelineLayout;
		c3d::ShaderModule m_shader;
		ashes::ComputePipelinePtr m_pipeline;
		ashes::DescriptorSetPoolPtr m_descriptorSetPool;
		ashes::DescriptorSetPtr m_descriptorSet;
		c3d::Extent2D m_extent{};
		c3d::Point2f m_heightMapSize;
		uint32_t m_displacementDownsample{};
		Data m_data{};
	};

	void createBakeHeightGradientPass( c3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, c3d::Extent2D const & extent
		, c3d::Point2f const & heightMapSize
		, uint32_t displacementDownsample
		, OceanUbo const & ubo
		, c3d::Buffer const & height
		, c3d::Buffer const & displacement
		, c3d::Array< c3d::Texture, 2u > & heightDisp
		, c3d::Array< c3d::Texture, 2u > & gradJacob );
}

#endif
