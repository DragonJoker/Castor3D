/*
See LICENSE file in root folder
*/
#ifndef ___C3DORFFT_GenerateHeightmapPass_H___
#define ___C3DORFFT_GenerateHeightmapPass_H___

#include "OceanFFTRenderingPrerequisites.hpp"

#include <Castor3D/Render/RenderModule.hpp>

#include <RenderGraph/RunnablePass.hpp>

namespace ocean_fft
{
	class GenerateHeightmapPass
		: public crg::RunnablePass
	{
	public:
		enum class Bindings : uint32_t
		{
			eConfig,
			eInput,
			eOutput,
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
		GenerateHeightmapPass( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, c3d::RenderDevice const & device
			, c3d::Extent2D const & extent
			, crg::RunnablePass::IsEnabledCallback isEnabled = crg::RunnablePass::IsEnabledCallback( [](){ return true; } ) );
		/**
		 *\copydoc		c3d::RenderTechniquePass::accept
		 */
		void accept( c3d::RenderTechniqueVisitor & visitor )const;

	public:
		static c3d::MbString const Name;

	private:
		void doRecordInto( crg::RecordContext const & context
			, VkCommandBuffer commandBuffer )const;
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
		c3d::Extent2D m_extent;
	};
}

#endif
