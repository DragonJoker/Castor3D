#include "FFTOceanRendering/FFTWavesComponent.hpp"

#include "FFTOceanRendering/OceanFFT.hpp"
#include "FFTOceanRendering/OceanFFTUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/ConfigurationVisitor.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/MeshFactory.hpp>
#include <Castor3D/Model/Mesh/MeshGenerator.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/SubmeshComponentRegister.hpp>
#include <Castor3D/Render/RenderPipeline.hpp>
#include <Castor3D/Scene/Camera.hpp>
#include <Castor3D/Scene/SceneNode.hpp>
#include <Castor3D/Scene/SceneFileParser.hpp>
#include <Castor3D/Shader/Shaders/GlslCullData.hpp>
#include <Castor3D/Shader/Shaders/GlslMaterial.hpp>
#include <Castor3D/Shader/Shaders/GlslMeshlet.hpp>
#include <Castor3D/Shader/Shaders/GlslMeshVertex.hpp>
#include <Castor3D/Shader/Shaders/GlslPassShaders.hpp>
#include <Castor3D/Shader/Shaders/GlslSubmeshShaders.hpp>
#include <Castor3D/Shader/Shaders/GlslTaskPayload.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>
#include <Castor3D/Shader/Ubos/CameraUbo.hpp>
#include <Castor3D/Shader/Ubos/BillboardUbo.hpp>
#include <Castor3D/Shader/Ubos/ModelDataUbo.hpp>
#include <Castor3D/Shader/Ubos/ObjectIdsUbo.hpp>
#include <Castor3D/Shader/Ubos/SceneUbo.hpp>

#include <CastorUtils/FileParser/ParserParameter.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

CU_ImplementSmartPtr( ocean_fft, FFTWavesComponent )

namespace ocean_fft
{
	using namespace castor3d;

	//*********************************************************************************************

	namespace parse
	{
		struct OceanContext
		{
			castor3d::MeshRPtr mesh{};
			OceanFFTConfig fftConfig{};
			std::vector< castor3d::MaterialObs > materials;
		};

		enum class FFTWavesSection
			: uint32_t
		{
			eWaves = CU_MakeSectionName( 'O', 'C', 'N', 'R' ),
		};

		static CU_ImplementAttributeParserNewBlock( parserFftWavesComponent, castor3d::MeshContext, OceanContext )
		{
			if ( !blockContext->mesh )
			{
				CU_ParsingError( "Mesh not initialised" );
			}
			else
			{
				newBlockContext->mesh = blockContext->mesh;
			}
		}
		CU_EndAttributePushNewBlock( FFTWavesSection::eWaves )

		static CU_ImplementAttributeParserBlock( parserFftDisableRandomSeed, OceanContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				params[0]->get( blockContext->fftConfig.disableRandomSeed );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFftSize, OceanContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				params[0]->get( blockContext->fftConfig.size );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFftHeightMapSamples, OceanContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				params[0]->get( blockContext->fftConfig.heightMapSamples );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFftDisplacementDownsample, OceanContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				params[0]->get( blockContext->fftConfig.displacementDownsample );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFftNormalMapFreqMod, OceanContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				params[0]->get( blockContext->fftConfig.normalFreqMod );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFftAmplitude, OceanContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				params[0]->get( blockContext->fftConfig.amplitude );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFftWindDirection, OceanContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				params[0]->get( blockContext->fftConfig.windDirection );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFftWindVelocity, OceanContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				params[0]->get( blockContext->fftConfig.windVelocity );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFftPatchSize, OceanContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				params[0]->get( blockContext->fftConfig.patchSize );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFftBlocksCount, OceanContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				params[0]->get( blockContext->fftConfig.blocksCount );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFftLOD0Distance, OceanContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				params[0]->get( blockContext->fftConfig.lod0Distance );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFftWavesComponentEnd, OceanContext )
		{
			auto & factory = blockContext->mesh->getOwner()->getMeshFactory();
			castor3d::Parameters parameters;
			parameters.add( "width_subdiv"
				, castor::string::toString( blockContext->fftConfig.blocksCount->x - 1u ) );
			parameters.add( "depth_subdiv"
				, castor::string::toString( blockContext->fftConfig.blocksCount->y - 1u ) );
			parameters.add( "width"
				, castor::string::toString( float( blockContext->fftConfig.blocksCount->x ) * blockContext->fftConfig.patchSize->x ) );
			parameters.add( "depth"
				, castor::string::toString( float( blockContext->fftConfig.blocksCount->y ) * blockContext->fftConfig.patchSize->y ) );
			parameters.add( cuT( "flipYZ" ), true );
			parameters.add( cuT( "sort_around_center" ), true );
			factory.create( "plane" )->generate( *blockContext->mesh, parameters );

			auto submesh = blockContext->mesh->getSubmesh( 0u );
			auto & component = *submesh->createComponent< FFTWavesComponent >();
			component.setFftConfig( std::move( blockContext->fftConfig ) );
		}
		CU_EndAttributePop()
	}

	//*********************************************************************************************

	namespace shd
	{
		static uint32_t constexpr OutputVertices = 1u;

		template< ast::var::Flag FlagT >
		struct PatchT
			: public sdw::StructInstance
		{
			PatchT( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled )
				: StructInstance{ writer, std::move( expr ), enabled }
			{
			}

			SDW_DeclStructInstance( , PatchT );

			static ast::type::IOStructPtr makeIOType( ast::type::TypesCache & cache
				, sdw::EntryPoint entryPoint
				, castor3d::PipelineFlags flags )
			{
				auto result = cache.getIOStruct( "C3DORFFT_Patch"
					, entryPoint
					, FlagT );

				if ( result->empty() )
				{
					uint32_t index = 0u;
					result->declMember( "patchWorldPosition"
						, ast::type::Kind::eVec3F
						, ast::type::NotArray
						, index++ );
					result->declMember( "patchLods"
						, ast::type::Kind::eVec4F
						, ast::type::NotArray
						, index++ );
					result->declMember( "colour"
						, ast::type::Kind::eVec3F
						, ast::type::NotArray
						, ( flags.enableColours() ? index++ : 0 )
						, flags.enableColours() );
					result->declMember( "nodeId"
						, ast::type::Kind::eInt
						, ast::type::NotArray
						, index++ );
				}

				return result;
			}

			static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache
				, castor3d::PipelineFlags flags )
			{
				auto result = cache.getStruct( ast::type::MemoryLayout::eC
					, "C3DORFFT_Patch" );

				if ( result->empty() )
				{
					result->declMember( "patchWorldPosition"
						, ast::type::Kind::eVec3F
						, ast::type::NotArray );
					result->declMember( "patchLods"
						, ast::type::Kind::eVec4F
						, ast::type::NotArray );
					result->declMember( "colour"
						, ast::type::Kind::eVec3F
						, ast::type::NotArray
						, flags.enableColours() );
					result->declMember( "nodeId"
						, ast::type::Kind::eInt
						, ast::type::NotArray );
				}

				return result;
			}

			auto patchWorldPosition()const { return getMember< sdw::Vec3 >( "patchWorldPosition" ); }
			auto patchLods()const { return getMember< sdw::Vec4 >( "patchLods" ); }
			auto colour()const { return getMember< sdw::Vec3 >( "colour" ); }
			auto nodeId()const { return getMember< sdw::Int >( "nodeId" ); }

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;
		};
	}

	//*********************************************************************************************
	
	void FFTWavesComponent::SurfaceShader::fillSurfaceType( sdw::type::Struct & type
		, uint32_t * index )const
	{
		if ( index )
		{
			static_cast< sdw::type::IOStruct & >( type ).declMember( "gradientJacobianUV", ast::type::Kind::eVec2F, ast::type::NotArray, ( *index )++ );
			static_cast< sdw::type::IOStruct & >( type ).declMember( "noiseGradientUV", ast::type::Kind::eVec2F, ast::type::NotArray, ( *index )++ );
			static_cast< sdw::type::IOStruct & >( type ).declMember( "mdlPosition", ast::type::Kind::eVec3F, ast::type::NotArray, ( *index )++ );
		}
		else
		{
			static_cast< sdw::type::BaseStruct & >( type ).declMember( "gradientJacobianUV", ast::type::Kind::eVec2F, ast::type::NotArray );
			static_cast< sdw::type::BaseStruct & >( type ).declMember( "noiseGradientUV", ast::type::Kind::eVec2F, ast::type::NotArray );
			static_cast< sdw::type::BaseStruct & >( type ).declMember( "mdlPosition", ast::type::Kind::eVec3F, ast::type::NotArray );
		}
	}

	//*********************************************************************************************

	FFTWavesComponent::RenderData::RenderData( castor3d::SubmeshComponent const & component )
		: SubmeshRenderData{}
		, m_component{ static_cast< FFTWavesComponent const & >( component ) }
	{
	}

	bool FFTWavesComponent::RenderData::initialise( RenderDevice const & device )
	{
		if ( m_ubo )
		{
			return true;
		}

		m_ubo = std::make_unique< OceanUbo >( device );
		m_linearWrapSampler = device->createSampler( m_component.getOwner()->getParent().getName()
			, VK_SAMPLER_ADDRESS_MODE_REPEAT
			, VK_SAMPLER_ADDRESS_MODE_REPEAT
			, VK_SAMPLER_ADDRESS_MODE_REPEAT
			, VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_LINEAR );
		m_pointClampSampler = device->createSampler( m_component.getOwner()->getParent().getName()
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_FILTER_NEAREST
			, VK_FILTER_NEAREST
			, VK_SAMPLER_MIPMAP_MODE_NEAREST );
		return true;
	}

	crg::FramePassArray FFTWavesComponent::RenderData::record( RenderDevice const & device
		, crg::ResourcesCache & resources
		, crg::FrameGraph & graph
		, crg::FramePassArray previousPasses )
	{
		if ( !m_ubo )
		{
			initialise( device );
		}

		if ( !m_oceanFFT )
		{
			m_oceanFFT = std::make_unique< OceanFFT >( device
				, resources
				, graph.createPassGroup( m_component.getOwner()->getParent().getName() + "/FFTWaves" )
				, std::move( previousPasses )
				, *m_ubo
				, m_component.getFftConfig() );
			graph.addInput( m_oceanFFT->getHeightDisplacement().sampledViewId
				, { VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, { VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT } } );
			graph.addInput( m_oceanFFT->getGradientJacobian().sampledViewId
				, { VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, { VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT } } );
			graph.addInput( m_oceanFFT->getNormals().sampledViewId
				, { VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, { VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT } } );
		}

		return m_oceanFFT->getLastPasses();
	}

	void FFTWavesComponent::RenderData::registerDependencies( crg::FramePass & pass )const
	{
		pass.addImplicitColourView( m_oceanFFT->getHeightDisplacement().sampledViewId
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		pass.addImplicitColourView( m_oceanFFT->getGradientJacobian().sampledViewId
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
		pass.addImplicitColourView( m_oceanFFT->getNormals().sampledViewId
			, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
	}

	void FFTWavesComponent::RenderData::cleanup( RenderDevice const & device )
	{
		m_pointClampSampler.reset();
		m_linearWrapSampler.reset();
		m_oceanFFT.reset();
		m_ubo.reset();
	}

	void FFTWavesComponent::RenderData::update( CpuUpdater & updater )
	{
		auto tslf = updater.tslf > 0_ms
			? updater.tslf
			: std::chrono::duration_cast< castor::Milliseconds >( m_timer.getElapsed() );
		m_config.time += float( tslf.count() ) / 1000.0f;
		m_ubo->cpuUpdate( m_config
			, m_component.getFftConfig()
			, updater.camera->getParent()->getDerivedPosition() );
	}

	void FFTWavesComponent::RenderData::fillBindings( castor3d::PipelineFlags const & flags
		, ashes::VkDescriptorSetLayoutBindingArray & bindings
		, uint32_t & index )const
	{
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_ALL_GRAPHICS ) );
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, ( VK_SHADER_STAGE_FRAGMENT_BIT
				| VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT ) ) );
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
			, VK_SHADER_STAGE_FRAGMENT_BIT ) );
	}

	void FFTWavesComponent::RenderData::fillDescriptor( castor3d::PipelineFlags const & flags
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, uint32_t & index )const
	{
		descriptorWrites.push_back( m_ubo->getDescriptorWrite( index++ ) );
		bindTexture( m_oceanFFT->getHeightDisplacement().sampledView, *m_linearWrapSampler, descriptorWrites, index );
		bindTexture( m_oceanFFT->getGradientJacobian().sampledView, *m_linearWrapSampler, descriptorWrites, index );
		bindTexture( m_oceanFFT->getNormals().sampledView, *m_linearWrapSampler, descriptorWrites, index );
	}

	void FFTWavesComponent::RenderData::accept( castor3d::ConfigurationVisitorBase & vis )
	{
		if ( m_oceanFFT )
		{
			m_oceanFFT->accept( vis );
		}
	}

	//*********************************************************************************************

	void FFTWavesComponent::RenderShader::getShaderSource( castor3d::Engine const & engine
		, castor3d::PipelineFlags const & flags
		, castor3d::ComponentModeFlags const & componentsMask
		, ast::ShaderBuilder & builder )const
	{
		using namespace castor3d;
		sdw::TraditionalGraphicsWriter writer{ builder };
		shader::Utils utils{ writer };
		shader::PassShaders passShaders{ engine.getPassComponentsRegister()
			, flags
			, componentsMask
			, utils };
		shader::SubmeshShaders submeshShaders{ engine.getSubmeshComponentsRegister()
			, flags };

		C3D_Camera( writer
			, GlobalBuffersIdx::eCamera
			, RenderPipeline::eBuffers );
		C3D_ObjectIdsData( writer
			, flags
			, GlobalBuffersIdx::eObjectsNodeID
			, RenderPipeline::eBuffers );
		C3D_ModelsData( writer
			, GlobalBuffersIdx::eModelsData
			, RenderPipeline::eBuffers );
		auto index = uint32_t( castor3d::GlobalBuffersIdx::eCount );
		C3D_FftOcean( writer
			, index++
			, RenderPipeline::eBuffers );
		auto c3d_heightDisplacementMap = writer.declCombinedImg< sdw::CombinedImage2DRgba32 >( "c3d_heightDisplacementMap"
			, index++
			, RenderPipeline::eBuffers );
		auto c3d_gradientJacobianMap = writer.declCombinedImg< sdw::CombinedImage2DRgba16 >( "c3d_gradientJacobianMap"
			, index++
			, RenderPipeline::eBuffers );
		auto c3d_normalsMap = writer.declCombinedImg< sdw::CombinedImage2DRg32 >( "c3d_normalsMap"
			, index++
			, RenderPipeline::eBuffers );

		sdw::PushConstantBuffer pcb{ writer, "C3D_DrawData", "c3d_drawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		auto tessLevel1f = writer.implementFunction< sdw::Float >( "tessLevel1f"
			, [&]( sdw::Float lod
				, sdw::Vec2 maxTessLevel )
			{
				writer.returnStmt( maxTessLevel.y() * exp2( -lod ) );
			}
			, sdw::InFloat{ writer, "lod" }
			, sdw::InVec2{ writer, "maxTessLevel" } );

		auto tessLevel4f = writer.implementFunction< sdw::Vec4 >( "tessLevel4f"
			, [&]( sdw::Vec4 lod
				, sdw::Vec2 maxTessLevel )
			{
				writer.returnStmt( maxTessLevel.y() * exp2( -lod ) );
			}
			, sdw::InVec4{ writer, "lod" }
			, sdw::InVec2{ writer, "maxTessLevel" } );

		auto lodFactors = writer.implementFunction< sdw::Float >( "lodFactors"
			, [&]( sdw::Vec3 worldPos
				, sdw::Vec3 worldEye
				, sdw::Vec2 tileScale
				, sdw::Vec2 maxTessLevel
				, sdw::Float distanceMod )
			{
				worldPos.xz() *= tileScale;
				auto distToCam = writer.declLocale( "distToCam"
					, worldEye - worldPos );
				auto level = writer.declLocale( "level"
					, log2( ( length( distToCam ) + 0.0001_f ) * distanceMod ) );
				writer.returnStmt( clamp( level
					, 0.0_f
					, maxTessLevel.x() ) );
			}
			, sdw::InVec3{ writer, "worldPos" }
			, sdw::InVec3{ writer, "worldEye" }
			, sdw::InVec2{ writer, "tileScale" }
			, sdw::InVec2{ writer, "maxTessLevel" }
			, sdw::InFloat{ writer, "distanceMod" } );

		auto lerpVertex = writer.implementFunction< sdw::Vec2 >( "lerpVertex"
			, [&]( sdw::Vec3 patchPosBase
				, sdw::Vec2 tessCoord
				, sdw::Vec2 patchSize )
			{
				writer.returnStmt( fma( tessCoord, patchSize, patchPosBase.xz() ) );
			}
			, sdw::InVec3{ writer, "patchPosBase" }
			, sdw::InVec2{ writer, "tessCoord" }
			, sdw::InVec2{ writer, "patchSize" } );

		auto lodFactor = writer.implementFunction< sdw::Vec2 >( "lodFactor"
			, [&]( sdw::Vec2 tessCoord
				, sdw::Vec4 patchLods )
			{
				// Bilinear interpolation from patch corners.
				auto x = writer.declLocale( "x"
					, mix( patchLods.yx(), patchLods.zw(), tessCoord.xx() ) );
				auto level = writer.declLocale( "level"
					, mix( x.x(), x.y(), tessCoord.y() ) );

				auto floorLevel = writer.declLocale( "floorLevel"
					, floor( level ) );
				auto fractLevel = writer.declLocale( "fractLevel"
					, level - floorLevel );
				writer.returnStmt( vec2( floorLevel, fractLevel ) );
			}
			, sdw::InVec2{ writer, "tessCoord" }
			, sdw::InVec4{ writer, "patchLods" } );

		auto sampleHeightDisplacement = writer.implementFunction< sdw::Vec3 >( "sampleHeightDisplacement"
			, [&]( sdw::Vec2 uv
				, sdw::Vec2 off
				, sdw::Vec2 lod )
			{
				writer.returnStmt( mix( c3d_heightDisplacementMap.lod( uv + vec2( 0.5_f ) * off, lod.x() ).xyz()
					, c3d_heightDisplacementMap.lod( uv + vec2( 1.0_f ) * off, lod.x() + 1.0_f ).xyz()
					, vec3( lod.y() ) ) );
			}
			, sdw::InVec2{ writer, "uv" }
			, sdw::InVec2{ writer, "off" }
			, sdw::InVec2{ writer, "lod" } );

		if ( flags.isBillboard() )
		{
			C3D_Billboard( writer
				, GlobalBuffersIdx::eBillboardsData
				, RenderPipeline::eBuffers );

			writer.implementEntryPointT< shader::BillboardSurfaceT, shd::PatchT >( sdw::VertexInT< shader::BillboardSurfaceT >{ writer, flags }
				, sdw::VertexOutT< shd::PatchT >{ writer, flags }
				, [&]( sdw::VertexInT< shader::BillboardSurfaceT > in
					, sdw::VertexOutT< shd::PatchT > out )
				{
					auto nodeId = writer.declLocale( "nodeId"
						, shader::getNodeId( c3d_objectIdsData
							, pipelineID
							, writer.cast< sdw::UInt >( in.drawID ) ) );
					auto modelData = writer.declLocale( "modelData"
						, c3d_modelsData[nodeId - 1u] );

					auto curBbcenter = writer.declLocale( "curBbcenter"
						, modelData.modelToCurWorld( vec4( in.center, 1.0_f ) ).xyz() );
					auto prvBbcenter = writer.declLocale( "prvBbcenter"
						, modelData.modelToPrvWorld( vec4( in.center, 1.0_f ) ).xyz() );
					auto curToCamera = writer.declLocale( "curToCamera"
						, c3d_cameraData.getPosToCamera( curBbcenter ) );
					curToCamera.y() = 0.0_f;
					curToCamera = normalize( curToCamera );

					auto billboardData = writer.declLocale( "billboardData"
						, c3d_billboardData[nodeId - 1u] );
					auto right = writer.declLocale( "right"
						, billboardData.getCameraRight( c3d_cameraData ) );
					auto up = writer.declLocale( "up"
						, billboardData.getCameraUp( c3d_cameraData ) );
					auto width = writer.declLocale( "width"
						, billboardData.getWidth( c3d_cameraData ) );
					auto height = writer.declLocale( "height"
						, billboardData.getHeight( c3d_cameraData ) );
					auto scaledRight = writer.declLocale( "scaledRight"
						, right * in.position.x() * width );
					auto scaledUp = writer.declLocale( "scaledUp"
						, up * in.position.y() * height );
					auto worldPos = writer.declLocale( "worldPos"
						, ( curBbcenter + scaledRight + scaledUp ) );

					out.vtx.position = modelData.worldToModel( vec4( worldPos, 1.0_f ) );
					out.colour() = vec3( 1.0_f );
					out.nodeId() = writer.cast< sdw::Int >( nodeId );
					out.patchWorldPosition() = worldPos;
				} );
		}
		else
		{
			writer.implementEntryPointT< castor3d::shader::MeshVertexT, shd::PatchT >( sdw::VertexInT< castor3d::shader::MeshVertexT >{ writer, submeshShaders }
				, sdw::VertexOutT< shd::PatchT >{ writer, flags }
				, [&]( sdw::VertexInT< castor3d::shader::MeshVertexT > in
					, sdw::VertexOutT< shd::PatchT > out )
				{
					auto nodeId = writer.declLocale( "nodeId"
						, shader::getNodeId( c3d_objectIdsData
							, in
							, pipelineID
							, writer.cast< sdw::UInt >( in.drawID )
							, flags ) );
					auto modelData = writer.declLocale( "modelData"
						, c3d_modelsData[nodeId - 1u] );
					auto pos = writer.declLocale( "pos"
						, ( ( in.position.xz() / c3d_oceanData.patchSize() ) + c3d_oceanData.blockOffset() ) * c3d_oceanData.patchSize() );

					out.vtx.position = vec4( pos.x(), 0.0_f, pos.y(), 1.0_f );
					out.patchWorldPosition() = out.vtx.position.xyz();
					out.colour() = in.colour;
					out.nodeId() = writer.cast< sdw::Int >( nodeId );
				} );
		}

		writer.implementPatchRoutineT< shd::PatchT, shd::OutputVertices, shd::PatchT >( sdw::TessControlListInT< shd::PatchT, shd::OutputVertices >{ writer
				, false
				, flags }
			, sdw::QuadsTessPatchOutT< shd::PatchT >{ writer
				, 9u
				, flags }
			, [&]( sdw::TessControlPatchRoutineIn in
				, sdw::TessControlListInT< shd::PatchT, shd::OutputVertices > listIn
				, sdw::QuadsTessPatchOutT< shd::PatchT > patchOut )
			{
				auto patchSize = writer.declLocale( "patchSize"
					, vec3( c3d_oceanData.patchSize().x(), 0.0_f, c3d_oceanData.patchSize().y() ) );
				auto p0 = writer.declLocale( "p0"
					, listIn[0u].patchWorldPosition() );

				auto l0 = writer.declLocale( "l0"
					, lodFactors( p0 + vec3( 0.0_f, 0.0f, 1.0f ) * patchSize
						, c3d_cameraData.position()
						, c3d_oceanData.tileScale()
						, c3d_oceanData.maxTessLevel()
						, c3d_oceanData.distanceMod() ) );
				auto l1 = writer.declLocale( "l1"
					, lodFactors( p0 + vec3( 0.0_f, 0.0f, 0.0f ) * patchSize
						, c3d_cameraData.position()
						, c3d_oceanData.tileScale()
						, c3d_oceanData.maxTessLevel()
						, c3d_oceanData.distanceMod() ) );
				auto l2 = writer.declLocale( "l2"
					, lodFactors( p0 + vec3( 1.0_f, 0.0f, 0.0f ) * patchSize
						, c3d_cameraData.position()
						, c3d_oceanData.tileScale()
						, c3d_oceanData.maxTessLevel()
						, c3d_oceanData.distanceMod() ) );
				auto l3 = writer.declLocale( "l3"
					, lodFactors( p0 + vec3( 1.0_f, 0.0f, 1.0f ) * patchSize
						, c3d_cameraData.position()
						, c3d_oceanData.tileScale()
						, c3d_oceanData.maxTessLevel()
						, c3d_oceanData.distanceMod() ) );

				auto lods = writer.declLocale( "lods"
					, vec4( l0, l1, l2, l3 ) );
				patchOut.patchWorldPosition() = p0;
				patchOut.patchLods() = lods;
				patchOut.colour() = listIn[0u].colour();
				patchOut.nodeId() = listIn[0u].nodeId();

				auto outerLods = writer.declLocale( "outerLods"
					, min( lods.xyzw(), lods.yzwx() ) );
				auto tessLevels = writer.declLocale( "tessLevels"
					, tessLevel4f( outerLods, c3d_oceanData.maxTessLevel() ) );
				auto innerLevel = writer.declLocale( "innerLevel"
					, max( max( tessLevels.x(), tessLevels.y() )
						, max( tessLevels.z(), tessLevels.w() ) ) );

				patchOut.tessLevelOuter[0] = tessLevels.x();
				patchOut.tessLevelOuter[1] = tessLevels.y();
				patchOut.tessLevelOuter[2] = tessLevels.z();
				patchOut.tessLevelOuter[3] = tessLevels.w();

				patchOut.tessLevelInner[0] = innerLevel;
				patchOut.tessLevelInner[1] = innerLevel;
			} );

		writer.implementEntryPointT< shd::PatchT, shd::OutputVertices, sdw::VoidT >( sdw::TessControlListInT< shd::PatchT, shd::OutputVertices >{ writer
				, true
				, flags }
			, sdw::TrianglesTessControlListOut{ writer
				, ast::type::Partitioning::eFractionalEven
				, ast::type::OutputTopology::eQuad
				, ast::type::PrimitiveOrdering::eCW
				, shd::OutputVertices }
			, [&]( sdw::TessControlMainIn in
				, sdw::TessControlListInT< shd::PatchT, shd::OutputVertices > listIn
				, sdw::TrianglesTessControlListOut listOut )
			{
				listOut.vtx.position = listIn[in.invocationID].vtx.position;
			} );

		writer.implementEntryPointT< shd::PatchT, shd::OutputVertices, shd::PatchT, castor3d::shader::FragmentSurfaceT >( sdw::TessEvalListInT< shd::PatchT, shd::OutputVertices >{ writer
				, ast::type::PatchDomain::eQuads
				, ast::type::Partitioning::eFractionalEven
				, ast::type::PrimitiveOrdering::eCW
				, flags }
			, sdw::QuadsTessPatchInT< shd::PatchT >{ writer
				, 9u
				, flags }
			, sdw::TessEvalDataOutT< castor3d::shader::FragmentSurfaceT >{ writer
				, submeshShaders
				, passShaders
				, flags }
			, [&]( sdw::TessEvalMainIn mainIn
				, sdw::TessEvalListInT< shd::PatchT, shd::OutputVertices > listIn
				, sdw::QuadsTessPatchInT< shd::PatchT > patchIn
				, sdw::TessEvalDataOutT< castor3d::shader::FragmentSurfaceT > out )
			{
				auto tessCoord = writer.declLocale( "tessCoord"
					, patchIn.tessCoord.xy() );
				auto pos = writer.declLocale( "pos"
					, lerpVertex( patchIn.patchWorldPosition(), tessCoord, c3d_oceanData.patchSize() ) );
				auto lod = writer.declLocale( "lod"
					, lodFactor( tessCoord, patchIn.patchLods() ) );

				auto tex = writer.declLocale( "tex"
					, pos * c3d_oceanData.invHeightmapSize() );
				pos *= c3d_oceanData.tileScale();

				auto deltaMod = writer.declLocale( "deltaMod"
					, exp2( lod.x() ) );
				auto off = writer.declLocale( "off"
					, c3d_oceanData.invHeightmapSize() * deltaMod );

				auto gradientJacobianUV = out.getMember< sdw::Vec2 >( "gradientJacobianUV" );
				auto noiseGradientUV = out.getMember< sdw::Vec2 >( "noiseGradientUV" );
				auto mdlPosition = out.getMember< sdw::Vec3 >( "mdlPosition" );
				gradientJacobianUV = tex + vec2( 0.5_f ) * c3d_oceanData.invHeightmapSize();
				noiseGradientUV = tex * c3d_oceanData.normalScale();
				auto heightDisplacement = writer.declLocale( "heightDisplacement"
					, sampleHeightDisplacement( tex, off, lod ) );

				pos += heightDisplacement.yz();

				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[writer.cast< sdw::UInt >( patchIn.nodeId() ) - 1u] );
				auto curMtxModel = writer.declLocale( "curMtxModel"
					, modelData.getModelMtx() );
				auto prvMtxModel = writer.declLocale( "prvMtxModel"
					, modelData.getPrvModelMtx( flags, curMtxModel ) );

				auto curPosition = writer.declLocale( "curPosition"
					, vec4( pos.x(), heightDisplacement.x(), pos.y(), 1.0_f ) );
				auto prvPosition = writer.declLocale( "prvPosition"
					, c3d_cameraData.worldToPrvProj( prvMtxModel * curPosition ) );
				auto worldPos = writer.declLocale( "worldPos"
					, curMtxModel * curPosition );
				auto mtxNormal = writer.declLocale( "mtxNormal"
					, modelData.getNormalMtx( flags, curMtxModel ) );
				out.computeTangentSpace( flags
					, c3d_cameraData.position()
					, curPosition.xyz()
					, mtxNormal
					, vec3( 0.0_f, 1.0_f, 0.0_f )
					, vec4( 1.0_f, 0.0_f, 0.0_f, 1.0_f )
					, vec3( 0.0_f, 0.0_f, 1.0_f ) );

				mdlPosition = curPosition.xyz();
				out.colour = patchIn.colour();
				out.nodeId = patchIn.nodeId();
				out.curPosition = curPosition;
				out.worldPosition = worldPos;
				out.viewPosition = c3d_cameraData.worldToCurView( worldPos );
				curPosition = c3d_cameraData.worldToCurProj( worldPos );
				out.computeVelocity( c3d_cameraData
					, curPosition
					, prvPosition );
				out.vtx.position = curPosition;
			} );
	}

	//*********************************************************************************************

	FFTWavesComponent::Plugin::Plugin( castor3d::SubmeshComponentRegister const & submeshComponents )
		: castor3d::SubmeshComponentPlugin{ submeshComponents, nullptr }
	{
	}

	void FFTWavesComponent::Plugin::createParsers( castor::AttributeParsers & parsers )const
	{
		castor3d::BlockParserContextT< MeshContext > meshContext{ parsers, castor3d::CSCNSection::eMesh };
		castor3d::BlockParserContextT< parse::OceanContext > wavesContext{ parsers, parse::FFTWavesSection::eWaves, castor3d::CSCNSection::eMesh };

		meshContext.addPushParser( cuT( "fft_waves" ), parse::FFTWavesSection::eWaves, &parse::parserFftWavesComponent );

		wavesContext.addParser( cuT( "disableRandomSeed" ), &parse::parserFftDisableRandomSeed, { castor::makeParameter< castor::ParameterType::eBool >() } );
		wavesContext.addParser( cuT( "size" ), &parse::parserFftSize, { castor::makeParameter< castor::ParameterType::ePoint2F >() } );
		wavesContext.addParser( cuT( "heightMapSamples" ), &parse::parserFftHeightMapSamples, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		wavesContext.addParser( cuT( "displacementDownsample" ), &parse::parserFftDisplacementDownsample, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		wavesContext.addParser( cuT( "normalMapFreqMod" ), &parse::parserFftNormalMapFreqMod, { castor::makeParameter< castor::ParameterType::ePoint2F >() } );
		wavesContext.addParser( cuT( "amplitude" ), &parse::parserFftAmplitude, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		wavesContext.addParser( cuT( "windDirection" ), &parse::parserFftWindDirection, { castor::makeParameter< castor::ParameterType::ePoint2F >() } );
		wavesContext.addParser( cuT( "windVelocity" ), &parse::parserFftWindVelocity, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		wavesContext.addParser( cuT( "patchSize" ), &parse::parserFftPatchSize, { castor::makeParameter< castor::ParameterType::ePoint2F >() } );
		wavesContext.addParser( cuT( "blocksCount" ), &parse::parserFftBlocksCount, { castor::makeParameter< castor::ParameterType::ePoint2U >() } );
		wavesContext.addParser( cuT( "lod0Distance" ), &parse::parserFftLOD0Distance, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		wavesContext.addPopParser( cuT( "}" ), &parse::parserFftWavesComponentEnd );
	}

	void FFTWavesComponent::Plugin::createSections( castor::StrUInt32Map & sections )const
	{
		sections.emplace( uint32_t( parse::FFTWavesSection::eWaves ), cuT( "fft_waves" ) );
	}

	//*********************************************************************************************

	castor::String const FFTWavesComponent::TypeName = C3D_PluginMakeSubmeshRenderComponentName( "fft_ocean", "waves" );
	castor::String const FFTWavesComponent::FullName = "FFT Waves Rendering";

	FFTWavesComponent::FFTWavesComponent( Submesh & submesh )
		: SubmeshComponent{ submesh, TypeName }
	{
	}

	SubmeshComponentUPtr FFTWavesComponent::clone( Submesh & submesh )const
	{
		auto result = castor::makeUnique< FFTWavesComponent >( submesh );
		result->initialiseRenderData();
		return castor::ptrRefCast< SubmeshComponent >( result );
	}

	void FFTWavesComponent::accept( castor3d::ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Tile XZ size" ), m_fftConfig.size );
		vis.visit( cuT( "Amplitude" ), m_fftConfig.amplitude );
		vis.visit( cuT( "Wind XZ direction" ), m_fftConfig.windDirection );
		vis.visit( cuT( "Wind velocity" ), m_fftConfig.windVelocity );
		vis.visit( cuT( "Normal Freq. Mod." ), m_fftConfig.normalFreqMod );
		vis.visit( cuT( "Patch XZ size" ), m_fftConfig.patchSize );
		vis.visit( cuT( "Blocks XZ Count" ), m_fftConfig.blocksCount );
		vis.visit( cuT( "LOD 0 Distance" ), m_fftConfig.lod0Distance );

		getRenderDataT< RenderData >()->accept( vis );
	}

	//*********************************************************************************************
}
