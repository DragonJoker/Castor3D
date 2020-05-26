#include "Castor3D/Render/Technique/Opaque/Lighting/RsmGIPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/Parameter.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/Passes/LineariseDepthPass.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapResult.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaquePassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/RsmConfig.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassResult.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslFog.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslMetallicBrdfLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslSpecularBrdfLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslSssTransmittance.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelMatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/RsmConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <CastorUtils/Design/ArrayView.hpp>
#include <CastorUtils/Graphics/Image.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>
#include <ashespp/RenderPass/SubpassDescription.hpp>

#include <ShaderWriter/Source.hpp>

#include <numeric>
#include <random>

using namespace castor;

namespace castor3d
{
	namespace
	{
		static constexpr uint32_t RsmCfgUboIdx = 0u;
		static constexpr uint32_t RsmSamplesIdx = 1u;
		static constexpr uint32_t GpInfoUboIdx = 2u;
		static constexpr uint32_t LightsMapIdx = 3u;
		static constexpr uint32_t DepthMapIdx = 4u;
		static constexpr uint32_t Data1MapIdx = 5u;
		static constexpr uint32_t RsmNormalsIdx = 6u;
		static constexpr uint32_t RsmPositionIdx = 7u;
		static constexpr uint32_t RsmFluxIdx = 8u;

		std::unique_ptr< ast::Shader > getVertexProgram()
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			Vec2 position = writer.declInput< Vec2 >( "position", 0u );
			Vec2 uv = writer.declInput< Vec2 >( "uv", 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto out = writer.getOut();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					vtx_texture = uv;
					out.vtx.position = vec4( position, 0.0_f, 1.0_f );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr getDirectionalPixelShaderSource( RenderSystem const & renderSystem
			, LightType lightType
			, uint32_t width
			, uint32_t height )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			UBO_RSM_CONFIG( writer, RsmCfgUboIdx, 0u );
			ArraySsboT< Vec2 > c3d_rsmSamples
			{
				writer,
				"c3d_rsmSamples",
				writer.getTypesCache().getVec2F(),
				type::MemoryLayout::eStd430,
				RsmSamplesIdx,
				0u
			};
			UBO_GPINFO( writer, GpInfoUboIdx, 0u );
			auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights", LightsMapIdx, 0u );
			auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eDepth ), DepthMapIdx, 0u );
			auto c3d_mapData1 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData1 ), Data1MapIdx, 0u );
			auto c3d_rsmNormalMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( lightType, SmTexture::eNormalLinear ), RsmNormalsIdx, 0u );
			auto c3d_rsmPositionMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( lightType, SmTexture::ePosition ), RsmPositionIdx, 0u );
			auto c3d_rsmFluxMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( lightType, SmTexture::eFlux ), RsmFluxIdx, 0u );
			auto in = writer.getIn();

			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_rsmGI = writer.declOutput< Vec3 >( "pxl_rsmGI", 0 );

			// Utility functions
			shader::Utils utils{ writer };
			utils.declareCalcWSPosition();
			utils.declareCalcVSPosition();

			uint32_t index = 0;
			auto lightingModel = shader::PhongLightingModel::createModel( writer
					, utils
					, LightType::eDirectional
					, ShadowType::eNone
					, false // lightUbo
					, false // volumetric
					, true // rsm
					, index );

			auto reflectiveShadowMapping = writer.implementFunction< Vec3 >( "reflectiveShadowMapping"
				, [&]( Vec3 const & viewPosition
					, Vec3 const & worldPosition
					, Vec3 const & worldNormal )
				{
					auto cascadeIndex = writer.declLocale( "cascadeIndex"
						, 0_u );
					auto light = writer.declLocale( "light"
						, lightingModel->getDirectionalLight( 0_i ) );
					auto maxCount = writer.declLocale( "maxCount"
						, max( light.m_cascadeCount, 1_u ) - 1_u );

					// Get cascade index for the current fragment's view position
					FOR( writer, UInt, i, 0u, i < maxCount, ++i )
					{
						IF( writer, viewPosition.z() < light.m_splitDepths[i] )
						{
							cascadeIndex = i + 1_u;
						}
						FI;
					}
					ROF;

					auto indirectIllumination = writer.declLocale( "indirectIllumination"
						, vec3( 0.0_f ) );
					auto rMax = writer.declLocale< Float >( "rMax"
						, c3d_rsmRMax );
					auto lightSpacePosition = writer.declLocale< Vec4 >( "lightSpacePosition"
						, light.m_transforms[cascadeIndex] * vec4( worldPosition, 1.0 ) );
					lightSpacePosition.xy() = sdw::fma( lightSpacePosition.xy()
						, vec2( 0.5_f )
						, vec2( 0.5_f ) );

					FOR( writer, UInt, i, 0_u, i < c3d_rsmSampleCount, ++i )
					{
						auto rnd = writer.declLocale( "rnd"
							, c3d_rsmSamples[i].xy() );

						auto coords = writer.declLocale( "coords"
							, vec3( lightSpacePosition.xy() + rMax * rnd
								, writer.cast< Float >( cascadeIndex ) ) );

						auto vplPositionWS = writer.declLocale( "vplPositionWS"
							, texture( c3d_rsmPositionMap, coords ).xyz() );
						auto vplNormalWS = writer.declLocale( "vplNormalWS"
							, texture( c3d_rsmNormalMap, coords ).xyz() );
						auto vplFlux = writer.declLocale( "vplFlux"
							, texture( c3d_rsmFluxMap, coords ).xyz() );
						auto dot1 = writer.declLocale( "dot1"
							, max( 0.0_f, dot( vplNormalWS, worldPosition - vplPositionWS ) ) );
						auto dot2 = writer.declLocale( "dot2"
							, max( 0.0_f, dot( worldNormal, vplPositionWS - worldPosition ) ) );
						auto diff = writer.declLocale( "diff"
							, worldPosition - vplPositionWS );
						auto sqdist = writer.declLocale( "sqdist"
							, dot( diff, diff ) );

						auto result = writer.declLocale( "result"
							, vplFlux * ( dot1 * dot2 ) / ( sqdist * sqdist ) );

						result *= rnd.x() * rnd.x();
						indirectIllumination += result;
					}
					ROF;

					writer.returnStmt( clamp( indirectIllumination * c3d_rsmIntensity
						, vec3( 0.0_f )
						, vec3( 1.0_f ) ) );
				}
				, InVec3{ writer, "viewPosition" }
				, InVec3{ writer, "worldPosition" }
				, InVec3{ writer, "worldNormal" } );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto texCoord = writer.declLocale( "texCoord"
						, vtx_texture );
					auto depth = writer.declLocale( "depth"
						, textureLod( c3d_mapDepth, texCoord, 0.0_f ).x() );

					IF( writer, depth == 1.0_f )
					{
						writer.discard();
					}
					FI;

					auto data1 = writer.declLocale( "data1"
						, textureLod( c3d_mapData1, texCoord, 0.0_f ) );
					auto vsPosition = writer.declLocale( "vsPosition"
						, utils.calcVSPosition( texCoord, depth, c3d_mtxInvProj ) );
					auto wsPosition = writer.declLocale( "wsPosition"
						, utils.calcWSPosition( texCoord, depth, c3d_mtxInvViewProj ) );
					auto wsNormal = writer.declLocale( "wsNormal"
						, data1.xyz() );
					pxl_rsmGI = reflectiveShadowMapping( vsPosition, wsPosition, wsNormal );
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr getSpotPixelShaderSource( RenderSystem const & renderSystem
			, LightType lightType
			, uint32_t width
			, uint32_t height )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			UBO_RSM_CONFIG( writer, RsmCfgUboIdx, 0u );
			ArraySsboT< Vec2 > c3d_rsmSamples
			{
				writer,
				"c3d_rsmSamples",
				writer.getTypesCache().getVec2F(),
				type::MemoryLayout::eStd430,
				RsmSamplesIdx,
				0u
			};
			UBO_GPINFO( writer, GpInfoUboIdx, 0u );
			auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights", LightsMapIdx, 0u );
			auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eDepth ), DepthMapIdx, 0u );
			auto c3d_mapData1 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData1 ), Data1MapIdx, 0u );
			auto c3d_rsmNormalMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( lightType, SmTexture::eNormalLinear ), RsmNormalsIdx, 0u );
			auto c3d_rsmPositionMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( lightType, SmTexture::ePosition ), RsmPositionIdx, 0u );
			auto c3d_rsmFluxMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( lightType, SmTexture::eFlux ), RsmFluxIdx, 0u );
			auto in = writer.getIn();

			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_rsmGI = writer.declOutput< Vec3 >( "pxl_rsmGI", 0 );

			// Utility functions
			shader::Utils utils{ writer };
			utils.declareCalcWSPosition();

			uint32_t index = 0;
			auto lightingModel = shader::PhongLightingModel::createModel( writer
				, utils
				, LightType::eSpot
				, ShadowType::eNone
				, false // lightUbo
				, false // volumetric
				, true // rsm
				, index );

			auto reflectiveShadowMapping = writer.implementFunction< Vec3 >( "reflectiveShadowMapping"
				, [&]( Vec3 const & worldPosition
					, Vec3 const & worldNormal )
				{
					auto light = writer.declLocale( "light"
						, lightingModel->getSpotLight( 0_i ) );
					auto lightSpacePosition = writer.declLocale( "lightSpacePosition"
						, c3d_lightViewProj * vec4( worldPosition, 1.0 ) );
					lightSpacePosition.xy() = sdw::fma( lightSpacePosition.xy()
						, vec2( 0.5_f )
						, vec2( 0.5_f ) );
					lightSpacePosition /= lightSpacePosition.w();
					lightSpacePosition.xy() += vec2( 0.5_f );
					auto indirectIllumination = writer.declLocale( "indirectIllumination"
						, vec3( 0.0_f ) );
					auto rMax = writer.declLocale( "rMax"
						, vec2( c3d_rsmRMax ) );

					FOR( writer, UInt, i, 0_u, i < c3d_rsmSampleCount, ++i )
					{
						auto rnd = writer.declLocale( "rnd"
							, c3d_rsmSamples[i].xy() );

						auto coords = writer.declLocale( "coords"
							, vec3( sdw::fma( rnd, rMax, lightSpacePosition.xy() ), c3d_rsmIndex ) );

						auto vplPositionWS = writer.declLocale( "vplPositionWS"
							, texture( c3d_rsmPositionMap, coords ).xyz() );
						auto vplNormalWS = writer.declLocale( "vplNormalWS"
							, texture( c3d_rsmNormalMap, coords ).xyz() );
						auto vplFlux = writer.declLocale( "vplFlux"
							, texture( c3d_rsmFluxMap, coords ).xyz() );
						auto diff = writer.declLocale( "diff"
							, worldPosition - vplPositionWS );
						auto dot1 = writer.declLocale( "dot1"
							, max( 0.0_f, dot( vplNormalWS, diff ) ) );
						auto dot2 = writer.declLocale( "dot2"
							, max( 0.0_f, dot( worldNormal, vplPositionWS - worldPosition ) ) );
						auto sqdist = writer.declLocale( "sqdist"
							, dot( diff, diff ) );

						auto result = writer.declLocale( "result"
							, vplFlux * ( dot1 * dot2 ) / ( sqdist * sqdist ) );

						result *= rnd.x() * rnd.x();
						indirectIllumination += result;
					}
					ROF;

					writer.returnStmt( clamp( indirectIllumination * c3d_rsmIntensity
						, vec3( 0.0_f )
						, vec3( 1.0_f ) ) );
				}
				, InVec3{ writer, "worldPosition" }
				, InVec3{ writer, "worldNormal" } );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto texCoord = writer.declLocale( "texCoord"
						, vtx_texture );
					auto depth = writer.declLocale( "depth"
						, textureLod( c3d_mapDepth, texCoord, 0.0_f ).x() );

					IF( writer, depth == 1.0_f )
					{
						writer.discard();
					}
					FI;

					auto data1 = writer.declLocale( "data1"
						, textureLod( c3d_mapData1, texCoord, 0.0_f ) );
					auto wsPosition = writer.declLocale( "wsPosition"
						, utils.calcWSPosition( texCoord, depth, c3d_mtxInvViewProj ) );
					auto wsNormal = writer.declLocale( "wsNormal"
						, data1.xyz() );

					IF( writer, dot( wsNormal, wsNormal ) == 0 )
					{
						writer.discard();
					}
					FI;

					pxl_rsmGI = reflectiveShadowMapping( wsPosition, wsNormal );
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr getPointPixelShaderSource( RenderSystem const & renderSystem
			, LightType lightType
			, uint32_t width
			, uint32_t height )
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			UBO_RSM_CONFIG( writer, RsmCfgUboIdx, 0u );
			ArraySsboT< Vec2 > c3d_rsmSamples
			{
				writer,
				"c3d_rsmSamples",
				writer.getTypesCache().getVec2F(),
				type::MemoryLayout::eStd430,
				RsmSamplesIdx,
				0u
			};
			UBO_GPINFO( writer, GpInfoUboIdx, 0u );
			auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights", LightsMapIdx, 0u );
			auto c3d_mapDepth = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eDepth ), DepthMapIdx, 0u );
			auto c3d_mapData1 = writer.declSampledImage< FImg2DRgba32 >( getTextureName( DsTexture::eData1 ), Data1MapIdx, 0u );
			auto c3d_rsmNormalMap = writer.declSampledImage< FImgCubeArrayRgba32 >( getTextureName( lightType, SmTexture::eNormalLinear ), RsmNormalsIdx, 0u );
			auto c3d_rsmPositionMap = writer.declSampledImage< FImgCubeArrayRgba32 >( getTextureName( lightType, SmTexture::ePosition ), RsmPositionIdx, 0u );
			auto c3d_rsmFluxMap = writer.declSampledImage< FImgCubeArrayRgba32 >( getTextureName( lightType, SmTexture::eFlux ), RsmFluxIdx, 0u );

			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

			// Shader outputs
			auto pxl_rsmGI = writer.declOutput< Vec3 >( "pxl_rsmGI", 0 );

			// Utility functions
			shader::Utils utils{ writer };
			utils.declareCalcWSPosition();

			uint32_t index = 0;
			auto lightingModel = shader::PhongLightingModel::createModel( writer
				, utils
				, LightType::ePoint
				, ShadowType::eNone
				, false // lightUbo
				, false // volumetric
				, true // rsm
				, index );

			auto reflectiveShadowMapping = writer.implementFunction< Vec3 >( "reflectiveShadowMapping"
				, [&]( Vec3 const & worldPosition
					, Vec3 const & worldNormal )
				{
					auto light = writer.declLocale( "light"
						, lightingModel->getPointLight( 0_i ) );
					auto indirectIllumination = writer.declLocale( "indirectIllumination"
						, vec3( 0.0_f ) );
					auto rMax = writer.declLocale< Float >( "rMax"
						, c3d_rsmRMax );

					FOR( writer, UInt, i, 0_u, i < c3d_rsmSampleCount, ++i )
					{
						auto rnd = writer.declLocale( "rnd"
							, c3d_rsmSamples[i].xy() );
						auto vertexToLight = writer.declLocale( "vertexToLight"
							, worldPosition - light.m_position );
						auto coords = writer.declLocale( "coords"
							, vec4( vec3( vertexToLight.xy() + rMax * rnd, vertexToLight.z() )
								, writer.cast< Float >( c3d_rsmIndex ) ) );

						auto vplPositionWS = writer.declLocale( "vplPositionWS"
							, texture( c3d_rsmPositionMap, coords ).xyz() );
						auto vplNormalWS = writer.declLocale( "vplNormalWS"
							, texture( c3d_rsmNormalMap, coords ).xyz() );
						auto vplFlux = writer.declLocale( "vplFlux"
							, texture( c3d_rsmFluxMap, coords ).xyz() );
						auto diff = writer.declLocale( "diff"
							, worldPosition - vplPositionWS );
						auto dot1 = writer.declLocale( "dot1"
							, max( 0.0_f, dot( vplNormalWS, diff ) ) );
						auto dot2 = writer.declLocale( "dot2"
							, max( 0.0_f, dot( worldNormal, vplPositionWS - worldPosition ) ) );
						auto sqdist = writer.declLocale( "sqdist"
							, dot( diff, diff ) );

						auto result = writer.declLocale( "result"
							, vplFlux * ( dot1 * dot2 ) / ( sqdist * sqdist ) );

						result *= rnd.x() * rnd.x();
						indirectIllumination += result;
					}
					ROF;

					writer.returnStmt( clamp( indirectIllumination * c3d_rsmIntensity
						, vec3( 0.0_f )
						, vec3( 1.0_f ) ) );
				}
				, InVec3{ writer, "worldPosition" }
				, InVec3{ writer, "worldNormal" } );

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto texCoord = writer.declLocale( "texCoord"
						, vtx_texture );
					auto data1 = writer.declLocale( "data1"
						, textureLod( c3d_mapData1, texCoord, 0.0_f ) );
					auto depth = writer.declLocale( "depth"
						, textureLod( c3d_mapDepth, texCoord, 0.0_f ).x() );
					auto wsPosition = writer.declLocale( "wsPosition"
						, utils.calcWSPosition( texCoord, depth, c3d_mtxInvViewProj ) );
					auto wsNormal = writer.declLocale( "wsNormal"
						, data1.xyz() );
					pxl_rsmGI = reflectiveShadowMapping( wsPosition, wsNormal );
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< ast::Shader > getPixelProgram( Engine const & engine
			, LightType lightType
			, uint32_t width
			, uint32_t height )
		{
			switch ( lightType )
			{
			case LightType::eDirectional:
				return getDirectionalPixelShaderSource( *engine.getRenderSystem()
					, lightType
					, width
					, height );
			case LightType::eSpot:
				return getSpotPixelShaderSource( *engine.getRenderSystem()
					, lightType
					, width
					, height );
			case LightType::ePoint:
				return getPointPixelShaderSource( *engine.getRenderSystem()
					, lightType
					, width
					, height );
			default:
				CU_Failure( "Unexpected MaterialType" );
				return nullptr;
			}
		}

		ashes::RenderPassPtr doCreateRenderPass( RenderDevice const & device
			, VkFormat format )
		{
			ashes::VkAttachmentDescriptionArray attaches
			{
				{
					0u,
					format,
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_LOAD,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				}
			};
			ashes::SubpassDescriptionArray subpasses;
			subpasses.emplace_back( ashes::SubpassDescription
				{
					0u,
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					{},
					{ { 0u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } },
					{},
					ashes::nullopt,
					{},
				} );
			ashes::VkSubpassDependencyArray dependencies
			{
				{
					VK_SUBPASS_EXTERNAL,
					0u,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					VK_ACCESS_SHADER_READ_BIT,
					VK_DEPENDENCY_BY_REGION_BIT,
				},
				{
					0u,
					VK_SUBPASS_EXTERNAL,
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
					VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					VK_ACCESS_SHADER_READ_BIT,
					VK_DEPENDENCY_BY_REGION_BIT,
				},
			};
			ashes::RenderPassCreateInfo createInfo
			{
				0u,
				std::move( attaches ),
				std::move( subpasses ),
				std::move( dependencies ),
			};
			auto result = device->createRenderPass( "SsgiRawGI"
				, std::move( createInfo ) );
			return result;
		}

		SamplerSPtr doCreateSampler( Engine & engine
			, String const & name
			, VkSamplerAddressMode mode )
		{
			SamplerSPtr sampler;

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setMinFilter( VK_FILTER_LINEAR );
				sampler->setMagFilter( VK_FILTER_LINEAR );
				sampler->setWrapS( mode );
				sampler->setWrapT( mode );
			}

			return sampler;
		}

		TextureUnit doCreateTexture( Engine & engine
			, castor::String const & name
			, VkFormat format
			, VkExtent2D const & size
			, bool transfer )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto sampler = doCreateSampler( engine, name, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );

			ashes::ImageCreateInfo image
			{
				0u,
				VK_IMAGE_TYPE_2D,
				format,
				{ size.width, size.height, 1u },
				1u,
				1u,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
					| VK_IMAGE_USAGE_SAMPLED_BIT 
					| ( transfer ? VK_IMAGE_USAGE_TRANSFER_DST_BIT : 0u ) ),
			};
			auto ssaoResult = std::make_shared< TextureLayout >( renderSystem
				, image
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, name );
			TextureUnit result{ engine };
			result.setTexture( ssaoResult );
			result.setSampler( sampler );
			result.initialise();
			return result;
		}

		ashes::FrameBufferPtr doCreateFrameBuffer( ashes::RenderPass const & renderPass
			, ashes::ImageView const & view )
		{
			ashes::ImageViewCRefArray attaches;
			attaches.emplace_back( view );
			auto size = view.image->getDimensions();
			return renderPass.createFrameBuffer( "SsgiRawGI"
				, VkExtent2D{ size.width, size.height }
				, std::move( attaches ) );
		}
	}

	//*********************************************************************************************

	RsmGIPass::RsmGIPass( Engine & engine
		, LightCache const & lightCache
		, LightType lightType
		, VkExtent2D const & size
		, GpInfoUbo const & gpInfo
		, OpaquePassResult const & gpResult
		, ShadowMapResult const & smResult
		, LightPassResult const & lpResult )
		: RenderQuad{ *engine.getRenderSystem(), "RsmGI", VK_FILTER_LINEAR, { ashes::nullopt, RenderQuadConfig::Texcoord{}, BlendMode::eAdditive } }
		, m_lightCache{ lightCache }
		, m_gpResult{ gpResult }
		, m_smResult{ smResult }
		, m_lpResult{ lpResult }
		, m_gpInfo{ gpInfo }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, getName(), getVertexProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, getName(), getPixelProgram( engine, lightType, size.width, size.height ) }
		, m_rsmConfigUbo{ engine }
		, m_rsmSamplesSsbo{ makeBuffer< castor::Point2f >( getCurrentRenderDevice( engine )
			, RsmConfig::MaxRange
			, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "RsmSamples" ) }
		, m_renderPass{ doCreateRenderPass( getCurrentRenderDevice( m_renderSystem ), m_lpResult[LpTexture::eDiffuse].getTexture()->getPixelFormat() ) }
		, m_frameBuffer{ doCreateFrameBuffer( *m_renderPass, m_lpResult[LpTexture::eDiffuse].getTexture()->getDefaultView().getTargetView() ) }
		, m_timer{ std::make_shared< RenderPassTimer >( engine, cuT( "Lighting" ), cuT( "RSM GI" ) ) }
		, m_finished{ getCurrentRenderDevice( engine )->createSemaphore( getName() ) }
	{
		if ( auto buffer = m_rsmSamplesSsbo->lock( 0u, RsmConfig::MaxRange, 0u ) )
		{
			std::random_device rd;
			std::mt19937 rng( rd() );
			std::uniform_real_distribution< float > dist( 0.0f, 1.0f );

			for ( auto & point : makeArrayView( buffer, buffer + RsmConfig::MaxRange ) )
			{
				auto rnd = dist( rng );
				auto twoPIy = PiMult2< float > * rnd;
				rnd = dist( rng );
				point[0] = rnd * sin( twoPIy );
				point[1] = rnd * cos( twoPIy );
			}

			m_rsmSamplesSsbo->flush( 0u, RsmConfig::MaxRange );
			m_rsmSamplesSsbo->unlock();
		}

		auto & device = getCurrentRenderDevice( m_renderSystem );
		ashes::PipelineShaderStageCreateInfoArray shaderStages;
		shaderStages.push_back( makeShaderState( device, m_vertexShader ) );
		shaderStages.push_back( makeShaderState( device, m_pixelShader ) );

		ashes::VkDescriptorSetLayoutBindingArray bindings
		{
			makeDescriptorSetLayoutBinding( RsmCfgUboIdx
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
			makeDescriptorSetLayoutBinding( RsmSamplesIdx
				, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
			makeDescriptorSetLayoutBinding( GpInfoUboIdx
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
			makeDescriptorSetLayoutBinding( LightsMapIdx
				, VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
			makeDescriptorSetLayoutBinding( DepthMapIdx
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
			makeDescriptorSetLayoutBinding( Data1MapIdx
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
			makeDescriptorSetLayoutBinding( RsmNormalsIdx
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
			makeDescriptorSetLayoutBinding( RsmPositionIdx
				, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
				, VK_SHADER_STAGE_FRAGMENT_BIT ),
		};
		this->createPipeline( size
			, {}
			, shaderStages
			, m_smResult[SmTexture::eFlux].getTexture()->getDefaultView().getSampledView()
			, *m_renderPass
			, std::move( bindings )
			, {} );
		auto commands = getCommands( *m_timer, 0u );
		m_commandBuffer = std::move( commands.commandBuffer );
		m_finished = std::move( commands.semaphore );
	}

	ashes::Semaphore const & RsmGIPass::compute( ashes::Semaphore const & toWait )const
	{
		auto & renderSystem = m_renderSystem;
		auto & device = getCurrentRenderDevice( renderSystem );
		auto timerBlock = m_timer->start();
		m_timer->notifyPassRender();
		auto * result = &toWait;
		auto fence = device->createFence( getName() );

		device.graphicsQueue->submit( *m_commandBuffer
			, toWait
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, *m_finished
			, fence.get() );
		fence->wait( ashes::MaxTimeout );
		result = m_finished.get();

		return *result;
	}

	CommandsSemaphore RsmGIPass::getCommands( RenderPassTimer const & timer
		, uint32_t index )const
	{
		auto & device = getCurrentRenderDevice( m_renderSystem );
		castor3d::CommandsSemaphore commands
		{
			device.graphicsCommandPool->createCommandBuffer( getName() ),
			device->createSemaphore( getName() )
		};
		auto & cmd = *commands.commandBuffer;

		cmd.begin( VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT );
		timer.beginPass( cmd, index );
		cmd.beginDebugBlock(
			{
				"Lighting - RSM GI",
				castor3d::makeFloatArray( m_renderSystem.getEngine()->getNextRainbowColour() ),
			} );
		cmd.memoryBarrier( VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
			, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, m_gpResult[DsTexture::eDepth].getTexture()->getDefaultView().getTargetView().makeShaderInputResource( VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL ) );
		cmd.beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, { castor3d::transparentBlackClearColor }
			, VK_SUBPASS_CONTENTS_INLINE );
		registerFrame( cmd );
		cmd.endRenderPass();
		cmd.memoryBarrier( VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
			, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
			, m_gpResult[DsTexture::eDepth].getTexture()->getDefaultView().getTargetView().makeDepthStencilReadOnly( VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) );
		cmd.endDebugBlock();
		timer.endPass( cmd, index );
		cmd.end();

		return commands;
	}

	void RsmGIPass::accept( RsmConfig & config
		, PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
		config.accept( m_pixelShader.name, visitor );
	}

	void RsmGIPass::update( Light const & light )
	{
		m_rsmConfigUbo.update( light );
	}

	void RsmGIPass::doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
		, ashes::DescriptorSet & descriptorSet )
	{
		descriptorSet.createSizedBinding( descriptorSetLayout.getBinding( RsmCfgUboIdx )
			, *m_rsmConfigUbo.getUbo().buffer
			, m_rsmConfigUbo.getUbo().offset
			, 1u );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( RsmSamplesIdx )
			, m_rsmSamplesSsbo->getBuffer()
			, 0u
			, uint32_t( m_rsmSamplesSsbo->getMemoryRequirements().size ) );
		descriptorSet.createSizedBinding( descriptorSetLayout.getBinding( GpInfoUboIdx )
			, m_gpInfo.getUbo() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( LightsMapIdx )
			, m_lightCache.getBuffer()
			, m_lightCache.getView() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( DepthMapIdx )
			, m_gpResult[DsTexture::eDepth].getTexture()->getDefaultView().getSampledView()
			, m_gpResult[DsTexture::eDepth].getSampler()->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( Data1MapIdx )
			, m_gpResult[DsTexture::eData1].getTexture()->getDefaultView().getSampledView()
			, m_gpResult[DsTexture::eData1].getSampler()->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( RsmNormalsIdx )
			, m_smResult[SmTexture::eNormalLinear].getTexture()->getDefaultView().getSampledView()
			, m_smResult[SmTexture::eNormalLinear].getSampler()->getSampler() );
		descriptorSet.createBinding( descriptorSetLayout.getBinding( RsmPositionIdx )
			, m_smResult[SmTexture::ePosition].getTexture()->getDefaultView().getSampledView()
			, m_smResult[SmTexture::ePosition].getSampler()->getSampler() );
	}

	void RsmGIPass::doRegisterFrame( ashes::CommandBuffer & commandBuffer )const
	{
	}
}
