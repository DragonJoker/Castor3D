/*
See LICENSE file in root folder
*/
#include "Castor3D/Render/Upscale/UpscaleDLSS.hpp"

#if C3D_UseDLSS

#include "Castor3D/Engine.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/Upscale/UpscaleBlit.hpp"

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Command/CommandPool.hpp>

#include <RenderGraph/RunnableGraph.hpp>

#include <nvsdk_ngx.h>
#include <nvsdk_ngx_helpers.h>
#include <nvsdk_ngx_vk.h>
#include <nvsdk_ngx_helpers_vk.h>

namespace c3d
{
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

		void NVSDK_CONV progressCallback( float currentProgress
			, [[maybe_unused]] bool * OutShouldCancel )
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

	DLSSUpscalingSDKInstance::DLSSUpscalingSDKInstance( RenderDevice const & device
		, NVSDK_NGX_Parameter * m_ngxParameters )
		: m_device{ device }
		, m_ngxParameters{ m_ngxParameters }
	{
	}

	bool DLSSUpscalingSDKInstance::initialise( Extent2D const & renderSize
		, Extent2D const & displaySize
		, UpscaleConfig const & config )
	{
		auto renderPreset = NVSDK_NGX_DLSS_Hint_Render_Preset_Default;
		unsigned int creationNodeMask = 1;
		unsigned int visibilityNodeMask = 1;

		// Next create features
		int dlssCreateFeatureFlags = NVSDK_NGX_DLSS_Feature_Flags_None;
		dlssCreateFeatureFlags |= NVSDK_NGX_DLSS_Feature_Flags_IsHDR;
		dlssCreateFeatureFlags |= NVSDK_NGX_DLSS_Feature_Flags_DepthInverted;
		dlssCreateFeatureFlags |= NVSDK_NGX_DLSS_Feature_Flags_MVLowRes;

		// Select render preset (DL weights)
		NVSDK_NGX_Parameter_SetUI( m_ngxParameters, NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_DLAA, renderPreset );              // will remain the chosen weights after OTA
		NVSDK_NGX_Parameter_SetUI( m_ngxParameters, NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_Quality, renderPreset );           // ^
		NVSDK_NGX_Parameter_SetUI( m_ngxParameters, NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_Balanced, renderPreset );          // ^
		NVSDK_NGX_Parameter_SetUI( m_ngxParameters, NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_Performance, renderPreset );       // ^
		NVSDK_NGX_Parameter_SetUI( m_ngxParameters, NVSDK_NGX_Parameter_DLSS_Hint_Render_Preset_UltraPerformance, renderPreset );  // ^

		// Set parameters
		NVSDK_NGX_Parameter_SetUI( m_ngxParameters, NVSDK_NGX_Parameter_CreationNodeMask, creationNodeMask );
		NVSDK_NGX_Parameter_SetUI( m_ngxParameters, NVSDK_NGX_Parameter_VisibilityNodeMask, visibilityNodeMask );
		NVSDK_NGX_Parameter_SetUI( m_ngxParameters, NVSDK_NGX_Parameter_Width, renderSize.width );
		NVSDK_NGX_Parameter_SetUI( m_ngxParameters, NVSDK_NGX_Parameter_Height, renderSize.height );
		NVSDK_NGX_Parameter_SetUI( m_ngxParameters, NVSDK_NGX_Parameter_OutWidth, displaySize.width );
		NVSDK_NGX_Parameter_SetUI( m_ngxParameters, NVSDK_NGX_Parameter_OutHeight, displaySize.height );
		NVSDK_NGX_Parameter_SetI( m_ngxParameters, NVSDK_NGX_Parameter_PerfQualityValue, NVSDK_NGX_PerfQuality_Value( config.perfQualityMode ) );
		NVSDK_NGX_Parameter_SetI( m_ngxParameters, NVSDK_NGX_Parameter_DLSS_Feature_Create_Flags, dlssCreateFeatureFlags );
		NVSDK_NGX_Parameter_SetI( m_ngxParameters, NVSDK_NGX_Parameter_DLSS_Enable_Output_Subrects, 0 );

		auto commandList = m_device.computeCommandPool->createCommandBuffer();
		commandList->begin();
		auto resultDLSS = NVSDK_NGX_VULKAN_CreateFeature1( *m_device, *commandList, NVSDK_NGX_Feature_SuperSampling, m_ngxParameters, &m_dlssFeature );
		commandList->end();

		if ( NVSDK_NGX_FAILED( resultDLSS ) )
		{
			log::printError( cuT( "Failed to create DLSS Features = 0x%08x, info: %ls\n" )
				, resultDLSS, GetNGXResultAsString( resultDLSS ) );
			return false;
		}

		auto fence = m_device->createFence();
		m_device.computeQueue->submit( *commandList, fence.get() );
		fence->wait( ashes::MaxTimeout );

		m_dlssAvailable = true;
		return true;
	}

	void DLSSUpscalingSDKInstance::cleanup()noexcept
	{
		m_device->waitIdle();
		NVSDK_NGX_Result resultDLSS = ( m_dlssFeature != nullptr ) ? NVSDK_NGX_VULKAN_ReleaseFeature( m_dlssFeature ) : NVSDK_NGX_Result_Success;
		if ( NVSDK_NGX_FAILED( resultDLSS ) )
		{
			log::printError( cuT( "Failed to NVSDK_NGX_D3D12_ReleaseFeature, code = 0x%08x, info: %ls\n" )
				, resultDLSS, GetNGXResultAsString( resultDLSS ) );
		}

		m_dlssFeature = nullptr;
	}

	Point2f DLSSUpscalingSDKInstance::getJitter( u32 & frameIndex )const noexcept
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

	void DLSSUpscalingSDKInstance::evaluate( crg::RecordContext & recContext
		, VkCommandBuffer commandBuffer
		, crg::ImageViewId resolvedColor
		, crg::ImageViewId unresolvedColor
		, crg::ImageViewId motionVectors
		, crg::ImageViewId depth
		, bool resetAccumulation
		, Point2f const & jitterOffset
		, Point2f const & mVScale )const
	{
		if ( m_dlssAvailable )
		{
			log::error << cuT( "DLSS is not available - could not evaluate Upscaling\n" );
			return;
		}

		auto extent = getExtent( unresolvedColor );

		auto & resources = recContext.getResources();
		NVSDK_NGX_Resource_VK resolvedColorResource = ss::textureToResourceVK( resources, resolvedColor, true );
		NVSDK_NGX_Resource_VK unresolvedColorResource = ss::textureToResourceVK( resources, unresolvedColor, false );
		NVSDK_NGX_Resource_VK motionVectorsResource = ss::textureToResourceVK( resources, motionVectors, false );
		NVSDK_NGX_Resource_VK depthResource = ss::textureToResourceVK( resources, depth, false );

		NVSDK_NGX_Parameter_SetF( m_ngxParameters, NVSDK_NGX_Parameter_Jitter_Offset_X, jitterOffset->x );
		NVSDK_NGX_Parameter_SetF( m_ngxParameters, NVSDK_NGX_Parameter_Jitter_Offset_Y, jitterOffset->y );
		NVSDK_NGX_Parameter_SetI( m_ngxParameters, NVSDK_NGX_Parameter_Reset, resetAccumulation ? 1 : 0 );
		NVSDK_NGX_Parameter_SetF( m_ngxParameters, NVSDK_NGX_Parameter_MV_Scale_X, mVScale->x == 0.0f ? 1.0f : mVScale->x );
		NVSDK_NGX_Parameter_SetF( m_ngxParameters, NVSDK_NGX_Parameter_MV_Scale_Y, mVScale->y == 0.0f ? 1.0f : mVScale->y );
		NVSDK_NGX_Parameter_SetUI( m_ngxParameters, NVSDK_NGX_Parameter_DLSS_Render_Subrect_Dimensions_Width, extent.width );
		NVSDK_NGX_Parameter_SetUI( m_ngxParameters, NVSDK_NGX_Parameter_DLSS_Render_Subrect_Dimensions_Height, extent.height );

		/// Resources
		NVSDK_NGX_Parameter_SetVoidPointer( m_ngxParameters, NVSDK_NGX_Parameter_Color, &unresolvedColorResource );
		NVSDK_NGX_Parameter_SetVoidPointer( m_ngxParameters, NVSDK_NGX_Parameter_Output, &resolvedColorResource );
		NVSDK_NGX_Parameter_SetVoidPointer( m_ngxParameters, NVSDK_NGX_Parameter_Depth, &depthResource );
		NVSDK_NGX_Parameter_SetVoidPointer( m_ngxParameters, NVSDK_NGX_Parameter_MotionVectors, &motionVectorsResource );

		// Unused parameters
		NVSDK_NGX_Parameter_SetVoidPointer( m_ngxParameters, NVSDK_NGX_Parameter_TransparencyMask, nullptr );
		NVSDK_NGX_Parameter_SetVoidPointer( m_ngxParameters, NVSDK_NGX_Parameter_ExposureTexture, nullptr );
		NVSDK_NGX_Parameter_SetVoidPointer( m_ngxParameters, NVSDK_NGX_Parameter_DLSS_Input_Bias_Current_Color_Mask, nullptr );
		NVSDK_NGX_Parameter_SetVoidPointer( m_ngxParameters, NVSDK_NGX_Parameter_GBuffer_Albedo, nullptr );
		NVSDK_NGX_Parameter_SetVoidPointer( m_ngxParameters, NVSDK_NGX_Parameter_GBuffer_Roughness, nullptr );
		NVSDK_NGX_Parameter_SetVoidPointer( m_ngxParameters, NVSDK_NGX_Parameter_GBuffer_Metallic, nullptr );
		NVSDK_NGX_Parameter_SetVoidPointer( m_ngxParameters, NVSDK_NGX_Parameter_GBuffer_Specular, nullptr );
		NVSDK_NGX_Parameter_SetVoidPointer( m_ngxParameters, NVSDK_NGX_Parameter_GBuffer_Subsurface, nullptr );
		NVSDK_NGX_Parameter_SetVoidPointer( m_ngxParameters, NVSDK_NGX_Parameter_GBuffer_Normals, nullptr );
		NVSDK_NGX_Parameter_SetVoidPointer( m_ngxParameters, NVSDK_NGX_Parameter_GBuffer_ShadingModelId, nullptr );
		NVSDK_NGX_Parameter_SetVoidPointer( m_ngxParameters, NVSDK_NGX_Parameter_GBuffer_MaterialId, nullptr );
		NVSDK_NGX_Parameter_SetVoidPointer( m_ngxParameters, NVSDK_NGX_Parameter_GBuffer_Atrrib_8, nullptr );
		NVSDK_NGX_Parameter_SetVoidPointer( m_ngxParameters, NVSDK_NGX_Parameter_GBuffer_Atrrib_9, nullptr );
		NVSDK_NGX_Parameter_SetVoidPointer( m_ngxParameters, NVSDK_NGX_Parameter_GBuffer_Atrrib_10, nullptr );
		NVSDK_NGX_Parameter_SetVoidPointer( m_ngxParameters, NVSDK_NGX_Parameter_GBuffer_Atrrib_11, nullptr );
		NVSDK_NGX_Parameter_SetVoidPointer( m_ngxParameters, NVSDK_NGX_Parameter_GBuffer_Atrrib_12, nullptr );
		NVSDK_NGX_Parameter_SetVoidPointer( m_ngxParameters, NVSDK_NGX_Parameter_GBuffer_Atrrib_13, nullptr );
		NVSDK_NGX_Parameter_SetVoidPointer( m_ngxParameters, NVSDK_NGX_Parameter_GBuffer_Atrrib_14, nullptr );
		NVSDK_NGX_Parameter_SetVoidPointer( m_ngxParameters, NVSDK_NGX_Parameter_GBuffer_Atrrib_15, nullptr );
		NVSDK_NGX_Parameter_SetUI( m_ngxParameters, NVSDK_NGX_Parameter_TonemapperType, NVSDK_NGX_TONEMAPPER_STRING );
		NVSDK_NGX_Parameter_SetVoidPointer( m_ngxParameters, NVSDK_NGX_Parameter_MotionVectors3D, nullptr );
		NVSDK_NGX_Parameter_SetVoidPointer( m_ngxParameters, NVSDK_NGX_Parameter_IsParticleMask, nullptr );
		NVSDK_NGX_Parameter_SetVoidPointer( m_ngxParameters, NVSDK_NGX_Parameter_AnimatedTextureMask, nullptr );
		NVSDK_NGX_Parameter_SetVoidPointer( m_ngxParameters, NVSDK_NGX_Parameter_DepthHighRes, nullptr );
		NVSDK_NGX_Parameter_SetVoidPointer( m_ngxParameters, NVSDK_NGX_Parameter_Position_ViewSpace, nullptr );
		NVSDK_NGX_Parameter_SetF( m_ngxParameters, NVSDK_NGX_Parameter_FrameTimeDeltaInMsec, 0.0f );
		NVSDK_NGX_Parameter_SetVoidPointer( m_ngxParameters, NVSDK_NGX_Parameter_RayTracingHitDistance, nullptr );
		NVSDK_NGX_Parameter_SetVoidPointer( m_ngxParameters, NVSDK_NGX_Parameter_MotionVectorsReflection, nullptr );
		NVSDK_NGX_Parameter_SetUI( m_ngxParameters, NVSDK_NGX_Parameter_DLSS_Input_Color_Subrect_Base_X, 0u );
		NVSDK_NGX_Parameter_SetUI( m_ngxParameters, NVSDK_NGX_Parameter_DLSS_Input_Color_Subrect_Base_Y, 0u );
		NVSDK_NGX_Parameter_SetUI( m_ngxParameters, NVSDK_NGX_Parameter_DLSS_Input_Depth_Subrect_Base_X, 0u );
		NVSDK_NGX_Parameter_SetUI( m_ngxParameters, NVSDK_NGX_Parameter_DLSS_Input_Depth_Subrect_Base_Y, 0u );
		NVSDK_NGX_Parameter_SetUI( m_ngxParameters, NVSDK_NGX_Parameter_DLSS_Input_MV_SubrectBase_X, 0u );
		NVSDK_NGX_Parameter_SetUI( m_ngxParameters, NVSDK_NGX_Parameter_DLSS_Input_MV_SubrectBase_Y, 0u );
		NVSDK_NGX_Parameter_SetUI( m_ngxParameters, NVSDK_NGX_Parameter_DLSS_Input_Translucency_SubrectBase_X, 0u );
		NVSDK_NGX_Parameter_SetUI( m_ngxParameters, NVSDK_NGX_Parameter_DLSS_Input_Translucency_SubrectBase_Y, 0u );
		NVSDK_NGX_Parameter_SetUI( m_ngxParameters, NVSDK_NGX_Parameter_DLSS_Input_Bias_Current_Color_SubrectBase_X, 0u );
		NVSDK_NGX_Parameter_SetUI( m_ngxParameters, NVSDK_NGX_Parameter_DLSS_Input_Bias_Current_Color_SubrectBase_Y, 0u );
		NVSDK_NGX_Parameter_SetUI( m_ngxParameters, NVSDK_NGX_Parameter_DLSS_Output_Subrect_Base_X, 0u );
		NVSDK_NGX_Parameter_SetUI( m_ngxParameters, NVSDK_NGX_Parameter_DLSS_Output_Subrect_Base_Y, 0u );
		NVSDK_NGX_Parameter_SetF( m_ngxParameters, NVSDK_NGX_Parameter_DLSS_Pre_Exposure, 1.0f );
		NVSDK_NGX_Parameter_SetF( m_ngxParameters, NVSDK_NGX_Parameter_DLSS_Exposure_Scale, 1.0f );
		NVSDK_NGX_Parameter_SetI( m_ngxParameters, NVSDK_NGX_Parameter_DLSS_Indicator_Invert_X_Axis, 0 );
		NVSDK_NGX_Parameter_SetI( m_ngxParameters, NVSDK_NGX_Parameter_DLSS_Indicator_Invert_Y_Axis, 0 );

		auto result = NVSDK_NGX_VULKAN_EvaluateFeature_C( commandBuffer, m_dlssFeature, m_ngxParameters, ss::progressCallback );

		if ( NVSDK_NGX_FAILED( result ) )
		{
			log::printError( cuT( "Failed to NVSDK_NGX_VULKAN_EvaluateFeature for DLSS, code = 0x%08x, info: %ls" )
				, result, GetNGXResultAsString( result ) );
		}
	}

	//*********************************************************************************************

	bool DLSSUpscalingSDK::initialise( RenderDevice const & device )
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

	void DLSSUpscalingSDK::cleanup()noexcept
	{
		if ( isNGXInitialized() )
		{
			m_device->device->waitIdle();
			NVSDK_NGX_VULKAN_DestroyParameters( m_ngxParameters );
			NVSDK_NGX_VULKAN_Shutdown1( *m_device->device );
			m_ngxInitialized = false;
		}
	}

	bool DLSSUpscalingSDK::queryOptimalSettings( Extent2D const & displaySize
		, UpscaleConfig const & config
		, Extent2D & recommendedSize )const
	{
		recommendedSize = displaySize;

		if ( !isNGXInitialized() )
		{
			log::info << cuT( "NGX was not initialized when querying Optimal Settings\n" );
			return false;
		}

		void * callback = nullptr;
		NVSDK_NGX_Result result = NVSDK_NGX_Result_Success;
		NVSDK_NGX_Parameter_GetVoidPointer( m_ngxParameters, NVSDK_NGX_Parameter_DLSSOptimalSettingsCallback, &callback );
		if ( NVSDK_NGX_FAILED( result ) || !callback )
		{
			log::error << cuT( "NGX SDK 'getOptimalSettings' callback is missing, please make sure DLSSContext feature is up to date\n" );
			return false;
		}

		NVSDK_NGX_Parameter_SetUI( m_ngxParameters, NVSDK_NGX_Parameter_Width, displaySize.width );
		NVSDK_NGX_Parameter_SetUI( m_ngxParameters, NVSDK_NGX_Parameter_Height, displaySize.height );
		NVSDK_NGX_Parameter_SetI( m_ngxParameters, NVSDK_NGX_Parameter_PerfQualityValue, NVSDK_NGX_PerfQuality_Value( config.perfQualityMode ) );
		NVSDK_NGX_Parameter_SetI( m_ngxParameters, NVSDK_NGX_Parameter_RTXValue, false ); // Some older DLSS dlls still expect this value to be set

		result = NVSDK_NGX_Result_Success;
		auto getOptimalSettingsCallback = reinterpret_cast< PFN_NVSDK_NGX_DLSS_GetOptimalSettingsCallback >( callback );
		result = getOptimalSettingsCallback( m_ngxParameters );
		if ( NVSDK_NGX_FAILED( result ) )
		{
			log::printWarn( cuT( "NGX SDK 'getOptimalSettings' callback failed! code = 0x%08x, info: %ls\n" )
				, result, GetNGXResultAsString( result ) );
			return false;
		}

		Extent2D recommendedOptimalRenderSize;
		NVSDK_NGX_Parameter_GetUI( m_ngxParameters, NVSDK_NGX_Parameter_OutWidth, &recommendedOptimalRenderSize.width );
		NVSDK_NGX_Parameter_GetUI( m_ngxParameters, NVSDK_NGX_Parameter_OutHeight, &recommendedOptimalRenderSize.height );
		// If we have an older DLSS Dll those might need to be set to the optimal dimensions instead
		Extent2D dynamicMaximumRenderSize{ recommendedOptimalRenderSize };
		Extent2D dynamicMinimumRenderSize{ recommendedOptimalRenderSize };

		NVSDK_NGX_Parameter_GetUI( m_ngxParameters, NVSDK_NGX_Parameter_DLSS_Get_Dynamic_Max_Render_Width, &dynamicMaximumRenderSize.width );
		NVSDK_NGX_Parameter_GetUI( m_ngxParameters, NVSDK_NGX_Parameter_DLSS_Get_Dynamic_Max_Render_Height, &dynamicMaximumRenderSize.height );
		NVSDK_NGX_Parameter_GetUI( m_ngxParameters, NVSDK_NGX_Parameter_DLSS_Get_Dynamic_Min_Render_Width, &dynamicMinimumRenderSize.width );
		NVSDK_NGX_Parameter_GetUI( m_ngxParameters, NVSDK_NGX_Parameter_DLSS_Get_Dynamic_Min_Render_Height, &dynamicMinimumRenderSize.height );

		if ( dynamicMaximumRenderSize == Extent2D{}
			&& dynamicMinimumRenderSize == Extent2D{}
			&& recommendedOptimalRenderSize == Extent2D{} )
		{
			log::error << cuT( "NGX SDK Couldn't retrieve proper settings, the configured mode may not be supported\n" );
			return false;
		}

		recommendedSize = recommendedOptimalRenderSize;
		return true;
	}

	bool DLSSUpscalingSDK::isFeatureSupported( NVSDK_NGX_FeatureDiscoveryInfo const * dis )const
	{
		VkPhysicalDevice vkphysicaldevice = m_device->gpu;
		VkInstance vkinstance = m_device->device->getInstance();

		uint32_t nDeviceExtensions;
		VkExtensionProperties * deviceExtensions;
		auto res = NVSDK_NGX_VULKAN_GetFeatureDeviceExtensionRequirements( vkinstance, vkphysicaldevice, dis, &nDeviceExtensions, &deviceExtensions );
		if ( res != NVSDK_NGX_Result_Success )
		{
			log::printError( cuT( "NGX SDK GetFeatureRequirements error: GetFeatureDeviceExtensionRequirements returned 0x%08x info: %ls\n" )
				, res, GetNGXResultAsString( res ) );
			return false;
		}
		else
		{
			log::printInfo( cuT( "NGX SDK GetFeatureDeviceExtensionRequirements returned %d device extension requirements: " )
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
			log::printError( cuT( "NGX SDK GetFeatureRequirements error: GetFeatureInstanceExtensionRequirements returned 0x%08x info: %ls\n" )
				, res, GetNGXResultAsString( res ) );
			return false;
		}
		else
		{
			log::printInfo( cuT( "NGX SDK GetFeatureInstanceExtensionRequirements returned %d instance extension requirements: " )
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
			log::printError( cuT( "NGX SDK GetFeatureRequirements error: 0x%08x info: %ls\n" )
				, res, GetNGXResultAsString( res ) );
			return false;
		}

		log::printInfo( cuT( "NGX SDK GetFeatureRequirements returned 0x%08x: Min GPU Arch: 0x%08x MinOS: %s\n" )
			, req.FeatureSupported, req.MinHWArchitecture, req.MinOSVersion );

		return true;
	}

	RawUniquePtr< UpscalingSDKInstance > DLSSUpscalingSDK::createInstance()const
	{
		if ( !m_ngxInitialized )
			return makeRawUnique< BlitUpscalingSDKInstance >();

		return makeRawUnique< DLSSUpscalingSDKInstance >( *m_device, m_ngxParameters );
	}

	//*********************************************************************************************
}

#endif
