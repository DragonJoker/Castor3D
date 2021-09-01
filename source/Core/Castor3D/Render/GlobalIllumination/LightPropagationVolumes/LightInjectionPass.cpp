#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightInjectionPass.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/RenderPass.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapResult.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/GpInfoUbo.hpp"
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/LpvLightConfigUbo.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/Image.hpp>

#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/GraphContext.hpp>

#include <numeric>
#include <random>

CU_ImplementCUSmartPtr( castor3d, LightInjectionPass )

using namespace castor;

namespace castor3d
{
	namespace
	{
		std::unique_ptr< ast::Shader > getDirectionalVertexProgram( uint32_t rsmTexSize
			, RenderSystem const & renderSystem )
		{
			using namespace sdw;
			VertexWriter writer;

			if ( shader::DirectionalMaxCascadesCount > 1u )
			{
				auto inPosition = writer.declInput< Vec2 >( "inPosition", 0u );
#if C3D_UseTiledDirectionalShadowMap
				auto c3d_rsmNormalMap = writer.declSampledImage< FImg2DRgba32 >( getTextureName( LightType::eDirectional, SmTexture::eNormalLinear )
					, RsmNormalsIdx
					, 0u );
				auto c3d_rsmPositionMap = writer.declSampledImage< FImg2DRgba32 >( getTextureName( LightType::eDirectional, SmTexture::ePosition )
					, RsmPositionIdx
					, 0u );
				auto c3d_rsmFluxMap = writer.declSampledImage< FImg2DRgba32 >( getTextureName( LightType::eDirectional, SmTexture::eFlux )
					, RsmFluxIdx
					, 0u );
#else
				auto c3d_rsmNormalMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( LightType::eDirectional, SmTexture::eNormalLinear )
					, LightInjectionPass::RsmNormalsIdx
					, 0u );
				auto c3d_rsmPositionMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( LightType::eDirectional, SmTexture::ePosition )
					, LightInjectionPass::RsmPositionIdx
					, 0u );
				auto c3d_rsmFluxMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( LightType::eDirectional, SmTexture::eFlux )
					, LightInjectionPass::RsmFluxIdx
					, 0u );
#endif
				UBO_LPVGRIDCONFIG( writer, LightInjectionPass::LpvGridUboIdx, 0u, true );
				UBO_LPVLIGHTCONFIG( writer, LightInjectionPass::LpvLightUboIdx, 0u );
				auto in = writer.getIn();

				uint32_t index = 0u;
				auto outVolumeCellIndex = writer.declOutput< IVec3 >( "outVolumeCellIndex", index++ );
				auto outRsmPos = writer.declOutput< Vec3 >( "outRsmPos", index++ );
				auto outRsmNormal = writer.declOutput< Vec3 >( "outRsmNormal", index++ );
				auto outRsmFlux = writer.declOutput< Vec4 >( "outRsmFlux", index++ );
				auto out = writer.getOut();

				// Utility functions
				shader::Utils utils{ writer, *renderSystem.getEngine() };
				index = 0;
				auto lightingModel = shader::LightingModel::createModel( utils
					, renderSystem.getEngine()->getPassFactory().getLightingModelName( 1u )
					, LightType::eDirectional
					, uint32_t( LightInjectionPass::LightsIdx )
					, 0u
					, false
					, shader::ShadowOptions{ SceneFlag::eNone, true }
					, index
					, 1u );

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
						outRsmFlux = c3d_rsmFluxMap.fetch( rsmCoords, 0_i );
						outVolumeCellIndex = c3d_lpvGridData.worldToGrid( outRsmPos
							, outRsmNormal );

						auto screenPos = writer.declLocale( "screenPos"
							, c3d_lpvGridData.gridToScreen( outVolumeCellIndex.xy() ) );

						out.vtx.position = vec4( screenPos, 0.0, 1.0 );
						out.vtx.pointSize = 1.0f;
					} );
			}
			else
			{
				auto inPosition = writer.declInput< Vec2 >( "inPosition", 0u );
				auto c3d_rsmNormalMap = writer.declSampledImage< FImg2DRgba32 >( getTextureName( LightType::eDirectional, SmTexture::eNormalLinear )
					, LightInjectionPass::RsmNormalsIdx
					, 0u );
				auto c3d_rsmPositionMap = writer.declSampledImage< FImg2DRgba32 >( getTextureName( LightType::eDirectional, SmTexture::ePosition )
					, LightInjectionPass::RsmPositionIdx
					, 0u );
				auto c3d_rsmFluxMap = writer.declSampledImage< FImg2DRgba32 >( getTextureName( LightType::eDirectional, SmTexture::eFlux )
					, LightInjectionPass::RsmFluxIdx
					, 0u );
				UBO_LPVGRIDCONFIG( writer, LightInjectionPass::LpvGridUboIdx, 0u, true );
				UBO_LPVLIGHTCONFIG( writer, LightInjectionPass::LpvLightUboIdx, 0u );
				auto in = writer.getIn();

				uint32_t index = 0u;
				auto outVolumeCellIndex = writer.declOutput< IVec3 >( "outVolumeCellIndex", index++ );
				auto outRsmPos = writer.declOutput< Vec3 >( "outRsmPos", index++ );
				auto outRsmNormal = writer.declOutput< Vec3 >( "outRsmNormal", index++ );
				auto outRsmFlux = writer.declOutput< Vec4 >( "outRsmFlux", index++ );
				auto out = writer.getOut();

				// Utility functions
				shader::Utils utils{ writer, *renderSystem.getEngine() };
				index = 0;
				auto lightingModel = shader::LightingModel::createModel( utils
					, renderSystem.getEngine()->getPassFactory().getLightingModelName( 1u )
					, LightType::eDirectional
					, uint32_t( LightInjectionPass::LightsIdx )
					, 0u
					, false
					, shader::ShadowOptions{ SceneFlag::eNone, true }
					, index
					, 1u );

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
						outRsmFlux = c3d_rsmFluxMap.fetch( rsmCoords, 0_i );
						outVolumeCellIndex = c3d_lpvGridData.worldToGrid( outRsmPos, outRsmNormal );

						auto screenPos = writer.declLocale( "screenPos"
							, c3d_lpvGridData.gridToScreen( outVolumeCellIndex.xy() ) );

						out.vtx.position = vec4( screenPos, 0.0, 1.0 );
						out.vtx.pointSize = 1.0f;
					} );
			}

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< ast::Shader > getPointVertexProgram( CubeMapFace face
			, uint32_t rsmTexSize
			, RenderSystem const & renderSystem )
		{
			using namespace sdw;
			VertexWriter writer;

			auto inPosition = writer.declInput< Vec2 >( "inPosition", 0u );
			auto c3d_rsmNormalMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( LightType::ePoint, SmTexture::eNormalLinear )
				, LightInjectionPass::RsmNormalsIdx
				, 0u );
			auto c3d_rsmPositionMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( LightType::ePoint, SmTexture::ePosition )
				, LightInjectionPass::RsmPositionIdx
				, 0u );
			auto c3d_rsmFluxMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( LightType::ePoint, SmTexture::eFlux )
				, LightInjectionPass::RsmFluxIdx
				, 0u );
			UBO_LPVGRIDCONFIG( writer, LightInjectionPass::LpvGridUboIdx, 0u, true );
			UBO_LPVLIGHTCONFIG( writer, LightInjectionPass::LpvLightUboIdx, 0u );
			auto in = writer.getIn();

			uint32_t index = 0u;
			auto outVolumeCellIndex = writer.declOutput< IVec3 >( "outVolumeCellIndex", index++ );
			auto outRsmPos = writer.declOutput< Vec3 >( "outRsmPos", index++ );
			auto outRsmNormal = writer.declOutput< Vec3 >( "outRsmNormal", index++ );
			auto outRsmFlux = writer.declOutput< Vec4 >( "outRsmFlux", index++ );
			auto out = writer.getOut();

			// Utility functions
			shader::Utils utils{ writer, *renderSystem.getEngine() };
			index = 0;
			auto lightingModel = shader::LightingModel::createModel( utils
				, renderSystem.getEngine()->getPassFactory().getLightingModelName( 1u )
				, LightType::ePoint
				, uint32_t( LightInjectionPass::LightsIdx )
				, 0u
				, false
				, shader::ShadowOptions{ SceneFlag::eNone, true }
				, index
				, 1u );

			writer.implementFunction< Void >( "main"
				, [&]()
				{
					auto light = writer.declLocale( "light"
						, lightingModel->getPointLight( c3d_lpvLightData.lightIndex ) );
					auto rsmCoords = writer.declLocale( "rsmCoords"
						, ivec3( in.vertexIndex % rsmTexSize
							, in.vertexIndex / rsmTexSize
							, light.m_lightBase.m_index * 6_i + int32_t( face ) ) );

					outRsmPos = c3d_rsmPositionMap.fetch( rsmCoords, 0_i ).rgb();
					outRsmNormal = c3d_rsmNormalMap.fetch( rsmCoords, 0_i ).rgb();
					outRsmFlux = c3d_rsmFluxMap.fetch( rsmCoords, 0_i );
					outVolumeCellIndex = c3d_lpvGridData.worldToGrid( outRsmPos, outRsmNormal );

					auto screenPos = writer.declLocale( "screenPos"
						, c3d_lpvGridData.gridToScreen( outVolumeCellIndex.xy() ) );
					out.vtx.position = vec4( screenPos, 0.0_f, 1.0_f );
					out.vtx.pointSize = 1.0f;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< ast::Shader > getSpotVertexProgram( uint32_t rsmTexSize
			, RenderSystem const & renderSystem )
		{
			using namespace sdw;
			VertexWriter writer;

			auto inPosition = writer.declInput< Vec2 >( "inPosition", 0u );
			auto c3d_rsmNormalMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( LightType::eSpot, SmTexture::eNormalLinear )
				, LightInjectionPass::RsmNormalsIdx
				, 0u );
			auto c3d_rsmPositionMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( LightType::eSpot, SmTexture::ePosition )
				, LightInjectionPass::RsmPositionIdx
				, 0u );
			auto c3d_rsmFluxMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( LightType::eSpot, SmTexture::eFlux )
				, LightInjectionPass::RsmFluxIdx
				, 0u );
			UBO_LPVGRIDCONFIG( writer, LightInjectionPass::LpvGridUboIdx, 0u, true );
			UBO_LPVLIGHTCONFIG( writer, LightInjectionPass::LpvLightUboIdx, 0u );
			auto in = writer.getIn();

			uint32_t index = 0u;
			auto outVolumeCellIndex = writer.declOutput< IVec3 >( "outVolumeCellIndex", index++ );
			auto outRsmPos = writer.declOutput< Vec3 >( "outRsmPos", index++ );
			auto outRsmNormal = writer.declOutput< Vec3 >( "outRsmNormal", index++ );
			auto outRsmFlux = writer.declOutput< Vec4 >( "outRsmFlux", index++ );
			auto out = writer.getOut();

			// Utility functions
			shader::Utils utils{ writer, *renderSystem.getEngine() };
			index = 0;
			auto lightingModel = shader::LightingModel::createModel( utils
				, renderSystem.getEngine()->getPassFactory().getLightingModelName( 1u )
				, LightType::eSpot
				, uint32_t( LightInjectionPass::LightsIdx )
				, 0u
				, false
				, shader::ShadowOptions{ SceneFlag::eNone, true }
				, index
				, 1u );

			writer.implementFunction< Void >( "main"
				, [&]()
				{
					auto light = writer.declLocale( "light"
						, lightingModel->getSpotLight( c3d_lpvLightData.lightIndex ) );
					auto rsmCoords = writer.declLocale( "rsmCoords"
						, ivec3( in.vertexIndex % rsmTexSize
							, in.vertexIndex / rsmTexSize
							, light.m_lightBase.m_index ) );

					outRsmPos = c3d_rsmPositionMap.fetch( rsmCoords, 0_i ).rgb();
					outRsmNormal = c3d_rsmNormalMap.fetch( rsmCoords, 0_i ).rgb();
					outRsmFlux = c3d_rsmFluxMap.fetch( rsmCoords, 0_i );
					outVolumeCellIndex = c3d_lpvGridData.worldToGrid( outRsmPos, outRsmNormal );

					auto screenPos = writer.declLocale( "screenPos"
						, c3d_lpvGridData.gridToScreen( outVolumeCellIndex.xy() ) );
					out.vtx.position = vec4( screenPos, 0.0_f, 1.0_f );
					out.vtx.pointSize = 1.0f;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr getVertexProgram( LightType lightType
			, uint32_t rsmTexSize
			, RenderSystem const & renderSystem )
		{
			switch ( lightType )
			{
			case castor3d::LightType::eDirectional:
				return getDirectionalVertexProgram( rsmTexSize, renderSystem );
			case castor3d::LightType::eSpot:
				return getSpotVertexProgram( rsmTexSize, renderSystem );
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

			uint32_t index = 0u;
			auto inVolumeCellIndex = writer.declInput< IVec3 >( "inVolumeCellIndex", index++ );
			auto inRsmPos = writer.declInput< Vec3 >( "inRsmPos", index++ );
			auto inRsmNormal = writer.declInput< Vec3 >( "inRsmNormal", index++ );
			auto inRsmFlux = writer.declInput< Vec4 >( "inRsmFlux", index++ );
			auto in = writer.getIn();

			UBO_LPVLIGHTCONFIG( writer, LightInjectionPass::LpvLightUboIdx, 0u );

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
					auto lobeDir = writer.declLocale( "lobeDir"
						, evalCosineLobeToDir( inRsmNormal ) );
					auto SHCoeffsR = writer.declLocale( "SHCoeffsR"
						, lobeDir / Float{ Pi< float > } * inRsmFlux.r() );
					auto SHCoeffsG = writer.declLocale( "SHCoeffsG"
						, lobeDir / Float{ Pi< float > } * inRsmFlux.g() );
					auto SHCoeffsB = writer.declLocale( "SHCoeffsB"
						, lobeDir / Float{ Pi< float > } * inRsmFlux.b() );

					outLpvGridR = SHCoeffsR;
					outLpvGridG = SHCoeffsG;
					outLpvGridB = SHCoeffsB;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
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
	}

	//*********************************************************************************************

	LightInjectionPass::PipelineHolder::PipelineHolder( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, crg::pp::Config config
		, uint32_t lpvSize )
		: m_holder{ pass, context, graph, std::move( config ), VK_PIPELINE_BIND_POINT_GRAPHICS, 1u }
		, m_lpvSize{ lpvSize }
	{
	}

	void LightInjectionPass::PipelineHolder::initialise( VkRenderPass renderPass )
	{
		m_renderPass = renderPass;
		m_holder.initialise();
		doCreatePipeline( 0u );
	}

	void LightInjectionPass::PipelineHolder::recordInto( VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		m_holder.recordInto( commandBuffer, index );
	}

	void LightInjectionPass::PipelineHolder::doCreatePipeline( uint32_t index )
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
		auto blendState = SceneRenderPass::createBlendState( BlendMode::eNoBlend, BlendMode::eNoBlend, 3u );
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
		auto & program = m_holder.getProgram( index );
		auto & pipeline = m_holder.getPipeline( index );
		VkGraphicsPipelineCreateInfo createInfo{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO
			, nullptr
			, 0u
			, uint32_t( program.size() )
			, program.data()
			, &viState
			, &iaState
			, nullptr
			, &vpState
			, &rsState
			, &msState
			, &dsState
			, &cbState
			, nullptr
			, m_holder.getPipelineLayout()
			, m_renderPass
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

	//*********************************************************************************************

	LightInjectionPass::LightInjectionPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, LightType lightType
		, uint32_t gridSize
		, uint32_t rsmSize )
		: Named{ pass.name }
		, crg::RenderPass{ pass
			, context
			, graph
			, { [this](){ doSubInitialise(); }
				, [this]( VkCommandBuffer cb, uint32_t i ){ doSubRecordInto( cb, i ); } }
			, { gridSize, gridSize }
			, 1u }
		, m_device{ device }
		, m_rsmSize{ rsmSize }
		, m_vertexBuffer{ createVertexBuffer( getName(), m_device, m_rsmSize ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, getName(), getVertexProgram( lightType, m_rsmSize, device.renderSystem ) }
		, m_geometryShader{ VK_SHADER_STAGE_GEOMETRY_BIT, getName(), getGeometryProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, getName(), getPixelProgram() }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_geometryShader )
			, makeShaderState( device, m_pixelShader ) }
		, m_holder{ pass
			, context
			, graph
			, crg::pp::Config{}
				.programs( { crg::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) } )
			, gridSize }
	{
	}

	LightInjectionPass::LightInjectionPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, CubeMapFace face
		, uint32_t gridSize
		, uint32_t rsmSize )
		: Named{ pass.name }
		, crg::RenderPass{ pass
			, context
			, graph
			, { [this](){ doSubInitialise(); }
				, [this]( VkCommandBuffer cb, uint32_t i ){ doSubRecordInto( cb, i ); } }
			, { gridSize, gridSize }
			, 1u }
		, m_device{ device }
		, m_rsmSize{ rsmSize }
		, m_vertexBuffer{ createVertexBuffer( getName(), m_device, m_rsmSize ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, getName(), getPointVertexProgram( face, m_rsmSize, device.renderSystem ) }
		, m_geometryShader{ VK_SHADER_STAGE_GEOMETRY_BIT, getName(), getGeometryProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, getName(), getPixelProgram() }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_geometryShader )
			, makeShaderState( device, m_pixelShader ) }
		, m_holder{ pass
			, context
			, graph
			, crg::pp::Config{}
				.programs( { crg::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) } )
			, gridSize }
	{
	}

	void LightInjectionPass::doSubInitialise()
	{
		m_holder.initialise( getRenderPass() );
	}

	void LightInjectionPass::doSubRecordInto( VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		m_holder.recordInto( commandBuffer, index );
		auto vplCount = m_rsmSize * m_rsmSize;
		VkDeviceSize offset{};
		VkBuffer vertexBuffer = m_vertexBuffer->getBuffer();
		m_context.vkCmdBindVertexBuffers( commandBuffer, 0u, 1u, &vertexBuffer, &offset );
		m_context.vkCmdDraw( commandBuffer, vplCount, 1u, 0u, 0u );
	}

	void LightInjectionPass::accept( PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_geometryShader );
		visitor.visit( m_pixelShader );
	}
}
