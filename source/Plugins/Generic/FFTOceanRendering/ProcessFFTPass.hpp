/*
See LICENSE file in root folder
*/
#ifndef ___C3DORFFT_ProcessFFTPass_H___
#define ___C3DORFFT_ProcessFFTPass_H___

#include "OceanFFTRenderingPrerequisites.hpp"

#include <Castor3D/Render/RenderModule.hpp>
#include <Castor3D/Render/Technique/TechniqueModule.hpp>

#include <RenderGraph/RunnablePass.hpp>

namespace ocean_fft
{
	class ProcessFFTPass
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
		ProcessFFTPass( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, castor3d::RenderDevice const & device
			, FFTConfig const & config
			, VkExtent2D const & extent
			, ashes::BufferBase const & input
			, std::array< ashes::BufferBasePtr, 2u > const & output
			, crg::RunnablePass::IsEnabledCallback isEnabled = crg::RunnablePass::IsEnabledCallback( [](){ return true; } ) );
		~ProcessFFTPass()override;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		void accept( castor3d::RenderTechniqueVisitor & visitor );

	public:
		static castor::String const Name;

	private:
		void doInitialise();
		void doRecordInto( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t index );
		VkPipelineStageFlags doGetSemaphoreWaitFlags()const;
		uint32_t doGetPassIndex()const;
		bool doIsComputePass()const;

	private:
		castor3d::RenderDevice const & m_device;
		VkExtent2D m_extent;
		VkDeviceSize m_inBufferSize{};
		VkBuffer m_vkInput{};
		VkDeviceSize m_outBufferSize{};
		std::array< VkBuffer, 2u > m_vkOutput{};
		VkFFTApplication m_app{};
	};

	crg::FramePass const & createProcessFFTPass( castor::String const & name
		, castor3d::RenderDevice const & device
		, crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, VkExtent2D const & extent
		, FFTConfig const & config
		, ashes::BufferBase const & input
		, std::array< ashes::BufferBasePtr, 2u > const & output
		, std::shared_ptr< IsRenderPassEnabled > isEnabled );
}

#endif
