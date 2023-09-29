#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightInjectionPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Buffer/PoolUniformBuffer.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapResult.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslBRDFHelpers.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslPassShaders.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/LpvGridConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/LpvLightConfigUbo.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/Image.hpp>

#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/GraphContext.hpp>

#include <numeric>
#include <random>

#pragma GCC diagnostic ignored "-Wuseless-cast"

CU_ImplementSmartPtr( castor3d, LightInjectionPass )

namespace castor3d
{
	namespace lpvlgt
	{
		template< sdw::var::Flag FlagT >
		struct SurfaceT
			: sdw::StructInstance
		{
			SurfaceT( sdw::ShaderWriter & writer
				, sdw::expr::ExprPtr expr
				, bool enabled = true )
				: sdw::StructInstance{ writer, std::move( expr ), enabled }
				, layer{ getMember< sdw::Int >( "layer" ) }
				, rsmNormal{ getMember< sdw::Vec3 >( "rsmNormal" ) }
				, rsmFlux{ getMember< sdw::Vec3 >( "rsmFlux" ) }
			{
			}

			SDW_DeclStructInstance( , SurfaceT );

			static sdw::type::IOStructPtr makeIOType( sdw::type::TypesCache & cache
				, bool isFragment = false )
			{
				auto result = cache.getIOStruct( sdw::type::MemoryLayout::eStd430
					, ( FlagT == sdw::var::Flag::eShaderOutput
						? std::string{ "Output" }
						: std::string{ "Input" } ) + "LgtInjSurface"
					, FlagT );

				if ( result->empty() )
				{
					uint32_t index = 0u;

					if ( !isFragment )
					{
						result->declMember( "layer"
							, sdw::type::Kind::eInt
							, sdw::type::NotArray
							, index++ );
					}

					result->declMember( "rsmNormal"
						, sdw::type::Kind::eVec3F
						, sdw::type::NotArray
						, index++ );
					result->declMember( "rsmFlux"
						, sdw::type::Kind::eVec3F
						, sdw::type::NotArray
						, index++ );
				}

				return result;
			}

			static sdw::type::BaseStructPtr makeType( sdw::type::TypesCache & cache
				, bool isFragment = false )
			{
				auto result = cache.getStruct( sdw::type::MemoryLayout::eStd430
					, "C3D_LgtInjSurface" );

				if ( result->empty() )
				{
					if ( !isFragment )
					{
						result->declMember( "layer"
							, sdw::type::Kind::eInt
							, sdw::type::NotArray );
					}

					result->declMember( "rsmNormal"
						, sdw::type::Kind::eVec3F
						, sdw::type::NotArray );
					result->declMember( "rsmFlux"
						, sdw::type::Kind::eVec3F
						, sdw::type::NotArray );
				}

				return result;
			}

			sdw::Int layer;
			sdw::Vec3 rsmNormal;
			sdw::Vec3 rsmFlux;
		};

		static std::unique_ptr< ast::Shader > getDirectionalVertexProgram( uint32_t rsmTexSize
			, RenderSystem const & renderSystem )
		{
			using namespace sdw;
			VertexWriter writer{ &renderSystem.getEngine()->getShaderAllocator() };

			auto c3d_rsmNormalMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( LightType::eDirectional, SmTexture::eNormal )
				, LightInjectionPass::RsmNormalsIdx
				, 0u );
			auto c3d_rsmPositionMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( LightType::eDirectional, SmTexture::ePosition )
				, LightInjectionPass::RsmPositionIdx
				, 0u );
			auto c3d_rsmFluxMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( LightType::eDirectional, SmTexture::eFlux )
				, LightInjectionPass::RsmFluxIdx
				, 0u );
			C3D_LpvGridConfig( writer, LightInjectionPass::LpvGridUboIdx, 0u, true );
			C3D_LpvLightConfig( writer, LightInjectionPass::LpvLightUboIdx, 0u );

			// Utility functions
			shader::Utils utils{ writer };
			shader::BRDFHelpers brdf{ writer };
			shader::PassShaders passShaders{ renderSystem.getEngine()->getPassComponentsRegister()
				, TextureCombine{}
				, ComponentModeFlag::eDiffuseLighting
				, utils };
			shader::Materials materials{ writer, passShaders };
			uint32_t index = 0;
			shader::Lights lights{ *renderSystem.getEngine()
				, materials
				, brdf
				, utils
				, shader::ShadowOptions{ SceneFlag::eNone, false, true }
				, nullptr /* sssProfiles */
				, LightType::eDirectional
				, LightInjectionPass::LightsIdx /* lightBinding */
				, 0u /* lightSet */
				, index /* shadowMapBinding */
				, 1u /* shadowMapSet */ };

			writer.implementMainT< VoidT, lpvlgt::SurfaceT >( [&]( VertexIn in
				, VertexOutT< lpvlgt::SurfaceT > out )
				{
					auto light = writer.declLocale( "light"
						, lights.getDirectionalLight( writer.cast< UInt >( c3d_lpvLightData.lightOffset() ) ) );
					auto cascadeIndex = writer.declLocale( "cascadeIndex"
						, writer.cast< Int >( max( 1_u, light.cascadeCount() - 1_u ) ) );
					auto rsmCoords = writer.declLocale( "rsmCoords"
						, ivec3( in.vertexIndex % int32_t( rsmTexSize )
							, in.vertexIndex / int32_t( rsmTexSize )
							, cascadeIndex ) );

					auto rsmPosition = writer.declLocale( "rsmPosition"
						, c3d_rsmPositionMap.fetch( rsmCoords, 0_i ).rgb() );
					out.rsmNormal = c3d_rsmNormalMap.fetch( rsmCoords, 0_i ).rgb();
					out.rsmFlux = c3d_rsmFluxMap.fetch( rsmCoords, 0_i ).rgb();
					auto volumeCellIndex = writer.declLocale( "volumeCellIndex"
						, c3d_lpvGridData.worldToGrid( rsmPosition, out.rsmNormal ) );
					out.layer = volumeCellIndex.z();

					auto screenPos = writer.declLocale( "screenPos"
						, c3d_lpvGridData.gridToScreen( volumeCellIndex.xy() ) );

					out.vtx.position = vec4( screenPos, 0.0, 1.0 );
					out.vtx.pointSize = 1.0f;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static std::unique_ptr< ast::Shader > getPointVertexProgram( CubeMapFace face
			, uint32_t rsmTexSize
			, RenderSystem const & renderSystem )
		{
			using namespace sdw;
			VertexWriter writer{ &renderSystem.getEngine()->getShaderAllocator() };

			auto c3d_rsmNormalMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( LightType::ePoint, SmTexture::eNormal )
				, LightInjectionPass::RsmNormalsIdx
				, 0u );
			auto c3d_rsmPositionMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( LightType::ePoint, SmTexture::ePosition )
				, LightInjectionPass::RsmPositionIdx
				, 0u );
			auto c3d_rsmFluxMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( LightType::ePoint, SmTexture::eFlux )
				, LightInjectionPass::RsmFluxIdx
				, 0u );
			C3D_LpvGridConfig( writer, LightInjectionPass::LpvGridUboIdx, 0u, true );
			C3D_LpvLightConfig( writer, LightInjectionPass::LpvLightUboIdx, 0u );

			// Utility functions
			shader::Utils utils{ writer };
			shader::BRDFHelpers brdf{ writer };
			shader::PassShaders passShaders{ renderSystem.getEngine()->getPassComponentsRegister()
				, TextureCombine{}
				, ComponentModeFlag::eDiffuseLighting
				, utils };
			shader::Materials materials{ writer, passShaders };
			uint32_t index = 0;
			shader::Lights lights{ *renderSystem.getEngine()
				, materials
				, brdf
				, utils
				, shader::ShadowOptions{ SceneFlag::eNone, false, true }
				, nullptr /* sssProfiles */
				, LightType::ePoint
				, LightInjectionPass::LightsIdx /* lightBinding */
				, 0u /* lightSet */
				, index /* shadowMapBinding */
				, 1u /* shadowMapSet */ };

			writer.implementMainT< VoidT, lpvlgt::SurfaceT >( [&]( VertexIn in
				, VertexOutT< lpvlgt::SurfaceT > out )
				{
					auto light = writer.declLocale( "light"
						, lights.getPointLight( writer.cast< UInt >( c3d_lpvLightData.lightOffset() ) ) );
					auto rsmCoords = writer.declLocale( "rsmCoords"
						, ivec3( in.vertexIndex % int32_t( rsmTexSize )
							, in.vertexIndex / int32_t( rsmTexSize )
							, light.shadowMapIndex() * 6_i + int32_t( face ) ) );

					auto rsmPosition = writer.declLocale( "rsmPosition"
						, c3d_rsmPositionMap.fetch( rsmCoords, 0_i ).rgb() );
					out.rsmNormal = c3d_rsmNormalMap.fetch( rsmCoords, 0_i ).rgb();
					out.rsmFlux = c3d_rsmFluxMap.fetch( rsmCoords, 0_i ).rgb();
					auto volumeCellIndex = writer.declLocale( "volumeCellIndex"
						, c3d_lpvGridData.worldToGrid( rsmPosition, out.rsmNormal ) );
					out.layer = volumeCellIndex.z();

					auto screenPos = writer.declLocale( "screenPos"
						, c3d_lpvGridData.gridToScreen( volumeCellIndex.xy() ) );
					out.vtx.position = vec4( screenPos, 0.0_f, 1.0_f );
					out.vtx.pointSize = 1.0f;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static std::unique_ptr< ast::Shader > getSpotVertexProgram( uint32_t rsmTexSize
			, RenderSystem const & renderSystem )
		{
			using namespace sdw;
			VertexWriter writer{ &renderSystem.getEngine()->getShaderAllocator() };

			auto c3d_rsmNormalMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( LightType::eSpot, SmTexture::eNormal )
				, LightInjectionPass::RsmNormalsIdx
				, 0u );
			auto c3d_rsmPositionMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( LightType::eSpot, SmTexture::ePosition )
				, LightInjectionPass::RsmPositionIdx
				, 0u );
			auto c3d_rsmFluxMap = writer.declCombinedImg< FImg2DArrayRgba32 >( getTextureName( LightType::eSpot, SmTexture::eFlux )
				, LightInjectionPass::RsmFluxIdx
				, 0u );
			C3D_LpvGridConfig( writer, LightInjectionPass::LpvGridUboIdx, 0u, true );
			C3D_LpvLightConfig( writer, LightInjectionPass::LpvLightUboIdx, 0u );

			// Utility functions
			shader::Utils utils{ writer };
			shader::BRDFHelpers brdf{ writer };
			shader::PassShaders passShaders{ renderSystem.getEngine()->getPassComponentsRegister()
				, TextureCombine{}
				, ComponentModeFlag::eDiffuseLighting
				, utils };
			shader::Materials materials{ writer, passShaders };
			uint32_t index = 0;
			shader::Lights lights{ *renderSystem.getEngine()
				, materials
				, brdf
				, utils
				, shader::ShadowOptions{ SceneFlag::eNone, false, true }
				, nullptr /* sssProfiles */
				, LightType::eSpot
				, LightInjectionPass::LightsIdx /* lightBinding */
				, 0u /* lightSet */
				, index /* shadowMapBinding */
				, 1u /* shadowMapSet */ };

			writer.implementMainT< VoidT, lpvlgt::SurfaceT >( [&]( VertexIn in
				, VertexOutT< lpvlgt::SurfaceT > out )
				{
					auto light = writer.declLocale( "light"
						, lights.getSpotLight( writer.cast< UInt >( c3d_lpvLightData.lightOffset() ) ) );
					auto rsmCoords = writer.declLocale( "rsmCoords"
						, ivec3( in.vertexIndex % int32_t( rsmTexSize )
							, in.vertexIndex / int32_t( rsmTexSize )
							, light.shadowMapIndex() ) );

					auto rsmPosition = writer.declLocale( "rsmPosition"
						, c3d_rsmPositionMap.fetch( rsmCoords, 0_i ).rgb() );
					out.rsmNormal = c3d_rsmNormalMap.fetch( rsmCoords, 0_i ).rgb();
					out.rsmFlux = c3d_rsmFluxMap.fetch( rsmCoords, 0_i ).rgb();
					auto volumeCellIndex = writer.declLocale( "volumeCellIndex"
						, c3d_lpvGridData.worldToGrid( rsmPosition, out.rsmNormal ) );
					out.layer = volumeCellIndex.z();

					auto screenPos = writer.declLocale( "screenPos"
						, c3d_lpvGridData.gridToScreen( volumeCellIndex.xy() ) );
					out.vtx.position = vec4( screenPos, 0.0_f, 1.0_f );
					out.vtx.pointSize = 1.0f;
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static ShaderPtr getVertexProgram( LightType lightType
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

		static std::unique_ptr< ast::Shader > getGeometryProgram( RenderSystem const & renderSystem )
		{
			using namespace sdw;
			GeometryWriter writer{ &renderSystem.getEngine()->getShaderAllocator() };

			writer.implementMainT< PointListT< lpvlgt::SurfaceT >, PointStreamT< lpvlgt::SurfaceT > >( PointListT< lpvlgt::SurfaceT >{ writer, false }
				, PointStreamT< lpvlgt::SurfaceT >{ writer, 1u, true }
				, [&]( GeometryIn in
					, PointListT< lpvlgt::SurfaceT > list
					, PointStreamT< lpvlgt::SurfaceT > out )
				{
					out.vtx.position = list[0].vtx.position;
					out.layer = list[0].layer;
					out.vtx.pointSize = 1.0f;

					out.rsmNormal = list[0].rsmNormal;
					out.rsmFlux = list[0].rsmFlux;

					out.append();
					out.restartStrip();
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static ShaderPtr getPixelProgram( RenderSystem const & renderSystem )
		{
			using namespace sdw;
			FragmentWriter writer{ &renderSystem.getEngine()->getShaderAllocator() };

			/*Cosine lobe coeff*/
			auto SH_cosLobe_C0 = writer.declConstant( "SH_cosLobe_C0"
				, Float{ float( sqrt( castor::Pi< float > ) / 2.0f ) } );
			auto SH_cosLobe_C1 = writer.declConstant( "SH_cosLobe_C1"
				, Float{ float( sqrt( castor::Pi< float > ) / 3.0f ) } );

			// SH_C0 * SH_cosLobe_C0 = 0.25000000007f
			// SH_C1 * SH_cosLobe_C1 = 0.5000000011f

			auto outLpvGridR = writer.declOutput< Vec4 >( "outLpvGridR", 0u );
			auto outLpvGridG = writer.declOutput< Vec4 >( "outLpvGridG", 1u );
			auto outLpvGridB = writer.declOutput< Vec4 >( "outLpvGridB", 2u );

			//layout( early_fragment_tests )in;//turn on early depth tests
			C3D_LpvLightConfig( writer, LightInjectionPass::LpvLightUboIdx, 0u );

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

			writer.implementMainT< lpvlgt::SurfaceT, VoidT >( FragmentInT< lpvlgt::SurfaceT >{ writer, true }
				, FragmentOut{ writer }
				, [&]( FragmentInT< lpvlgt::SurfaceT > in
					, FragmentOut out )
				{
					auto lobeDir = writer.declLocale( "lobeDir"
						, evalCosineLobeToDir( in.rsmNormal ) );
					auto SHCoeffsR = writer.declLocale( "SHCoeffsR"
						, lobeDir / Float{ castor::Pi< float > } * in.rsmFlux.r() );
					auto SHCoeffsG = writer.declLocale( "SHCoeffsG"
						, lobeDir / Float{ castor::Pi< float > } * in.rsmFlux.g() );
					auto SHCoeffsB = writer.declLocale( "SHCoeffsB"
						, lobeDir / Float{ castor::Pi< float > } * in.rsmFlux.b() );

					outLpvGridR = SHCoeffsR;
					outLpvGridG = SHCoeffsG;
					outLpvGridB = SHCoeffsB;
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		static GpuBufferOffsetT< NonTexturedQuad::Vertex > createVertexBuffer( castor::String const & name
			, RenderDevice const & device
			, uint32_t rsmSize )
		{
			auto vplCount = rsmSize * rsmSize;

			auto result = device.bufferPool->getBuffer< NonTexturedQuad::Vertex >( VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
				, vplCount
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
			NonTexturedQuad::Vertex vtx;
			auto buffer = result.getData().data();

			for ( auto i = 0u; i < vplCount; ++i )
			{
				*buffer = vtx;
				++buffer;
			}

			result.markDirty( VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT
				, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT );
			return result;
		}
	}

	//*********************************************************************************************

	LightInjectionPass::PipelineHolder::PipelineHolder( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, crg::pp::Config config
		, uint32_t lpvSize )
		: m_lpvSize{ lpvSize }
		, m_holder{ pass, context, graph, std::move( config ), VK_PIPELINE_BIND_POINT_GRAPHICS, 1u }
	{
	}

	void LightInjectionPass::PipelineHolder::initialise( VkRenderPass renderPass
		, uint32_t index )
	{
		m_renderPass = renderPass;
		m_holder.initialise();

		if ( !m_holder.getPipeline( index ) )
		{
			doCreatePipeline( index );
		}
	}

	void LightInjectionPass::PipelineHolder::recordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		m_holder.recordInto( context, commandBuffer, index );
	}

	void LightInjectionPass::PipelineHolder::doCreatePipeline( uint32_t index )
	{
		ashes::PipelineVertexInputStateCreateInfo vertexState{ 0u
			, ashes::VkVertexInputBindingDescriptionArray{}
			, ashes::VkVertexInputAttributeDescriptionArray{} };
		VkViewport viewport{ 0.0f, 0.0f, float( m_lpvSize ), float( m_lpvSize ), 0.0f, 1.0f };
		VkRect2D scissor{ 0, 0, m_lpvSize, m_lpvSize };
		ashes::PipelineViewportStateCreateInfo viewportState{ 0u
			, 1u
			, ashes::VkViewportArray{ viewport }
			, 1u
			, ashes::VkScissorArray{ scissor } };
		auto blendState = RenderNodesPass::createBlendState( BlendMode::eNoBlend, BlendMode::eNoBlend, 3u );
		auto iaState = makeVkStruct< VkPipelineInputAssemblyStateCreateInfo >( 0u
			, VK_PRIMITIVE_TOPOLOGY_POINT_LIST
			, VK_FALSE );
		auto msState = makeVkStruct< VkPipelineMultisampleStateCreateInfo >( 0u
			, VK_SAMPLE_COUNT_1_BIT
			, VK_FALSE
			, 0.0f
			, nullptr
			, VK_FALSE
			, VK_FALSE );
		auto rsState = makeVkStruct< VkPipelineRasterizationStateCreateInfo >( 0u
			, VK_FALSE
			, VK_FALSE
			, VK_POLYGON_MODE_FILL
			, VkCullModeFlags( VK_CULL_MODE_NONE )
			, VK_FRONT_FACE_COUNTER_CLOCKWISE
			, VK_FALSE
			, 0.0f
			, 0.0f
			, 0.0f
			, 0.0f );
		auto dsState = makeVkStruct< VkPipelineDepthStencilStateCreateInfo >( 0u
			, VK_FALSE
			, VK_FALSE
			, VkCompareOp{}
			, VK_FALSE
			, VK_FALSE
			, VkStencilOpState{}
			, VkStencilOpState{}
			, float{}
			, float{} );
		VkPipelineViewportStateCreateInfo vpState = viewportState;
		VkPipelineVertexInputStateCreateInfo viState = vertexState;
		VkPipelineColorBlendStateCreateInfo cbState = blendState;
		auto & program = m_holder.getProgram( index );
		auto createInfo = makeVkStruct< VkGraphicsPipelineCreateInfo >( 0u
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
			, 0 );
		m_holder.createPipeline( index, createInfo );
	}

	//*********************************************************************************************

	LightInjectionPass::LightInjectionPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, LightType lightType
		, uint32_t gridSize
		, uint32_t rsmSize )
		: castor::Named{ pass.getName() }
		, crg::RenderPass{ pass
			, context
			, graph
			, { [this]( uint32_t index ){ doSubInitialise( index ); }
				, [this]( crg::RecordContext & context, VkCommandBuffer cb, uint32_t i ){ doSubRecordInto( context, cb, i ); } }
			, { gridSize, gridSize } }
		, m_device{ device }
		, m_rsmSize{ rsmSize }
		, m_vertexBuffer{ lpvlgt::createVertexBuffer( getName(), m_device, m_rsmSize ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, getName(), lpvlgt::getVertexProgram( lightType, m_rsmSize, device.renderSystem ) }
		, m_geometryShader{ VK_SHADER_STAGE_GEOMETRY_BIT, getName(), lpvlgt::getGeometryProgram( device.renderSystem ) }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, getName(), lpvlgt::getPixelProgram( device.renderSystem ) }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_geometryShader )
			, makeShaderState( device, m_pixelShader ) }
		, m_holder{ pass
			, context
			, graph
			, crg::pp::Config{}
				.program( crg::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
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
		: castor::Named{ pass.getName() }
		, crg::RenderPass{ pass
			, context
			, graph
			, { [this]( uint32_t index ){ doSubInitialise( index ); }
				, [this]( crg::RecordContext & context, VkCommandBuffer cb, uint32_t i ){ doSubRecordInto( context, cb, i ); } }
			, { gridSize, gridSize } }
		, m_device{ device }
		, m_rsmSize{ rsmSize }
		, m_vertexBuffer{ lpvlgt::createVertexBuffer( getName(), m_device, m_rsmSize ) }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, getName(), lpvlgt::getPointVertexProgram( face, m_rsmSize, device.renderSystem ) }
		, m_geometryShader{ VK_SHADER_STAGE_GEOMETRY_BIT, getName(), lpvlgt::getGeometryProgram( device.renderSystem ) }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, getName(), lpvlgt::getPixelProgram( device.renderSystem ) }
		, m_stages{ makeShaderState( device, m_vertexShader )
			, makeShaderState( device, m_geometryShader )
			, makeShaderState( device, m_pixelShader ) }
		, m_holder{ pass
			, context
			, graph
			, crg::pp::Config{}
				.program( crg::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
			, gridSize }
	{
	}

	void LightInjectionPass::doSubInitialise( uint32_t index )
	{
		m_holder.initialise( getRenderPass( index ), index );
	}

	void LightInjectionPass::doSubRecordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		m_holder.recordInto( context, commandBuffer, index );
		auto vplCount = m_rsmSize * m_rsmSize;
		VkDeviceSize offset{ m_vertexBuffer.getOffset() };
		VkBuffer vertexBuffer = m_vertexBuffer.getBuffer();
		m_context.vkCmdBindVertexBuffers( commandBuffer, 0u, 1u, &vertexBuffer, &offset );
		m_context.vkCmdDraw( commandBuffer, vplCount, 1u, 0u, 0u );
	}

	void LightInjectionPass::accept( ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_geometryShader );
		visitor.visit( m_pixelShader );
	}
}
