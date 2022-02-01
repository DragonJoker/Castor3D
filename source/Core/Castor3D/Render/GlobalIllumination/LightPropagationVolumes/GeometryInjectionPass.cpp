#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/GeometryInjectionPass.hpp"

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
#include <CastorUtils/Math/TransformationMatrix.hpp>

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
		template< sdw::var::Flag FlagT >
		struct SurfaceT
			: sdw::StructInstance
		{
			SurfaceT( sdw::ShaderWriter & writer
				, sdw::expr::ExprPtr expr
				, bool enabled = true )
				: sdw::StructInstance{ writer, std::move( expr ), enabled }
				, volumeCellIndex{ getMember< sdw::IVec3 >( "volumeCellIndex" ) }
				, rsmPosition{ getMember< sdw::Vec3 >( "rsmPosition" ) }
				, rsmNormal{ getMember< sdw::Vec3 >( "rsmNormal" ) }
				, surfelArea{ getMember< sdw::Vec3 >( "surfelArea" ) }
				, lightPosition{ getMember< sdw::Vec3 >( "lightPosition" ) }
			{
			}

			SDW_DeclStructInstance( , SurfaceT );

			static sdw::type::IOStructPtr makeIOType( sdw::type::TypesCache & cache )
			{
				auto result = cache.getIOStruct( sdw::type::MemoryLayout::eStd430
					, ( FlagT == sdw::var::Flag::eShaderOutput
						? std::string{ "Output" }
						: std::string{ "Input" } ) + "Surface"
					, FlagT );

				if ( result->empty() )
				{
					uint32_t index = 0u;
					result->declMember( "volumeCellIndex"
						, sdw::type::Kind::eVec3I
						, sdw::type::NotArray
						, index++ );
					result->declMember( "rsmPosition"
						, sdw::type::Kind::eVec3F
						, sdw::type::NotArray
						, index++ );
					result->declMember( "rsmNormal"
						, sdw::type::Kind::eVec3F
						, sdw::type::NotArray
						, index++ );
					result->declMember( "surfelArea"
						, sdw::type::Kind::eFloat
						, sdw::type::NotArray
						, index++ );
					result->declMember( "lightPosition"
						, sdw::type::Kind::eVec3F
						, sdw::type::NotArray
						, index++ );
				}

				return result;
			}

			sdw::IVec3 volumeCellIndex;
			sdw::Vec3 rsmPosition;
			sdw::Vec3 rsmNormal;
			sdw::Float surfelArea;
			sdw::Vec3 lightPosition;
		};

		std::unique_ptr< ast::Shader > getDirectionalVertexProgram( uint32_t rsmTexSize
			, RenderSystem const & renderSystem )
		{
			using namespace sdw;
			VertexWriter writer;

			if constexpr ( shader::DirectionalMaxCascadesCount > 1u )
			{
				auto inPosition = writer.declInput< Vec2 >( "inPosition", 0u );
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

				// Utility functions
				shader::Utils utils{ writer, *renderSystem.getEngine() };
				uint32_t index = 0;
				auto lightingModel = shader::LightingModel::createModel( utils
					, renderSystem.getEngine()->getPassFactory().getLightingModelName( 1u )
					, LightType::eDirectional
					, GeometryInjectionPass::LightsIdx
					, 0u
					, false
					, shader::ShadowOptions{ SceneFlag::eNone, true }
					, index
					, 1u
					, renderSystem.getGpuInformations().hasShaderStorageBuffers() );

				//Sample from camera
				auto calculateSurfelAreaLightViewM = writer.implementFunction< Float >( "calculateSurfelAreaLightViewM"
					, [&]( Vec3 viewPos )
					{
						writer.returnStmt( ( 4.0_f * viewPos.z() * viewPos.z() * c3d_lpvLightData.tanFovXHalf * c3d_lpvLightData.tanFovYHalf ) / Float{ float( rsmTexSize * rsmTexSize ) } );
					}
					, InVec3{ writer, "viewPos" } );

				writer.implementMainT< VoidT, SurfaceT >( [&]( VertexIn in
					, VertexOutT< SurfaceT > out )
					{
						auto light = writer.declLocale( "light"
							, lightingModel->getDirectionalLight( writer.cast< UInt >( c3d_lpvLightData.lightIndex ) ) );
						auto cascadeIndex = writer.declLocale( "cascadeIndex"
							, writer.cast< Int >( max( 1_u, light.m_cascadeCount ) - 1_u ) );
#if C3D_UseTiledDirectionalShadowMap
						auto rsmCoords = writer.declLocale( "rsmCoords"
							, ivec2( in.vertexIndex % rsmTexSize
								, in.vertexIndex / rsmTexSize ) );
#else
						auto rsmCoords = writer.declLocale( "rsmCoords"
							, ivec3( in.vertexIndex % int32_t( rsmTexSize )
								, in.vertexIndex / int32_t( rsmTexSize )
								, cascadeIndex ) );
#endif

						out.rsmPosition = c3d_rsmPositionMap.fetch( rsmCoords, 0_i ).rgb();
						out.rsmNormal = c3d_rsmNormalMap.fetch( rsmCoords, 0_i ).rgb();
						auto viewPos = writer.declLocale( "viewPos"
							, c3d_lpvLightData.lightView * vec4( out.rsmPosition, 1.0 ) );
						out.surfelArea = calculateSurfelAreaLightViewM( viewPos.xyz() ) * c3d_lpvLightData.texelAreaModifier;
						out.lightPosition = out.rsmPosition - light.m_direction;

						out.volumeCellIndex = c3d_lpvGridData.worldToGrid( out.rsmPosition );

						auto screenPos = writer.declLocale( "screenPos"
							, c3d_lpvGridData.gridToScreen( out.volumeCellIndex.xy() ) );

						out.vtx.position = vec4( screenPos, 0.0, 1.0 );
					} );
			}
			else
			{
				auto inPosition = writer.declInput< Vec2 >( "inPosition", 0u );
				auto c3d_rsmNormalMap = writer.declSampledImage< FImg2DRgba32 >( getTextureName( LightType::eDirectional, SmTexture::eNormalLinear )
					, GeometryInjectionPass::RsmNormalsIdx
					, 0u );
				auto c3d_rsmPositionMap = writer.declSampledImage< FImg2DRgba32 >( getTextureName( LightType::eDirectional, SmTexture::ePosition )
					, GeometryInjectionPass::RsmPositionIdx
					, 0u );
				UBO_LPVGRIDCONFIG( writer, GeometryInjectionPass::LpvGridUboIdx, 0u, true );
				UBO_LPVLIGHTCONFIG( writer, GeometryInjectionPass::LpvLightUboIdx, 0u );

				// Utility functions
				shader::Utils utils{ writer, *renderSystem.getEngine() };
				uint32_t index = 0u;
				auto lightingModel = shader::LightingModel::createModel( utils
					, renderSystem.getEngine()->getPassFactory().getLightingModelName( 1u )
					, LightType::eDirectional
					, GeometryInjectionPass::LightsIdx
					, 0u
					, false
					, shader::ShadowOptions{ SceneFlag::eNone, true }
					, index
					, 1u
					, renderSystem.getGpuInformations().hasShaderStorageBuffers() );

				//Sample from camera
				auto calculateSurfelAreaLightViewM = writer.implementFunction< Float >( "calculateSurfelAreaLightViewM"
					, [&]( Vec3 viewPos )
					{
						writer.returnStmt( ( 4.0_f * viewPos.z() * viewPos.z() * c3d_lpvLightData.tanFovXHalf * c3d_lpvLightData.tanFovYHalf ) / Float{ float( rsmTexSize * rsmTexSize ) } );
					}
					, InVec3{ writer, "viewPos" } );

				writer.implementMainT< VoidT, SurfaceT >( [&]( VertexIn in
					, VertexOutT< SurfaceT > out )
					{
						auto light = writer.declLocale( "light"
							, lightingModel->getDirectionalLight( writer.cast< UInt >( c3d_lpvLightData.lightIndex ) ) );
						auto rsmCoords = writer.declLocale( "rsmCoords"
							, ivec2( in.vertexIndex % int32_t( rsmTexSize )
								, in.vertexIndex / int32_t( rsmTexSize ) ) );

						out.rsmPosition = c3d_rsmPositionMap.fetch( rsmCoords, 0_i ).rgb();
						out.rsmNormal = c3d_rsmNormalMap.fetch( rsmCoords, 0_i ).rgb();
						auto viewPos = writer.declLocale( "viewPos"
							, c3d_lpvLightData.lightView * vec4( out.rsmPosition, 1.0 ) );
						out.surfelArea = calculateSurfelAreaLightViewM( viewPos.xyz() ) * c3d_lpvLightData.texelAreaModifier;
						out.lightPosition = out.rsmPosition - light.m_direction;

						out.volumeCellIndex = c3d_lpvGridData.worldToGrid( out.rsmPosition );

						auto screenPos = writer.declLocale( "screenPos"
							, c3d_lpvGridData.gridToScreen( out.volumeCellIndex.xy() ) );

						out.vtx.position = vec4( screenPos, 0.0, 1.0 );
					} );
			}

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< ast::Shader > getSpotVertexProgram( uint32_t rsmTexSize
			, RenderSystem const & renderSystem )
		{
			using namespace sdw;
			VertexWriter writer;

			auto inPosition = writer.declInput< Vec2 >( "inPosition", 0u );
			auto c3d_rsmNormalMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( LightType::eSpot, SmTexture::eNormalLinear )
				, GeometryInjectionPass::RsmNormalsIdx
				, 0u );
			auto c3d_rsmPositionMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( LightType::eSpot, SmTexture::ePosition )
				, GeometryInjectionPass::RsmPositionIdx
				, 0u );
			UBO_LPVGRIDCONFIG( writer, GeometryInjectionPass::LpvGridUboIdx, 0u, true );
			UBO_LPVLIGHTCONFIG( writer, GeometryInjectionPass::LpvLightUboIdx, 0u );

			// Utility functions
			shader::Utils utils{ writer, *renderSystem.getEngine() };
			uint32_t index = 0u;
			auto lightingModel = shader::LightingModel::createModel( utils
				, renderSystem.getEngine()->getPassFactory().getLightingModelName( 1u )
				, LightType::eSpot
				, GeometryInjectionPass::LightsIdx
				, 0u
				, false
				, shader::ShadowOptions{ SceneFlag::eNone, true }
				, index
				, 1u
				, renderSystem.getGpuInformations().hasShaderStorageBuffers() );

			//Sample from camera
			auto calculateSurfelAreaLightViewM = writer.implementFunction< Float >( "calculateSurfelAreaLightViewM"
				, [&]( Vec3 viewPos )
				{
					writer.returnStmt( ( 4.0 * viewPos.z() * viewPos.z() * c3d_lpvLightData.tanFovXHalf * c3d_lpvLightData.tanFovYHalf ) / Float{ float( rsmTexSize * rsmTexSize ) } );
				}
				, InVec3{ writer, "viewPos" } );

			writer.implementMainT< VoidT, SurfaceT >( [&]( VertexIn in
				, VertexOutT< SurfaceT > out )
				{
					auto light = writer.declLocale( "light"
						, lightingModel->getSpotLight( writer.cast< UInt >( c3d_lpvLightData.lightIndex ) ) );
					auto rsmCoords = writer.declLocale( "rsmCoords"
						, ivec3( in.vertexIndex % int32_t( rsmTexSize )
							, in.vertexIndex / int32_t( rsmTexSize )
							, light.m_lightBase.m_index ) );

					out.rsmPosition = c3d_rsmPositionMap.fetch( rsmCoords, 0_i ).rgb();
					out.rsmNormal = c3d_rsmNormalMap.fetch( rsmCoords, 0_i ).rgb();
					auto viewPos = writer.declLocale( "viewPos"
						, c3d_lpvLightData.lightView * vec4( out.rsmPosition, 1.0 ) );
					out.surfelArea = calculateSurfelAreaLightViewM( viewPos.xyz() ) * c3d_lpvLightData.texelAreaModifier;
					out.lightPosition = light.m_position;

					out.volumeCellIndex = c3d_lpvGridData.worldToGrid( out.rsmPosition );

					auto screenPos = writer.declLocale( "screenPos"
						, c3d_lpvGridData.gridToScreen( out.volumeCellIndex.xy() ) );

					out.vtx.position = vec4( screenPos, 0.0, 1.0 );
				} );
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
				, GeometryInjectionPass::RsmNormalsIdx
				, 0u );
			auto c3d_rsmPositionMap = writer.declSampledImage< FImg2DArrayRgba32 >( getTextureName( LightType::ePoint, SmTexture::ePosition )
				, GeometryInjectionPass::RsmPositionIdx
				, 0u );
			UBO_LPVGRIDCONFIG( writer, GeometryInjectionPass::LpvGridUboIdx, 0u, true );
			UBO_LPVLIGHTCONFIG( writer, GeometryInjectionPass::LpvLightUboIdx, 0u );

			// Utility functions
			shader::Utils utils{ writer, *renderSystem.getEngine() };
			uint32_t index = 0u;
			auto lightingModel = shader::LightingModel::createModel( utils
				, renderSystem.getEngine()->getPassFactory().getLightingModelName( 1u )
				, LightType::ePoint
				, GeometryInjectionPass::LightsIdx
				, 0u
				, false
				, shader::ShadowOptions{ SceneFlag::eNone, true }
				, index
				, 1u
				, renderSystem.getGpuInformations().hasShaderStorageBuffers() );

			//Sample from camera
			auto calculateSurfelAreaLightViewM = writer.implementFunction< Float >( "calculateSurfelAreaLightViewM"
				, [&]( Vec3 viewPos )
				{
					writer.returnStmt( ( 4.0 * viewPos.z() * viewPos.z() * c3d_lpvLightData.tanFovXHalf * c3d_lpvLightData.tanFovYHalf ) / Float{ float( rsmTexSize * rsmTexSize ) } );
				}
				, InVec3{ writer, "viewPos" } );

			writer.implementMainT< VoidT, SurfaceT >( [&]( VertexIn in
				, VertexOutT< SurfaceT > out )
				{
					auto light = writer.declLocale( "light"
						, lightingModel->getPointLight( writer.cast< UInt >( c3d_lpvLightData.lightIndex ) ) );
					auto rsmCoords = writer.declLocale( "rsmCoords"
						, ivec3( in.vertexIndex % int32_t( rsmTexSize )
							, in.vertexIndex / int32_t( rsmTexSize )
							, light.m_lightBase.m_index * 6_i + int32_t( face ) ) );

					out.rsmPosition = c3d_rsmPositionMap.fetch( rsmCoords, 0_i ).rgb();
					out.rsmNormal = c3d_rsmNormalMap.fetch( rsmCoords, 0_i ).rgb();
					auto viewPos = writer.declLocale( "viewPos"
						, c3d_lpvLightData.lightView * vec4( out.rsmPosition, 1.0 ) );
					out.surfelArea = calculateSurfelAreaLightViewM( viewPos.xyz() ) * c3d_lpvLightData.texelAreaModifier;
					out.lightPosition = light.m_position;

					out.volumeCellIndex = c3d_lpvGridData.worldToGrid( out.rsmPosition );

					auto screenPos = writer.declLocale( "screenPos"
						, c3d_lpvGridData.gridToScreen( out.volumeCellIndex.xy() ) );

					out.vtx.position = vec4( screenPos, 0.0, 1.0 );
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

			writer.implementMainT< 1u, PointListT< SurfaceT >, PointStreamT< SurfaceT > >( [&]( GeometryIn in
				, PointListT< SurfaceT > list
				, PointStreamT< SurfaceT > out )
				{
					out.vtx.position = list[0].vtx.position;
					out.layer = list[0].volumeCellIndex.z();
					out.vtx.pointSize = 1.0f;

					out.volumeCellIndex = list[0].volumeCellIndex;
					out.rsmPosition = list[0].rsmPosition;
					out.rsmNormal = list[0].rsmNormal;
					out.surfelArea = list[0].surfelArea;
					out.lightPosition = list[0].lightPosition;

					out.append();
					out.restartStrip();
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		ShaderPtr getPixelProgram()
		{
			using namespace sdw;
			FragmentWriter writer;

			/*Cosine lobe coeff*/
			auto SH_cosLobe_C0 = writer.declConstant( "SH_cosLobe_C0"
				, 0.886226925_f );// sqrt(pi) / 2 
			auto SH_cosLobe_C1 = writer.declConstant( "SH_cosLobe_C1"
				, 1.02332671_f ); // sqrt(pi / 3)

			//layout( early_fragment_tests )in;//turn on early depth tests

			UBO_LPVGRIDCONFIG( writer, GeometryInjectionPass::LpvGridUboIdx, 0u, true );

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
					, Vec3 normal
					, Float surfelArea )
				{
					writer.returnStmt( clamp( ( surfelArea * clamp( dot( normal, dir ), 0.0_f, 1.0_f ) ) / ( c3d_lpvGridData.cellSize() * c3d_lpvGridData.cellSize() )
						, 0.0_f
						, 1.0_f ) ); //It is probability so 0.0 - 1.0
				}
				, InVec3{ writer, "dir" }
				, InVec3{ writer, "normal" }
				, InFloat{ writer, "surfelArea" } );

			writer.implementMainT< SurfaceT, VoidT >( [&]( FragmentInT< SurfaceT > in
				, FragmentOut out )
				{
					//Discard pixels with really small normal
					IF( writer, length( in.rsmNormal ) < 0.01_f )
					{
						writer.demote();
					}
					FI;

					auto lightDir = writer.declLocale( "lightDir"
						, normalize( in.lightPosition - in.rsmPosition ) );
					auto blockingPotential = writer.declLocale( "blockingPotential"
						, calculateBlockingPotential( lightDir, in.rsmNormal, in.surfelArea ) );

					auto SHCoeffGV = writer.declLocale( "SHCoeffGV"
						, evalCosineLobeToDir( in.rsmNormal ) * blockingPotential );

					outGeometryVolume = SHCoeffGV;
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

	GeometryInjectionPass::PipelineHolder::PipelineHolder( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, crg::pp::Config config
		, uint32_t lpvSize )
		: m_holder{ pass, context, graph, std::move( config ), VK_PIPELINE_BIND_POINT_GRAPHICS, 1u }
		, m_lpvSize{ lpvSize }
	{
	}

	void GeometryInjectionPass::PipelineHolder::initialise( VkRenderPass renderPass )
	{
		m_renderPass = renderPass;
		m_holder.initialise();
		doCreatePipeline();
	}

	void GeometryInjectionPass::PipelineHolder::recordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		m_holder.recordInto( context, commandBuffer, index );
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
		VkViewport viewport{ 0.0f, 0.0f, float( m_lpvSize ), float( m_lpvSize ), 0.0f, 1.0f };
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
			, VK_FALSE
			, {}
			, {}
			, {}
			, {}
			, {}
			, {}
			, {} };
		VkPipelineViewportStateCreateInfo vpState = viewportState;
		VkPipelineVertexInputStateCreateInfo viState = vertexState;
		VkPipelineColorBlendStateCreateInfo cbState = blendState;
		auto & program = m_holder.getProgram( 0u );
		auto & pipeline = m_holder.getPipeline( 0u );
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
		crg::checkVkResult( res, m_holder.getPass().getGroupName() + " - Pipeline creation" );
		crgRegisterObject( m_holder.getContext(), m_holder.getPass().getGroupName(), pipeline );
	}

	//*********************************************************************************************

	GeometryInjectionPass::GeometryInjectionPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, LightType lightType
		, uint32_t gridSize
		, uint32_t rsmSize )
		: Named{ pass.getName() }
		, crg::RenderPass{ pass
			, context
			, graph
			, { [this](){ doSubInitialise(); }
				, [this]( crg::RecordContext & context, VkCommandBuffer cb, uint32_t i ){ doSubRecordInto( context, cb, i ); } }
			, { gridSize, gridSize } }
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
	
	GeometryInjectionPass::GeometryInjectionPass( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, RenderDevice const & device
		, CubeMapFace face
		, uint32_t gridSize
		, uint32_t rsmSize )
		: Named{ pass.getName() }
		, crg::RenderPass{ pass
			, context
			, graph
			, { [this](){ doSubInitialise(); }
				, [this]( crg::RecordContext & context, VkCommandBuffer cb, uint32_t i ){ doSubRecordInto( context, cb, i ); } }
			, { gridSize, gridSize } }
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

	void GeometryInjectionPass::doSubInitialise()
	{
		m_holder.initialise( getRenderPass() );
	}

	void GeometryInjectionPass::doSubRecordInto( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, uint32_t index )
	{
		m_holder.recordInto( context, commandBuffer, index );
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

	TexturePtr GeometryInjectionPass::createResult( crg::ResourceHandler & handler
		, RenderDevice const & device
		, castor::String const & prefix
		, uint32_t index
		, uint32_t gridSize )
	{
		return std::make_shared< Texture >( device
			, handler
			, prefix + cuT( "GeometryInjection" ) + string::toString( index )
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
			, false );
	}
}
