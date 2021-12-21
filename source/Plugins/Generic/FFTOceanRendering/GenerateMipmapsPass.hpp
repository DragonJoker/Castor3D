/*
See LICENSE file in root folder
*/
#ifndef ___C3DORFFT_GenerateMipmapsPass_H___
#define ___C3DORFFT_GenerateMipmapsPass_H___

#include "OceanFFTRenderingPrerequisites.hpp"

#include <RenderGraph/RunnablePasses/GenerateMipmaps.hpp>

namespace ocean_fft
{
	class GenerateMipmapsPass
		: public crg::RunnablePass
	{
	public:
		enum Bindings : uint32_t
		{
			eInput,
			eOutput,
		};
		GenerateMipmapsPass( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, castor3d::RenderDevice const & device );

	private:
		void doInitialise();
		void doRecordInto( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t index );
		VkPipelineStageFlags doGetSemaphoreWaitFlags()const;
		uint32_t doGetPassIndex()const;
		bool doIsComputePass()const;

	public:
		static castor::String const Name;

	private:
		crg::LayoutState m_outputLayout;
		castor3d::RenderDevice const & m_device;
		ashes::DescriptorSetLayoutPtr m_descriptorSetLayout;
		ashes::PipelineLayoutPtr m_pipelineLayout;
		castor3d::ShaderModule m_shader;
		ashes::ComputePipelinePtr m_pipeline;
		ashes::DescriptorSetPoolPtr m_descriptorSetPool;
		std::vector< ashes::DescriptorSetPtr > m_descriptorSets;
		std::vector< castor::Point2f > m_invSizes;
	};
}

#endif
