/*
See LICENSE file in root folder
*/
#ifndef ___C3D_UpscalingPass_H___
#define ___C3D_UpscalingPass_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/Upscale/UpscaleModule.hpp"

#include <RenderGraph/RunnablePass.hpp>

namespace c3d
{
	class UpscalingFramePass
		: public crg::RunnablePass
	{
	public:
		UpscalingFramePass( crg::FramePass const & pass
			, crg::GraphContext & context
			, crg::RunnableGraph & graph
			, RenderDevice const & device
			, RenderTarget & target
			, UpscaleConfig const & config
			, crg::ru::Config const & ruConfig = {} );

		void update();

	private:
		void doInitialise( uint32_t index );
		void doRecordInto( crg::RecordContext & context
			, VkCommandBuffer commandBuffer
			, uint32_t index )const;
		uint32_t doGetPassIndex()const;
		bool doIsEnabled()const;

	private:
		RenderDevice const & m_device;
		RenderTarget & m_target;
		UpscaleConfig const & m_config;
		UpscalingInstanceUPtr m_upscaler;
		uint64_t m_updateCount{};
	};
}

#endif
