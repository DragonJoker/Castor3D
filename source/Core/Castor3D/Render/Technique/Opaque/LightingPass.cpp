#include "Castor3D/Render/Technique/Opaque/LightingPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LayeredLightPropagationVolumes.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumes.hpp"
#include "Castor3D/Render/Technique/RenderTechniquePass.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePass.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/DirectionalLightPass.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassShadow.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/PointLightPass.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/SpotLightPass.hpp"
#include "Castor3D/Render/Technique/Opaque/ReflectiveShadowMapGI/LightPassReflectiveShadow.hpp"
#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LightPassVolumePropagationShadow.hpp"
#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LightPassLayeredVolumePropagationShadow.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/LayeredLpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"
#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

#include <CastorUtils/Graphics/RgbaColour.hpp>
#include <CastorUtils/Math/TransformationMatrix.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>

#include <RenderGraph/RunnablePasses/ImageCopy.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

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

		LightingPass::Type getLightingType( RenderDevice const & device
			, bool voxelConeTracing
			, bool shadow
			, GlobalIlluminationType giType )
		{
			if ( voxelConeTracing )
			{
				giType = GlobalIlluminationType::eVoxelConeTracing;
			}

			if ( !device.renderSystem.getGpuInformations().hasShaderType( VK_SHADER_STAGE_GEOMETRY_BIT ) )
			{
				giType = std::min( GlobalIlluminationType::eRsm, giType );
			}

			if ( shadow )
			{
				switch ( giType )
				{
				case GlobalIlluminationType::eNone:
					return LightingPass::Type::eShadowNoGI;
				case GlobalIlluminationType::eVoxelConeTracing:
					return LightingPass::Type::eShadowVoxelConeTracingGI;
				case GlobalIlluminationType::eRsm:
					return LightingPass::Type::eShadowRsmGI;
				case GlobalIlluminationType::eLpv:
					return LightingPass::Type::eShadowLpvGI;
				case GlobalIlluminationType::eLpvG:
					return LightingPass::Type::eShadowLpvGGI;
				case GlobalIlluminationType::eLayeredLpv:
					return LightingPass::Type::eShadowLayeredLpvGI;
				case GlobalIlluminationType::eLayeredLpvG:
					return LightingPass::Type::eShadowLayeredLpvGGI;
				default:
					return LightingPass::Type::eShadowNoGI;
				}
			}

			switch ( giType )
			{
			case GlobalIlluminationType::eNone:
				return LightingPass::Type::eNoShadow;
			case GlobalIlluminationType::eVoxelConeTracing:
				return LightingPass::Type::eNoShadowVoxelConeTracingGI;
			default:
				CU_Failure( "Unsupported GI without shadow maps (WTF are you doing here)" );
				return LightingPass::Type::eNoShadow;
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
		struct PassInfoT< LightingPass::Type::eNoShadowVoxelConeTracingGI, LightType::eDirectional >
		{
			using Type = DirectionalLightPass;
		};

		template<>
		struct PassInfoT< LightingPass::Type::eNoShadowVoxelConeTracingGI, LightType::ePoint >
		{
			using Type = PointLightPass;
		};

		template<>
		struct PassInfoT< LightingPass::Type::eNoShadowVoxelConeTracingGI, LightType::eSpot >
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
		struct PassInfoT< LightingPass::Type::eShadowVoxelConeTracingGI, LightType::eDirectional >
		{
			using Type = DirectionalLightPassShadow;
		};

		template<>
		struct PassInfoT< LightingPass::Type::eShadowVoxelConeTracingGI, LightType::ePoint >
		{
			using Type = PointLightPassShadow;
		};

		template<>
		struct PassInfoT< LightingPass::Type::eShadowVoxelConeTracingGI, LightType::eSpot >
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
			, crg::FrameGraph & graph
			, crg::FramePass const *& previousPass
			, RenderDevice const & device
			, LightPassResult const & lpResult
			, GpInfoUbo const & gpInfoUbo
			, bool shadows )
		{
			LightPassConfig lpConfig{ lpResult, gpInfoUbo, shadows, false, false };

			switch ( lightType )
			{
			case LightType::eDirectional:
				return std::make_unique< PassTypeT< PassT, LightType::eDirectional > >( graph
					, previousPass
					, device
					, lpConfig );
			case LightType::ePoint:
				return std::make_unique< PassTypeT< PassT, LightType::ePoint > >( graph
					, previousPass
					, device
					, lpConfig );
			case LightType::eSpot:
				return std::make_unique< PassTypeT< PassT, LightType::eSpot > >( graph
					, previousPass
					, device
					, lpConfig );
			default:
				CU_Failure( "Unsupported LightType" );
				return LightPassUPtr{};
			}
		}

		template< LightingPass::Type PassT >
		LightPassUPtr makeLightPassVctGIT( LightType lightType
			, crg::FrameGraph & graph
			, crg::FramePass const *& previousPass
			, RenderDevice const & device
			, LightPassResult const & lpResult
			, GpInfoUbo const & gpInfoUbo
			, bool shadows
			, VoxelizerUbo const & vctConfig )
		{
			LightPassConfig lpConfig{ lpResult, gpInfoUbo, shadows, true, true };

			switch ( lightType )
			{
			case LightType::eDirectional:
				return std::make_unique< PassTypeT< PassT, LightType::eDirectional > >( graph
					, previousPass
					, device
					, lpConfig
					, &vctConfig );
			case LightType::ePoint:
				return std::make_unique< PassTypeT< PassT, LightType::ePoint > >( graph
					, previousPass
					, device
					, lpConfig
					, &vctConfig );
			case LightType::eSpot:
				return std::make_unique< PassTypeT< PassT, LightType::eSpot > >( graph
					, previousPass
					, device
					, lpConfig
					, &vctConfig );
			default:
				CU_Failure( "Unsupported LightType" );
				return LightPassUPtr{};
			}
		}

		template< LightingPass::Type PassT >
		LightPassUPtr makeLightPassRsmGIT( LightType lightType
			, crg::FrameGraph & graph
			, crg::FramePass const *& previousPass
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
				return std::make_unique< PassTypeT< PassT, LightType::eDirectional > >( graph
					, previousPass
					, device
					, RsmLightPassConfig{ { lpResult, gpInfoUbo }
						, lightCache
						, gpResult
						, smDirectionalResult } );
			case LightType::ePoint:
				return std::make_unique< PassTypeT< PassT, LightType::ePoint > >( graph
					, previousPass
					, device
					, RsmLightPassConfig{ { lpResult, gpInfoUbo }
						, lightCache
						, gpResult
						, smPointResult } );
			case LightType::eSpot:
				return std::make_unique< PassTypeT< PassT, LightType::eSpot > >( graph
					, previousPass
					, device
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
			, crg::FrameGraph & graph
			, crg::FramePass const *& previousPass
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
				return std::make_unique< PassTypeT< PassT, LightType::eDirectional > >( graph
					, previousPass
					, device
					, LpvLightPassConfig{ LightPassConfig{ lpResult, gpInfoUbo }
						, lightCache
						, gpResult
						, smDirectionalResult
						, lpvResult
						, lpvConfigUbo } );
			//case LightType::ePoint:
			//	return std::make_unique< PassTypeT< PassT, LightType::ePoint > >( graph
			//		, previousPass
			//		, device
			//		, LpvLightPassConfig{ LightPassConfig{ lpResult, gpInfoUbo }
			//			, lightCache
			//			, gpResult
			//			, smPointResult
			//			, lpvResult
			//			, lpvConfigUbo } );
			case LightType::eSpot:
				return std::make_unique< PassTypeT< PassT, LightType::eSpot > >( graph
					, previousPass
					, device
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
			, crg::FrameGraph & graph
			, crg::FramePass const *& previousPass
			, RenderDevice const & device
			, LightPassResult const & lpResult
			, GpInfoUbo const & gpInfoUbo
			, LightCache const & lightCache
			, OpaquePassResult const & gpResult
			, ShadowMapResult const & smDirectionalResult
			, ShadowMapResult const & smPointResult
			, ShadowMapResult const & smSpotResult
			, LightVolumePassResultArray const & lpvResult
			, LayeredLpvGridConfigUbo const & lpvConfigUbo )
		{
			switch ( lightType )
			{
			case LightType::eDirectional:
				return std::make_unique< PassTypeT< PassT, LightType::eDirectional > >( graph
					, previousPass
					, device
					, LayeredLpvLightPassConfig{ LightPassConfig{ lpResult, gpInfoUbo }
						, lightCache
						, gpResult
						, smDirectionalResult
						, lpvResult
						, lpvConfigUbo } );
			//case LightType::ePoint:
			//	return std::make_unique< PassTypeT< PassT, LightType::ePoint > >( graph
			//		, previousPass
			//		, device
			//		, LayeredLpvLightPassConfig{ LightPassConfig{ lpResult, gpInfoUbo }
			//			, lightCache
			//			, gpResult
			//			, smPointResult
			//			, lpvResult
			//			, lpvConfigUbo
			//			, lpvConfigUbos } );
			//case LightType::eSpot:
			//	return std::make_unique< PassTypeT< PassT, LightType::eSpot > >( graph
			//		, previousPass
			//		, device
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

		LightPassUPtr makeLightPass( crg::FrameGraph & graph
			, crg::FramePass const *& previousPass
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
			, LightVolumePassResultArray const & llpvResult
			, GpInfoUbo const & gpInfoUbo
			, LpvGridConfigUbo const & lpvConfigUbo
			, LayeredLpvGridConfigUbo const & llpvConfigUbo
			, VoxelizerUbo const & vctConfig )
		{
			switch ( passType )
			{
			case LightingPass::Type::eNoShadow:
				return makeLightPassNoGIT< LightingPass::Type::eNoShadow >( lightType
					, graph
					, previousPass
					, device
					, lpResult
					, gpInfoUbo
					, false );
			case LightingPass::Type::eNoShadowVoxelConeTracingGI:
				return makeLightPassVctGIT< LightingPass::Type::eNoShadowVoxelConeTracingGI >( lightType
					, graph
					, previousPass
					, device
					, lpResult
					, gpInfoUbo
					, false
					, vctConfig );
			case LightingPass::Type::eShadowNoGI:
				return makeLightPassNoGIT< LightingPass::Type::eShadowNoGI >( lightType
					, graph
					, previousPass
					, device
					, lpResult
					, gpInfoUbo
					, true );
			case LightingPass::Type::eShadowVoxelConeTracingGI:
				return makeLightPassVctGIT< LightingPass::Type::eShadowVoxelConeTracingGI >( lightType
					, graph
					, previousPass
					, device
					, lpResult
					, gpInfoUbo
					, true
					, vctConfig );
			case LightingPass::Type::eShadowRsmGI:
				return makeLightPassRsmGIT< LightingPass::Type::eShadowRsmGI >( lightType
					, graph
					, previousPass
					, device
					, lpResult
					, gpInfoUbo
					, lightCache
					, gpResult
					, smDirectionalResult
					, smPointResult
					, smSpotResult );
			case LightingPass::Type::eShadowLpvGI:
				if ( device.renderSystem.getGpuInformations().hasShaderType( VK_SHADER_STAGE_GEOMETRY_BIT ) )
				{
					return makeLightPassLpvGIT< LightingPass::Type::eShadowLpvGI >( lightType
						, graph
						, previousPass
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
				if ( device.renderSystem.getGpuInformations().hasShaderType( VK_SHADER_STAGE_GEOMETRY_BIT ) )
				{
					return makeLightPassLpvGIT< LightingPass::Type::eShadowLpvGGI >( lightType
						, graph
						, previousPass
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
				if ( device.renderSystem.getGpuInformations().hasShaderType( VK_SHADER_STAGE_GEOMETRY_BIT ) )
				{
					return makeLightPassLlpvGIT< LightingPass::Type::eShadowLayeredLpvGI >( lightType
						, graph
						, previousPass
						, device
						, lpResult
						, gpInfoUbo
						, lightCache
						, gpResult
						, smDirectionalResult
						, smPointResult
						, smSpotResult
						, llpvResult
						, llpvConfigUbo );
				}
				return LightPassUPtr{};
			case LightingPass::Type::eShadowLayeredLpvGGI:
				if ( device.renderSystem.getGpuInformations().hasShaderType( VK_SHADER_STAGE_GEOMETRY_BIT ) )
				{
					return makeLightPassLlpvGIT< LightingPass::Type::eShadowLayeredLpvGGI >( lightType
						, graph
						, previousPass
						, device
						, lpResult
						, gpInfoUbo
						, lightCache
						, gpResult
						, smDirectionalResult
						, smPointResult
						, smSpotResult
						, llpvResult
						, llpvConfigUbo );
				}
				return LightPassUPtr{};
			default:
				CU_Failure( "Unsupported LightingPass::Type" );
				return LightPassUPtr{};
			}
		}

		castor::Point2f doCalcSpotLightBCone( const castor3d::SpotLight & light
			, float max )
		{
			float length{ getMaxDistance( light
				, light.getAttenuation()
				, max ) };
			float width{ light.getCutOff().degrees() / ( 45.0f ) };
			return castor::Point2f{ length * width, length };
		}

		float doCalcPointLightBSphere( const castor3d::PointLight & light
			, float max )
		{
			return getMaxDistance( light
				, light.getAttenuation()
				, max );
		}
	}

	//*********************************************************************************************

	LightPipelineConfig::LightPipelineConfig( MaterialType materialType
		, SceneFlags const & sceneFlags
		, Light const & light )
		: materialType{ materialType }
		, sceneFlags{ sceneFlags }
		, lightType{ light.getLightType() }
		, shadowType{ light.getShadowType() }
		, rsm{ light.needsRsmShadowMaps() }
		, voxels{ checkFlag( sceneFlags, SceneFlag::eVoxelConeTracing ) }
		, generatesIndirect{ voxels
			|| light.getGlobalIlluminationType() != GlobalIlluminationType::eNone }
	{
	}

	size_t LightPipelineConfig::makeHash()const
	{
		size_t hash = std::hash< uint32_t >{}( uint32_t( materialType ) );
		hash = castor::hashCombine( hash, uint32_t( sceneFlags ) );
		hash = castor::hashCombine( hash, uint32_t( lightType ) );
		hash = castor::hashCombine( hash, uint32_t( shadowType ) );
		hash = castor::hashCombine( hash, shadows );
		hash = castor::hashCombine( hash, rsm );
		hash = castor::hashCombine( hash, voxels );
		hash = castor::hashCombine( hash, generatesIndirect );
		return hash;
	}

	//*********************************************************************************************

	LightDescriptors::LightDescriptors( RenderDevice const & device )
		: matrixUbo{ device }
	{
	}

	//*********************************************************************************************

	LightPipeline::LightPipeline( crg::FramePass const & pass
		, crg::GraphContext const & context
		, crg::RunnableGraph & graph
		, LightPipelineConfig const & config
		, std::vector< LightRenderPass > const & renderPasses
		, ashes::PipelineShaderStageCreateInfoArray stages
		, VkDescriptorSetLayout descriptorSetLayout )
		: m_holder{ pass
			, context
			, graph
			, crg::pp::Config{ std::vector< crg::VkPipelineShaderStageCreateInfoArray >{ crg::makeVkArray< VkPipelineShaderStageCreateInfo >( stages )
					, crg::makeVkArray< VkPipelineShaderStageCreateInfo >( stages ) }
				, std::vector< VkDescriptorSetLayout >{ descriptorSetLayout } }
			, VK_PIPELINE_BIND_POINT_GRAPHICS
			, 1u }
		, m_config{ config }
		, m_renderPasses{ renderPasses }
	{
		m_holder.initialise();
		doCreatePipeline( 0u );
		doCreatePipeline( 1u );
	}

	void LightPipeline::doCreatePipeline( uint32_t index )
	{
		ashes::PipelineVertexInputStateCreateInfo vertexLayout = doCreateVertexLayout();
		crg::VkVertexInputAttributeDescriptionArray vertexAttribs;
		crg::VkVertexInputBindingDescriptionArray vertexBindings;
		auto viewportState = doCreateViewportState( *m_renderPasses[index].framebuffer );
		auto colourBlend = doCreateBlendState( index != 0u );
		ashes::PipelineDepthStencilStateCreateInfo depthStencil{ 0u, false, false };
		VkPipelineInputAssemblyStateCreateInfo iaState{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO
			, nullptr
			, 0u
			, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP
			, VK_FALSE };
		VkPipelineMultisampleStateCreateInfo msState{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO
			, nullptr
			, 0u
			, VK_SAMPLE_COUNT_1_BIT
			, VK_FALSE
			, 0.0f
			, nullptr
			, VK_FALSE
			, VK_FALSE };
		VkPipelineRasterizationStateCreateInfo rsState{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO
			, nullptr
			, 0u
			, VK_FALSE
			, VK_FALSE
			, VK_POLYGON_MODE_FILL
			, VK_CULL_MODE_NONE
			, VK_FRONT_FACE_COUNTER_CLOCKWISE
			, VK_FALSE
			, 0.0f
			, 0.0f
			, 0.0f
			, 0.0f };
		auto & program = m_holder.getProgram( index );
		auto & pipeline = m_holder.getPipeline( index );
		VkPipelineVertexInputStateCreateInfo const & vsState = vertexLayout;
		VkPipelineDepthStencilStateCreateInfo const & dsState = depthStencil;
		VkPipelineColorBlendStateCreateInfo const & cbState = colourBlend;
		VkPipelineViewportStateCreateInfo const & vpState = viewportState;
		VkGraphicsPipelineCreateInfo createInfo{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO
			, nullptr
			, 0u
			, uint32_t( program.size() )
			, program.data()
			, &vsState
			, &iaState
			, nullptr
			, &vpState
			, &rsState
			, &msState
			, &dsState
			, &cbState
			, nullptr
			, m_holder.getPipelineLayout()
			, *m_renderPasses[index].renderPass
			, 0u
			, VK_NULL_HANDLE
			, 0u };
		auto res = m_holder.getContext().vkCreateGraphicsPipelines( m_holder.getContext().device
			, m_holder.getContext().cache
			, 1u
			, &createInfo
			, m_holder.getContext().allocator
			, &pipeline );
		crg::checkVkResult( res, m_holder.getPass().name + " - Pipeline creation" );
		crgRegisterObject( m_holder.getContext(), m_holder.getPass().name, pipeline );
	}

	ashes::PipelineVertexInputStateCreateInfo LightPipeline::doCreateVertexLayout()
	{
		if ( m_config.lightType == LightType::eDirectional )
		{
			return { 0u
				, ashes::VkVertexInputBindingDescriptionArray{ { 0u
					, sizeof( castor::Point2f )
					, VK_VERTEX_INPUT_RATE_VERTEX } }
				, ashes::VkVertexInputAttributeDescriptionArray{ { 0u
					, 0u
					, VK_FORMAT_R32G32_SFLOAT
					, 0u } } };
		}

		return { 0u
			, ashes::VkVertexInputBindingDescriptionArray{ { 0u
				, sizeof( castor::Point3f )
				, VK_VERTEX_INPUT_RATE_VERTEX } }
			, ashes::VkVertexInputAttributeDescriptionArray{ { 0u
				, 0u
				, VK_FORMAT_R32G32B32_SFLOAT
				, 0u } } };
	}

	ashes::PipelineViewportStateCreateInfo LightPipeline::doCreateViewportState( ashes::FrameBuffer const & framebuffer )
	{
		auto dim = framebuffer.getDimensions();
		ashes::VkViewportArray viewports{ { 0.0f
			, 0.0f
			, float( dim.width )
			, float( dim.height )
			, 0.0f
			, 1.0f } };
		ashes::VkScissorArray scissors{ { {}, dim } };
		return { 0u
			, 1u
			, viewports
			, 1u
			, scissors };
	}

	ashes::PipelineColorBlendStateCreateInfo LightPipeline::doCreateBlendState( bool blend )
	{
		ashes::VkPipelineColorBlendAttachmentStateArray attachs;

		if ( blend )
		{
			attachs.push_back( { VK_TRUE
				, VK_BLEND_FACTOR_ONE
				, VK_BLEND_FACTOR_ONE
				, VK_BLEND_OP_ADD
				, VK_BLEND_FACTOR_ONE
				, VK_BLEND_FACTOR_ONE
				, VK_BLEND_OP_ADD
				, defaultColorWriteMask } );
		}
		else
		{
			attachs.push_back( { VK_FALSE
				, VK_BLEND_FACTOR_ONE
				, VK_BLEND_FACTOR_ZERO
				, VK_BLEND_OP_ADD
				, VK_BLEND_FACTOR_ONE
				, VK_BLEND_FACTOR_ZERO
				, VK_BLEND_OP_ADD
				, defaultColorWriteMask } );
		}

		attachs.push_back( attachs.back() );
		attachs.push_back( attachs.back() );
		attachs.push_back( attachs.back() );

		return { 0u
			, VK_FALSE
			, VK_LOGIC_OP_COPY
			, std::move( attachs ) };
	}

	//*********************************************************************************************

	LightsPipeline::LightsPipeline( crg::FramePass const & pass
		, crg::GraphContext const & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, LightPipelineConfig const & config
		, LightPassResult const & lpResult
		, ShadowMapResult const & smResult
		, std::vector< LightRenderPass > const & renderPasses )
		: m_context{ context }
		, m_smResult{ smResult }
		, m_device{ device }
		, m_renderPasses{ renderPasses }
		, m_config{ config }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT
			, castor::string::snakeToCamelCase( getName( m_config.lightType ) )
			, LightPass::getVertexShaderSource( m_config.lightType ) }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT
			, castor::string::snakeToCamelCase( getName( m_config.lightType ) )
			, LightPass::getPixelShaderSource( m_config.materialType
				, m_device.renderSystem
				, m_config.sceneFlags
				, m_config.lightType
				, m_config.shadowType
				, m_config.shadows
				, m_config.rsm
				, m_config.generatesIndirect
				, m_config.voxels ) }
		, m_stages{ makeShaderState( m_device, m_vertexShader )
			, makeShaderState( m_device, m_pixelShader ) }
		, m_descriptorLayout{ doCreateDescriptorLayout() }
		, m_descriptorPool{ m_descriptorLayout->createPool( 300u ) }
		, m_pipeline{ pass, context, graph, m_config, m_renderPasses, m_stages, *m_descriptorLayout }
		, m_vertexBuffer{ doCreateVertexBuffer() }
		, m_viewport{ *device.renderSystem.getEngine() }
	{
		m_viewport.setOrtho( -1, 1, -1, 1, -1, 1 );
	}

	void LightsPipeline::clear()
	{
		m_viewport.resize( makeSize( m_renderPasses[0].framebuffer->getDimensions() ) );
		m_viewport.update();
		m_enabledLights.clear();
	}

	void LightsPipeline::addLight( Camera const & camera
		, Light const & light )
	{
		auto & entry = doCreateLightEntry( light );

		if ( m_config.lightType != LightType::eDirectional )
		{
			entry.matrixUbo.cpuUpdate( camera.getView(), camera.getProjection() );
			auto model = doComputeModelMatrix( light, camera );
			auto & data = entry.modelMatrixUbo.getData();
			data.prvModel = data.curModel;
			data.curModel = model;
		}
		else
		{
			entry.matrixUbo.cpuUpdate( camera.getView(), m_viewport.getProjection() );
		}

		m_enabledLights.push_back( &entry );
	}

	void LightsPipeline::recordInto( VkCommandBuffer commandBuffer
		, uint32_t & index )
	{
		if ( !m_enabledLights.empty() )
		{
			auto pipeline = m_pipeline.getPipeline( index );
			m_context.vkCmdBindPipeline( commandBuffer
				, VK_PIPELINE_BIND_POINT_GRAPHICS
				, pipeline );
			auto baseDS = m_pipeline.getDescriptorSet();
			m_context.vkCmdBindDescriptorSets( commandBuffer
				, VK_PIPELINE_BIND_POINT_GRAPHICS
				, m_pipeline.getPipelineLayout()
				, 0u
				, 1u
				, &baseDS
				, 0u
				, nullptr );
			auto & descriptors = m_enabledLights[0];
			VkDescriptorSet specDS = *descriptors->descriptorSet;
			m_context.vkCmdBindDescriptorSets( commandBuffer
				, VK_PIPELINE_BIND_POINT_GRAPHICS
				, m_pipeline.getPipelineLayout()
				, 1u
				, 1u
				, &specDS
				, 0u
				, nullptr );
			VkBuffer vertexBuffer = m_vertexBuffer->getBuffer();
			VkDeviceSize offset{};
			m_context.vkCmdBindVertexBuffers( commandBuffer
				, 0u
				, 1u
				, &vertexBuffer
				, &offset );
			m_context.vkCmdDraw( commandBuffer
				, m_count
				, 1u
				, 0u
				, 0u );

			if ( m_enabledLights.size() > 1u )
			{
				if ( index == 0u )
				{
					index = 1u;
					m_context.vkCmdEndRenderPass( commandBuffer );
					auto & renderPasses = m_renderPasses[index];
					VkRenderPassBeginInfo beginRenderPass{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO
						, nullptr
						, *renderPasses.renderPass
						, *renderPasses.framebuffer
						, { {}, renderPasses.framebuffer->getDimensions() }
						, uint32_t( renderPasses.clearValues.size() )
						, renderPasses.clearValues.data() };
					m_context.vkCmdBeginRenderPass( commandBuffer
						, &beginRenderPass
						, VK_SUBPASS_CONTENTS_INLINE );
					pipeline = m_pipeline.getPipeline( index );
					m_context.vkCmdBindPipeline( commandBuffer
						, VK_PIPELINE_BIND_POINT_GRAPHICS
						, pipeline );
					m_context.vkCmdBindDescriptorSets( commandBuffer
						, VK_PIPELINE_BIND_POINT_GRAPHICS
						, m_pipeline.getPipelineLayout()
						, 0u
						, 1u
						, &baseDS
						, 0u
						, nullptr );
				}

				for ( size_t i = 1u; i < m_enabledLights.size(); ++i )
				{
					auto & descriptors = m_enabledLights[i];
					specDS = *descriptors->descriptorSet;
					m_context.vkCmdBindDescriptorSets( commandBuffer
						, VK_PIPELINE_BIND_POINT_GRAPHICS
						, m_pipeline.getPipelineLayout()
						, 1u
						, 1u
						, &specDS
						, 0u
						, nullptr );
					m_context.vkCmdBindVertexBuffers( commandBuffer
						, 0u
						, 1u
						, &vertexBuffer
						, &offset );
					m_context.vkCmdDraw( commandBuffer
						, m_count
						, 1u
						, 0u
						, 0u );
				}
			}
		}
	}

	ashes::DescriptorSetLayoutPtr LightsPipeline::doCreateDescriptorLayout()
	{
		ashes::VkDescriptorSetLayoutBindingArray setLayoutBindings;
		setLayoutBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( LightPassLgtIdx::eLight )
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		setLayoutBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( LightPassLgtIdx::eMatrix )
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_VERTEX_BIT ) );

		if ( m_config.lightType != LightType::eDirectional )
		{
			setLayoutBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( LightPassLgtIdx::eModelMatrix )
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_VERTEX_BIT ) );
		}

		setLayoutBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( LightPassLgtIdx::eSmNormalLinear )
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		setLayoutBindings.emplace_back( makeDescriptorSetLayoutBinding( uint32_t( LightPassLgtIdx::eSmVariance )
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );

		return m_device->createDescriptorSetLayout( std::move( setLayoutBindings ) );
	}

	LightDescriptors & LightsPipeline::doCreateLightEntry( Light const & light )
	{
		auto ires = m_lightDescriptors.emplace( light.getBufferIndex(), nullptr );

		if ( ires.second )
		{
			ires.first->second = std::make_unique< LightDescriptors >( m_device );
			auto & result = *ires.first->second;
			auto lightSize = shader::getMaxLightComponentsCount() * sizeof( castor::Point4f );
			auto & scene = *light.getScene();
			ashes::WriteDescriptorSetArray writes;
			writes.emplace_back( uint32_t( LightPassLgtIdx::eLight )
				, 0u
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, ashes::VkDescriptorBufferInfoArray{ { scene.getLightCache().getBuffer().getBuffer()
					, light.getBufferIndex() * lightSize
					, lightSize } } );
			writes.emplace_back( result.matrixUbo.getDescriptorWrite( uint32_t( LightPassLgtIdx::eMatrix ) ) );

			if ( m_config.lightType != LightType::eDirectional )
			{
				result.modelMatrixUbo = m_device.uboPools->getBuffer< ModelUboConfiguration >( 0u );
				writes.emplace_back( result.modelMatrixUbo.getDescriptorWrite( uint32_t( LightPassLgtIdx::eModelMatrix ) ) );
			}

			writes.emplace_back( uint32_t( LightPassLgtIdx::eSmNormalLinear )
				, 0u
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, ashes::VkDescriptorImageInfoArray{ { m_device.renderSystem.getEngine()->getDefaultSampler()->getSampler()
					, m_smResult[SmTexture::eNormalLinear].wholeView
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } } );
			writes.emplace_back( uint32_t( LightPassLgtIdx::eSmVariance )
				, 0u
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, ashes::VkDescriptorImageInfoArray{ { m_device.renderSystem.getEngine()->getDefaultSampler()->getSampler()
					, m_smResult[SmTexture::eVariance].wholeView
					, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } } );

			result.descriptorSet = m_descriptorPool->createDescriptorSet( 1u );
			result.descriptorSet->setBindings( writes );
			result.descriptorSet->update();
		}

		return *ires.first->second;
	}

	castor::Matrix4x4f LightsPipeline::doComputeModelMatrix( castor3d::Light const & light
		, Camera const & camera )const
	{
		auto lightPos = light.getParent()->getDerivedPosition();
		auto camPos = camera.getParent()->getDerivedPosition();
		auto farZ = camera.getFar();
		castor::Matrix4x4f model{ 1.0f };

		if ( m_config.lightType == LightType::ePoint )
		{
			auto scale = doCalcPointLightBSphere( *light.getPointLight()
				, float( farZ - castor::point::distance( lightPos, camPos ) - ( farZ / 50.0f ) ) );
			castor::matrix::setTransform( model
				, lightPos
				, castor::Point3f{ scale, scale, scale }
				, castor::Quaternion::identity() );
			return model;
		}
		else
		{
			auto scale = doCalcSpotLightBCone( *light.getSpotLight()
				, float( farZ - castor::point::distance( lightPos, camPos ) - ( farZ / 50.0f ) ) );
			castor::matrix::setTransform( model
				, lightPos
				, castor::Point3f{ scale[0], scale[0], scale[1] }
				, light.getParent()->getDerivedOrientation() );
		}

		return model;
	}

	ashes::VertexBufferPtr< float > LightsPipeline::doCreateVertexBuffer()
	{
		ashes::VertexBufferPtr< float > result;

		if ( m_config.lightType == LightType::eDirectional )
		{
			m_count = 6u;
			result = makeVertexBuffer< float >( m_device
				, 12u
				, 0u
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, "DirectionalLightPassVbo" );

			if ( auto buffer = result->getBuffer().lock( 0u
				, ~( 0ull )
				, 0u ) )
			{
				float data[] =
				{
					-1.0f, -1.0f,
					-1.0f, +1.0f,
					+1.0f, -1.0f,
					+1.0f, -1.0f,
					-1.0f, +1.0f,
					+1.0f, +1.0f,
				};
				std::memcpy( buffer, data, sizeof( data ) );
				result->getBuffer().flush( 0u, ~( 0ull ) );
				result->getBuffer().unlock();
			}
		}
		else
		{
			castor::Point3fArray data;

			if ( m_config.lightType == LightType::ePoint )
			{
				data = PointLight::generateVertices();
			}
			else
			{
				data = SpotLight::generateVertices();
			}

			m_count = data.size();
			result = makeVertexBuffer< float >( m_device
				, uint32_t( data.size() * 3u )
				, 0u
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, castor::string::snakeToCamelCase( getName( m_config.lightType ) ) + "LightPassVbo" );

			if ( auto * buffer = result->lock( 0u, result->getCount(), 0u ) )
			{
				std::memcpy( buffer, data.data()->constPtr(), result->getSize() );
				result->flush( 0u, result->getCount() );
				result->unlock();
			}
		}

		return result;
	}

	//*********************************************************************************************

	RunnableLightingPass::RunnableLightingPass( crg::FramePass const & pass
		, crg::GraphContext const & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, Scene const & scene
		, LightPassResult const & lpResult
		, ShadowMapResult const & smDirectionalResult
		, ShadowMapResult const & smPointResult
		, ShadowMapResult const & smSpotResult )
		: crg::RunnablePass{ pass, context, graph, 1u, false }
		, m_device{ device }
		, m_scene{ scene }
		, m_lpResult{ lpResult }
		, m_smDirectionalResult{ smDirectionalResult }
		, m_smPointResult{ smPointResult }
		, m_smSpotResult{ smSpotResult }
	{
		m_renderPasses.emplace_back( doCreateRenderPass( false, lpResult ) );
		m_renderPasses.emplace_back( doCreateRenderPass( true, lpResult ) );
	}

	void RunnableLightingPass::clear()
	{
		for ( auto & pipeline : m_pipelines )
		{
			pipeline.second->clear();
		}
	}

	void RunnableLightingPass::enableLight( Camera const & camera
		, Light const & light )
	{
		auto & pipeline = doFindPipeline( light );
		pipeline.addLight( camera, light );
	}

	void RunnableLightingPass::resetCommandBuffer()
	{
		crg::RunnablePass::resetCommandBuffer();
		crg::RunnablePass::record();
	}

	void RunnableLightingPass::doInitialise( uint32_t index )
	{
	}

	void RunnableLightingPass::doRecordInto( VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		auto & renderPass = m_renderPasses[0];
		VkRenderPassBeginInfo beginRenderPass{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO
			, nullptr
			, *renderPass.renderPass
			, *renderPass.framebuffer
			, { {}, renderPass.framebuffer->getDimensions() }
			, uint32_t( renderPass.clearValues.size() )
			, renderPass.clearValues.data() };
		m_context.vkCmdBeginRenderPass( commandBuffer
			, &beginRenderPass
			, VK_SUBPASS_CONTENTS_INLINE );
		uint32_t pipelineIndex = 0u;

		for ( auto & pipeline : m_pipelines )
		{
			pipeline.second->recordInto( commandBuffer, pipelineIndex );
		}

		m_context.vkCmdEndRenderPass( commandBuffer );
	}

	LightRenderPass RunnableLightingPass::doCreateRenderPass( bool blend
		, LightPassResult const & lpResult )
	{
		castor::String name = blend
			? castor::String{ cuT( "Blend" ) }
			: castor::String{ cuT( "First" ) };
		VkImageLayout layout = blend
			? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			: VK_IMAGE_LAYOUT_UNDEFINED;
		VkAttachmentLoadOp loadOp = blend
			? VK_ATTACHMENT_LOAD_OP_LOAD
			: VK_ATTACHMENT_LOAD_OP_CLEAR;
		ashes::VkAttachmentDescriptionArray attaches{
			{ 0u
				, lpResult[LpTexture::eDepth].getFormat()
				, VK_SAMPLE_COUNT_1_BIT
				, VK_ATTACHMENT_LOAD_OP_LOAD
				, VK_ATTACHMENT_STORE_OP_STORE
				, VK_ATTACHMENT_LOAD_OP_DONT_CARE
				, VK_ATTACHMENT_STORE_OP_DONT_CARE
				, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
				, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
			, { 0u
				, lpResult[LpTexture::eDiffuse].getFormat()
				, VK_SAMPLE_COUNT_1_BIT
				, loadOp
				, VK_ATTACHMENT_STORE_OP_STORE
				, VK_ATTACHMENT_LOAD_OP_DONT_CARE
				, VK_ATTACHMENT_STORE_OP_DONT_CARE
				, layout
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL }
			, { 0u
				, lpResult[LpTexture::eSpecular].getFormat()
				, VK_SAMPLE_COUNT_1_BIT
				, loadOp
				, VK_ATTACHMENT_STORE_OP_STORE
				, VK_ATTACHMENT_LOAD_OP_DONT_CARE
				, VK_ATTACHMENT_STORE_OP_DONT_CARE
				, layout
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL }
			, { 0u
				, lpResult[LpTexture::eIndirectDiffuse].getFormat()
				, VK_SAMPLE_COUNT_1_BIT
				, loadOp
				, VK_ATTACHMENT_STORE_OP_STORE
				, VK_ATTACHMENT_LOAD_OP_DONT_CARE
				, VK_ATTACHMENT_STORE_OP_DONT_CARE
				, layout
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL }
			, { 0u
				, lpResult[LpTexture::eIndirectSpecular].getFormat()
				, VK_SAMPLE_COUNT_1_BIT
				, loadOp
				, VK_ATTACHMENT_STORE_OP_STORE
				, VK_ATTACHMENT_LOAD_OP_DONT_CARE
				, VK_ATTACHMENT_STORE_OP_DONT_CARE
				, layout
				, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } };
		ashes::VkAttachmentReferenceArray references{ { 1u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
			, { 2u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
			, { 3u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL }
			, { 4u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } };

		ashes::SubpassDescriptionArray subpasses;
		subpasses.emplace_back( ashes::SubpassDescription{ 0u
			, VK_PIPELINE_BIND_POINT_GRAPHICS
			, {}
			, references
			, {}
			, VkAttachmentReference{ 0u, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL }
			, {} } );

		ashes::VkSubpassDependencyArray dependencies{
			{ VK_SUBPASS_EXTERNAL
				, 0u
				, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
				, VK_ACCESS_SHADER_READ_BIT
				, VK_DEPENDENCY_BY_REGION_BIT }
			, { 0u
				, VK_SUBPASS_EXTERNAL
				, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
				, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
				, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
				, VK_ACCESS_SHADER_READ_BIT
				, VK_DEPENDENCY_BY_REGION_BIT } };
		ashes::RenderPassCreateInfo rpCreateInfo
		{
			0u,
			std::move( attaches ),
			std::move( subpasses ),
			std::move( dependencies ),
		};
		LightRenderPass result{};
		result.renderPass = m_device->createRenderPass( "LightPass" + name
			, std::move( rpCreateInfo ) );

		std::vector< VkImageView > viewAttaches{ lpResult[LpTexture::eDepth].targetView
			, lpResult[LpTexture::eDiffuse].targetView
			, lpResult[LpTexture::eSpecular].targetView
			, lpResult[LpTexture::eIndirectDiffuse].targetView
			, lpResult[LpTexture::eIndirectSpecular].targetView };
		VkFramebufferCreateInfo fbCreateInfo{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO, nullptr, 0u };
		fbCreateInfo.renderPass = *result.renderPass;
		fbCreateInfo.attachmentCount = uint32_t( viewAttaches.size() );
		fbCreateInfo.pAttachments = viewAttaches.data();
		fbCreateInfo.width = lpResult[LpTexture::eDiffuse].getExtent().width;
		fbCreateInfo.height = lpResult[LpTexture::eDiffuse].getExtent().height;
		fbCreateInfo.layers = 1u;
		result.framebuffer = result.renderPass->createFrameBuffer( "LightPass" + name
			, std::move( fbCreateInfo ) );

		result.clearValues.push_back( getClearValue( LpTexture::eDepth ) );
		result.clearValues.push_back( getClearValue( LpTexture::eDiffuse ) );
		result.clearValues.push_back( getClearValue( LpTexture::eSpecular ) );
		result.clearValues.push_back( getClearValue( LpTexture::eIndirectDiffuse ) );
		result.clearValues.push_back( getClearValue( LpTexture::eIndirectSpecular ) );
		return result;
	}

	LightsPipeline & RunnableLightingPass::doFindPipeline( Light const & light )
	{
		LightPipelineConfig config{ m_scene.getMaterialsType()
			, m_scene.getFlags()
			, light };
		auto hash = config.makeHash();
		auto it = m_pipelines.find( hash );

		if ( it == m_pipelines.end() )
		{
			it = m_pipelines.emplace( hash
				, std::make_unique< LightsPipeline >( m_pass
					, m_context
					, m_graph
					, m_device
					, config
					, m_lpResult
					, ( config.lightType == LightType::eDirectional
						? m_smDirectionalResult
						: ( config.lightType == LightType::ePoint
							? m_smPointResult
							: m_smSpotResult ) )
					, m_renderPasses ) ).first;
		}

		return *it->second;
	}

	//*********************************************************************************************

	LightingPass::LightingPass( crg::FrameGraph & graph
		, crg::FramePass const *& previousPass
		, RenderDevice const & device
		, castor::Size const & size
		, Scene & scene
		, OpaquePassResult const & gpResult
		, ShadowMapResult const & smDirectionalResult
		, ShadowMapResult const & smPointResult
		, ShadowMapResult const & smSpotResult
		, LightVolumePassResult const & lpvResult
		, LightVolumePassResultArray const & llpvResult
		, Texture const & vctFirstBounce
		, Texture const & vctSecondaryBounce
		, Texture const & depthView
		, SceneUbo const & sceneUbo
		, GpInfoUbo const & gpInfoUbo
		, LpvGridConfigUbo const & lpvConfigUbo
		, LayeredLpvGridConfigUbo const & llpvConfigUbo
		, VoxelizerUbo const & vctConfigUbo )
		: m_graph{ graph }
		, m_previousPass{ *previousPass }
		, m_device{ device }
		, m_gpResult{ gpResult }
		, m_smDirectionalResult{ smDirectionalResult }
		, m_smPointResult{ smPointResult }
		, m_smSpotResult{ smSpotResult }
		, m_lpvResult{ lpvResult }
		, m_llpvResult{ llpvResult }
		, m_vctFirstBounce{ vctFirstBounce }
		, m_vctSecondaryBounce{ vctSecondaryBounce }
		, m_depthView{ depthView }
		, m_sceneUbo{ sceneUbo }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_lpvConfigUbo{ lpvConfigUbo }
		, m_llpvConfigUbo{ llpvConfigUbo }
		, m_vctConfigUbo{ vctConfigUbo }
		, m_srcDepth{ depthView }
		, m_size{ size }
		, m_result{ scene.getOwner()->getGraphResourceHandler(), device, size }
	{
		previousPass = &doCreateDepthBlitPass( graph
			, *previousPass );
		previousPass = &doCreateLightingPass( graph
			, *previousPass
			, scene );
		m_result.create();
	}

	void LightingPass::update( CpuUpdater & updater )
	{
		auto & scene = *updater.camera->getScene();
		auto & cache = scene.getLightCache();
		m_lightPass->clear();

		if ( !cache.isEmpty() )
		{
			doUpdateLightPasses( updater, LightType::eDirectional );
			doUpdateLightPasses( updater, LightType::ePoint );
			doUpdateLightPasses( updater, LightType::eSpot );
		}

		m_lightPass->resetCommandBuffer();
	}

	void LightingPass::update( GpuUpdater & updater )
	{
		m_voxelConeTracing = updater.voxelConeTracing;
	}

	void LightingPass::accept( PipelineVisitorBase & visitor )
	{
		visitor.visit( "Light Diffuse"
			, m_result[LpTexture::eDiffuse]
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );
		visitor.visit( "Light Specular"
			, m_result[LpTexture::eSpecular]
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );
		visitor.visit( "Light Indirect Diffuse"
			, m_result[LpTexture::eIndirectDiffuse]
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );
		visitor.visit( "Light Indirect Specular"
			, m_result[LpTexture::eIndirectSpecular]
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
			, TextureFactors{}.invert( true ) );
	}

	void LightingPass::doUpdateLightPasses( CpuUpdater & updater
		, LightType lightType )
	{
		auto & camera = *updater.camera;
		auto & scene = *updater.camera->getScene();
		auto & cache = scene.getLightCache();
		auto previous = &m_previousPass;

		for ( auto & light : cache.getLights( lightType ) )
		{
			if ( light->getLightType() == LightType::eDirectional
				|| camera.isVisible( light->getBoundingBox(), light->getParent()->getDerivedTransformationMatrix() ) )
			{
				m_lightPass->enableLight( camera, *light );
			}
		}
	}

	crg::FramePass const & LightingPass::doCreateDepthBlitPass( crg::FrameGraph & graph
		, crg::FramePass const & previousPass )
	{
		auto & engine = *m_device.renderSystem.getEngine();
		auto & pass = graph.createPass( "DepthBlit"
			, [this, &engine]( crg::FramePass const & pass
				, crg::GraphContext const & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< crg::ImageCopy >( pass
					, context
					, graph
					, makeExtent3D( m_size ) );
				engine.registerTimer( "Opaque"
					, result->getTimer() );
				return result;
			} );
		pass.addDependency( previousPass );
		pass.addTransferInputView( m_srcDepth.wholeViewId
			, VK_IMAGE_LAYOUT_UNDEFINED );
		pass.addTransferOutputView( m_result[LpTexture::eDepth].wholeViewId
			, VK_IMAGE_LAYOUT_UNDEFINED );
		return pass;
	}

	crg::FramePass const & LightingPass::doCreateLightingPass( crg::FrameGraph & graph
		, crg::FramePass const & previousPass
		, Scene const & scene )
	{
		auto & engine = *m_device.renderSystem.getEngine();
		auto & pass = graph.createPass( "LightPass"
			, [this, &engine, &scene]( crg::FramePass const & pass
				, crg::GraphContext const & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< RunnableLightingPass >( pass
					, context
					, graph
					, m_device
					, scene
					, m_result
					, m_smDirectionalResult
					, m_smPointResult
					, m_smSpotResult );
				engine.registerTimer( "Opaque"
					, result->getTimer() );
				m_lightPass = result.get();
				return result;
			} );
		pass.addDependency( previousPass );
		engine.getMaterialCache().getPassBuffer().createPassBinding( pass
			, uint32_t( LightPassIdx::eMaterials ) );
		m_gpInfoUbo.createPassBinding( pass
			, uint32_t( LightPassIdx::eGpInfo ) );
		m_sceneUbo.createPassBinding( pass
			, uint32_t( LightPassIdx::eScene ) );
		m_lpvConfigUbo.createPassBinding( pass
			, uint32_t( LightPassIdx::eLpvGridConfig ) );
		m_llpvConfigUbo.createPassBinding( pass
			, uint32_t( LightPassIdx::eLayeredLpvGridConfig ) );
		m_vctConfigUbo.createPassBinding( pass
			, uint32_t( LightPassIdx::eVoxelData ) );
		pass.addSampledView( m_gpResult[DsTexture::eDepth].sampledViewId
			, uint32_t( LightPassIdx::eDepth )
			, VK_IMAGE_LAYOUT_UNDEFINED );
		pass.addSampledView( m_gpResult[DsTexture::eData1].sampledViewId
			, uint32_t( LightPassIdx::eData1 )
			, VK_IMAGE_LAYOUT_UNDEFINED );
		pass.addSampledView( m_gpResult[DsTexture::eData2].sampledViewId
			, uint32_t( LightPassIdx::eData2 )
			, VK_IMAGE_LAYOUT_UNDEFINED );
		pass.addSampledView( m_gpResult[DsTexture::eData3].sampledViewId
			, uint32_t( LightPassIdx::eData3 )
			, VK_IMAGE_LAYOUT_UNDEFINED );
		pass.addSampledView( m_gpResult[DsTexture::eData4].sampledViewId
			, uint32_t( LightPassIdx::eData4 )
			, VK_IMAGE_LAYOUT_UNDEFINED );
		pass.addSampledView( m_gpResult[DsTexture::eData5].sampledViewId
			, uint32_t( LightPassIdx::eData5 )
			, VK_IMAGE_LAYOUT_UNDEFINED );
		pass.addSampledView( m_vctFirstBounce.sampledViewId
			, uint32_t( LightPassIdx::eVctFirstBounce )
			, VK_IMAGE_LAYOUT_UNDEFINED );
		pass.addSampledView( m_vctSecondaryBounce.sampledViewId
			, uint32_t( LightPassIdx::eVctSecondBounce )
			, VK_IMAGE_LAYOUT_UNDEFINED );

		pass.addInOutDepthStencilView( m_result[LpTexture::eDepth].targetViewId );
		pass.addOutputColourView( m_result[LpTexture::eDiffuse].targetViewId );
		pass.addOutputColourView( m_result[LpTexture::eSpecular].targetViewId );
		pass.addOutputColourView( m_result[LpTexture::eIndirectDiffuse].targetViewId );
		pass.addOutputColourView( m_result[LpTexture::eIndirectSpecular].targetViewId );
		return pass;
	}
}
