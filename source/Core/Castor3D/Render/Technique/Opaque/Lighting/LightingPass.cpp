#include "Castor3D/Render/Technique/Opaque/Lighting/LightingPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LayeredLightPropagationVolumes.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumes.hpp"
#include "Castor3D/Render/Technique/RenderTechniquePass.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePass.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/DirectionalLightPass.hpp"
#include "Castor3D/Render/Technique/Opaque/ReflectiveShadowMapGI/LightPassReflectiveShadow.hpp"
#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LightPassVolumePropagationShadow.hpp"
#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LightPassLayeredVolumePropagationShadow.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassShadow.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/PointLightPass.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/SpotLightPass.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelMatrixUbo.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	namespace
	{
		constexpr uint32_t getDirectionalGIidx( LightingPass::Type type )
		{
			return std::min( uint32_t( type ), uint32_t( LightingPass::Type::eShadowLayeredLpvGGI ) );
		}

		constexpr uint32_t getPointGIidx( LightingPass::Type type )
		{
			return std::min( uint32_t( type ), uint32_t( LightingPass::Type::eShadowRsmGI ) );
		}

		constexpr uint32_t getSpotGIidx( LightingPass::Type type )
		{
			return std::min( uint32_t( type ), uint32_t( LightingPass::Type::eShadowLpvGGI ) );
		}

		constexpr LightingPass::Type getLightType( LightingPass::Type passType
			, LightType lightType )
		{
			switch ( lightType )
			{
			case LightType::eDirectional:
				return LightingPass::Type( getDirectionalGIidx( passType ) );
			case LightType::ePoint:
				return LightingPass::Type( getPointGIidx( passType ) );
			case LightType::eSpot:
				return LightingPass::Type( getSpotGIidx( passType ) );
			default:
				CU_Failure( "Unsupported LightType" );
				return LightingPass::Type( std::min( uint32_t( passType ), uint32_t( LightingPass::Type::eShadowNoGI ) ) );
			}
		}

		LightPass * getLightPass( LightingPass::Type passType
			, LightType lightType
			, LightingPass::LightPasses const & passes )
		{
			auto lt = uint32_t( lightType );

			switch ( lightType )
			{
			case LightType::eDirectional:
				return passes[getDirectionalGIidx( passType )][lt].get();
			case LightType::ePoint:
				return passes[getPointGIidx( passType )][lt].get();
			case LightType::eSpot:
				return passes[getSpotGIidx( passType )][lt].get();
			default:
				CU_Failure( "Unsupported LightType" );
				return nullptr;
			}
		}

		LightPassUPtr & getLightPass( LightingPass::Type passType
			, LightType lightType
			, LightingPass::LightPasses & passes )
		{
			auto lt = uint32_t( lightType );

			switch ( lightType )
			{
			case LightType::eDirectional:
				return passes[getDirectionalGIidx( passType )][lt];
			case LightType::ePoint:
				return passes[getPointGIidx( passType )][lt];
			case LightType::eSpot:
				return passes[getSpotGIidx( passType )][lt];
			default:
				{
					static LightPassUPtr dummy;
					CU_Failure( "Unsupported LightType" );
					return dummy;
				}
			}
		}

		template< LightingPass::Type PassT, LightType LightT >
		struct PassInfoT;

		template<>
		struct PassInfoT< LightingPass::Type::eNoShadow, LightType::eDirectional >
		{
			using Type = DirectionalLightPass;
		};

		template<>
		struct PassInfoT< LightingPass::Type::eNoShadow, LightType::ePoint >
		{
			using Type = PointLightPass;
		};

		template<>
		struct PassInfoT< LightingPass::Type::eNoShadow, LightType::eSpot >
		{
			using Type = SpotLightPass;
		};

		template<>
		struct PassInfoT< LightingPass::Type::eShadowNoGI, LightType::eDirectional >
		{
			using Type = DirectionalLightPassShadow;
		};

		template<>
		struct PassInfoT< LightingPass::Type::eShadowNoGI, LightType::ePoint >
		{
			using Type = PointLightPassShadow;
		};

		template<>
		struct PassInfoT< LightingPass::Type::eShadowNoGI, LightType::eSpot >
		{
			using Type = SpotLightPassShadow;
		};

		template<>
		struct PassInfoT< LightingPass::Type::eShadowRsmGI, LightType::eDirectional >
		{
			using Type = DirectionalLightPassReflectiveShadow;
		};

		template<>
		struct PassInfoT< LightingPass::Type::eShadowRsmGI, LightType::ePoint >
		{
			using Type = PointLightPassReflectiveShadow;
		};

		template<>
		struct PassInfoT< LightingPass::Type::eShadowRsmGI, LightType::eSpot >
		{
			using Type = SpotLightPassReflectiveShadow;
		};

		template<>
		struct PassInfoT< LightingPass::Type::eShadowLpvGI, LightType::eDirectional >
		{
			using Type = DirectionalLightPassLPVShadow;
		};

		template<>
		struct PassInfoT< LightingPass::Type::eShadowLpvGI, LightType::ePoint >
		{
			using Type = PointLightPassShadow;
		};

		template<>
		struct PassInfoT< LightingPass::Type::eShadowLpvGI, LightType::eSpot >
		{
			using Type = SpotLightPassLPVShadow;
		};

		template<>
		struct PassInfoT< LightingPass::Type::eShadowLayeredLpvGI, LightType::eDirectional >
		{
			using Type = DirectionalLightPassLayeredLPVShadow;
		};

		template<>
		struct PassInfoT< LightingPass::Type::eShadowLayeredLpvGI, LightType::ePoint >
		{
			using Type = PointLightPassShadow;
		};

		template<>
		struct PassInfoT< LightingPass::Type::eShadowLayeredLpvGI, LightType::eSpot >
		{
			using Type = SpotLightPassLPVShadow;
		};

		template<>
		struct PassInfoT< LightingPass::Type::eShadowLpvGGI, LightType::eDirectional >
		{
			using Type = DirectionalLightPassLPVShadow;
		};

		template<>
		struct PassInfoT< LightingPass::Type::eShadowLpvGGI, LightType::ePoint >
		{
			using Type = PointLightPassShadow;
		};

		template<>
		struct PassInfoT< LightingPass::Type::eShadowLpvGGI, LightType::eSpot >
		{
			using Type = SpotLightPassLPVShadow;
		};

		template<>
		struct PassInfoT< LightingPass::Type::eShadowLayeredLpvGGI, LightType::eDirectional >
		{
			using Type = DirectionalLightPassLayeredLPVShadow;
		};

		template<>
		struct PassInfoT< LightingPass::Type::eShadowLayeredLpvGGI, LightType::ePoint >
		{
			using Type = PointLightPassShadow;
		};

		template<>
		struct PassInfoT< LightingPass::Type::eShadowLayeredLpvGGI, LightType::eSpot >
		{
			using Type = SpotLightPassLPVShadow;
		};

		template< LightingPass::Type PassT, LightType LightT >
		using PassTypeT = typename PassInfoT< PassT, LightT >::Type;

		template< LightingPass::Type PassT >
		LightPassUPtr makeLightPassNoGIT( LightType lightType
			, RenderDevice const & device
			, LightPassResult const & lpResult
			, GpInfoUbo const & gpInfoUbo )
		{
			LightPassConfig lpConfig{ lpResult, gpInfoUbo };

			switch ( lightType )
			{
			case LightType::eDirectional:
				return std::make_unique< PassTypeT< PassT, LightType::eDirectional > >( device, lpConfig );
			case LightType::ePoint:
				return std::make_unique< PassTypeT< PassT, LightType::ePoint > >( device, lpConfig );
			case LightType::eSpot:
				return std::make_unique< PassTypeT< PassT, LightType::eSpot > >( device, lpConfig );
			default:
				CU_Failure( "Unsupported LightType" );
				return LightPassUPtr{};
			}
		}

		template< LightingPass::Type PassT >
		LightPassUPtr makeLightPassRsmGIT( LightType lightType
			, RenderDevice const & device
			, LightPassResult const & lpResult
			, GpInfoUbo const & gpInfoUbo
			, LightCache const & lightCache
			, OpaquePassResult const & gpResult
			, ShadowMapResult const & smDirectionalResult
			, ShadowMapResult const & smPointResult
			, ShadowMapResult const & smSpotResult )
		{
			switch ( lightType )
			{
			case LightType::eDirectional:
				return std::make_unique< PassTypeT< PassT, LightType::eDirectional > >( device
					, RsmLightPassConfig{ { lpResult, gpInfoUbo }
						, lightCache
						, gpResult
						, smDirectionalResult } );
			case LightType::ePoint:
				return std::make_unique< PassTypeT< PassT, LightType::ePoint > >( device
					, RsmLightPassConfig{ { lpResult, gpInfoUbo }
						, lightCache
						, gpResult
						, smPointResult } );
			case LightType::eSpot:
				return std::make_unique< PassTypeT< PassT, LightType::eSpot > >( device
					, RsmLightPassConfig{ { lpResult, gpInfoUbo }
						, lightCache
						, gpResult
						, smSpotResult } );
			default:
				CU_Failure( "Unsupported LightType" );
				return LightPassUPtr{};
			}
		}

		template< LightingPass::Type PassT >
		LightPassUPtr makeLightPassLpvGIT( LightType lightType
			, RenderDevice const & device
			, LightPassResult const & lpResult
			, GpInfoUbo const & gpInfoUbo
			, LightCache const & lightCache
			, OpaquePassResult const & gpResult
			, ShadowMapResult const & smDirectionalResult
			, ShadowMapResult const & smPointResult
			, ShadowMapResult const & smSpotResult
			, LightVolumePassResult const & lpvResult
			, LpvGridConfigUbo const & lpvConfigUbo )
		{
			switch ( lightType )
			{
			case LightType::eDirectional:
				return std::make_unique< PassTypeT< PassT, LightType::eDirectional > >( device
					, LpvLightPassConfig{ LightPassConfig{ lpResult, gpInfoUbo }
						, lightCache
						, gpResult
						, smDirectionalResult
						, lpvResult
						, lpvConfigUbo } );
			//case LightType::ePoint:
			//	return std::make_unique< PassTypeT< PassT, LightType::ePoint > >( device
			//		, LpvLightPassConfig{ LightPassConfig{ lpResult, gpInfoUbo }
			//			, lightCache
			//			, gpResult
			//			, smPointResult
			//			, lpvResult
			//			, lpvConfigUbo } );
			case LightType::eSpot:
				return std::make_unique< PassTypeT< PassT, LightType::eSpot > >( device
					, LpvLightPassConfig{ LightPassConfig{ lpResult, gpInfoUbo }
						, lightCache
						, gpResult
						, smSpotResult
						, lpvResult
						, lpvConfigUbo } );
			default:
				CU_Failure( "Unsupported LightType" );
				return LightPassUPtr{};
			}
		}

		template< LightingPass::Type PassT >
		LightPassUPtr makeLightPassLlpvGIT( LightType lightType
			, RenderDevice const & device
			, LightPassResult const & lpResult
			, GpInfoUbo const & gpInfoUbo
			, LightCache const & lightCache
			, OpaquePassResult const & gpResult
			, ShadowMapResult const & smDirectionalResult
			, ShadowMapResult const & smPointResult
			, ShadowMapResult const & smSpotResult
			, LightVolumePassResult const & lpvResult
			, LayeredLpvGridConfigUbo const & lpvConfigUbo )
		{
			switch ( lightType )
			{
			case LightType::eDirectional:
				return std::make_unique< PassTypeT< PassT, LightType::eDirectional > >( device
					, LayeredLpvLightPassConfig{ LightPassConfig{ lpResult, gpInfoUbo }
						, lightCache
						, gpResult
						, smDirectionalResult
						, lpvResult
						, lpvConfigUbo } );
			//case LightType::ePoint:
			//	return std::make_unique< PassTypeT< PassT, LightType::ePoint > >( device
			//		, LayeredLpvLightPassConfig{ LightPassConfig{ lpResult, gpInfoUbo }
			//			, lightCache
			//			, gpResult
			//			, smPointResult
			//			, lpvResult
			//			, lpvConfigUbo
			//			, lpvConfigUbos } );
			//case LightType::eSpot:
			//	return std::make_unique< PassTypeT< PassT, LightType::eSpot > >( device
			//		, LayeredLpvLightPassConfig{ LightPassConfig{ lpResult, gpInfoUbo }
			//			, lightCache
			//			, gpResult
			//			, smSpotResult
			//			, lpvResult
			//			, lpvConfigUbo
			//			, lpvConfigUbos } );
			default:
				CU_Failure( "Unsupported LightType" );
				return LightPassUPtr{};
			}
		}

		LightPassUPtr makeLightPass( Engine & engine
			, LightingPass::Type passType
			, LightType lightType
			, RenderDevice const & device
			, LightCache const & lightCache
			, OpaquePassResult const & gpResult
			, ShadowMapResult const & smDirectionalResult
			, ShadowMapResult const & smPointResult
			, ShadowMapResult const & smSpotResult
			, LightPassResult const & lpResult
			, LightVolumePassResult const & lpvResult
			, GpInfoUbo const & gpInfoUbo
			, LpvGridConfigUbo const & lpvConfigUbo
			, LayeredLpvGridConfigUbo const & llpvConfigUbo )
		{
			switch ( passType )
			{
			case LightingPass::Type::eNoShadow:
				return makeLightPassNoGIT< LightingPass::Type::eNoShadow >( lightType
					, device
					, lpResult
					, gpInfoUbo );
			case LightingPass::Type::eShadowNoGI:
				return makeLightPassNoGIT< LightingPass::Type::eShadowNoGI >( lightType
					, device
					, lpResult
					, gpInfoUbo );
			case LightingPass::Type::eShadowRsmGI:
				return makeLightPassRsmGIT< LightingPass::Type::eShadowRsmGI >( lightType
					, device
					, lpResult
					, gpInfoUbo
					, lightCache
					, gpResult
					, smDirectionalResult
					, smPointResult
					, smSpotResult );
			case LightingPass::Type::eShadowLpvGI:
				if ( engine.getRenderSystem()->getGpuInformations().hasShaderType( VK_SHADER_STAGE_GEOMETRY_BIT ) )
				{
					return makeLightPassLpvGIT< LightingPass::Type::eShadowLpvGI >( lightType
						, device
						, lpResult
						, gpInfoUbo
						, lightCache
						, gpResult
						, smDirectionalResult
						, smPointResult
						, smSpotResult
						, lpvResult
						, lpvConfigUbo );
				}
				return LightPassUPtr{};
			case LightingPass::Type::eShadowLpvGGI:
				if ( engine.getRenderSystem()->getGpuInformations().hasShaderType( VK_SHADER_STAGE_GEOMETRY_BIT ) )
				{
					return makeLightPassLpvGIT< LightingPass::Type::eShadowLpvGGI >( lightType
						, device
						, lpResult
						, gpInfoUbo
						, lightCache
						, gpResult
						, smDirectionalResult
						, smPointResult
						, smSpotResult
						, lpvResult
						, lpvConfigUbo );
				}
				return LightPassUPtr{};
			case LightingPass::Type::eShadowLayeredLpvGI:
				if ( engine.getRenderSystem()->getGpuInformations().hasShaderType( VK_SHADER_STAGE_GEOMETRY_BIT ) )
				{
					return makeLightPassLlpvGIT< LightingPass::Type::eShadowLayeredLpvGI >( lightType
						, device
						, lpResult
						, gpInfoUbo
						, lightCache
						, gpResult
						, smDirectionalResult
						, smPointResult
						, smSpotResult
						, lpvResult
						, llpvConfigUbo );
				}
				return LightPassUPtr{};
			case LightingPass::Type::eShadowLayeredLpvGGI:
				if ( engine.getRenderSystem()->getGpuInformations().hasShaderType( VK_SHADER_STAGE_GEOMETRY_BIT ) )
				{
					return makeLightPassLlpvGIT< LightingPass::Type::eShadowLayeredLpvGGI >( lightType
						, device
						, lpResult
						, gpInfoUbo
						, lightCache
						, gpResult
						, smDirectionalResult
						, smPointResult
						, smSpotResult
						, lpvResult
						, llpvConfigUbo );
				}
				return LightPassUPtr{};
			default:
				CU_Failure( "Unsupported LightingPass::Type" );
				return LightPassUPtr{};
			}
		}
	}

	LightingPass::LightingPass( Engine & engine
		, RenderDevice const & device
		, castor::Size const & size
		, Scene & scene
		, OpaquePassResult const & gpResult
		, ShadowMapResult const & smDirectionalResult
		, ShadowMapResult const & smPointResult
		, ShadowMapResult const & smSpotResult
		, LightVolumePassResult const & lpvResult
		, ashes::ImageView const & depthView
		, SceneUbo & sceneUbo
		, GpInfoUbo const & gpInfoUbo
		, LpvGridConfigUbo const & lpvConfigUbo
		, LayeredLpvGridConfigUbo const & llpvConfigUbo )
		: m_engine{ engine }
		, m_device{ device }
		, m_gpResult{ gpResult }
		, m_smDirectionalResult{ smDirectionalResult }
		, m_smPointResult{ smPointResult }
		, m_smSpotResult{ smSpotResult }
		, m_lpvResult{ lpvResult }
		, m_depthView{ depthView }
		, m_sceneUbo{ sceneUbo }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_lpvConfigUbo{ lpvConfigUbo }
		, m_llpvConfigUbo{ llpvConfigUbo }
		, m_size{ size }
		, m_result{ engine, size }
		, m_timer{ std::make_shared< RenderPassTimer >( engine, device, cuT( "Opaque" ), cuT( "Lighting pass" ) ) }
		, m_srcDepth{ depthView }
		, m_blitDepth
		{
			m_device.graphicsCommandPool->createCommandBuffer( "LPBlitDepth", VK_COMMAND_BUFFER_LEVEL_PRIMARY ),
			m_device->createSemaphore( "LPBlitDepth" ),
		}
		, m_lpResultBarrier
		{
			m_device.graphicsCommandPool->createCommandBuffer( "LPResultBarrier", VK_COMMAND_BUFFER_LEVEL_PRIMARY ),
			m_device->createSemaphore( "LPResultBarrier" ),
		}
	{
		auto & lightCache = scene.getLightCache();
		lightCache.initialise();
		m_result.initialise( m_device );

		VkImageCopy copy
		{
			{ m_srcDepth->subresourceRange.aspectMask, 0u, 0u, 1u },
			VkOffset3D{ 0, 0, 0 },
			{ m_srcDepth->subresourceRange.aspectMask, 0u, 0u, 1u },
			VkOffset3D{ 0, 0, 0 },
			m_srcDepth.image->getDimensions(),
		};
		m_blitDepth.commandBuffer->begin();
		m_blitDepth.commandBuffer->beginDebugBlock(
			{
				"Deferred - Ligth Depth Blit",
				makeFloatArray( m_engine.getNextRainbowColour() ),
			} );
		// Src depth buffer from depth attach to transfer source
		m_blitDepth.commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, m_srcDepth.makeTransferSource( VK_IMAGE_LAYOUT_UNDEFINED ) );
		// Dst depth buffer from unknown to transfer destination
		m_blitDepth.commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
			, VK_PIPELINE_STAGE_TRANSFER_BIT
			, m_result[LpTexture::eDepth].getTexture()->getDefaultView().getTargetView().makeTransferDestination( VK_IMAGE_LAYOUT_UNDEFINED ) );
		// Copy Src to Dst
		m_blitDepth.commandBuffer->copyImage( copy
			, *m_srcDepth.image
			, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
			, m_result[LpTexture::eDepth].getTexture()->getTexture()
			, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );
		// Dst depth buffer from transfer destination to depth attach
		m_blitDepth.commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
			, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
			, m_result[LpTexture::eDepth].getTexture()->getDefaultView().getTargetView().makeDepthStencilAttachment( VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) );
		// Src depth buffer from transfer source to depth stencil read only
		m_blitDepth.commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TRANSFER_BIT
			, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
			, m_srcDepth.makeDepthStencilReadOnly( VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL ) );
		m_blitDepth.commandBuffer->endDebugBlock();
		m_blitDepth.commandBuffer->end();

		m_lpResultBarrier.commandBuffer->begin();
		m_lpResultBarrier.commandBuffer->beginDebugBlock(
			{
				"Deferred - Ligth Pass Result Barrier",
				makeFloatArray( m_engine.getNextRainbowColour() ),
			} );

		for ( auto i = size_t( LpTexture::eMin ) + 1u; i < size_t( LpTexture::eCount ); ++i )
		{
			m_lpResultBarrier.commandBuffer->memoryBarrier( VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, m_result[LpTexture( i )].getTexture()->getDefaultView().getSampledView().makeShaderInputResource( VK_IMAGE_LAYOUT_UNDEFINED ) );
		}

		m_lpResultBarrier.commandBuffer->endDebugBlock();
		m_lpResultBarrier.commandBuffer->end();
	}

	LightingPass::~LightingPass()
	{
		m_lpResultBarrier.semaphore.reset();
		m_lpResultBarrier.commandBuffer.reset();
		m_blitDepth.semaphore.reset();
		m_blitDepth.commandBuffer.reset();

		for ( auto & lightPasses : m_lightPasses )
		{
			for ( auto & lightPass : lightPasses )
			{
				if ( lightPass )
				{
					lightPass->cleanup();
					lightPass.reset();
				}
			}
		}

		m_result.cleanup();
	}

	void LightingPass::update( CpuUpdater & updater )
	{
		m_active.clear();
	}

	void LightingPass::update( GpuUpdater & updater )
	{
		auto & scene = *updater.camera->getScene();
		auto & cache = scene.getLightCache();

		if ( !cache.isEmpty() )
		{
			doUpdateLightPasses( updater, LightType::eDirectional );
			doUpdateLightPasses( updater, LightType::ePoint );
			doUpdateLightPasses( updater, LightType::eSpot );
		}
	}

	ashes::Semaphore const & LightingPass::render( Scene const & scene
		, Camera const & camera
		, OpaquePassResult const & gp
		, ashes::Semaphore const & toWait )
	{
		auto & cache = scene.getLightCache();
		ashes::Semaphore const * result = &toWait;
		m_active.clear();

		if ( !cache.isEmpty() )
		{
			RenderPassTimerBlock timerBlock{ m_timer->start() };
			using LockType = std::unique_lock< LightCache const >;
			LockType lock{ castor::makeUniqueLock( cache ) };
			auto count = cache.getLightsCount( LightType::eDirectional )
				+ cache.getLightsCount( LightType::ePoint )
				+ cache.getLightsCount( LightType::eSpot );

			if ( timerBlock->getCount() != count )
			{
				timerBlock->updateCount( count );
			}

			m_device.graphicsQueue->submit( *m_blitDepth.commandBuffer
				, *result
				, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, *m_blitDepth.semaphore
				, nullptr );
			result = m_blitDepth.semaphore.get();

			uint32_t index = 0;
			result = &doRenderLights( scene
				, camera
				, LightType::eDirectional
				, gp
				, *result
				, index );
			result = &doRenderLights( scene
				, camera
				, LightType::ePoint
				, gp
				, *result
				, index );
			result = &doRenderLights( scene
				, camera
				, LightType::eSpot
				, gp
				, *result
				, index );
		}
		else
		{
			m_device.graphicsQueue->submit( *m_lpResultBarrier.commandBuffer
				, *result
				, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, *m_lpResultBarrier.semaphore
				, nullptr );
			result = m_lpResultBarrier.semaphore.get();
		}

		return *result;
	}

	void LightingPass::accept( PipelineVisitorBase & visitor )
	{
		visitor.visit( "Light Diffuse"
			, m_result[LpTexture::eDiffuse].getTexture()->getDefaultView().getSampledView()
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		visitor.visit( "Light Specular"
			, m_result[LpTexture::eSpecular].getTexture()->getDefaultView().getSampledView()
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		visitor.visit( "Light Indirect Diffuse"
			, m_result[LpTexture::eIndirectDiffuse].getTexture()->getDefaultView().getSampledView()
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );

		for ( auto & lightPass : m_active )
		{
			lightPass->accept( visitor );
		}
	}

	void LightingPass::doUpdateLightPasses( GpuUpdater & updater
		, LightType lightType )
	{
		auto & camera = *updater.camera;
		auto & scene = *updater.camera->getScene();
		auto & cache = scene.getLightCache();

		for ( auto & light : cache.getLights( lightType ) )
		{
			if ( light->getLightType() == LightType::eDirectional
				|| camera.isVisible( light->getBoundingBox(), light->getParent()->getDerivedTransformationMatrix() ) )
			{
				Type type = Type::eNoShadow;

				if ( light->isShadowProducer() )
				{
					auto giType = light->getGlobalIlluminationType();

					if ( !m_engine.getRenderSystem()->getGpuInformations().hasShaderType( VK_SHADER_STAGE_GEOMETRY_BIT ) )
					{
						giType = std::min( GlobalIlluminationType::eRsm, giType );
					}

					type = Type( uint32_t( giType ) + 1u );
				}

				auto & lightPass = getLightPass( type
					, lightType
					, m_lightPasses );

				if ( !lightPass )
				{
					lightPass = makeLightPass( m_engine
						, type
						, lightType
						, m_device
						, cache
						, m_gpResult
						, m_smDirectionalResult
						, m_smPointResult
						, m_smSpotResult
						, m_result
						, m_lpvResult
						, m_gpInfoUbo
						, m_lpvConfigUbo
						, m_llpvConfigUbo );
					lightPass->initialise( scene
						, m_gpResult
						, m_sceneUbo
						, *m_timer );
				}
			}
		}
	}

	ashes::Semaphore const & LightingPass::doRenderLights( Scene const & scene
		, Camera const & camera
		, LightType type
		, OpaquePassResult const & gp
		, ashes::Semaphore const & toWait
		, uint32_t & index )
	{
		auto result = &toWait;
		auto & cache = scene.getLightCache();

		if ( cache.getLightsCount( type ) )
		{
			for ( auto & light : cache.getLights( type ) )
			{
				if ( light->getLightType() == LightType::eDirectional
					|| camera.isVisible( light->getBoundingBox(), light->getParent()->getDerivedTransformationMatrix() ) )
				{
					LightPass * pass = ( light->isShadowProducer() && light->getShadowMap() )
						? doGetShadowLightPass( type, light->getGlobalIlluminationType() )
						: doGetLightPass( type );
					CU_Require( pass != nullptr );
					m_active.insert( pass );
					pass->update( index == 0u
						, camera.getSize()
						, *light
						, camera
						, light->getShadowMap()
						, light->getShadowMapIndex() );
					result = &pass->render( index
						, *result );
					++index;
				}
			}
		}

		return *result;
	}

	LightPass * LightingPass::doGetLightPass( LightType lightType )const
	{
		return getLightPass( Type::eNoShadow
			, lightType
			, m_lightPasses );
	}

	LightPass * LightingPass::doGetShadowLightPass( LightType lightType
		, GlobalIlluminationType giType )const
	{
		if ( !m_engine.getRenderSystem()->getGpuInformations().hasShaderType( VK_SHADER_STAGE_GEOMETRY_BIT ) )
		{
			giType = std::min( GlobalIlluminationType::eRsm, giType );
		}

		return getLightPass( Type( uint32_t( giType ) + 1u )
			, lightType
			, m_lightPasses );
	}
}
