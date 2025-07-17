/*
See LICENSE file in root folder
*/
#include "Castor3D/Render/UpscalingWrapper.hpp"

#include "Castor3D/Config.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderTarget.hpp"

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Command/CommandPool.hpp>

#include <RenderGraph/RunnableGraph.hpp>

#if C3D_UseDLSS
#	include <nvsdk_ngx.h>
#	include <nvsdk_ngx_helpers.h>
#	include <nvsdk_ngx_vk.h>
#	include <nvsdk_ngx_helpers_vk.h>
#endif

namespace c3d
{
	class UpscalingSDK
	{
	public:
		virtual ~UpscalingSDK() = default;
		virtual bool initialise( RenderDevice const & device ) = 0;
		virtual void cleanup()noexcept = 0;
		virtual bool queryOptimalSettings( Extent2D const & displaySize
			, UpscalingConfig const & config
			, UpscalingRecommendedSettings & recommendedSettings )const = 0;
		virtual RawUniquePtr< UpscalingSDKInstance > createInstance()const = 0;
	};

	class UpscalingSDKInstance
	{
	public:
		virtual ~UpscalingSDKInstance() = default;
		virtual bool initialise( Extent2D const & renderSize
			, Extent2D const & displaySize
			, UpscalingConfig const & config ) = 0;
		virtual void cleanup()noexcept = 0;
		virtual Point2f getJitter( u32 & frameIndex )const noexcept = 0;
		virtual void evaluate( crg::RecordContext & recContext
			, VkCommandBuffer commandBuffer
			, crg::ImageViewId resolvedColor
			, crg::ImageViewId unresolvedColor
			, crg::ImageViewId motionVectors
			, crg::ImageViewId depth
			, bool resetAccumulation
			, Point2f jitterOffset
			, Point2f mVScale )const = 0;
	};

#if C3D_UseDLSS

	//*********************************************************************************************

	namespace ss
	{
		static constexpr uint64_t APP_ID = 231313132;
		constexpr size_t NUM_OFFSET_SEQUENCES = 64; // Use a large number of Halton sequence offsets to accomodate large scaling ratios.

		StringView getName( NVSDK_NGX_Feature feature )
		{
			switch ( feature )
			{
			case NVSDK_NGX_Feature_Reserved0: return cuT( "Reserved0" );
			case NVSDK_NGX_Feature_SuperSampling: return cuT( "SuperSampling" );
			case NVSDK_NGX_Feature_InPainting: return cuT( "InPainting" );
			case NVSDK_NGX_Feature_ImageSuperResolution: return cuT( "ImageSuperResolution" );
			case NVSDK_NGX_Feature_SlowMotion: return cuT( "SlowMotion" );
			case NVSDK_NGX_Feature_VideoSuperResolution: return cuT( "VideoSuperResolution" );
			case NVSDK_NGX_Feature_Reserved1: return cuT( "Reserved1" );
			case NVSDK_NGX_Feature_Reserved2: return cuT( "Reserved2" );
			case NVSDK_NGX_Feature_Reserved3: return cuT( "Reserved3" );
			case NVSDK_NGX_Feature_ImageSignalProcessing: return cuT( "ImageSignalProcessing" );
			case NVSDK_NGX_Feature_DeepResolve: return cuT( "DeepResolve" );
			case NVSDK_NGX_Feature_FrameGeneration: return cuT( "FrameGeneration" );
			case NVSDK_NGX_Feature_DeepDVC: return cuT( "DeepDVC" );
			case NVSDK_NGX_Feature_RayReconstruction: return cuT( "RayReconstruction" );
			case NVSDK_NGX_Feature_Reserved14: return cuT( "Reserved14" );
			case NVSDK_NGX_Feature_Reserved15: return cuT( "Reserved15" );
			case NVSDK_NGX_Feature_Reserved16: return cuT( "Reserved16" );
			case NVSDK_NGX_Feature_Reserved_SDK: return cuT( "Reserved_SDK" );
			case NVSDK_NGX_Feature_Reserved_Core: return cuT( "Reserved_Core" );
			case NVSDK_NGX_Feature_Reserved_Unknown: return cuT( "Reserved_Unknown" );
			default: return cuT( "Unknown" );
			}
		}

		void NVSDK_CONV logCallback( const char * message
			, [[maybe_unused]] NVSDK_NGX_Logging_Level loggingLevel
			, NVSDK_NGX_Feature sourceComponent )
		{
			log::info << cuT( "NGX SDK [" ) << getName( sourceComponent ) << cuT( "]: " ) << message << "\n";
		}

		void NVSDK_CONV progressCallback( float currentProgress, bool * OutShouldCancel )
		{
			log::trace << cuT( "NGX SDK " ) << currentProgress << "\n";
		}

		static NVSDK_NGX_Resource_VK textureToResourceVK( crg::ContextResourcesCache & resources
			, crg::ImageViewId tex, bool readWrite )
		{
			VkFormat format = convert( getFormat( tex ) );
			VkImageView imageView = resources.createImageView( tex );
			VkImage image = resources.createImage( tex.data->image );
			VkImageSubresourceRange subresourceRange = convert( tex.data->info.subresourceRange );
			Extent3D extent = getExtent( tex );

			return NVSDK_NGX_Create_ImageView_Resource_VK( imageView, image, subresourceRange, format, extent.width, extent.height, readWrite );
		}
	}

	//*********************************************************************************************

	class DLSSUpscalingSDK
		: public UpscalingSDK
	{
	public:
		RawUniquePtr< UpscalingSDKInstance > createInstance()const override;

		bool initialise( RenderDevice const & device )override
		{
			auto path = makePath( File::getExecutableDirectory() );
			NVSDK_NGX_FeatureCommonInfo featureInfo{};
			featureInfo.LoggingInfo = { ss::logCallback, NVSDK_NGX_LOGGING_LEVEL_VERBOSE, true };

			m_device = &device;
			VkPhysicalDevice vkphysicaldevice = device.gpu;
			VkInstance vkinstance = device->getInstance();
			auto result = NVSDK_NGX_VULKAN_Init( ss::APP_ID, path.c_str(), vkinstance, vkphysicaldevice, *m_device->device
				, device.desc.getInstanceProcAddr, device->getInstance().vkGetDeviceProcAddr, &featureInfo );
			m_ngxInitialized = !NVSDK_NGX_FAILED( result );
			if ( !m_ngxInitialized )
			{
				log::printError( cuT( "NVIDIA NGX not available on this hardware/platform., code = 0x%08x, info: %ls\n" )
					, result, GetNGXResultAsString( result ) );
				return false;
			}

			result = NVSDK_NGX_VULKAN_GetCapabilityParameters( &m_ngxParameters );

			if ( NVSDK_NGX_FAILED( result ) )
			{
				log::printError( cuT( "NVSDK_NGX_GetCapabilityParameters failed, code = 0x%08x, info: %ls" )
					, result, GetNGXResultAsString( result ) );
				cleanup();
				return false;
			}

			// If NGX Successfully initialized then it should set those flags in return
			unsigned int minDriverVersionMajor = 0;
			unsigned int minDriverVersionMinor = 0;
			if ( int needsUpdatedDriver = 0;
				m_ngxParameters->Get( NVSDK_NGX_Parameter_SuperSampling_NeedsUpdatedDriver, &needsUpdatedDriver ) == NVSDK_NGX_Result_Success
					&& m_ngxParameters->Get( NVSDK_NGX_Parameter_SuperSampling_MinDriverVersionMajor, &minDriverVersionMajor ) == NVSDK_NGX_Result_Success
					&& m_ngxParameters->Get( NVSDK_NGX_Parameter_SuperSampling_MinDriverVersionMinor, &minDriverVersionMinor ) == NVSDK_NGX_Result_Success )
			{
				if ( needsUpdatedDriver )
				{
					log::printError( cuT( "NVIDIA DLSS cannot be loaded due to outdated driver. Minimum Driver Version required : %u.%u\n" )
						, minDriverVersionMajor, minDriverVersionMinor );
					cleanup();
					return false;
				}
				else
				{
					log::printInfo( cuT( "NVIDIA DLSS Minimum driver version was reported as : %u.%u\n" )
						, minDriverVersionMajor, minDriverVersionMinor );
				}
			}
			else
			{
				log::printInfo( cuT( "NVIDIA DLSS Minimum driver version was not reported.\n" ) );
			}

			int dlssAvailable = 0;
			if ( NVSDK_NGX_Result resultDLSS = m_ngxParameters->Get( NVSDK_NGX_Parameter_SuperSampling_Available, &dlssAvailable );
				resultDLSS != NVSDK_NGX_Result_Success || !dlssAvailable )
			{
				// More details about what failed (per feature init result)
				NVSDK_NGX_Result featureInitResult = NVSDK_NGX_Result_Fail;
				NVSDK_NGX_Parameter_GetI( m_ngxParameters, NVSDK_NGX_Parameter_SuperSampling_FeatureInitResult, ( int * )&featureInitResult );
				log::printError( cuT( "NVIDIA DLSS not available on this hardward/platform., FeatureInitResult = 0x%08x, info: %ls\n" )
					, featureInitResult, GetNGXResultAsString( featureInitResult ) );
				cleanup();
				return false;
			}

			return true;
		}

		void cleanup()noexcept override
		{
			if ( isNGXInitialized() )
			{
				m_device->device->waitIdle();
				NVSDK_NGX_VULKAN_DestroyParameters( m_ngxParameters );
				NVSDK_NGX_VULKAN_Shutdown1( *m_device->device );
				m_ngxInitialized = false;
			}
		}

		bool queryOptimalSettings( Extent2D const & displaySize
			, UpscalingConfig const & config
			, UpscalingRecommendedSettings & recommendedSettings )const override
		{
			if ( !isNGXInitialized() )
			{
				recommendedSettings.recommendedOptimalRenderSize = displaySize;
				recommendedSettings.dynamicMaximumRenderSize = displaySize;
				recommendedSettings.dynamicMinimumRenderSize = displaySize;

				log::info << cuT( "NGX was not initialized when querying Optimal Settings\n" );
				return false;
			}

			float recommendedSharpness;
			NVSDK_NGX_Result result = NGX_DLSS_GET_OPTIMAL_SETTINGS( m_ngxParameters
				, displaySize.width, displaySize.height, NVSDK_NGX_PerfQuality_Value( config.perfQualityMode )
				, &recommendedSettings.recommendedOptimalRenderSize.width, &recommendedSettings.recommendedOptimalRenderSize.height
				, &recommendedSettings.dynamicMaximumRenderSize.width, &recommendedSettings.dynamicMaximumRenderSize.height
				, &recommendedSettings.dynamicMinimumRenderSize.width, &recommendedSettings.dynamicMinimumRenderSize.height
				, &recommendedSharpness );

			if ( NVSDK_NGX_FAILED( result ) )
			{
				recommendedSettings.recommendedOptimalRenderSize = displaySize;
				recommendedSettings.dynamicMaximumRenderSize = displaySize;
				recommendedSettings.dynamicMinimumRenderSize = displaySize;

				log::printWarn( cuT( "Querying Optimal Settings failed! code = 0x%08x, info: %ls\n" )
					, result, GetNGXResultAsString( result ) );
				return false;
			}

			return true;
		}

		bool isFeatureSupported( NVSDK_NGX_FeatureDiscoveryInfo const * dis )const
		{
			VkPhysicalDevice vkphysicaldevice = m_device->gpu;
			VkInstance vkinstance = m_device->device->getInstance();

			uint32_t nDeviceExtensions;
			VkExtensionProperties * deviceExtensions;
			auto res = NVSDK_NGX_VULKAN_GetFeatureDeviceExtensionRequirements( vkinstance, vkphysicaldevice, dis, &nDeviceExtensions, &deviceExtensions );
			if ( res != NVSDK_NGX_Result_Success )
			{
				log::printError( cuT( "GetFeatureRequirements error: NVSDK_NGX_VULKAN_GetFeatureDeviceExtensionRequirements returned 0x%08x info: %ls\n" )
					, res, GetNGXResultAsString( res ) );
				return false;
			}
			else
			{
				log::printInfo( cuT( "NVSDK_NGX_VULKAN_GetFeatureDeviceExtensionRequirements returned %d device extension requirements: " )
					, nDeviceExtensions );
				for ( uint32_t i = 0; i < nDeviceExtensions; i++ )
				{
					log::printInfo( cuT( "%s, " ), deviceExtensions[i].extensionName );
				}
				log::info << cuT( "\n" );
			}

			uint32_t nInstanceExtensions;
			VkExtensionProperties * instanceExtensions;
			res = NVSDK_NGX_VULKAN_GetFeatureInstanceExtensionRequirements( dis, &nInstanceExtensions, &instanceExtensions );
			if ( res != NVSDK_NGX_Result_Success )
			{
				log::printError( cuT( "GetFeatureRequirements error: NVSDK_NGX_VULKAN_GetFeatureInstanceExtensionRequirements returned 0x%08x info: %ls\n" )
					, res, GetNGXResultAsString( res ) );
				return false;
			}
			else
			{
				log::printInfo( cuT( "NVSDK_NGX_VULKAN_GetFeatureInstanceExtensionRequirements returned %d instance extension requirements: " )
					, nInstanceExtensions );
				for ( uint32_t i = 0; i < nInstanceExtensions; i++ )
				{
					log::printInfo( cuT( "%s, " ), instanceExtensions[i].extensionName );
				}
				log::info << cuT( "\n" );
			}

			NVSDK_NGX_FeatureRequirement req{};
			res = NVSDK_NGX_VULKAN_GetFeatureRequirements( vkinstance, vkphysicaldevice, dis, &req );

			if ( res != NVSDK_NGX_Result_Success && res != NVSDK_NGX_Result_FAIL_NotImplemented )
			{
				log::printError( cuT( "GetFeatureRequirements error: 0x%08x info: %ls\n" )
					, res, GetNGXResultAsString( res ) );
				return false;
			}

			log::printInfo( cuT( "GetFeatureRequirements returned 0x%08x: Min GPU Arch: 0x%08x MinOS: %s\n" )
				, req.FeatureSupported, req.MinHWArchitecture, req.MinOSVersion );

			return true;
		}

		bool isNGXInitialized()const noexcept
		{
			return m_ngxInitialized;
		}

		NVSDK_NGX_Parameter * getParameters()const noexcept
		{
			return m_ngxParameters;
		}

		RenderDevice const & getDevice()const noexcept
		{
			return *m_device;
		}

	private:
		RenderDevice const * m_device{};
		bool m_ngxInitialized{};
		NVSDK_NGX_Parameter * m_ngxParameters{};
	};

	//*********************************************************************************************

	class DLSSUpscalingSDKInstance
		: public UpscalingSDKInstance
	{
	public:
		explicit DLSSUpscalingSDKInstance( DLSSUpscalingSDK const & parent )
			: m_parent{ &parent }
		{
		}

		bool initialise( Extent2D const & renderSize
			, Extent2D const & displaySize
			, UpscalingConfig const & config )override
		{
			if ( !m_parent->isNGXInitialized() )
			{
				log::error << cuT( "Attempt to InitializeDLSSFeature without NGX being initialized.\n" );
				return false;
			}

			auto renderPreset = NVSDK_NGX_DLSS_Hint_Render_Preset_Default;
			auto ngxParameters = m_parent->getParameters();
			auto & device = m_parent->getDevice();
			unsigned int creationNodeMask = 1;
			unsigned int visibilityNodeMask = 1;

			// Next create features
			int dlssCreateFeatureFlags = NVSDK_NGX_DLSS_Feature_Flags_None;
			dlssCreateFeatureFlags |= NVSDK_NGX_DLSS_Feature_Flags_IsHDR;
			dlssCreateFeatureFlags |= NVSDK_NGX_DLSS_Feature_Flags_DepthInverted;
			dlssCreateFeatureFlags |= NVSDK_NGX_DLSS_Feature_Flags_MVLowRes;

			// Select render preset (DL weights)
			NVSDK_NGX_Parameter_SetUI( ngxParameters, NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_DLAA, renderPreset );              // will remain the chosen weights after OTA
			NVSDK_NGX_Parameter_SetUI( ngxParameters, NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_Quality, renderPreset );           // ^
			NVSDK_NGX_Parameter_SetUI( ngxParameters, NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_Balanced, renderPreset );          // ^
			NVSDK_NGX_Parameter_SetUI( ngxParameters, NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_Performance, renderPreset );       // ^
			NVSDK_NGX_Parameter_SetUI( ngxParameters, NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_UltraPerformance, renderPreset );  // ^

			// Set parameters
			NVSDK_NGX_Parameter_SetUI( ngxParameters, NVSDK_NGX_Parameter_CreationNodeMask, creationNodeMask );
			NVSDK_NGX_Parameter_SetUI( ngxParameters, NVSDK_NGX_Parameter_VisibilityNodeMask, visibilityNodeMask );
			NVSDK_NGX_Parameter_SetUI( ngxParameters, NVSDK_NGX_Parameter_Width, renderSize.width );
			NVSDK_NGX_Parameter_SetUI( ngxParameters, NVSDK_NGX_Parameter_Height, renderSize.height );
			NVSDK_NGX_Parameter_SetUI( ngxParameters, NVSDK_NGX_Parameter_OutWidth, displaySize.width );
			NVSDK_NGX_Parameter_SetUI( ngxParameters, NVSDK_NGX_Parameter_OutHeight, displaySize.height );
			NVSDK_NGX_Parameter_SetI( ngxParameters, NVSDK_NGX_Parameter_PerfQualityValue, NVSDK_NGX_PerfQuality_Value( config.perfQualityMode ) );
			NVSDK_NGX_Parameter_SetI( ngxParameters, NVSDK_NGX_Parameter_DLSS_Feature_Create_Flags, dlssCreateFeatureFlags );
			NVSDK_NGX_Parameter_SetI( ngxParameters, NVSDK_NGX_Parameter_DLSS_Enable_Output_Subrects, 0 );

			auto commandList = device.computeCommandPool->createCommandBuffer();
			commandList->begin();
			auto resultDLSS = NVSDK_NGX_VULKAN_CreateFeature1( *m_parent->getDevice(), *commandList, NVSDK_NGX_Feature_SuperSampling, ngxParameters, &m_dlssFeature );;
			commandList->end();

			if ( NVSDK_NGX_FAILED( resultDLSS ) )
			{
				log::printError( cuT( "Failed to create DLSS Features = 0x%08x, info: %ls\n" )
					, resultDLSS, GetNGXResultAsString( resultDLSS ) );
				return false;
			}

			auto fence = device.device->createFence();
			device.computeQueue->submit( *commandList, fence.get() );
			fence->wait( ashes::MaxTimeout );

			m_dlssAvailable = true;
			return true;
		}

		void cleanup()noexcept override
		{
			if ( !m_parent->isNGXInitialized() )
			{
				log::error << cuT( "Attempt to ReleaseDLSSFeatures without NGX being initialized.\n" );
				return;
			}

			auto & device = m_parent->getDevice();
			device.device->waitIdle();
			NVSDK_NGX_Result resultDLSS = ( m_dlssFeature != nullptr ) ? NVSDK_NGX_VULKAN_ReleaseFeature( m_dlssFeature ) : NVSDK_NGX_Result_Success;
			if ( NVSDK_NGX_FAILED( resultDLSS ) )
			{
				log::printError( cuT( "Failed to NVSDK_NGX_D3D12_ReleaseFeature, code = 0x%08x, info: %ls\n" )
					, resultDLSS, GetNGXResultAsString( resultDLSS ) );
			}

			m_dlssFeature = nullptr;
		}

		Point2f getJitter( u32 & frameIndex )const noexcept override
		{
			// Halton jitter
			Point2f result{ 0.0f, 0.0f };

			constexpr u32 baseX = 2;
			u32 index = frameIndex + 1;
			float invBase = 1.0f / baseX;
			float fraction = invBase;
			while ( index > 0 )
			{
				result->x += float( index % baseX ) * fraction;
				index /= baseX;
				fraction *= invBase;
			}

			constexpr u32 baseY = 3;
			index = frameIndex + 1;
			invBase = 1.0f / baseY;
			fraction = invBase;
			while ( index > 0 )
			{
				result->y += float( index % baseY ) * fraction;
				index /= baseY;
				fraction *= invBase;
			}

			result->x -= 0.5f;
			result->y -= 0.5f;

			frameIndex = ( frameIndex + 1 ) % ss::NUM_OFFSET_SEQUENCES;

			return result;
		}

		void evaluate( crg::RecordContext & recContext
			, VkCommandBuffer commandBuffer
			, crg::ImageViewId resolvedColor
			, crg::ImageViewId unresolvedColor
			, crg::ImageViewId motionVectors
			, crg::ImageViewId depth
			, bool resetAccumulation
			, Point2f jitterOffset
			, Point2f mVScale )const override
		{
			if ( !isDLSSAvailable() )
			{
				log::error << cuT( "DLSS is not available - could not evaluate Upscaling\n" );
				return;
			}

			auto ngxParameters = m_parent->getParameters();
			auto extent = getExtent( unresolvedColor );

			auto & resources = recContext.getResources();
			NVSDK_NGX_Resource_VK resolvedColorResource = ss::textureToResourceVK( resources, resolvedColor, true );
			NVSDK_NGX_Resource_VK unresolvedColorResource = ss::textureToResourceVK( resources, unresolvedColor, false );
			NVSDK_NGX_Resource_VK motionVectorsResource = ss::textureToResourceVK( resources, motionVectors, false );
			NVSDK_NGX_Resource_VK depthResource = ss::textureToResourceVK( resources, depth, false );

			NVSDK_NGX_Parameter_SetVoidPointer( ngxParameters, NVSDK_NGX_Parameter_Color, &unresolvedColorResource );
			NVSDK_NGX_Parameter_SetVoidPointer( ngxParameters, NVSDK_NGX_Parameter_Output, &resolvedColorResource );
			NVSDK_NGX_Parameter_SetVoidPointer( ngxParameters, NVSDK_NGX_Parameter_Depth, &depthResource );
			NVSDK_NGX_Parameter_SetVoidPointer( ngxParameters, NVSDK_NGX_Parameter_MotionVectors, &motionVectorsResource );
			NVSDK_NGX_Parameter_SetF( ngxParameters, NVSDK_NGX_Parameter_Jitter_Offset_X, jitterOffset->x );
			NVSDK_NGX_Parameter_SetF( ngxParameters, NVSDK_NGX_Parameter_Jitter_Offset_Y, jitterOffset->y );
			NVSDK_NGX_Parameter_SetI( ngxParameters, NVSDK_NGX_Parameter_Reset, resetAccumulation ? 1 : 0 );
			NVSDK_NGX_Parameter_SetF( ngxParameters, NVSDK_NGX_Parameter_MV_Scale_X, mVScale->x == 0.0f ? 1.0f : mVScale->x );
			NVSDK_NGX_Parameter_SetF( ngxParameters, NVSDK_NGX_Parameter_MV_Scale_Y, mVScale->y == 0.0f ? 1.0f : mVScale->y );
			NVSDK_NGX_Parameter_SetUI( ngxParameters, NVSDK_NGX_Parameter_DLSS_Render_Subrect_Dimensions_Width, extent.width );
			NVSDK_NGX_Parameter_SetUI( ngxParameters, NVSDK_NGX_Parameter_DLSS_Render_Subrect_Dimensions_Height, extent.height );

			// Unused parameters
			NVSDK_NGX_Parameter_SetVoidPointer( ngxParameters, NVSDK_NGX_Parameter_TransparencyMask, nullptr );
			NVSDK_NGX_Parameter_SetVoidPointer( ngxParameters, NVSDK_NGX_Parameter_ExposureTexture, nullptr );
			NVSDK_NGX_Parameter_SetVoidPointer( ngxParameters, NVSDK_NGX_Parameter_DLSS_Input_Bias_Current_Color_Mask, nullptr );
			NVSDK_NGX_Parameter_SetVoidPointer( ngxParameters, NVSDK_NGX_Parameter_GBuffer_Albedo, nullptr );
			NVSDK_NGX_Parameter_SetVoidPointer( ngxParameters, NVSDK_NGX_Parameter_GBuffer_Roughness, nullptr );
			NVSDK_NGX_Parameter_SetVoidPointer( ngxParameters, NVSDK_NGX_Parameter_GBuffer_Metallic, nullptr );
			NVSDK_NGX_Parameter_SetVoidPointer( ngxParameters, NVSDK_NGX_Parameter_GBuffer_Specular, nullptr );
			NVSDK_NGX_Parameter_SetVoidPointer( ngxParameters, NVSDK_NGX_Parameter_GBuffer_Subsurface, nullptr );
			NVSDK_NGX_Parameter_SetVoidPointer( ngxParameters, NVSDK_NGX_Parameter_GBuffer_Normals, nullptr );
			NVSDK_NGX_Parameter_SetVoidPointer( ngxParameters, NVSDK_NGX_Parameter_GBuffer_ShadingModelId, nullptr );
			NVSDK_NGX_Parameter_SetVoidPointer( ngxParameters, NVSDK_NGX_Parameter_GBuffer_MaterialId, nullptr );
			NVSDK_NGX_Parameter_SetVoidPointer( ngxParameters, NVSDK_NGX_Parameter_GBuffer_Atrrib_8, nullptr );
			NVSDK_NGX_Parameter_SetVoidPointer( ngxParameters, NVSDK_NGX_Parameter_GBuffer_Atrrib_9, nullptr );
			NVSDK_NGX_Parameter_SetVoidPointer( ngxParameters, NVSDK_NGX_Parameter_GBuffer_Atrrib_10, nullptr );
			NVSDK_NGX_Parameter_SetVoidPointer( ngxParameters, NVSDK_NGX_Parameter_GBuffer_Atrrib_11, nullptr );
			NVSDK_NGX_Parameter_SetVoidPointer( ngxParameters, NVSDK_NGX_Parameter_GBuffer_Atrrib_12, nullptr );
			NVSDK_NGX_Parameter_SetVoidPointer( ngxParameters, NVSDK_NGX_Parameter_GBuffer_Atrrib_13, nullptr );
			NVSDK_NGX_Parameter_SetVoidPointer( ngxParameters, NVSDK_NGX_Parameter_GBuffer_Atrrib_14, nullptr );
			NVSDK_NGX_Parameter_SetVoidPointer( ngxParameters, NVSDK_NGX_Parameter_GBuffer_Atrrib_15, nullptr );
			NVSDK_NGX_Parameter_SetUI( ngxParameters, NVSDK_NGX_Parameter_TonemapperType, NVSDK_NGX_TONEMAPPER_STRING );
			NVSDK_NGX_Parameter_SetVoidPointer( ngxParameters, NVSDK_NGX_Parameter_MotionVectors3D, nullptr );
			NVSDK_NGX_Parameter_SetVoidPointer( ngxParameters, NVSDK_NGX_Parameter_IsParticleMask, nullptr );
			NVSDK_NGX_Parameter_SetVoidPointer( ngxParameters, NVSDK_NGX_Parameter_AnimatedTextureMask, nullptr );
			NVSDK_NGX_Parameter_SetVoidPointer( ngxParameters, NVSDK_NGX_Parameter_DepthHighRes, nullptr );
			NVSDK_NGX_Parameter_SetVoidPointer( ngxParameters, NVSDK_NGX_Parameter_Position_ViewSpace, nullptr );
			NVSDK_NGX_Parameter_SetF( ngxParameters, NVSDK_NGX_Parameter_FrameTimeDeltaInMsec, 0.0f );
			NVSDK_NGX_Parameter_SetVoidPointer( ngxParameters, NVSDK_NGX_Parameter_RayTracingHitDistance, nullptr );
			NVSDK_NGX_Parameter_SetVoidPointer( ngxParameters, NVSDK_NGX_Parameter_MotionVectorsReflection, nullptr );
			NVSDK_NGX_Parameter_SetUI( ngxParameters, NVSDK_NGX_Parameter_DLSS_Input_Color_Subrect_Base_X, 0u );
			NVSDK_NGX_Parameter_SetUI( ngxParameters, NVSDK_NGX_Parameter_DLSS_Input_Color_Subrect_Base_Y, 0u );
			NVSDK_NGX_Parameter_SetUI( ngxParameters, NVSDK_NGX_Parameter_DLSS_Input_Depth_Subrect_Base_X, 0u );
			NVSDK_NGX_Parameter_SetUI( ngxParameters, NVSDK_NGX_Parameter_DLSS_Input_Depth_Subrect_Base_Y, 0u );
			NVSDK_NGX_Parameter_SetUI( ngxParameters, NVSDK_NGX_Parameter_DLSS_Input_MV_SubrectBase_X, 0u );
			NVSDK_NGX_Parameter_SetUI( ngxParameters, NVSDK_NGX_Parameter_DLSS_Input_MV_SubrectBase_Y, 0u );
			NVSDK_NGX_Parameter_SetUI( ngxParameters, NVSDK_NGX_Parameter_DLSS_Input_Translucency_SubrectBase_X, 0u );
			NVSDK_NGX_Parameter_SetUI( ngxParameters, NVSDK_NGX_Parameter_DLSS_Input_Translucency_SubrectBase_Y, 0u );
			NVSDK_NGX_Parameter_SetUI( ngxParameters, NVSDK_NGX_Parameter_DLSS_Input_Bias_Current_Color_SubrectBase_X, 0u );
			NVSDK_NGX_Parameter_SetUI( ngxParameters, NVSDK_NGX_Parameter_DLSS_Input_Bias_Current_Color_SubrectBase_Y, 0u );
			NVSDK_NGX_Parameter_SetUI( ngxParameters, NVSDK_NGX_Parameter_DLSS_Output_Subrect_Base_X, 0u );
			NVSDK_NGX_Parameter_SetUI( ngxParameters, NVSDK_NGX_Parameter_DLSS_Output_Subrect_Base_Y, 0u );
			NVSDK_NGX_Parameter_SetF( ngxParameters, NVSDK_NGX_Parameter_DLSS_Pre_Exposure, 1.0f );
			NVSDK_NGX_Parameter_SetF( ngxParameters, NVSDK_NGX_Parameter_DLSS_Exposure_Scale, 1.0f );
			NVSDK_NGX_Parameter_SetI( ngxParameters, NVSDK_NGX_Parameter_DLSS_Indicator_Invert_X_Axis, 0 );
			NVSDK_NGX_Parameter_SetI( ngxParameters, NVSDK_NGX_Parameter_DLSS_Indicator_Invert_Y_Axis, 0 );

			auto result = NVSDK_NGX_VULKAN_EvaluateFeature_C( commandBuffer, m_dlssFeature, ngxParameters, ss::progressCallback );

			if ( NVSDK_NGX_FAILED( result ) )
			{
				log::printError( cuT( "Failed to NVSDK_NGX_VULKAN_EvaluateFeature for DLSS, code = 0x%08x, info: %ls" )
					, result, GetNGXResultAsString( result ) );
			}
		}

		bool isDLSSInitialized()const noexcept
		{
			return m_parent->isNGXInitialized() && m_dlssFeature != nullptr;
		}

		bool isDLSSAvailable()const noexcept
		{
			return m_dlssAvailable;
		}

	private:
		DLSSUpscalingSDK const * m_parent;
		bool m_dlssAvailable{};
		NVSDK_NGX_Handle * m_dlssFeature{};
	};

	RawUniquePtr< UpscalingSDKInstance > DLSSUpscalingSDK::createInstance()const
	{
		return makeRawUnique< DLSSUpscalingSDKInstance >( *this );
	}

	//*********************************************************************************************

#endif

	class BlitUpscalingSDK
		: public UpscalingSDK
	{
	public:
		bool initialise( [[maybe_unused]] RenderDevice const & device )override
		{
			return true;
		}

		void cleanup()noexcept override
		{
		}

		bool queryOptimalSettings( Extent2D const & displaySize
			, [[maybe_unused]] UpscalingConfig const & config
			, UpscalingRecommendedSettings & recommendedSettings )const override
		{
			auto result = Extent2D{ displaySize.width / 2, displaySize.height / 2 };
			recommendedSettings.recommendedOptimalRenderSize = result;
			recommendedSettings.dynamicMaximumRenderSize = result;
			recommendedSettings.dynamicMinimumRenderSize = result;
			return true;
		}

		RawUniquePtr< UpscalingSDKInstance > createInstance()const override;
	};

	class BlitUpscalingSDKInstance
		: public UpscalingSDKInstance
	{
	public:
		bool initialise( [[maybe_unused]] Extent2D const & renderSize
			, [[maybe_unused]] Extent2D const & displaySize
			, [[maybe_unused]] UpscalingConfig const & config )override
		{
			return true;
		}

		void cleanup()noexcept override
		{
		}

		Point2f getJitter( [[maybe_unused]] u32 & frameIndex )const noexcept override
		{
			return Point2f{};
		}

		void evaluate( crg::RecordContext & recContext
			, VkCommandBuffer commandBuffer
			, crg::ImageViewId resolvedColor
			, crg::ImageViewId unresolvedColor
			, [[maybe_unused]] crg::ImageViewId motionVectors
			, [[maybe_unused]] crg::ImageViewId depth
			, [[maybe_unused]] bool resetAccumulation
			, [[maybe_unused]] Point2f jitterOffset
			, [[maybe_unused]] Point2f mVScale )const override
		{
			auto & srcSubresource = unresolvedColor.data->info.subresourceRange;
			auto & dstSubresource = resolvedColor.data->info.subresourceRange;
			auto srcExtent = getExtent( unresolvedColor );
			auto dstExtent = getExtent( resolvedColor );
			VkImageBlit region{ getSubresourceLayer( srcSubresource ), { VkOffset3D{}, VkOffset3D{ int32_t( srcExtent.width ), int32_t( srcExtent.height ), 1 } }
			, getSubresourceLayer( dstSubresource ), { VkOffset3D{}, VkOffset3D{ int32_t( dstExtent.width ), int32_t( dstExtent.height ), 1 } } };
			auto srcState = recContext.getLayoutState( unresolvedColor );
			auto dstState = recContext.getLayoutState( resolvedColor );
			recContext.memoryBarrier( commandBuffer, unresolvedColor, makeLayoutState( ImageLayout::eTransferSrc ) );
			recContext.memoryBarrier( commandBuffer, resolvedColor, makeLayoutState( ImageLayout::eTransferDst ) );
			auto & resources = recContext.getResources();
			resources->vkCmdBlitImage( commandBuffer
				, resources.createImage( unresolvedColor.data->image ), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
				, resources.createImage( resolvedColor.data->image ), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
				, 1u, &region, VK_FILTER_LINEAR );
			recContext.memoryBarrier( commandBuffer, resolvedColor, dstState );
			recContext.memoryBarrier( commandBuffer, unresolvedColor, srcState );
		}
	};

	RawUniquePtr< UpscalingSDKInstance > BlitUpscalingSDK::createInstance()const
	{
		return makeRawUnique< BlitUpscalingSDKInstance >();
	}

	//*********************************************************************************************

	UpscalingInstance::UpscalingInstance( UpscalingWrapper const & parent
		, RenderTarget & target
		, Extent2D const & renderSize
		, Extent2D const & displaySize
		, UpscalingConfig const & config )
		: m_sdk{ parent.m_sdk->createInstance() }
		, m_target{ target }
	{
		m_sdk->initialise( renderSize, displaySize, config );
	}

	UpscalingInstance::~UpscalingInstance()noexcept
	{
		m_sdk->cleanup();
		m_sdk.reset();
	}

	void UpscalingInstance::update()
	{
		m_target.setJitter( m_sdk->getJitter( m_frameIndex ) / m_target.getRenderSize() );
	}

	void UpscalingInstance::evaluateUpscaling( crg::RecordContext & recContext
		, VkCommandBuffer commandBuffer
		, crg::ImageViewId resolvedColor
		, crg::ImageViewId unresolvedColor
		, crg::ImageViewId motionVectors
		, crg::ImageViewId depth
		, bool resetAccumulation
		, Point2f jitterOffset
		, Point2f mVScale )const
	{
		m_sdk->evaluate( recContext, commandBuffer
			, resolvedColor, unresolvedColor, motionVectors, depth
			, resetAccumulation, jitterOffset, mVScale );
	}

	//*********************************************************************************************

	UpscalingWrapper::UpscalingWrapper( RenderDevice const & device )
	{
#if C3D_UseDLSS
		m_sdk = makeRawUnique< DLSSUpscalingSDK >();
		if ( !m_sdk->initialise( device ) )
#endif
		{
			m_sdk = makeRawUnique< BlitUpscalingSDK >();
			m_sdk->initialise( device );
		}
	}

	UpscalingWrapper::~UpscalingWrapper()noexcept
	{
		m_sdk->cleanup();
		m_sdk.reset();
	}

	bool UpscalingWrapper::queryOptimalSettings( Extent2D const & displaySize
		, UpscalingConfig const & config
		, UpscalingRecommendedSettings & recommendedSettings )const
	{
		if ( !config.enabled )
		{
			recommendedSettings.recommendedOptimalRenderSize = displaySize;
			recommendedSettings.dynamicMaximumRenderSize = displaySize;
			recommendedSettings.dynamicMinimumRenderSize = displaySize;
			return true;
		}

		return m_sdk->queryOptimalSettings( displaySize, config
			, recommendedSettings );
	}

	UpscalingInstanceUPtr UpscalingWrapper::createInstance( RenderTarget & target
		, Extent2D const & renderSize
		, Extent2D const & displaySize
		, UpscalingConfig const & config )const
	{
		return makeUnique< UpscalingInstance >( *this, target
			, renderSize, displaySize, config );
	}

	//*********************************************************************************************
}

CU_ImplementSmartPtr( c3d, UpscalingWrapper )
CU_ImplementSmartPtr( c3d, UpscalingInstance )
