/*
See LICENSE file in root folder
*/
#ifndef ___C3D_UpscalingWrapper_H___
#define ___C3D_UpscalingWrapper_H___

#include "RenderModule.hpp"

namespace c3d
{
	class UpscalingSDK;
	class UpscalingSDKInstance;

	struct UpscalingRecommendedSettings
	{
		Extent2D recommendedOptimalRenderSize = { ~0u, ~0u };
		Extent2D dynamicMaximumRenderSize = { ~0u, ~0u };
		Extent2D dynamicMinimumRenderSize = { ~0u, ~0u };
	};

	class UpscalingInstance
	{
	public:
		C3D_API UpscalingInstance( UpscalingWrapper const & parent
			, RenderTarget & target
			, Extent2D const & renderSize
			, Extent2D const & displaySize
			, UpscalingConfig const & config );
		C3D_API ~UpscalingInstance()noexcept;

		C3D_API void update();
		C3D_API void evaluateUpscaling( crg::RecordContext & recContext
			, VkCommandBuffer commandBuffer
			, crg::ImageViewId resolvedColor
			, crg::ImageViewId unresolvedColor
			, crg::ImageViewId motionVectors
			, crg::ImageViewId depth
			, bool resetAccumulation = false
			, Point2f jitterOffset = { 0.0f, 0.0f }
			, Point2f mVScale = { 1.0f, 1.0f } )const;

	private:
		RawUniquePtr< UpscalingSDKInstance > m_sdk;
		RenderTarget & m_target;
		u32 m_frameIndex{};
	};

	class UpscalingWrapper
	{
	public:
		C3D_API explicit UpscalingWrapper( RenderDevice const & device );
		C3D_API ~UpscalingWrapper()noexcept;

		C3D_API bool queryOptimalSettings( Extent2D const & displaySize
			, UpscalingConfig const & config
			, UpscalingRecommendedSettings & recommendedSettings )const;
		C3D_API UpscalingInstanceUPtr createInstance( RenderTarget & target
			, Extent2D const & renderSize
			, Extent2D const & displaySize
			, UpscalingConfig const & config )const;

	private:
		friend class UpscalingInstance;
		RawUniquePtr< UpscalingSDK > m_sdk;
	};
}

#endif
