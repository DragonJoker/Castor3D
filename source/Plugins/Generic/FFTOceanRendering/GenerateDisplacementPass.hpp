/*
See LICENSE file in root folder
*/
#ifndef ___C3DORFFT_GenerateDisplacementPass_H___
#define ___C3DORFFT_GenerateDisplacementPass_H___

#include "OceanFFTRenderingPrerequisites.hpp"

#include <Castor3D/Render/RenderModule.hpp>

#include <RenderGraph/RunnablePass.hpp>

namespace ocean_fft
{
	class GenerateDisplacementPass
		: public crg::RunnablePass
	{
	public:
		enum Bindings : uint32_t
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
		GenerateDisplacementPass( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, castor3d::RenderDevice const & device
			, VkExtent2D const & extent
			, crg::RunnablePass::IsEnabledCallback isEnabled = crg::RunnablePass::IsEnabledCallback( [](){ return true; } ) );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		void accept( castor3d::RenderTechniqueVisitor & visitor );

	public:
		static castor::MbString const Name;

	private:
		void doRecordInto( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t index );
		uint32_t doGetPassIndex()const;
		bool doIsComputePass()const;

	private:
		castor3d::RenderDevice const & m_device;
		ashes::DescriptorSetLayoutPtr m_descriptorSetLayout;
		ashes::PipelineLayoutPtr m_pipelineLayout;
		castor3d::ShaderModule m_shader;
		ashes::ComputePipelinePtr m_pipeline;
		ashes::DescriptorSetPoolPtr m_descriptorSetPool;
		ashes::DescriptorSetPtr m_descriptorSet;
		VkExtent2D m_extent;
	};
}

#endif
