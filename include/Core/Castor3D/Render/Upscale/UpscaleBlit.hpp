/*
See LICENSE file in root folder
*/
#ifndef ___C3D_UpscaleBlit_H___
#define ___C3D_UpscaleBlit_H___

#include "UpscaleWrapper.hpp"

namespace c3d
{
	class BlitUpscalingSDKInstance
		: public UpscalingSDKInstance
	{
	public:
		bool initialise( Extent2D const & renderSize
			, Extent2D const & displaySize
			, UpscaleConfig const & config )override;
		void cleanup()noexcept override;
		Point2f getJitter( [[maybe_unused]] u32 & frameIndex )const noexcept override;
		void evaluate( crg::RecordContext & recContext
			, VkCommandBuffer commandBuffer
			, crg::ImageViewId resolvedColor
			, crg::ImageViewId unresolvedColor
			, crg::ImageViewId motionVectors
			, crg::ImageViewId depth
			, bool resetAccumulation
			, Point2f const & jitterOffset
			, Point2f const & mVScale )const override;
	};

	class BlitUpscalingSDK
		: public UpscalingSDK
	{
	public:
		bool initialise( [[maybe_unused]] RenderDevice const & device )override;
		void cleanup()noexcept override;
		bool queryOptimalSettings( Extent2D const & displaySize
			, UpscaleConfig const & config
			, Extent2D & recommendedSize )const override;
		RawUniquePtr< UpscalingSDKInstance > createInstance()const override;
	};
}

#endif
