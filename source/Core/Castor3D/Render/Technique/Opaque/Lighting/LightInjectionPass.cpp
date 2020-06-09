#include "Castor3D/Render/Technique/Opaque/Lighting/LightInjectionPass.hpp"

#include "Castor3D/Engine.hpp"
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

#include <CastorUtils/Graphics/Image.hpp>

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
		static constexpr uint32_t GridSize = 128u;
		static constexpr float CellSize = 2.5f;

		static constexpr uint32_t LightsIdx = 0u;
		static constexpr uint32_t RsmNormalsIdx = 1u;
		static constexpr uint32_t RsmPositionIdx = 2u;
		static constexpr uint32_t RsmFluxIdx = 3u;
		static constexpr uint32_t LIUboIdx = 4u;
		static constexpr uint32_t GpUboIdx = 5u;

		std::unique_ptr< ast::Shader > getDirectionalVertexProgram( uint32_t rsmTexSize
			, uint32_t lpvTexSize
			, uint32_t cellSize )
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
			Ubo ubo{ writer, "LightInjectionUbo", LIUboIdx, 0u };
			auto c3d_minVolumeCorner = ubo.declMember< Vec4 >( "c3d_minVolumeCorner" );
			auto c3d_gridSizes = ubo.declMember< UVec4 >( "c3d_gridSizes" );
			ubo.end();
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
				, ShadowType::eNone
				, false // lightUbo
				, false // volumetric
				, true // rsm
				, index );

			auto convertPointToGridIndex = writer.implementFunction< IVec3 >( "convertPointToGridIndex"
				, [&]( Vec3 pos
					, Vec3 normal
					, Float cellScale )
				{
					auto cellSize = writer.declLocale( "cellSize"
						, vec3( c3d_minVolumeCorner.w() * cellScale ) );
					writer.returnStmt( ivec3( ( pos - c3d_minVolumeCorner.xyz() ) / cellSize + 0.5_f * normal ) );
				}
				, InVec3{ writer, "pos" }
				, InVec3{ writer, "normal" }
				, InFloat{ writer, "cellScale" } );

			writer.implementFunction< Void >( "main"
				, [&]()
				{
					auto light = writer.declLocale( "light"
						, lightingModel->getDirectionalLight( 0_i ) );
					auto cascadeIndex = writer.declLocale( "cascadeIndex"
						, max( light.m_cascadeCount, 1_u ) - 1_u );
					auto rsmCoords = writer.declLocale( "rsmCoords"
						, ivec3( in.vertexIndex % rsmTexSize
							, in.vertexIndex / rsmTexSize
							, cascadeIndex ) );

					outRsmPos = texelFetch( c3d_rsmPositionMap, rsmCoords, 0_i ).rgb();
					outRsmNormal = texelFetch( c3d_rsmNormalMap, rsmCoords, 0_i ).rgb();
					outRsmFlux = texelFetch( c3d_rsmFluxMap, rsmCoords, 0_i );
					outVolumeCellIndex = convertPointToGridIndex( outRsmPos
						, outRsmNormal
						, light.m_splitScales[cascadeIndex] );

					auto screenPos = writer.declLocale( "screenPos"
						, ( vec2( outVolumeCellIndex.xy() ) + 0.5_f ) / vec2( c3d_gridSizes.xy() ) * 2.0_f - 1.0_f );

					out.vtx.position = vec4( screenPos, 0.0, 1.0 );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< ast::Shader > getSpotVertexProgram( uint32_t rsmTexSize
			, uint32_t lpvTexSize
			, uint32_t cellSize )
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
			Ubo ubo{ writer, "LightInjectionUbo", LIUboIdx, 0u };
			auto c3d_minVolumeCorner = ubo.declMember< Vec4 >( "c3d_minVolumeCorner" );
			auto c3d_gridSizes = ubo.declMember< UVec4 >( "c3d_gridSizes" );
			ubo.end();
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
					writer.returnStmt( ivec3( ( pos - c3d_minVolumeCorner.xyz() ) / vec3( c3d_minVolumeCorner.w() ) + 0.5_f * normal ) );
				}
				, InVec3{ writer, "pos" }
				, InVec3{ writer, "normal" } );

			auto gridSize = float( lpvTexSize );

			writer.implementFunction< Void >( "main"
				, [&]()
				{
					auto rsmCoords = writer.declLocale( "rsmCoords"
						, ivec3( in.vertexIndex % rsmTexSize
							, in.vertexIndex / rsmTexSize
							, writer.cast< Int >( c3d_gridSizes.w() ) ) );

					outRsmPos = texelFetch( c3d_rsmPositionMap, rsmCoords, 0_i ).rgb();
					outRsmNormal = texelFetch( c3d_rsmNormalMap, rsmCoords, 0_i ).rgb();
					outRsmFlux = texelFetch( c3d_rsmFluxMap, rsmCoords, 0_i );
					outVolumeCellIndex = convertPointToGridIndex( outRsmPos, outRsmNormal );

					auto screenPos = writer.declLocale( "screenPos"
						, ( vec2( outVolumeCellIndex.xy() ) + 0.5_f ) / vec2( c3d_gridSizes.xy() ) * 2.0_f - 1.0_f );

					out.vtx.position = vec4( screenPos, 0.0, 1.0 );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr getVertexProgram( LightType lightType
			, uint32_t rsmTexSize
			, uint32_t lpvTexSize
			, uint32_t cellSize )
		{
			switch ( lightType )
			{
			case castor3d::LightType::eDirectional:
				return getDirectionalVertexProgram( rsmTexSize, lpvTexSize, cellSize );
			case castor3d::LightType::eSpot:
				return getSpotVertexProgram( rsmTexSize, lpvTexSize, cellSize );
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

			/*Spherical harmonics coefficients - precomputed*/
			auto SH_C0 = writer.declConstant( "SH_C0"
				, 0.282094792_f );// 1 / (2 * sqrt(pi))
			auto SH_C1 = writer.declConstant( "SH_C1"
				, 0.488602512_f ); // sqrt(3 / pi) / 2

			/*Cosine lobe coeff*/
			auto SH_cosLobe_C0 = writer.declConstant( "SH_cosLobe_C0"
				, 0.886226925_f );// sqrt(pi) / 2 
			auto SH_cosLobe_C1 = writer.declConstant( "SH_cosLobe_C1"
				, 1.02332671_f ); // sqrt(pi / 3)

			// SH_C0 * SH_cosLobe_C0 = 0.25000000007f
			// SH_C1 * SH_cosLobe_C1 = 0.5000000011f

			auto c3d_lpvGridR = writer.declOutput< Vec4 >( "c3d_lpvGridR", 0u );
			auto c3d_lpvGridG = writer.declOutput< Vec4 >( "c3d_lpvGridG", 1u );
			auto c3d_lpvGridB = writer.declOutput< Vec4 >( "c3d_lpvGridB", 2u );

			//layout( early_fragment_tests )in;//turn on early depth tests

			uint32_t index = 0u;
			auto inRsmPos = writer.declInput< Vec3 >( "inRsmPos", index++ );
			auto inRsmNormal = writer.declInput< Vec3 >( "inRsmNormal", index++ );
			auto inRsmFlux = writer.declInput< Vec4 >( "inRsmFlux", index++ );

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

					c3d_lpvGridR = SHCoeffsR;
					c3d_lpvGridG = SHCoeffsG;
					c3d_lpvGridB = SHCoeffsB;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
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
			, uint32_t size )
		{
			auto & renderSystem = *engine.getRenderSystem();
			auto sampler = doCreateSampler( engine, name, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE );

			ashes::ImageCreateInfo image
			{
				VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT,
				VK_IMAGE_TYPE_3D,
				format,
				{ size, size, size },
				1u,
				1u,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT ),
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

		TextureUnitArray doCreateTextures( Engine & engine
			, castor::String const & name
			, VkFormat format
			, uint32_t size )
		{
			TextureUnitArray result;
			result.emplace_back( doCreateTexture( engine, name, format, size ) );
			result.emplace_back( doCreateTexture( engine, name, format, size ) );
			result.emplace_back( doCreateTexture( engine, name, format, size ) );
			return result;
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
			auto result = device->createRenderPass( "LightInjection"
				, std::move( createInfo ) );
			return result;
		}

		ashes::FrameBufferPtr doCreateFrameBuffer( ashes::RenderPass const & renderPass
			, uint32_t size
			, ashes::ImageView const & img1
			, ashes::ImageView const & img2
			, ashes::ImageView const & img3 )
		{
			ashes::ImageViewCRefArray attaches;
			attaches.emplace_back( img1 );
			attaches.emplace_back( img2 );
			attaches.emplace_back( img3 );
			return renderPass.createFrameBuffer( "LightInjection"
				, VkExtent2D{ size, size }
				, std::move( attaches )
				, size );
		}

		ashes::VertexBufferPtr< NonTexturedQuad::Vertex > doCreateVertexBuffer( Engine & engine
			, uint32_t rsmSize )
		{
			auto & device = getCurrentRenderDevice( engine );
			auto vplCount = rsmSize * rsmSize;

			auto result = makeVertexBuffer< NonTexturedQuad::Vertex >( device
				, vplCount
				, 0u
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, "LightInjection" );
			NonTexturedQuad::Vertex vtx;

			if ( auto buffer = result->lock( 0u, vplCount, 0u ) )
			{
				*buffer = vtx;
				result->flush( 0u, vplCount );
				result->unlock();
			}

			return result;
		}

		ashes::DescriptorSetLayoutPtr doCreateDescriptorLayout( Engine & engine )
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
					, VK_SHADER_STAGE_VERTEX_BIT ),
				makeDescriptorSetLayoutBinding( GpUboIdx
					, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
					, VK_SHADER_STAGE_VERTEX_BIT ),
			};
			return getCurrentRenderDevice( engine )->createDescriptorSetLayout( "LightInjection"
				, std::move( bindings ) );
		}

		ashes::DescriptorSetPtr doCreateDescriptorSet( ashes::DescriptorSetPool & descriptorSetPool
			, LightCache const & lightCache
			, ShadowMapResult const & smResult
			, UniformBuffer< LightInjectionConfiguration > const & ubo
			, GpInfoUbo const & gpInfoUbo )
		{
			auto & descriptorSetLayout = descriptorSetPool.getLayout();
			auto result = descriptorSetPool.createDescriptorSet( "LightInjection" );
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
			result->createSizedBinding( descriptorSetLayout.getBinding( LIUboIdx )
				, ubo );
			result->createSizedBinding( descriptorSetLayout.getBinding( GpUboIdx )
				, gpInfoUbo.getUbo() );
			result->update();
			return result;
		}

		ashes::GraphicsPipelinePtr doCreatePipeline( Engine & engine
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
			auto & device = getCurrentRenderDevice( engine );
			ashes::PipelineShaderStageCreateInfoArray shaderStages;
			shaderStages.push_back( makeShaderState( device, vertexShader ) );
			shaderStages.push_back( makeShaderState( device, geometryShader ) );
			shaderStages.push_back( makeShaderState( device, pixelShader ) );
			return device->createPipeline( "LightInjection"
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
		, LightCache const & lightCache
		, LightType lightType
		, ShadowMapResult const & smResult
		, GpInfoUbo const & gpInfoUbo )
		: Named{ "LightInjection" }
		, m_engine{ engine }
		, m_lightCache{ lightCache }
		, m_smResult{ smResult }
		, m_gpInfoUbo{ gpInfoUbo }
		, m_result{ doCreateTextures( engine
			, getName() + "Result"
			, VK_FORMAT_R16G16B16A16_SFLOAT
			, GridSize ) }
		, m_ubo{ makeUniformBuffer< Configuration >( *engine.getRenderSystem()
			, 1u
			, 0u
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			, "LightInjection" ) }
		, m_lightType{ lightType }
		, m_timer{ std::make_shared< RenderPassTimer >( engine, cuT( "Lighting" ), cuT( "Light Injection" ) ) }
		, m_aabb{ lightCache.getScene()->getBoundingBox() }
		, m_grid{ GridSize, CellSize, m_aabb.getMax(), m_aabb.getMin(), 1.0f, 0 }
		, m_vertexBuffer{ doCreateVertexBuffer( engine, m_smResult[SmTexture::eDepth].getTexture()->getWidth() ) }
		, m_descriptorSetLayout{ doCreateDescriptorLayout( engine ) }
		, m_pipelineLayout{ getCurrentRenderDevice( m_engine )->createPipelineLayout( getName(), *m_descriptorSetLayout ) }
		, m_descriptorSetPool{ m_descriptorSetLayout->createPool( getName(), 1u ) }
		, m_descriptorSet{ doCreateDescriptorSet( *m_descriptorSetPool
			, m_lightCache
			, m_smResult
			, *m_ubo
			, m_gpInfoUbo ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, getName(), getVertexProgram( lightType, smResult[SmTexture::eDepth].getTexture()->getWidth(), GridSize, 4u ) }
		, m_geometryShader{ VK_SHADER_STAGE_GEOMETRY_BIT, getName(), getGeometryProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, getName(), getPixelProgram() }
		, m_renderPass{ doCreateRenderPass( getCurrentRenderDevice( m_engine )
			, m_result[0].getTexture()->getPixelFormat() ) }
		, m_pipeline{ doCreatePipeline( m_engine
			, *m_pipelineLayout
			, *m_renderPass
			, m_vertexShader
			, m_geometryShader
			, m_pixelShader
			, GridSize ) }
		, m_frameBuffer{ doCreateFrameBuffer( *m_renderPass
			, GridSize
			, m_result[0].getTexture()->getDefaultView().getTargetView()
			, m_result[1].getTexture()->getDefaultView().getTargetView()
			, m_result[2].getTexture()->getDefaultView().getTargetView() ) }
		, m_commands{ getCommands( *m_timer, 0u ) }
	{
	}

	ashes::Semaphore const & LightInjectionPass::compute( ashes::Semaphore const & toWait )const
	{
		auto & device = getCurrentRenderDevice( m_engine );
		auto timerBlock = m_timer->start();
		m_timer->notifyPassRender();
		auto * result = &toWait;

		device.graphicsQueue->submit( *m_commands.commandBuffer
			, toWait
			, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
			, *m_commands.semaphore
			, nullptr );
		result = m_commands.semaphore.get();

		return *result;
	}

	CommandsSemaphore LightInjectionPass::getCommands( RenderPassTimer const & timer
		, uint32_t index )const
	{
		auto & device = getCurrentRenderDevice( m_engine );
		castor3d::CommandsSemaphore commands
		{
			device.graphicsCommandPool->createCommandBuffer( getName() ),
			device->createSemaphore( getName() )
		};
		auto & cmd = *commands.commandBuffer;
		auto rsmSize = m_smResult[SmTexture::eDepth].getTexture()->getWidth();
		auto vplCount = rsmSize * rsmSize;

		cmd.begin( VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT );
		timer.beginPass( cmd, index );
		cmd.beginDebugBlock(
			{
				"Lighting - Light Injection",
				castor3d::makeFloatArray( m_engine.getNextRainbowColour() ),
			} );
		cmd.beginRenderPass( *m_renderPass
			, *m_frameBuffer
			, { castor3d::transparentBlackClearColor, castor3d::transparentBlackClearColor, castor3d::transparentBlackClearColor }
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

	void LightInjectionPass::update( Light const & light
		, Camera const & camera )
	{
		auto & scene = *light.getScene();
		auto aabb = scene.getBoundingBox();
		auto camPos = camera.getParent()->getDerivedPosition();
		Point3f camDir{ 0, 0, 1 };
		camera.getParent()->getDerivedOrientation().transform( camDir, camDir );

		if ( m_aabb != aabb
			|| m_lightIndex != light.getShadowMapIndex()
			|| m_cameraPos != camPos
			|| m_cameraDir != camDir )
		{
			m_lightIndex = light.getShadowMapIndex();
			m_aabb = aabb;
			m_cameraPos = camPos;
			m_cameraDir = camDir;
			m_grid = Grid{ GridSize, CellSize, m_aabb.getMax(), m_aabb.getMin(), 1.0f, 0 };
			m_grid.transform( m_cameraPos, m_cameraDir );

			auto min = m_grid.getMin();
			auto size = m_grid.getDimensions();
			auto & data = m_ubo->getData();
			data.minVolumeCorner = Point4f{ min->x, min->y, min->z, m_grid.getCellSize() };
			data.gridSizes = Point4ui{ size->x, size->y, size->z, m_lightIndex };
			m_ubo->upload();
		}
	}
}
