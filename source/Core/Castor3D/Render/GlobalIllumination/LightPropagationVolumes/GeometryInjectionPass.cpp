#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/GeometryInjectionPass.hpp"

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
#include <CastorUtils/Math/TransformationMatrix.hpp>

#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/GraphContext.hpp>

#include <numeric>
#include <random>

CU_ImplementSmartPtr( castor3d, GeometryInjectionPass )

namespace castor3d
{
	namespace lpvgeom
	{
		template< sdw::var::Flag FlagT >
		struct SurfaceT
			: sdw::StructInstance
		{
			SurfaceT( sdw::ShaderWriter & writer
				, sdw::expr::ExprPtr expr
				, bool enabled = true )
				: sdw::StructInstance{ writer, castor::move( expr ), enabled }
				, layer{ getMember< sdw::Int >( "layer" ) }
				, rsmPosition{ getMember< sdw::Vec3 >( "rsmPosition" ) }
				, rsmNormal{ getMember< sdw::Vec3 >( "rsmNormal" ) }
				, surfelArea{ getMember< sdw::Vec3 >( "surfelArea" ) }
				, lightPosition{ getMember< sdw::Vec3 >( "lightPosition" ) }
			{
			}

			SDW_DeclStructInstance( , SurfaceT );

			static sdw::type::IOStructPtr makeIOType( sdw::type::TypesCache & cache
				, sdw::EntryPoint entryPoint )
			{
				auto result = cache.getIOStruct( "C3D_GeomInjSurface"
					, entryPoint
					, FlagT );

				if ( result->empty() )
				{
					uint32_t index = 0u;
					result->declMember( "layer"
						, sdw::type::Kind::eInt
						, sdw::type::NotArray
						, index );
					++index;
					result->declMember( "rsmPosition"
						, sdw::type::Kind::eVec3F
						, sdw::type::NotArray
						, index );
					++index;
					result->declMember( "rsmNormal"
						, sdw::type::Kind::eVec3F
						, sdw::type::NotArray
						, index );
					++index;
					result->declMember( "surfelArea"
						, sdw::type::Kind::eFloat
						, sdw::type::NotArray
						, index );
					++index;
					result->declMember( "lightPosition"
						, sdw::type::Kind::eVec3F
						, sdw::type::NotArray
						, index );
				}

				return result;
			}

			static sdw::type::BaseStructPtr makeType( sdw::type::TypesCache & cache )
			{
				auto result = cache.getStruct( sdw::type::MemoryLayout::eStd430
					, "C3D_GeomInjSurface" );

				if ( result->empty() )
				{
					result->declMember( "layer"
						, sdw::type::Kind::eInt
						, sdw::type::NotArray );
					result->declMember( "rsmPosition"
						, sdw::type::Kind::eVec3F
						, sdw::type::NotArray );
					result->declMember( "rsmNormal"
						, sdw::type::Kind::eVec3F
						, sdw::type::NotArray );
					result->declMember( "surfelArea"
						, sdw::type::Kind::eFloat
						, sdw::type::NotArray );
					result->declMember( "lightPosition"
						, sdw::type::Kind::eVec3F
						, sdw::type::NotArray );
				}

				return result;
			}

			sdw::Int layer;
			sdw::Vec3 rsmPosition;
			sdw::Vec3 rsmNormal;
			sdw::Float surfelArea;
			sdw::Vec3 lightPosition;
		};

		static void getDirectionalVertexProgram( uint32_t rsmTexSize
			, RenderSystem const & renderSystem
			, sdw::TraditionalGraphicsWriter & writer
			, castor3d::shader::LpvGridData const & lpvGridData )
		{
			auto c3d_rsmNormalMap = writer.declCombinedImg< FImg2DArrayRgba32 >( castor::toUtf8( getTextureName( LightType::eDirectional, SmTexture::eNormal ) )
				, GeometryInjectionPass::RsmNormalsIdx
				, 0u );
			auto c3d_rsmPositionMap = writer.declCombinedImg< FImg2DArrayRgba32 >( castor::toUtf8( getTextureName( LightType::eDirectional, SmTexture::ePosition ) )
				, GeometryInjectionPass::RsmPositionIdx
				, 0u );
			C3D_LpvLightConfig( writer, GeometryInjectionPass::LpvLightUboIdx, 0u );

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
				, GeometryInjectionPass::LightsIdx /* lightBinding */
				, 0u /* lightSet */
				, index /* shadowMapBinding */
				, 1u /* shadowMapSet */ };

			//Sample from camera
			auto calculateSurfelAreaLightViewM = writer.implementFunction< sdw::Float >( "calculateSurfelAreaLightViewM"
				, [&]( sdw::Vec3 const & viewPos )
				{
					writer.returnStmt( ( 4.0_f * viewPos.z() * viewPos.z() * c3d_lpvLightData.tanFovXHalf() * c3d_lpvLightData.tanFovYHalf() ) / sdw::Float{ float( rsmTexSize * rsmTexSize ) } );
				}
				, sdw::InVec3{ writer, "viewPos" } );

			writer.implementEntryPointT< sdw::VoidT, lpvgeom::SurfaceT >( [&]( sdw::VertexIn const & in
				, sdw::VertexOutT< lpvgeom::SurfaceT > out )
				{
					auto light = writer.declLocale( "light"
						, lights.getDirectionalLight( writer.cast< sdw::UInt >( c3d_lpvLightData.lightOffset() ) ) );
					auto cascadeIndex = writer.declLocale( "cascadeIndex"
						, writer.cast< sdw::Int >( max( 1_u, light.cascadeCount() ) - 1_u ) );
					auto rsmCoords = writer.declLocale( "rsmCoords"
						, ivec3( in.vertexIndex % int32_t( rsmTexSize )
							, in.vertexIndex / int32_t( rsmTexSize )
							, cascadeIndex ) );

					out.rsmPosition = c3d_rsmPositionMap.fetch( rsmCoords, 0_i ).rgb();
					out.rsmNormal = c3d_rsmNormalMap.fetch( rsmCoords, 0_i ).rgb();
					auto viewPos = writer.declLocale( "viewPos"
						, c3d_lpvLightData.lightView() * vec4( out.rsmPosition, 1.0 ) );
					out.surfelArea = calculateSurfelAreaLightViewM( viewPos.xyz() ) * c3d_lpvLightData.texelAreaModifier();
					out.lightPosition = out.rsmPosition - light.direction();

					auto volumeCellIndex = writer.declLocale( "volumeCellIndex"
						, lpvGridData.worldToGrid( out.rsmPosition ) );
					out.layer = volumeCellIndex.z();

					auto screenPos = writer.declLocale( "screenPos"
						, lpvGridData.gridToScreen( volumeCellIndex.xy() ) );

					out.vtx.position = vec4( screenPos, 0.0, 1.0 );
				} );
		}

		static void getSpotVertexProgram( uint32_t rsmTexSize
			, RenderSystem const & renderSystem
			, sdw::TraditionalGraphicsWriter & writer
			, castor3d::shader::LpvGridData const & lpvGridData )
		{
			auto c3d_rsmNormalMap = writer.declCombinedImg< FImg2DArrayRgba32 >( castor::toUtf8( getTextureName( LightType::eSpot, SmTexture::eNormal ) )
				, GeometryInjectionPass::RsmNormalsIdx
				, 0u );
			auto c3d_rsmPositionMap = writer.declCombinedImg< FImg2DArrayRgba32 >( castor::toUtf8( getTextureName( LightType::eSpot, SmTexture::ePosition ) )
				, GeometryInjectionPass::RsmPositionIdx
				, 0u );
			C3D_LpvLightConfig( writer, GeometryInjectionPass::LpvLightUboIdx, 0u );

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
				, GeometryInjectionPass::LightsIdx /* lightBinding */
				, 0u /* lightSet */
				, index /* shadowMapBinding */
				, 1u /* shadowMapSet */ };

			//Sample from camera
			auto calculateSurfelAreaLightViewM = writer.implementFunction< sdw::Float >( "calculateSurfelAreaLightViewM"
				, [&]( sdw::Vec3 const & viewPos )
				{
					writer.returnStmt( ( 4.0f * viewPos.z() * viewPos.z() * c3d_lpvLightData.tanFovXHalf() * c3d_lpvLightData.tanFovYHalf() ) / sdw::Float{ float( rsmTexSize * rsmTexSize ) } );
				}
				, sdw::InVec3{ writer, "viewPos" } );

			writer.implementEntryPointT< sdw::VoidT, lpvgeom::SurfaceT >( [&]( sdw::VertexIn const & in
				, sdw::VertexOutT< lpvgeom::SurfaceT > out )
				{
					auto light = writer.declLocale( "light"
						, lights.getSpotLight( writer.cast< sdw::UInt >( c3d_lpvLightData.lightOffset() ) ) );
					auto rsmCoords = writer.declLocale( "rsmCoords"
						, ivec3( in.vertexIndex % int32_t( rsmTexSize )
							, in.vertexIndex / int32_t( rsmTexSize )
							, light.shadowMapIndex() ) );

					out.rsmPosition = c3d_rsmPositionMap.fetch( rsmCoords, 0_i ).rgb();
					out.rsmNormal = c3d_rsmNormalMap.fetch( rsmCoords, 0_i ).rgb();
					auto viewPos = writer.declLocale( "viewPos"
						, c3d_lpvLightData.lightView() * vec4( out.rsmPosition, 1.0 ) );
					out.surfelArea = calculateSurfelAreaLightViewM( viewPos.xyz() ) * c3d_lpvLightData.texelAreaModifier();
					out.lightPosition = light.position();

					auto volumeCellIndex = writer.declLocale( "volumeCellIndex"
						, lpvGridData.worldToGrid( out.rsmPosition ) );
					out.layer = volumeCellIndex.z();

					auto screenPos = writer.declLocale( "screenPos"
						, lpvGridData.gridToScreen( volumeCellIndex.xy() ) );

					out.vtx.position = vec4( screenPos, 0.0, 1.0 );
				} );
		}

		static void getPointVertexProgram( CubeMapFace face
			, uint32_t rsmTexSize
			, RenderSystem const & renderSystem
			, sdw::TraditionalGraphicsWriter & writer
			, castor3d::shader::LpvGridData const & lpvGridData )
		{
			auto c3d_rsmNormalMap = writer.declCombinedImg< FImg2DArrayRgba32 >( castor::toUtf8( getTextureName( LightType::ePoint, SmTexture::eNormal ) )
				, GeometryInjectionPass::RsmNormalsIdx
				, 0u );
			auto c3d_rsmPositionMap = writer.declCombinedImg< FImg2DArrayRgba32 >( castor::toUtf8( getTextureName( LightType::ePoint, SmTexture::ePosition ) )
				, GeometryInjectionPass::RsmPositionIdx
				, 0u );
			C3D_LpvLightConfig( writer, GeometryInjectionPass::LpvLightUboIdx, 0u );

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
				, GeometryInjectionPass::LightsIdx /* lightBinding */
				, 0u /* lightSet */
				, index /* shadowMapBinding */
				, 1u /* shadowMapSet */ };

			//Sample from camera
			auto calculateSurfelAreaLightViewM = writer.implementFunction< sdw::Float >( "calculateSurfelAreaLightViewM"
				, [&]( sdw::Vec3 const & viewPos )
				{
					writer.returnStmt( ( 4.0_f * viewPos.z() * viewPos.z() * c3d_lpvLightData.tanFovXHalf() * c3d_lpvLightData.tanFovYHalf() ) / sdw::Float{ float( rsmTexSize * rsmTexSize ) } );
				}
				, sdw::InVec3{ writer, "viewPos" } );

			writer.implementEntryPointT< sdw::VoidT, lpvgeom::SurfaceT >( [&]( sdw::VertexIn const & in
				, sdw::VertexOutT< lpvgeom::SurfaceT > out )
				{
					auto light = writer.declLocale( "light"
						, lights.getPointLight( writer.cast< sdw::UInt >( c3d_lpvLightData.lightOffset() ) ) );
					auto rsmCoords = writer.declLocale( "rsmCoords"
						, ivec3( in.vertexIndex % int32_t( rsmTexSize )
							, in.vertexIndex / int32_t( rsmTexSize )
							, light.shadowMapIndex() * 6_i + int32_t( face ) ) );

					out.rsmPosition = c3d_rsmPositionMap.fetch( rsmCoords, 0_i ).rgb();
					out.rsmNormal = c3d_rsmNormalMap.fetch( rsmCoords, 0_i ).rgb();
					auto viewPos = writer.declLocale( "viewPos"
						, c3d_lpvLightData.lightView() * vec4( out.rsmPosition, 1.0 ) );
					out.surfelArea = calculateSurfelAreaLightViewM( viewPos.xyz() ) * c3d_lpvLightData.texelAreaModifier();
					out.lightPosition = light.position();

					auto volumeCellIndex = writer.declLocale( "volumeCellIndex"
						, lpvGridData.worldToGrid( out.rsmPosition ) );
					out.layer = volumeCellIndex.z();

					auto screenPos = writer.declLocale( "screenPos"
						, lpvGridData.gridToScreen( volumeCellIndex.xy() ) );

					out.vtx.position = vec4( screenPos, 0.0, 1.0 );
				} );
		}

		static void getVertexProgram( LightType lightType
			, uint32_t rsmTexSize
			, RenderSystem const & renderSystem
			, sdw::TraditionalGraphicsWriter & writer
			, castor3d::shader::LpvGridData const & lpvGridData )
		{
			switch ( lightType )
			{
			case castor3d::LightType::eDirectional:
				getDirectionalVertexProgram( rsmTexSize, renderSystem, writer, lpvGridData );
				break;
			case castor3d::LightType::eSpot:
				getSpotVertexProgram( rsmTexSize, renderSystem, writer, lpvGridData );
				break;
			default:
				CU_Failure( "Unsupported light type" );
				break;
			}
		}

		static void getGeomFragProgram( sdw::TraditionalGraphicsWriter & writer
			, castor3d::shader::LpvGridData const & lpvGridData )
		{
			/*Cosine lobe coeff*/
			auto SH_cosLobe_C0 = writer.declConstant( "SH_cosLobe_C0"
				, 0.886226925_f );// sqrt(pi) / 2 
			auto SH_cosLobe_C1 = writer.declConstant( "SH_cosLobe_C1"
				, 1.02332671_f ); // sqrt(pi / 3)

			//layout( early_fragment_tests )in;//turn on early depth tests

			auto outGeometryVolume = writer.declOutput< sdw::Vec4 >( "outGeometryVolume", sdw::EntryPoint::eFragment, 0u );

			//Should I normalize the dir vector?
			auto evalCosineLobeToDir = writer.implementFunction< sdw::Vec4 >( "evalCosineLobeToDir"
				, [&]( sdw::Vec3 dir )
				{
					dir = normalize( dir );
					//f00, f-11, f01, f11
					writer.returnStmt( vec4( SH_cosLobe_C0
						, -SH_cosLobe_C1 * dir.y()
						, SH_cosLobe_C1 * dir.z()
						, -SH_cosLobe_C1 * dir.x() ) );
				}
				, sdw::InVec3{ writer, "dir" } );

			//(As * clamp(dot(ns,w),0.0,1.0))/(cellsize * cellsize)
			auto calculateBlockingPotential = writer.implementFunction< sdw::Float >( "calculateBlockingPotential"
				, [&]( sdw::Vec3 const & dir
					, sdw::Vec3 const & normal
					, sdw::Float const & surfelArea )
				{
					writer.returnStmt( clamp( ( surfelArea * clamp( dot( normal, dir ), 0.0_f, 1.0_f ) ) / ( lpvGridData.cellSize() * lpvGridData.cellSize() )
						, 0.0_f
						, 1.0_f ) ); //It is probability so 0.0 - 1.0
				}
				, sdw::InVec3{ writer, "dir" }
				, sdw::InVec3{ writer, "normal" }
				, sdw::InFloat{ writer, "surfelArea" } );
			
			writer.implementEntryPointT< 1u, sdw::PointListT< lpvgeom::SurfaceT >, sdw::PointStreamT< lpvgeom::SurfaceT > >( [&]( sdw::GeometryIn const &
				, sdw::PointListT< lpvgeom::SurfaceT > const & list
				, sdw::PointStreamT< lpvgeom::SurfaceT > out )
				{
					out.vtx.position = list[0].vtx.position;
					out.layer = list[0].layer;
					out.vtx.pointSize = 1.0f;

					out.rsmPosition = list[0].rsmPosition;
					out.rsmNormal = list[0].rsmNormal;
					out.surfelArea = list[0].surfelArea;
					out.lightPosition = list[0].lightPosition;

					out.append();
					out.restartStrip();
				} );

			writer.implementEntryPointT< lpvgeom::SurfaceT, sdw::VoidT >( [&]( sdw::FragmentInT< lpvgeom::SurfaceT > const & in
				, sdw::FragmentOut const & )
				{
					//Discard pixels with really small normal
					IF( writer, length( in.rsmNormal ) < 0.01_f )
					{
						writer.demote();
					}
					FI

					auto lightDir = writer.declLocale( "lightDir"
						, normalize( in.lightPosition - in.rsmPosition ) );
					auto blockingPotential = writer.declLocale( "blockingPotential"
						, calculateBlockingPotential( lightDir, in.rsmNormal, in.surfelArea ) );

					auto SHCoeffGV = writer.declLocale( "SHCoeffGV"
						, evalCosineLobeToDir( in.rsmNormal ) * blockingPotential );

					outGeometryVolume = SHCoeffGV;
				} );
		}

		static ShaderPtr getProgram( LightType lightType
			, uint32_t rsmTexSize
			, RenderSystem const & renderSystem )
		{
			sdw::TraditionalGraphicsWriter writer{ &renderSystem.getEngine()->getShaderAllocator() };
			C3D_LpvGridConfig( writer, GeometryInjectionPass::LpvGridUboIdx, 0u, true );
			getVertexProgram( lightType, rsmTexSize, renderSystem, writer, c3d_lpvGridData );
			getGeomFragProgram( writer, c3d_lpvGridData );
			return castor::make_unique< sdw::Shader >( castor::move( writer.getShader() ) );
		}

		static ShaderPtr getProgram( CubeMapFace face
			, uint32_t rsmTexSize
			, RenderSystem const & renderSystem )
		{
			sdw::TraditionalGraphicsWriter writer{ &renderSystem.getEngine()->getShaderAllocator() };
			C3D_LpvGridConfig( writer, GeometryInjectionPass::LpvGridUboIdx, 0u, true );
			getPointVertexProgram( face, rsmTexSize, renderSystem, writer, c3d_lpvGridData );
			getGeomFragProgram( writer, c3d_lpvGridData );
			return castor::make_unique< sdw::Shader >( castor::move( writer.getShader() ) );
		}

		static GpuBufferOffsetT< NonTexturedQuad::Vertex > createVertexBuffer( RenderDevice const & device
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

	GeometryInjectionPass::PipelineHolder::PipelineHolder( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, crg::pp::Config config
		, uint32_t lpvSize )
		: m_holder{ pass, context, graph, castor::move( config ), VK_PIPELINE_BIND_POINT_GRAPHICS, 1u }
		, m_lpvSize{ lpvSize }
	{
	}

	void GeometryInjectionPass::PipelineHolder::initialise( VkRenderPass renderPass
		, uint32_t index )
	{
		m_renderPass = renderPass;
		m_holder.initialise();

		if ( !m_holder.getPipeline( index ) )
		{
			doCreatePipeline( index );
		}
	}

	void GeometryInjectionPass::PipelineHolder::recordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		m_holder.recordInto( context, commandBuffer, index );
	}

	void GeometryInjectionPass::PipelineHolder::doCreatePipeline( uint32_t index )
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
		auto blendState = RenderNodesPass::createBlendState( BlendMode::eNoBlend, BlendMode::eNoBlend, 1u );
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

	GeometryInjectionPass::GeometryInjectionPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, LightType lightType
		, uint32_t gridSize
		, uint32_t rsmSize )
		: castor::Named{ castor::makeString( pass.getName() ) }
		, crg::RenderPass{ pass
			, context
			, graph
			, { [this]( uint32_t index ){ doSubInitialise( index ); }
				, [this]( crg::RecordContext & context, VkCommandBuffer cb, uint32_t i ){ doSubRecordInto( context, cb, i ); } }
			, { gridSize, gridSize } }
		, m_device{ device }
		, m_rsmSize{ rsmSize }
		, m_vertexBuffer{ lpvgeom::createVertexBuffer( m_device, m_rsmSize ) }
		, m_shader{ getName(), lpvgeom::getProgram( lightType, m_rsmSize, device.renderSystem ) }
		, m_stages{ makeProgramStates( device, m_shader ) }
		, m_holder{ pass
			, context
			, graph
			, crg::pp::Config{}
				.program( crg::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
			, gridSize }
	{
	}
	
	GeometryInjectionPass::GeometryInjectionPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, CubeMapFace face
		, uint32_t gridSize
		, uint32_t rsmSize )
		: castor::Named{ castor::makeString( pass.getName() ) }
		, crg::RenderPass{ pass
			, context
			, graph
			, { [this]( uint32_t index ){ doSubInitialise( index ); }
				, [this]( crg::RecordContext & context, VkCommandBuffer cb, uint32_t i ){ doSubRecordInto( context, cb, i ); } }
			, { gridSize, gridSize } }
		, m_device{ device }
		, m_rsmSize{ rsmSize }
		, m_vertexBuffer{ lpvgeom::createVertexBuffer( m_device, m_rsmSize ) }
		, m_shader{ getName(), lpvgeom::getProgram( face, m_rsmSize, device.renderSystem ) }
		, m_stages{ makeProgramStates( device, m_shader ) }
		, m_holder{ pass
			, context
			, graph
			, crg::pp::Config{}
				.program( crg::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
			, gridSize }
	{
	}

	void GeometryInjectionPass::doSubInitialise( uint32_t index )
	{
		m_holder.initialise( getRenderPass( index ), index );
	}

	void GeometryInjectionPass::doSubRecordInto( crg::RecordContext & context
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

	void GeometryInjectionPass::accept( ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_shader );
	}

	Texture GeometryInjectionPass::createResult( crg::ResourcesCache & resources
		, RenderDevice const & device
		, castor::String const & prefix
		, uint32_t index
		, uint32_t gridSize )
	{
		return { device
			, resources
			, prefix + cuT( "GeometryInjection" ) + castor::string::toString( index )
			, VK_IMAGE_CREATE_2D_ARRAY_COMPATIBLE_BIT
			, VkExtent3D{ gridSize, gridSize, gridSize }
			, 1u
			, 1u
			, VK_FORMAT_R16G16B16A16_SFLOAT
			, ( VK_IMAGE_USAGE_TRANSFER_DST_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_STORAGE_BIT
				| VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT )
			, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK
			, VK_COMPARE_OP_NEVER
			, false };
	}
}
