/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ComputeDepthRange_H___
#define ___C3D_ComputeDepthRange_H___

#include "PassesModule.hpp"
#include "Castor3D/Render/Technique/TechniqueModule.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/ComputePipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>

#include <RenderGraph/RunnablePass.hpp>

namespace castor3d
{
	class ComputeDepthRange
		: public crg::RunnablePass
	{
	public:
		enum Bindings : uint32_t
		{
			eInput,
			eOutput,
		};
		/**
		 *\~english
		 *\param[in]	pass		The parent frame pass.
		 *\param[in]	context		The rendering context.
		 *\param[in]	graph		The runnable graph.
		 *\param[in]	device		The GPU device.
		 *\param[in]	enabled		\p true to enable this pass.
		 *\~french
		 *\param[in]	pass		La frame pass parente.
		 *\param[in]	context		Le contexte de rendu.
		 *\param[in]	graph		Le runnable graph.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	enabled		\p true pour activer cette passe.
		 */
		C3D_API ComputeDepthRange( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, bool & enabled );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor );

	private:
		void doInitialise();
		void doRecordInto( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t index );
		VkPipelineStageFlags doGetSemaphoreWaitFlags()const;
		uint32_t doGetPassIndex()const;
		bool doIsComputePass()const;

	private:
		RenderDevice const & m_device;
		ashes::DescriptorSetLayoutPtr m_descriptorSetLayout;
		ashes::PipelineLayoutPtr m_pipelineLayout;
		ShaderModule m_shader;
		ashes::ComputePipelinePtr m_pipeline;
		ashes::DescriptorSetPoolPtr m_descriptorSetPool;
		ashes::DescriptorSetPtr m_descriptorSet;
	};
}

#endif
