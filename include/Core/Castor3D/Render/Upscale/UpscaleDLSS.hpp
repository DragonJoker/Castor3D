/*
See LICENSE file in root folder
*/
#ifndef ___C3D_UpscaleDLSS_H___
#define ___C3D_UpscaleDLSS_H___

#include "Castor3D/Config.hpp"

#if C3D_UseDLSS

#include "UpscaleWrapper.hpp"

struct NVSDK_NGX_FeatureDiscoveryInfo;
struct NVSDK_NGX_Handle;
struct NVSDK_NGX_Parameter;

namespace c3d
{
	class DLSSUpscalingSDKInstance
		: public UpscalingSDKInstance
	{
	public:
		DLSSUpscalingSDKInstance( RenderDevice const & device
			, NVSDK_NGX_Parameter * ngxParameters );
		bool initialise( Extent2D const & renderSize
			, Extent2D const & displaySize
			, UpscaleConfig const & config )override;
		void cleanup()noexcept override;
		Point2f getJitter( u32 & frameIndex )const noexcept override;
		void evaluate( crg::RecordContext & recContext
			, VkCommandBuffer commandBuffer
			, crg::ImageViewId resolvedColor
			, crg::ImageViewId unresolvedColor
			, crg::ImageViewId motionVectors
			, crg::ImageViewId depth
			, bool resetAccumulation
			, Point2f const & jitterOffset
			, Point2f const & mVScale )const override;

	private:
		RenderDevice const & m_device;
		NVSDK_NGX_Parameter * m_ngxParameters{};
		bool m_dlssAvailable{};
		NVSDK_NGX_Handle * m_dlssFeature{};
		u32 m_phaseCount{};
	};

	class DLSSUpscalingSDK
		: public UpscalingSDK
	{
	public:
		bool initialise( RenderDevice const & device )override;
		void cleanup()noexcept override;
		bool queryOptimalSettings( Extent2D const & displaySize
			, UpscaleConfig const & config
			, Extent2D & recommendedSize )const override;
		RawUniquePtr< UpscalingSDKInstance > createInstance()const override;

		bool isNGXInitialized()const noexcept
		{
			return m_ngxInitialized;
		}

	private:
		RenderDevice const * m_device{};
		bool m_ngxInitialized{};
		NVSDK_NGX_Parameter * m_ngxParameters{};
	};
}

#endif

#endif
