#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/GeometryInjectionPass.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/SamplerCache.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/RenderPass.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapResult.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/LpvLightConfigUbo.hpp"

#include <CastorUtils/Graphics/Image.hpp>

#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/GraphContext.hpp>

#include <numeric>
#include <random>

CU_ImplementCUSmartPtr( castor3d, GeometryInjectionPass )

using namespace castor;

namespace castor3d
{
	namespace
	{
		std::unique_ptr< ast::Shader > getDirectionalVertexProgram( uint32_t rsmTexSize
			, uint32_t layerIndex )
		{
			using namespace sdw;
			VertexWriter writer;

			if ( shader::DirectionalMaxCascadesCount > 1u )
			{
				auto inPosition = writer.declInput< Vec2 >( "inPosition", 0u );
				auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights"
					, GeometryInjectionPass::LightsIdx
					, 0u );
#if C3D_UseTiledDirectionalShadowMap
				auto c3d_rsmNormalMap = writer.declSampledImage< FImg2DRgba32 >( getTextureName( LightType::eDirectional, SmTexture::eNormalLinear )
					, GeometryInjectionPass::RsmNormalsIdx
					, 0u );
				auto c3d_rsmPositionMap = writer.declSampledImage< FImg2DRgba32 >( getTextureName( LightType::eDirectional, SmTexture::ePosition )
					, GeometryInjectionPass::RsmPositionIdx
					, 0u );
#else
				auto c3d_rsmNormalMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( LightType::eDirectional, SmTexture::eNormalLinear )
					, GeometryInjectionPass::RsmNormalsIdx
					, 0u );
				auto c3d_rsmPositionMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( LightType::eDirectional, SmTexture::ePosition )
					, GeometryInjectionPass::RsmPositionIdx
					, 0u );
#endif
				UBO_LPVGRIDCONFIG( writer, GeometryInjectionPass::LpvGridUboIdx, 0u, true );
				UBO_LPVLIGHTCONFIG( writer, GeometryInjectionPass::LpvLightUboIdx, 0u );
				auto in = writer.getIn();

				uint32_t index = 0u;
				auto outVolumeCellIndex = writer.declOutput< IVec3 >( "outVolumeCellIndex", index++ );
				auto outRsmPos = writer.declOutput< Vec3 >( "outRsmPos", index++ );
				auto outRsmNormal = writer.declOutput< Vec3 >( "outRsmNormal", index++ );
				auto outSurfelArea = writer.declOutput< Float >( "outSurfelArea", index++ );
				auto outLightPos = writer.declOutput< Vec3 >( "outLightPos", index++ );
				auto out = writer.getOut();

				// Utility functions
				shader::Utils utils{ writer };
				index = 0;
				auto lightingModel = shader::PhongLightingModel::createModel( writer
					, utils
					, LightType::eDirectional
					, shader::ShadowOptions{ SceneFlag::eNone, true }
					, index
					, 1u );

				auto convertPointToGridIndex = writer.implementFunction< IVec3 >( "convertPointToGridIndex"
					, [&]( Vec3 pos )
					{
						writer.returnStmt( ivec3( ( pos - c3d_lpvGridData.minVolumeCorner ) / vec3( c3d_lpvGridData.cellSize ) - vec3( 0.5_f ) ) );
					}
					, InVec3{ writer, "pos" } );

				//Sample from camera
				auto calculateSurfelAreaLightViewM = writer.implementFunction< Float >( "calculateSurfelAreaLightViewM"
					, [&]( Vec3 viewPos )
					{
						writer.returnStmt( ( 4.0_f * viewPos.z() * viewPos.z() * c3d_lpvLightData.tanFovXHalf * c3d_lpvLightData.tanFovYHalf ) / Float{ float( rsmTexSize * rsmTexSize ) } );
					}
					, InVec3{ writer, "viewPos" } );

				writer.implementFunction< Void >( "main"
					, [&]()
					{
						auto light = writer.declLocale( "light"
							, lightingModel->getDirectionalLight( c3d_lpvLightData.lightIndex ) );
						auto cascadeIndex = writer.declLocale( "cascadeIndex"
							, writer.cast< Int >( max( 1_u, light.m_cascadeCount ) - 1_u ) );
#if C3D_UseTiledDirectionalShadowMap
						auto rsmCoords = writer.declLocale( "rsmCoords"
							, ivec2( in.vertexIndex % rsmTexSize
								, in.vertexIndex / rsmTexSize ) );
#else
						auto rsmCoords = writer.declLocale( "rsmCoords"
							, ivec3( in.vertexIndex % rsmTexSize
								, in.vertexIndex / rsmTexSize
								, cascadeIndex ) );
#endif

						outRsmPos = c3d_rsmPositionMap.fetch( rsmCoords, 0_i ).rgb();
						outRsmNormal = c3d_rsmNormalMap.fetch( rsmCoords, 0_i ).rgb();
						auto viewPos = writer.declLocale( "viewPos"
							, c3d_lpvLightData.lightView * vec4( outRsmPos, 1.0 ) );
						outSurfelArea = calculateSurfelAreaLightViewM( viewPos.xyz() ) * c3d_lpvLightData.texelAreaModifier;
						outLightPos = outRsmPos - light.m_direction;

						outVolumeCellIndex = convertPointToGridIndex( outRsmPos );

						auto screenPos = writer.declLocale( "screenPos"
							, ( vec2( outVolumeCellIndex.xy() ) + 0.5_f ) / vec2( c3d_lpvGridData.gridSize.xy() ) * 2.0_f - 1.0_f );

						out.vtx.position = vec4( screenPos, 0.0, 1.0 );
					} );
			}
			else
			{
				auto inPosition = writer.declInput< Vec2 >( "inPosition", 0u );
				auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights"
					, GeometryInjectionPass::LightsIdx
					, 0u );
				auto c3d_rsmNormalMap = writer.declSampledImage< FImg2DRgba32 >( getTextureName( LightType::eDirectional, SmTexture::eNormalLinear )
					, GeometryInjectionPass::RsmNormalsIdx
					, 0u );
				auto c3d_rsmPositionMap = writer.declSampledImage< FImg2DRgba32 >( getTextureName( LightType::eDirectional, SmTexture::ePosition )
					, GeometryInjectionPass::RsmPositionIdx
					, 0u );
				UBO_LPVGRIDCONFIG( writer, GeometryInjectionPass::LpvGridUboIdx, 0u, true );
				UBO_LPVLIGHTCONFIG( writer, GeometryInjectionPass::LpvLightUboIdx, 0u );
				auto in = writer.getIn();

				uint32_t index = 0u;
				auto outVolumeCellIndex = writer.declOutput< IVec3 >( "outVolumeCellIndex", index++ );
				auto outRsmPos = writer.declOutput< Vec3 >( "outRsmPos", index++ );
				auto outRsmNormal = writer.declOutput< Vec3 >( "outRsmNormal", index++ );
				auto outSurfelArea = writer.declOutput< Float >( "outSurfelArea", index++ );
				auto outLightPos = writer.declOutput< Vec3 >( "outLightPos", index++ );
				auto out = writer.getOut();

				// Utility functions
				shader::Utils utils{ writer };
				index = 0;
				auto lightingModel = shader::PhongLightingModel::createModel( writer
					, utils
					, LightType::eDirectional
					, shader::ShadowOptions{ SceneFlag::eNone, true }
					, index
					, 1u );

				auto convertPointToGridIndex = writer.implementFunction< IVec3 >( "convertPointToGridIndex"
					, [&]( Vec3 pos )
					{
						writer.returnStmt( ivec3( ( pos - c3d_lpvGridData.minVolumeCorner ) / vec3( c3d_lpvGridData.cellSize ) - vec3( 0.5_f ) ) );
					}
					, InVec3{ writer, "pos" } );

				//Sample from camera
				auto calculateSurfelAreaLightViewM = writer.implementFunction< Float >( "calculateSurfelAreaLightViewM"
					, [&]( Vec3 viewPos )
					{
						writer.returnStmt( ( 4.0_f * viewPos.z() * viewPos.z() * c3d_lpvLightData.tanFovXHalf * c3d_lpvLightData.tanFovYHalf ) / Float{ float( rsmTexSize * rsmTexSize ) } );
					}
					, InVec3{ writer, "viewPos" } );

				writer.implementFunction< Void >( "main"
					, [&]()
					{
						auto light = writer.declLocale( "light"
							, lightingModel->getDirectionalLight( c3d_lpvLightData.lightIndex ) );
						auto rsmCoords = writer.declLocale( "rsmCoords"
							, ivec2( in.vertexIndex % rsmTexSize
								, in.vertexIndex / rsmTexSize ) );

						outRsmPos = c3d_rsmPositionMap.fetch( rsmCoords, 0_i ).rgb();
						outRsmNormal = c3d_rsmNormalMap.fetch( rsmCoords, 0_i ).rgb();
						auto viewPos = writer.declLocale( "viewPos"
							, c3d_lpvLightData.lightView * vec4( outRsmPos, 1.0 ) );
						outSurfelArea = calculateSurfelAreaLightViewM( viewPos.xyz() ) * c3d_lpvLightData.texelAreaModifier;
						outLightPos = outRsmPos - light.m_direction;

						outVolumeCellIndex = convertPointToGridIndex( outRsmPos );

						auto screenPos = writer.declLocale( "screenPos"
							, ( vec2( outVolumeCellIndex.xy() ) + 0.5_f ) / vec2( c3d_lpvGridData.gridSize.xy() ) * 2.0_f - 1.0_f );

						out.vtx.position = vec4( screenPos, 0.0, 1.0 );
					} );
			}

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< ast::Shader > getSpotVertexProgram( uint32_t rsmTexSize )
		{
			using namespace sdw;
			VertexWriter writer;

			auto inPosition = writer.declInput< Vec2 >( "inPosition", 0u );
			auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights"
				, GeometryInjectionPass::LightsIdx
				, 0u );
			auto c3d_rsmNormalMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( LightType::eSpot, SmTexture::eNormalLinear )
				, GeometryInjectionPass::RsmNormalsIdx
				, 0u );
			auto c3d_rsmPositionMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( LightType::eSpot, SmTexture::ePosition )
				, GeometryInjectionPass::RsmPositionIdx
				, 0u );
			UBO_LPVGRIDCONFIG( writer, GeometryInjectionPass::LpvGridUboIdx, 0u, true );
			UBO_LPVLIGHTCONFIG( writer, GeometryInjectionPass::LpvLightUboIdx, 0u );
			auto in = writer.getIn();

			uint32_t index = 0u;
			auto outVolumeCellIndex = writer.declOutput< IVec3 >( "outVolumeCellIndex", index++ );
			auto outRsmPos = writer.declOutput< Vec3 >( "outRsmPos", index++ );
			auto outRsmNormal = writer.declOutput< Vec3 >( "outRsmNormal", index++ );
			auto outSurfelArea = writer.declOutput< Float >( "outSurfelArea", index++ );
			auto outLightPos = writer.declOutput< Vec3 >( "outLightPos", index++ );
			auto out = writer.getOut();

			// Utility functions
			shader::Utils utils{ writer };
			index = 0;
			auto lightingModel = shader::PhongLightingModel::createModel( writer
				, utils
				, LightType::eSpot
				, shader::ShadowOptions{ SceneFlag::eNone, true }
				, index
				, 1u );

			auto convertPointToGridIndex = writer.implementFunction< IVec3 >( "convertPointToGridIndex"
				, [&]( Vec3 pos )
				{
					writer.returnStmt( ivec3( ( pos - c3d_lpvGridData.minVolumeCorner ) / vec3( c3d_lpvGridData.cellSize ) - vec3( 0.5_f ) ) );
				}
				, InVec3{ writer, "pos" } );

			//Sample from camera
			auto calculateSurfelAreaLightViewM = writer.implementFunction< Float >( "calculateSurfelAreaLightViewM"
				, [&]( Vec3 viewPos )
				{
					writer.returnStmt( ( 4.0 * viewPos.z() * viewPos.z() * c3d_lpvLightData.tanFovXHalf * c3d_lpvLightData.tanFovYHalf ) / Float{ float( rsmTexSize * rsmTexSize ) } );
				}
				, InVec3{ writer, "viewPos" } );

			writer.implementFunction< Void >( "main"
				, [&]()
				{
					auto light = writer.declLocale( "light"
						, lightingModel->getSpotLight( c3d_lpvLightData.lightIndex ) );
					auto rsmCoords = writer.declLocale( "rsmCoords"
						, ivec3( in.vertexIndex % rsmTexSize
							, in.vertexIndex / rsmTexSize
							, c3d_lpvLightData.lightIndex ) );

					outLightPos = light.m_position;
					outRsmPos = c3d_rsmPositionMap.fetch( rsmCoords, 0_i ).rgb();
					outRsmNormal = c3d_rsmNormalMap.fetch( rsmCoords, 0_i ).rgb();
					auto viewPos = writer.declLocale( "viewPos"
						, c3d_lpvLightData.lightView * vec4( outRsmPos, 1.0 ) );
					outSurfelArea = calculateSurfelAreaLightViewM( viewPos.xyz() ) * c3d_lpvLightData.texelAreaModifier;

					outVolumeCellIndex = convertPointToGridIndex( outRsmPos );

					auto screenPos = writer.declLocale( "screenPos"
						, ( vec2( outVolumeCellIndex.xy() ) + 0.5_f ) / c3d_lpvGridData.gridSize.xy() * 2.0_f - 1.0_f );

					out.vtx.position = vec4( screenPos, 0.0, 1.0 );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< ast::Shader > getPointVertexProgram( uint32_t rsmTexSize )
		{
			using namespace sdw;
			VertexWriter writer;

			auto inPosition = writer.declInput< Vec2 >( "inPosition", 0u );
			auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights"
				, GeometryInjectionPass::LightsIdx
				, 0u );
			auto c3d_rsmNormalMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( LightType::eSpot, SmTexture::eNormalLinear )
				, GeometryInjectionPass::RsmNormalsIdx
				, 0u );
			auto c3d_rsmPositionMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( LightType::eSpot, SmTexture::ePosition )
				, GeometryInjectionPass::RsmPositionIdx
				, 0u );
			UBO_LPVGRIDCONFIG( writer, GeometryInjectionPass::LpvGridUboIdx, 0u, true );
			UBO_LPVLIGHTCONFIG( writer, GeometryInjectionPass::LpvLightUboIdx, 0u );
			auto in = writer.getIn();

			uint32_t index = 0u;
			auto outVolumeCellIndex = writer.declOutput< IVec3 >( "outVolumeCellIndex", index++ );
			auto outRsmPos = writer.declOutput< Vec3 >( "outRsmPos", index++ );
			auto outRsmNormal = writer.declOutput< Vec3 >( "outRsmNormal", index++ );
			auto outSurfelArea = writer.declOutput< Float >( "outSurfelArea", index++ );
			auto outLightPos = writer.declOutput< Vec3 >( "outLightPos", index++ );
			auto out = writer.getOut();

			// Utility functions
			shader::Utils utils{ writer };
			index = 0;
			auto lightingModel = shader::PhongLightingModel::createModel( writer
				, utils
				, LightType::eSpot
				, shader::ShadowOptions{ SceneFlag::eNone, true }
				, index
				, 1u );

			auto convertPointToGridIndex = writer.implementFunction< IVec3 >( "convertPointToGridIndex"
				, [&]( Vec3 pos )
				{
					writer.returnStmt( ivec3( ( pos - c3d_lpvGridData.minVolumeCorner ) / vec3( c3d_lpvGridData.cellSize ) - vec3( 0.5_f ) ) );
				}
				, InVec3{ writer, "pos" } );

			//Sample from camera
			auto calculateSurfelAreaLightViewM = writer.implementFunction< Float >( "calculateSurfelAreaLightViewM"
				, [&]( Vec3 viewPos )
				{
					writer.returnStmt( ( 4.0 * viewPos.z() * viewPos.z() * c3d_lpvLightData.tanFovXHalf * c3d_lpvLightData.tanFovYHalf ) / Float{ float( rsmTexSize * rsmTexSize ) } );
				}
				, InVec3{ writer, "viewPos" } );

			writer.implementFunction< Void >( "main"
				, [&]()
				{
					auto light = writer.declLocale( "light"
						, lightingModel->getPointLight( c3d_lpvLightData.lightIndex ) );
					auto rsmCoords = writer.declLocale( "rsmCoords"
						, ivec3( in.vertexIndex % rsmTexSize
							, in.vertexIndex / rsmTexSize
							, c3d_lpvLightData.lightIndex ) );

					outLightPos = light.m_position;
					outRsmPos = c3d_rsmPositionMap.fetch( rsmCoords, 0_i ).rgb();
					outRsmNormal = c3d_rsmNormalMap.fetch( rsmCoords, 0_i ).rgb();
					outSurfelArea = calculateSurfelAreaLightViewM( outLightPos ) * c3d_lpvLightData.texelAreaModifier;

					outVolumeCellIndex = convertPointToGridIndex( outRsmPos );

					auto screenPos = writer.declLocale( "screenPos"
						, ( vec2( outVolumeCellIndex.xy() ) + 0.5_f ) / c3d_lpvGridData.gridSize.xy() * 2.0_f - 1.0_f );

					out.vtx.position = vec4( screenPos, 0.0, 1.0 );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr getVertexProgram( LightType lightType
			, uint32_t rsmTexSize
			, uint32_t layerIndex )
		{
			switch ( lightType )
			{
			case castor3d::LightType::eDirectional:
				return getDirectionalVertexProgram( rsmTexSize, layerIndex );
			case castor3d::LightType::eSpot:
				return getSpotVertexProgram( rsmTexSize );
			case castor3d::LightType::ePoint:
				return getPointVertexProgram( rsmTexSize );
			default:
				CU_Failure( "Unsupported light type" );
				return nullptr;
			}
		}

		std::unique_ptr< ast::Shader > getGeometryProgram()
		{
			using namespace sdw;
			GeometryWriter writer;
			writer.inputLayout( ast::stmt::InputLayout::ePointList );
			writer.outputLayout( ast::stmt::OutputLayout::ePointList, 1u );

			uint32_t index = 0u;
			auto inVolumeCellIndex = writer.declInputArray< IVec3 >( "inVolumeCellIndex", index++, 1u );
			auto inRsmPos = writer.declInputArray< Vec3 >( "inRsmPos", index++, 1u );
			auto inRsmNormal = writer.declInputArray< Vec3 >( "inRsmNormal", index++, 1u );
			auto inSurfelArea = writer.declInputArray< Float >( "inSurfelArea", index++, 1u );
			auto inLightPos = writer.declInputArray< Vec3 >( "inLightPos", index++, 1u );
			auto in = writer.getIn();

			index = 0u;
			auto outVolumeCellIndex = writer.declOutput< IVec3 >( "outVolumeCellIndex", index++ );
			auto outRsmPos = writer.declOutput< Vec3 >( "outRsmPos", index++ );
			auto outRsmNormal = writer.declOutput< Vec3 >( "outRsmNormal", index++ );
			auto outSurfelArea = writer.declOutput< Float >( "outSurfelArea", index++ );
			auto outLightPos = writer.declOutput< Vec3 >( "outLightPos", index++ );
			auto out = writer.getOut();

			writer.implementFunction< Void >( "main"
				, [&]()
				{
					out.vtx.position = in.vtx[0].position;
					out.layer = inVolumeCellIndex[0].z();
					out.vtx.pointSize = 1.0f;

					outVolumeCellIndex = inVolumeCellIndex[0];
					outRsmPos = inRsmPos[0];
					outRsmNormal = inRsmNormal[0];
					outSurfelArea = inSurfelArea[0];
					outLightPos = inLightPos[0];

					EmitVertex( writer );
					EndPrimitive( writer );
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr getPixelProgram( LightType lightType )
		{
			using namespace sdw;
			FragmentWriter writer;

			/*Cosine lobe coeff*/
			auto SH_cosLobe_C0 = writer.declConstant( "SH_cosLobe_C0"
				, 0.886226925_f );// sqrt(pi) / 2 
			auto SH_cosLobe_C1 = writer.declConstant( "SH_cosLobe_C1"
				, 1.02332671_f ); // sqrt(pi / 3)

			// SH_C0 * SH_cosLobe_C0 = 0.25000000007f
			// SH_C1 * SH_cosLobe_C1 = 0.5000000011f

			//layout( early_fragment_tests )in;//turn on early depth tests

			UBO_LPVGRIDCONFIG( writer, GeometryInjectionPass::LpvGridUboIdx, 0u, true );

			uint32_t index = 0u;
			auto inVolumeCellIndex = writer.declInput< IVec3 >( "inVolumeCellIndex", index++ );
			auto inRsmPos = writer.declInput< Vec3 >( "inRsmPos", index++ );
			auto inRsmNormal = writer.declInput< Vec3 >( "inRsmNormal", index++ );
			auto inSurfelArea = writer.declInput< Float >( "inSurfelArea", index++ );
			auto inLightPos = writer.declInput< Vec3 >( "inLightPos", index++ );
			auto in = writer.getIn();

			auto outGeometryVolume = writer.declOutput< Vec4 >( "outGeometryVolume", 0u );

			//Should I normalize the dir vector?
			auto evalCosineLobeToDir = writer.implementFunction< Vec4 >( "evalCosineLobeToDir"
				, [&]( Vec3 dir )
				{
					dir = normalize( dir );
					//f00, f-11, f01, f11
					writer.returnStmt( vec4( SH_cosLobe_C0
						, -SH_cosLobe_C1 * dir.y()
						, SH_cosLobe_C1 * dir.z()
						, -SH_cosLobe_C1 * dir.x() ) );
				}
				, InVec3{ writer, "dir" } );

			//(As * clamp(dot(ns,w),0.0,1.0))/(cellsize * cellsize)
			auto calculateBlockingPotential = writer.implementFunction< Float >( "calculateBlockingPotential"
				, [&]( Vec3 dir
					, Vec3 normal )
				{
					writer.returnStmt( clamp( ( inSurfelArea * clamp( dot( normal, dir ), 0.0_f, 1.0_f ) ) / ( c3d_lpvGridData.cellSize * c3d_lpvGridData.cellSize )
						, 0.0_f
						, 1.0_f ) ); //It is probability so 0.0 - 1.0
				}
				, InVec3{ writer, "dir" }
				, InVec3{ writer, "normal" } );
			
			writer.implementFunction< Void >( "main"
				, [&]()
				{
					//Discard pixels with really small normal
					IF( writer, length( inRsmNormal ) < 0.01_f )
					{
						writer.discard();
					}
					FI;

					auto lightDir = writer.declLocale( "lightDir"
						, normalize( inLightPos - inRsmPos ) );
					auto blockingPotential = writer.declLocale( "blockingPotential"
						, calculateBlockingPotential( lightDir, inRsmNormal ) );

					auto SHCoeffGV = writer.declLocale( "SHCoeffGV"
						, evalCosineLobeToDir( inRsmNormal ) * blockingPotential );

					outGeometryVolume = SHCoeffGV;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		SamplerSPtr createSampler( Engine & engine
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

		ashes::VertexBufferPtr< NonTexturedQuad::Vertex > createVertexBuffer( castor::String const & name
			, RenderDevice const & device
			, uint32_t rsmSize )
		{
			auto vplCount = rsmSize * rsmSize;

			auto result = makeVertexBuffer< NonTexturedQuad::Vertex >( device
				, vplCount
				, 0u
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, name );
			NonTexturedQuad::Vertex vtx;

			if ( auto buffer = result->lock( 0u, vplCount, 0u ) )
			{
				for ( auto i = 0u; i < vplCount; ++i )
				{
					*buffer = vtx;
					++buffer;
				}

				result->flush( 0u, vplCount );
				result->unlock();
			}

			return result;
		}

		crg::pp::Config getConfig( ashes::PipelineShaderStageCreateInfoArray const & stages )
		{
			crg::pp::Config result;
			result.program = crg::makeVkArray< VkPipelineShaderStageCreateInfo >( stages );
			return result;
		}
	}

	//*********************************************************************************************

	GeometryInjectionPass::PipelineHolder::PipelineHolder( crg::FramePass const & pass
		, crg::GraphContext const & context
		, crg::RunnableGraph & graph
		, crg::pp::Config config
		, uint32_t lpvSize )
		: crg::PipelineHolder{ pass, context, graph, std::move( config ), VK_PIPELINE_BIND_POINT_GRAPHICS }
		, m_lpvSize{ lpvSize }
	{
		m_descriptorSets.resize( 1u );
	}

	void GeometryInjectionPass::PipelineHolder::initialise( VkRenderPass renderPass )
	{
		m_renderPass = renderPass;
		doPreInitialise();
	}

	void GeometryInjectionPass::PipelineHolder::recordInto( VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		doPreRecordInto( commandBuffer, index );
	}

	void GeometryInjectionPass::PipelineHolder::doCreatePipeline()
	{
		ashes::PipelineVertexInputStateCreateInfo vertexState{ 0u
			, ashes::VkVertexInputBindingDescriptionArray{ { 0u
				, sizeof( NonTexturedQuad::Vertex )
				, VK_VERTEX_INPUT_RATE_VERTEX } }
			, ashes::VkVertexInputAttributeDescriptionArray{ { 0u
				, 0u
				, VK_FORMAT_R32G32_SFLOAT
				, offsetof( NonTexturedQuad::Vertex, position ) } } };
		VkViewport viewport{ 0.0f, 0.0f, float( m_lpvSize ), float( m_lpvSize ) };
		VkRect2D scissor{ 0, 0, m_lpvSize, m_lpvSize };
		ashes::PipelineViewportStateCreateInfo viewportState{ 0u
			, 1u
			, ashes::VkViewportArray{ viewport }
			, 1u
			, ashes::VkScissorArray{ scissor } };
		auto blendState = SceneRenderPass::createBlendState( BlendMode::eNoBlend, BlendMode::eNoBlend, 1u );
		VkPipelineInputAssemblyStateCreateInfo iaState{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO
			, nullptr
			, 0u
			, VK_PRIMITIVE_TOPOLOGY_POINT_LIST
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
		VkPipelineDepthStencilStateCreateInfo dsState{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO
			, nullptr
			, 0u
			, VK_FALSE
			, VK_FALSE };
		VkPipelineViewportStateCreateInfo vpState = viewportState;
		VkPipelineVertexInputStateCreateInfo viState = vertexState;
		VkPipelineColorBlendStateCreateInfo cbState = blendState;
		VkGraphicsPipelineCreateInfo createInfo{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO
			, nullptr
			, 0u
			, uint32_t( m_baseConfig.program.size() )
			, m_baseConfig.program.data()
			, &viState
			, &iaState
			, nullptr
			, &vpState
			, &rsState
			, &msState
			, &dsState
			, &cbState
			, nullptr
			, m_pipelineLayout
			, m_renderPass
			, 0u
			, VK_NULL_HANDLE
			, 0u };
		auto res = m_phContext.vkCreateGraphicsPipelines( m_phContext.device
			, m_phContext.cache
			, 1u
			, &createInfo
			, m_phContext.allocator
			, &m_pipeline );
		crg::checkVkResult( res, m_phPass.name + " - Pipeline creation" );
		crgRegisterObject( m_phContext, m_phPass.name, m_pipeline );
	}

	//*********************************************************************************************

	GeometryInjectionPass::GeometryInjectionPass( crg::FramePass const & pass
		, crg::GraphContext const & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, LightType lightType
		, uint32_t gridSize
		, uint32_t layerIndex
		, uint32_t rsmSize )
		: Named{ pass.name }
		, crg::RenderPass{ pass
			, context
			, graph
			, { gridSize, gridSize }
			, 1u }
		, m_device{ device }
		, m_rsmSize{ rsmSize }
		, m_vertexBuffer{ createVertexBuffer( getName(), m_device, m_rsmSize ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, getName(), getVertexProgram( lightType, m_rsmSize, layerIndex ) }
		, m_geometryShader{ VK_SHADER_STAGE_GEOMETRY_BIT, getName(), getGeometryProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, getName(), getPixelProgram( lightType ) }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_geometryShader )
			, makeShaderState( device, m_pixelShader ) }
		, m_holder{ pass
			, context
			, graph
			, getConfig( m_stages )
			, gridSize }
	{
	}

	void GeometryInjectionPass::doSubInitialise()
	{
		m_holder.initialise( m_renderPass );
	}

	void GeometryInjectionPass::doSubRecordInto( VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		m_holder.recordInto( commandBuffer, index );
		auto vplCount = m_rsmSize * m_rsmSize;
		VkDeviceSize offset{};
		VkBuffer vertexBuffer = m_vertexBuffer->getBuffer();
		m_context.vkCmdBindVertexBuffers( commandBuffer, 0u, 1u, &vertexBuffer, &offset );
		m_context.vkCmdDraw( commandBuffer, vplCount, 1u, 0u, 0u );
	}

	void GeometryInjectionPass::accept( PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_geometryShader );
		visitor.visit( m_pixelShader );
	}

	Texture GeometryInjectionPass::createResult( crg::ResourceHandler & handler
		, RenderDevice const & device
		, castor::String const & prefix
		, uint32_t index
		, uint32_t gridSize )
	{
		return Texture{ device
			, handler
			, prefix + cuT( "GeometryInjection" ) + string::toString( index )
			, VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT
			, { gridSize, gridSize, gridSize }
			, 1u
			, 1u
			, VK_FORMAT_R16G16B16A16_SFLOAT
			, ( VK_IMAGE_USAGE_TRANSFER_DST_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT )
			, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK
			, false };
	}
}
