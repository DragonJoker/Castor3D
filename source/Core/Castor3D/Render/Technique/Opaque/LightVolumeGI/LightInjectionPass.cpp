#include "Castor3D/Render/Technique/Opaque/LightVolumeGI/LightInjectionPass.hpp"

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
#include "Castor3D/Shader/Ubos/LpvConfigUbo.hpp"

#include <CastorUtils/Graphics/Image.hpp>

#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineViewportStateCreateInfo.hpp>
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
		enum Idx : uint32_t
		{
			LightsIdx,
			RsmNormalsIdx,
			RsmPositionIdx,
			RsmFluxIdx,
			LIUboIdx,
			GpUboIdx,
		};

		std::unique_ptr< ast::Shader > getDirectionalVertexProgram( uint32_t rsmTexSize
			, uint32_t layerIndex )
		{
			using namespace sdw;
			VertexWriter writer;

			auto inPosition = writer.declInput< Vec2 >( "inPosition", 0u );
			auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights"
				, LightsIdx
				, 0u );
			auto c3d_rsmNormalMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( LightType::eDirectional, SmTexture::eNormalLinear )
				, RsmNormalsIdx
				, 0u );
			auto c3d_rsmPositionMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( LightType::eDirectional, SmTexture::ePosition )
				, RsmPositionIdx
				, 0u );
			auto c3d_rsmFluxMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( LightType::eDirectional, SmTexture::eFlux )
				, RsmFluxIdx
				, 0u );
			UBO_LPVCONFIG( writer, LIUboIdx, 0u );
			UBO_GPINFO( writer, GpUboIdx, 0u );
			auto in = writer.getIn();

			uint32_t index = 0u;
			auto outVolumeCellIndex = writer.declOutput< IVec3 >( "outVolumeCellIndex", index++ );
			auto outRsmPos = writer.declOutput< Vec3 >( "outRsmPos", index++ );
			auto outRsmNormal = writer.declOutput< Vec3 >( "outRsmNormal", index++ );
			auto outRsmFlux = writer.declOutput< Vec4 >( "outRsmFlux", index++ );
			auto out = writer.getOut();

			// Utility functions
			shader::Utils utils{ writer };
			index = 0;
			auto lightingModel = shader::PhongLightingModel::createModel( writer
				, utils
				, LightType::eDirectional
				, false // lightUbo
				, false // shadows
				, true // rsm
				, index );

			auto convertPointToGridIndex = writer.implementFunction< IVec3 >( "convertPointToGridIndex"
				, [&]( Vec3 pos
					, Vec3 normal )
				{
					writer.returnStmt( ivec3( ( pos - c3d_minVolumeCorner ) / vec3( c3d_cellSize ) + 0.5_f * normal ) );
				}
				, InVec3{ writer, "pos" }
				, InVec3{ writer, "normal" } );

			writer.implementFunction< Void >( "main"
				, [&]()
				{
					auto light = writer.declLocale( "light"
						, lightingModel->getDirectionalLight( 0_i ) );
					auto cascadeIndex = writer.declLocale( "cascadeIndex"
						, min( UInt{ layerIndex }, max( 1_u, light.m_cascadeCount ) - 1_u ) );
					auto rsmCoords = writer.declLocale( "rsmCoords"
						, ivec3( in.vertexIndex % rsmTexSize
							, in.vertexIndex / rsmTexSize
							, cascadeIndex ) );

					outRsmPos = c3d_rsmPositionMap.fetch( rsmCoords, 0_i ).rgb();
					outRsmNormal = c3d_rsmNormalMap.fetch( rsmCoords, 0_i ).rgb();
					outRsmFlux = c3d_rsmFluxMap.fetch( rsmCoords, 0_i );
					outVolumeCellIndex = convertPointToGridIndex( outRsmPos
						, outRsmNormal );

					auto screenPos = writer.declLocale( "screenPos"
						, ( vec2( outVolumeCellIndex.xy() ) + 0.5_f ) / c3d_gridSizes.xy() * 2.0_f - 1.0_f );

					out.vtx.position = vec4( screenPos, 0.0, 1.0 );
					out.vtx.pointSize = 1.0f;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< ast::Shader > getSpotVertexProgram( uint32_t rsmTexSize )
		{
			using namespace sdw;
			VertexWriter writer;

			auto inPosition = writer.declInput< Vec2 >( "inPosition", 0u );
			auto c3d_sLights = writer.declSampledImage< FImgBufferRgba32 >( "c3d_sLights"
				, LightsIdx
				, 0u );
			auto c3d_rsmNormalMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( LightType::eSpot, SmTexture::eNormalLinear )
				, RsmNormalsIdx
				, 0u );
			auto c3d_rsmPositionMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( LightType::eSpot, SmTexture::ePosition )
				, RsmPositionIdx
				, 0u );
			auto c3d_rsmFluxMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( LightType::eSpot, SmTexture::eFlux )
				, RsmFluxIdx
				, 0u );
			UBO_LPVCONFIG( writer, LIUboIdx, 0u );
			UBO_GPINFO( writer, GpUboIdx, 0u );
			auto in = writer.getIn();

			uint32_t index = 0u;
			auto outVolumeCellIndex = writer.declOutput< IVec3 >( "outVolumeCellIndex", index++ );
			auto outRsmPos = writer.declOutput< Vec3 >( "outRsmPos", index++ );
			auto outRsmNormal = writer.declOutput< Vec3 >( "outRsmNormal", index++ );
			auto outRsmFlux = writer.declOutput< Vec4 >( "outRsmFlux", index++ );
			auto out = writer.getOut();

			auto convertPointToGridIndex = writer.implementFunction< IVec3 >( "convertPointToGridIndex"
				, [&]( Vec3 pos
					, Vec3 normal )
				{
					writer.returnStmt( ivec3( ( pos - c3d_minVolumeCorner ) / vec3( c3d_cellSize ) + 0.5_f * normal ) );
				}
				, InVec3{ writer, "pos" }
				, InVec3{ writer, "normal" } );

			writer.implementFunction< Void >( "main"
				, [&]()
				{
					auto rsmCoords = writer.declLocale( "rsmCoords"
						, ivec3( in.vertexIndex % rsmTexSize
							, in.vertexIndex / rsmTexSize
							, c3d_lightIndex ) );

					outRsmPos = c3d_rsmPositionMap.fetch( rsmCoords, 0_i ).rgb();
					outRsmNormal = c3d_rsmNormalMap.fetch( rsmCoords, 0_i ).rgb();
					outRsmFlux = c3d_rsmFluxMap.fetch( rsmCoords, 0_i );
					outVolumeCellIndex = convertPointToGridIndex( outRsmPos, outRsmNormal );

					auto screenPos = writer.declLocale( "screenPos"
						, ( vec2( outVolumeCellIndex.xy() ) + 0.5_f ) / c3d_gridSizes.xy() * 2.0_f - 1.0_f );
					out.vtx.position = vec4( screenPos, 0.0_f, 1.0_f );
					out.vtx.pointSize = 1.0f;
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
			auto inRsmFlux = writer.declInputArray< Vec4 >( "inRsmFlux", index++, 1u );
			auto in = writer.getIn();

			index = 0u;
			auto outVolumeCellIndex = writer.declOutput< IVec3 >( "outVolumeCellIndex", index++ );
			auto outRsmPos = writer.declOutput< Vec3 >( "outRsmPos", index++ );
			auto outRsmNormal = writer.declOutput< Vec3 >( "outRsmNormal", index++ );
			auto outRsmFlux = writer.declOutput< Vec4 >( "outRsmFlux", index++ );
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
					outRsmFlux = inRsmFlux[0];

					EmitVertex( writer );
					EndPrimitive( writer );
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr getPixelProgram()
		{
			using namespace sdw;
			FragmentWriter writer;

			/*Cosine lobe coeff*/
			auto SH_cosLobe_C0 = writer.declConstant( "SH_cosLobe_C0"
				, Float{ sqrt( castor::Pi< float > ) / 2.0f } );
			auto SH_cosLobe_C1 = writer.declConstant( "SH_cosLobe_C1"
				, Float{ sqrt( castor::Pi< float > ) / 3.0f } );

			// SH_C0 * SH_cosLobe_C0 = 0.25000000007f
			// SH_C1 * SH_cosLobe_C1 = 0.5000000011f

			auto outLpvGridR = writer.declOutput< Vec4 >( "outLpvGridR", 0u );
			auto outLpvGridG = writer.declOutput< Vec4 >( "outLpvGridG", 1u );
			auto outLpvGridB = writer.declOutput< Vec4 >( "outLpvGridB", 2u );

			//layout( early_fragment_tests )in;//turn on early depth tests

			UBO_LPVCONFIG( writer, LIUboIdx, 0u );

			uint32_t index = 0u;
			auto inVolumeCellIndex = writer.declInput< IVec3 >( "inVolumeCellIndex", index++ );
			auto inRsmPos = writer.declInput< Vec3 >( "inRsmPos", index++ );
			auto inRsmNormal = writer.declInput< Vec3 >( "inRsmNormal", index++ );
			auto inRsmFlux = writer.declInput< Vec4 >( "inRsmFlux", index++ );
			auto in = writer.getIn();

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

			writer.implementFunction< Void >( "main"
				, [&]()
				{
					auto SHCoeffsR = writer.declLocale( "SHCoeffsR"
						, evalCosineLobeToDir( inRsmNormal ) / Float{ Pi< float > } *inRsmFlux.r() );
					auto SHCoeffsG = writer.declLocale( "SHCoeffsG"
						, evalCosineLobeToDir( inRsmNormal ) / Float{ Pi< float > } *inRsmFlux.g() );
					auto SHCoeffsB = writer.declLocale( "SHCoeffsB"
						, evalCosineLobeToDir( inRsmNormal ) / Float{ Pi< float > } *inRsmFlux.b() );

					outLpvGridR = SHCoeffsR;
					outLpvGridG = SHCoeffsG;
					outLpvGridB = SHCoeffsB;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ashes::RenderPassPtr doCreateRenderPass( castor::String const & name
			, RenderDevice const & device
			, VkFormat format )
		{
			ashes::VkAttachmentDescriptionArray attaches
			{
				{
					0u,
					format,
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				},
				{
					0u,
					format,
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				},
				{
					0u,
					format,
					VK_SAMPLE_COUNT_1_BIT,
					VK_ATTACHMENT_LOAD_OP_CLEAR,
					VK_ATTACHMENT_STORE_OP_STORE,
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					VK_ATTACHMENT_STORE_OP_DONT_CARE,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				},
			};
			ashes::SubpassDescriptionArray subpasses;
			subpasses.emplace_back( ashes::SubpassDescription
				{
					0u,
					VK_PIPELINE_BIND_POINT_GRAPHICS,
					{},
					{
						{ 0u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
						{ 1u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
						{ 2u, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
					},
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
			auto result = device->createRenderPass( name
				, std::move( createInfo ) );
			return result;
		}

		ashes::VertexBufferPtr< NonTexturedQuad::Vertex > doCreateVertexBuffer( castor::String const & name
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

		ashes::DescriptorSetLayoutPtr doCreateDescriptorLayout( castor::String const & name
			, RenderDevice const & device )
		{
			ashes::VkDescriptorSetLayoutBindingArray bindings
			{
				makeDescriptorSetLayoutBinding( LightsIdx
					, VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER
					, VK_SHADER_STAGE_VERTEX_BIT ),
				makeDescriptorSetLayoutBinding( RsmNormalsIdx
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_VERTEX_BIT ),
				makeDescriptorSetLayoutBinding( RsmPositionIdx
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_VERTEX_BIT ),
				makeDescriptorSetLayoutBinding( RsmFluxIdx
					, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
					, VK_SHADER_STAGE_VERTEX_BIT ),
				makeDescriptorSetLayoutBinding( LIUboIdx
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT ),
				makeDescriptorSetLayoutBinding( GpUboIdx
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_VERTEX_BIT ),
			};
			return device->createDescriptorSetLayout( name
				, std::move( bindings ) );
		}

		ashes::DescriptorSetPtr doCreateDescriptorSet( castor::String const & name
			, ashes::DescriptorSetPool & descriptorSetPool
			, LightCache const & lightCache
			, ShadowMapResult const & smResult
			, UniformBufferOffsetT< LpvConfigUboConfiguration > const & ubo
			, GpInfoUbo const & gpInfoUbo
			, LightVolumePassResult const & lpvResult )
		{
			auto & descriptorSetLayout = descriptorSetPool.getLayout();
			auto result = descriptorSetPool.createDescriptorSet( name );
			result->createBinding( descriptorSetLayout.getBinding( LightsIdx )
				, lightCache.getBuffer()
				, lightCache.getView() );
			result->createBinding( descriptorSetLayout.getBinding( RsmNormalsIdx )
				, smResult[SmTexture::eNormalLinear].getTexture()->getDefaultView().getSampledView()
				, smResult[SmTexture::eNormalLinear].getSampler()->getSampler() );
			result->createBinding( descriptorSetLayout.getBinding( RsmPositionIdx )
				, smResult[SmTexture::ePosition].getTexture()->getDefaultView().getSampledView()
				, smResult[SmTexture::ePosition].getSampler()->getSampler() );
			result->createBinding( descriptorSetLayout.getBinding( RsmFluxIdx )
				, smResult[SmTexture::eFlux].getTexture()->getDefaultView().getSampledView()
				, smResult[SmTexture::eFlux].getSampler()->getSampler() );
			ubo.createSizedBinding( *result
				, descriptorSetLayout.getBinding( LIUboIdx ) );
			gpInfoUbo.createSizedBinding( *result
				, descriptorSetLayout.getBinding( GpUboIdx ) );
			result->update();
			return result;
		}

		ashes::GraphicsPipelinePtr doCreatePipeline( castor::String const & name
			, RenderDevice const & device
			, ashes::PipelineLayout const & pipelineLayout
			, ashes::RenderPass const & renderPass
			, ShaderModule const & vertexShader
			, ShaderModule const & geometryShader
			, ShaderModule const & pixelShader
			, uint32_t lpvSize )
		{
			ashes::VkVertexInputAttributeDescriptionArray attributes
			{
				{ 0u, 0u, VK_FORMAT_R32G32_SFLOAT, offsetof( NonTexturedQuad::Vertex, position ) },
			};
			ashes::PipelineVertexInputStateCreateInfo vertexState
			{
				0u,
				ashes::VkVertexInputBindingDescriptionArray
				{
					{ 0u, sizeof( NonTexturedQuad::Vertex ), VK_VERTEX_INPUT_RATE_VERTEX },
				},
				std::move( attributes ),
			};
			VkViewport viewport{ 0.0f, 0.0f, float( lpvSize ), float( lpvSize ) };
			VkRect2D scissor{ 0, 0, lpvSize, lpvSize };
			ashes::PipelineViewportStateCreateInfo vpState
			{
				0u,
				1u,
				ashes::VkViewportArray{ viewport },
				1u,
				ashes::VkScissorArray{ scissor },
			};
			ashes::PipelineShaderStageCreateInfoArray shaderStages;
			shaderStages.push_back( makeShaderState( device, vertexShader ) );
			shaderStages.push_back( makeShaderState( device, geometryShader ) );
			shaderStages.push_back( makeShaderState( device, pixelShader ) );
			return device->createPipeline( name
				, ashes::GraphicsPipelineCreateInfo
				{
					0u,
					shaderStages,
					std::move( vertexState ),
					ashes::PipelineInputAssemblyStateCreateInfo{ 0u, VK_PRIMITIVE_TOPOLOGY_POINT_LIST },
					ashes::nullopt,
					std::move( vpState ),
					ashes::PipelineRasterizationStateCreateInfo{ 0u, VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE },
					ashes::PipelineMultisampleStateCreateInfo{},
					ashes::PipelineDepthStencilStateCreateInfo{ 0u, VK_FALSE, VK_FALSE },
					RenderPass::createBlendState( BlendMode::eNoBlend, BlendMode::eNoBlend, 3u ),
					ashes::nullopt,
					pipelineLayout,
					renderPass,
				} );
		}
	}

	//*********************************************************************************************

	LightInjectionPass::LightInjectionPass( Engine & engine
		, RenderDevice const & device
		, castor::String const & prefix
		, LightCache const & lightCache
		, LightType lightType
		, ShadowMapResult const & smResult
		, GpInfoUbo const & gpInfoUbo
		, LpvConfigUbo const & lpvConfigUbo
		, LightVolumePassResult const & result
		, uint32_t gridSize
		, uint32_t layerIndex )
		: Named{ prefix + "LightInjection" + std::to_string( layerIndex ) }
		, m_engine{ engine }
		, m_device{ device }
		, m_lightCache{ lightCache }
		, m_smResult{ smResult }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_lpvConfigUbo{ lpvConfigUbo }
		, m_lightType{ lightType }
		, m_result{ result }
		, m_timer{ std::make_shared< RenderPassTimer >( engine, m_device, cuT( "Light Propagation Volumes" ), cuT( " Light Injection" ) ) }
		, m_vertexBuffer{ doCreateVertexBuffer( getName(), m_device, m_smResult[SmTexture::eDepth].getTexture()->getWidth() ) }
		, m_descriptorSetLayout{ doCreateDescriptorLayout( getName(), m_device ) }
		, m_pipelineLayout{ m_device->createPipelineLayout( getName(), *m_descriptorSetLayout ) }
		, m_descriptorSetPool{ m_descriptorSetLayout->createPool( getName(), 1u ) }
		, m_descriptorSet{ doCreateDescriptorSet( getName()
			, *m_descriptorSetPool
			, m_lightCache
			, m_smResult
			, m_lpvConfigUbo.getUbo()
			, m_gpInfoUbo
			, result ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, getName(), getVertexProgram( lightType, smResult[SmTexture::eDepth].getTexture()->getWidth(), layerIndex ) }
		, m_geometryShader{ VK_SHADER_STAGE_GEOMETRY_BIT, getName(), getGeometryProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, getName(), getPixelProgram() }
		, m_renderPass{ doCreateRenderPass( getName()
			, m_device
			, getFormat( LpvTexture::eR ) ) }
		, m_pipeline{ doCreatePipeline( getName()
			, m_device
			, *m_pipelineLayout
			, *m_renderPass
			, m_vertexShader
			, m_geometryShader
			, m_pixelShader
			, gridSize ) }
		, m_frameBuffer{ m_renderPass->createFrameBuffer( getName()
			, VkExtent2D{ gridSize, gridSize }
			, {
				result[LpvTexture::eR].getTexture()->getDefaultView().getTargetView(),
				result[LpvTexture::eG].getTexture()->getDefaultView().getTargetView(),
				result[LpvTexture::eB].getTexture()->getDefaultView().getTargetView(),
			}
			, gridSize ) }
		, m_commands{ getCommands( *m_timer, 0u ) }
	{
	}

	ashes::Semaphore const & LightInjectionPass::compute( ashes::Semaphore const & toWait )const
	{
		RenderPassTimerBlock timerBlock{ m_timer->start() };
		timerBlock->notifyPassRender();
		return m_commands.submit( *m_device.graphicsQueue, toWait );
	}

	CommandsSemaphore LightInjectionPass::getCommands( RenderPassTimer const & timer
		, uint32_t index )const
	{
		castor3d::CommandsSemaphore commands{ m_device, getName() };
		auto & cmd = *commands.commandBuffer;
		auto rsmSize = m_smResult[SmTexture::eDepth].getTexture()->getWidth();
		auto vplCount = rsmSize * rsmSize;

		cmd.begin( VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT );
		timer.beginPass( cmd, index );
		cmd.beginDebugBlock(
			{
				"Lighting - " + getName(),
				castor3d::makeFloatArray( m_engine.getNextRainbowColour() ),
			} );
		cmd.beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, {
				getClearValue( LpvTexture::eR ),
				getClearValue( LpvTexture::eG ),
				getClearValue( LpvTexture::eB )
			}
			, VK_SUBPASS_CONTENTS_INLINE );
		cmd.bindPipeline( *m_pipeline );
		cmd.bindVertexBuffer( 0u, m_vertexBuffer->getBuffer(), 0u );
		cmd.bindDescriptorSet( *m_descriptorSet, *m_pipelineLayout );
		cmd.draw( vplCount );
		cmd.endRenderPass();
		cmd.endDebugBlock();
		timer.endPass( cmd, index );
		cmd.end();

		return commands;
	}

	void LightInjectionPass::accept( PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_geometryShader );
		visitor.visit( m_pixelShader );
	}
}
