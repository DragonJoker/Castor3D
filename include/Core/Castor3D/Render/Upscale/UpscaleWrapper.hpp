/*
See LICENSE file in root folder
*/
#ifndef ___C3D_UpscalingWrapper_H___
#define ___C3D_UpscalingWrapper_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/Upscale/UpscaleModule.hpp"

namespace c3d
{
	class UpscalingSDKInstance
	{
	public:
		virtual ~UpscalingSDKInstance() = default;
		virtual bool initialise( Extent2D const & renderSize
			, Extent2D const & displaySize
			, UpscaleConfig const & config ) = 0;
		virtual void cleanup()noexcept = 0;
		virtual Point2f getJitter( u32 & frameIndex )const noexcept = 0;
		virtual void evaluate( crg::RecordContext & recContext
			, VkCommandBuffer commandBuffer
			, crg::ImageViewId resolvedColor
			, crg::ImageViewId unresolvedColor
			, crg::ImageViewId motionVectors
			, crg::ImageViewId depth
			, bool resetAccumulation
			, Point2f const & jitterOffset
			, Point2f const & mVScale )const = 0;
	};

	class UpscalingSDK
	{
	public:
		virtual ~UpscalingSDK() = default;
		virtual bool initialise( RenderDevice const & device ) = 0;
		virtual void cleanup()noexcept = 0;
		virtual bool queryOptimalSettings( Extent2D const & displaySize
			, UpscaleConfig const & config
			, Extent2D & recommendedSize )const = 0;
		virtual RawUniquePtr< UpscalingSDKInstance > createInstance()const = 0;
	};

	class UpscalingInstance
	{
	public:
		C3D_API UpscalingInstance( UpscalingWrapper const & parent
			, RenderTarget & target
			, Extent2D const & renderSize
			, Extent2D const & displaySize
			, UpscaleConfig const & config );
		C3D_API ~UpscalingInstance()noexcept;

		C3D_API void update();
		C3D_API void evaluateUpscaling( crg::RecordContext & recContext
			, VkCommandBuffer commandBuffer
			, crg::ImageViewId resolvedColor
			, crg::ImageViewId unresolvedColor
			, crg::ImageViewId motionVectors
			, crg::ImageViewId depth
			, bool resetAccumulation = false
			, Point2f const & jitterOffset = { 0.0f, 0.0f }
			, Point2f const & mVScale = { 1.0f, 1.0f } )const;

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
			, UpscaleConfig const & config
			, Extent2D & recommendedSize )const;
		C3D_API UpscalingInstanceUPtr createInstance( RenderTarget & target
			, Extent2D const & renderSize
			, Extent2D const & displaySize
			, UpscaleConfig const & config )const;

	private:
		friend class UpscalingInstance;
		RawUniquePtr< UpscalingSDK > m_sdk;
	};
}

#endif
