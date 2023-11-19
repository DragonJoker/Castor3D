#include "WavesComponent.hpp"

#include "WavesUbo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/MeshFactory.hpp>
#include <Castor3D/Model/Mesh/MeshGenerator.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/SubmeshComponentRegister.hpp>
#include <Castor3D/Render/RenderPipeline.hpp>
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

CU_ImplementSmartPtr( waves, WavesRenderComponent )

namespace waves
{
	using namespace castor3d;

	//*********************************************************************************************

	namespace parse
	{
		enum class WavesSection
			: uint32_t
		{
			eWaves = CU_MakeSectionName( 'W', 'A', 'V', 'S' ),
			eWave = CU_MakeSectionName( 'W', 'A', 'V', 'E' ),
		};

		static void * createContext( castor::FileParserContext & context )
		{
			waves::ParserContext * userContext = new waves::ParserContext;
			userContext->engine = static_cast< castor3d::SceneFileParser * >( context.parser )->getEngine();
			return userContext;
		}

		static ParserContext & getParserContext( castor::FileParserContext & context )
		{
			return *static_cast< ParserContext * >( context.getUserContext( WavesRenderComponent::TypeName ) );
		}

		static CU_ImplementAttributeParser( parserWavesComponent )
		{
			auto & parsingContext = getSceneParserContext( context );

			if ( !parsingContext.mesh )
			{
				CU_ParsingError( "Mesh not initialised" );
			}
		}
		CU_EndAttributePush( WavesSection::eWaves )

		static CU_ImplementAttributeParser( parserWidthSubdiv )
		{
			auto & wavesContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				wavesContext.parameters.add( "width_subdiv"
					, castor::string::toString( params[0]->get< uint32_t >() ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserDepthSubdiv )
		{
			auto & wavesContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				wavesContext.parameters.add( "depth_subdiv"
					, castor::string::toString( params[0]->get< uint32_t >() ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserWidth )
		{
			auto & wavesContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				wavesContext.parameters.add( "width"
					, castor::string::toString( params[0]->get< uint32_t >() ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserDepth )
		{
			auto & wavesContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				wavesContext.parameters.add( "depth"
					, castor::string::toString( params[0]->get< uint32_t >() ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserTessellationFactor )
		{
			auto & wavesContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				wavesContext.config.tessellationFactor = float( params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserDampeningFactor )
		{
			auto & wavesContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				params[0]->get( wavesContext.config.dampeningFactor );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserWave )
		{
		}
		CU_EndAttributePush( WavesSection::eWave )

		static CU_ImplementAttributeParser( parserWavesComponentEnd )
		{
			auto & parsingContext = getSceneParserContext( context );
			auto & wavesContext = getParserContext( context );
			auto & factory = wavesContext.engine->getMeshFactory();
			wavesContext.parameters.add( cuT( "flipYZ" ), true );
			factory.create( "plane" )->generate( *parsingContext.mesh, wavesContext.parameters );

			auto submesh = parsingContext.mesh->getSubmesh( 0u );
			auto & component = *submesh->createComponent< WavesRenderComponent >();
			wavesContext.config.numWaves = wavesContext.wave;
			component.setConfig( std::move( wavesContext.config ) );
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParser( parserWaveDirection )
		{
			auto & wavesContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				params[0]->get( wavesContext.config.waves[wavesContext.wave].direction );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserWaveSteepness )
		{
			auto & wavesContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				params[0]->get( wavesContext.config.waves[wavesContext.wave].steepness );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserWaveLength )
		{
			auto & wavesContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				params[0]->get( wavesContext.config.waves[wavesContext.wave].length );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserWaveAmplitude )
		{
			auto & wavesContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				params[0]->get( wavesContext.config.waves[wavesContext.wave].amplitude );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserWaveSpeed )
		{
			auto & wavesContext = getParserContext( context );

			if ( params.empty() )
			{
				CU_ParsingError( "Missing parameter" );
			}
			else
			{
				params[0]->get( wavesContext.config.waves[wavesContext.wave].speed );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParser( parserWaveEnd )
		{
			auto & wavesContext = getParserContext( context );
			++wavesContext.wave;
		}
		CU_EndAttributePop()
	}

	//*********************************************************************************************

	namespace shd
	{
		static uint32_t constexpr OutputVertices = 3u;

		struct WaveResult
			: sdw::StructInstance
		{
			WaveResult( sdw::ShaderWriter & writer
				, sdw::expr::ExprPtr expr
				, bool enabled = true )
				: sdw::StructInstance{ writer, std::move( expr ), enabled }
				, position{ getMember< sdw::Vec3 >( "position" ) }
				, normal{ getMember< sdw::Vec3 >( "normal" ) }
				, bitangent{ getMember< sdw::Vec3 >( "bitangent" ) }
				, tangent{ getMember< sdw::Vec3 >( "tangent" ) }
			{
			}

			SDW_DeclStructInstance( , WaveResult );

			static sdw::type::StructPtr makeType( sdw::type::TypesCache & cache )
			{
				auto result = cache.getStruct( sdw::type::MemoryLayout::eStd430
					, "WaveResult" );

				if ( result->empty() )
				{
					result->declMember( "position", sdw::type::Kind::eVec3F, sdw::type::NotArray );
					result->declMember( "normal", sdw::type::Kind::eVec3F, sdw::type::NotArray );
					result->declMember( "bitangent", sdw::type::Kind::eVec3F, sdw::type::NotArray );
					result->declMember( "tangent", sdw::type::Kind::eVec3F, sdw::type::NotArray );
					result->end();
				}

				return result;
			}

			sdw::Vec3 position;
			sdw::Vec3 normal;
			sdw::Vec3 bitangent;
			sdw::Vec3 tangent;
		};
	}

	//*********************************************************************************************

	WavesRenderComponent::RenderData::RenderData( castor3d::SubmeshComponent const & component )
		: SubmeshRenderData{}
		, m_component{ static_cast< WavesRenderComponent const & >( component ) }
	{
	}

	bool WavesRenderComponent::RenderData::initialise( RenderDevice const & device )
	{
		m_ubo = std::make_unique< WavesUbo >( device );
		return true;
	}

	void WavesRenderComponent::RenderData::cleanup( RenderDevice const & device )
	{
		m_ubo.reset();
	}

	void WavesRenderComponent::RenderData::update( CpuUpdater & updater )
	{
		m_ubo->cpuUpdate( m_component.getConfig() );
	}

	void WavesRenderComponent::RenderData::fillBindings( castor3d::PipelineFlags const & flags
		, ashes::VkDescriptorSetLayoutBindingArray & bindings
		, uint32_t & index )const
	{
		bindings.emplace_back( makeDescriptorSetLayoutBinding( index++
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
			, VK_SHADER_STAGE_ALL_GRAPHICS ) );
	}

	void WavesRenderComponent::RenderData::fillDescriptor( castor3d::PipelineFlags const & flags
		, ashes::WriteDescriptorSetArray & descriptorWrites
		, uint32_t & index )const
	{
		descriptorWrites.push_back( m_ubo->getDescriptorWrite( index++ ) );
	}

	//*********************************************************************************************

	void WavesRenderComponent::RenderShader::getShaderSource( castor3d::Engine const & engine
		, castor3d::PipelineFlags const & flags
		, castor3d::ComponentModeFlags const & componentsMask
		, ast::ShaderBuilder & builder )const
	{
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
		shader::Materials materials{ writer
			, passShaders
			, uint32_t( GlobalBuffersIdx::eMaterials )
			, RenderPipeline::eBuffers };
		auto index = uint32_t( castor3d::GlobalBuffersIdx::eCount );
		C3D_Waves( writer
			, index++
			, RenderPipeline::eBuffers );
		index++; // lights buffer

		sdw::PushConstantBuffer pcb{ writer, "C3D_DrawData", "c3d_drawData" };
		auto pipelineID = pcb.declMember< sdw::UInt >( "pipelineID" );
		pcb.end();

		auto calculateWave = writer.implementFunction< shd::WaveResult >( "calculateWave"
			, [&]( Wave wave
				, sdw::Vec3 wavePosition
				, sdw::Float edgeDampen
				, sdw::UInt numWaves
				, sdw::Float timeIndex )
			{
				auto result = writer.declLocale< shd::WaveResult >( "result" );

				auto frequency = writer.declLocale( "frequency"
					, 2.0_f / wave.length() );
				auto phaseConstant = writer.declLocale( "phaseConstant"
					, wave.speed() * frequency );
				auto qi = writer.declLocale( "qi"
					, wave.steepness() / ( wave.amplitude() * frequency * writer.cast< sdw::Float >( numWaves ) ) );
				auto rad = writer.declLocale( "rad"
					, frequency * dot( wave.direction().xz(), wavePosition.xz() ) + timeIndex * phaseConstant );
				auto sinR = writer.declLocale( "sinR"
					, sin( rad ) );
				auto cosR = writer.declLocale( "cosR"
					, cos( rad ) );

				result.position.x() = wavePosition.x() + qi * wave.amplitude() * wave.direction().x() * cosR * edgeDampen;
				result.position.y() = wave.amplitude() * sinR * edgeDampen;
				result.position.z() = wavePosition.z() + qi * wave.amplitude() * wave.direction().z() * cosR * edgeDampen;

				auto waFactor = writer.declLocale( "waFactor"
					, frequency * wave.amplitude() );
				auto radN = writer.declLocale( "radN"
					, frequency * dot( wave.direction(), result.position ) + timeIndex * phaseConstant );
				auto sinN = writer.declLocale( "sinN"
					, sin( radN ) );
				auto cosN = writer.declLocale( "cosN"
					, cos( radN ) );

				result.bitangent.x() = 1.0_f - ( qi * wave.direction().x() * wave.direction().x() * waFactor * sinN );
				result.bitangent.y() = wave.direction().x() * waFactor * cosN;
				result.bitangent.z() = -1.0_f * ( qi * wave.direction().x() * wave.direction().z() * waFactor * sinN );

				result.tangent.x() = -1.0_f * ( qi * wave.direction().x() * wave.direction().z() * waFactor * sinN );
				result.tangent.y() = wave.direction().z() * waFactor * cosN;
				result.tangent.z() = 1.0_f - ( qi * wave.direction().z() * wave.direction().z() * waFactor * sinN );

				result.normal.x() = -1.0_f * ( wave.direction().x() * waFactor * cosN );
				result.normal.y() = 1.0_f - ( qi * waFactor * sinN );
				result.normal.z() = -1.0_f * ( wave.direction().z() * waFactor * cosN );

				result.bitangent = normalize( result.bitangent );
				result.tangent = normalize( result.tangent );
				result.normal = normalize( result.normal );

				writer.returnStmt( result );
			}
			, InWave{ writer, "wave" }
			, sdw::InVec3{ writer, "wavePosition" }
			, sdw::InFloat{ writer, "edgeDampen" }
			, sdw::InUInt{ writer, "numWaves" }
			, sdw::InFloat{ writer, "timeIndex" } );

		if ( flags.isBillboard() )
		{
			C3D_Billboard( writer
				, GlobalBuffersIdx::eBillboardsData
				, RenderPipeline::eBuffers );
			
			writer.implementEntryPointT< shader::BillboardSurfaceT, shader::FragmentSurfaceT >( sdw::VertexInT< shader::BillboardSurfaceT >{ writer, flags }
				, sdw::VertexOutT< shader::FragmentSurfaceT >{ writer, passShaders, flags }
				, [&]( sdw::VertexInT< shader::BillboardSurfaceT > in
					, sdw::VertexOutT< shader::FragmentSurfaceT > out )
				{
					auto nodeId = writer.declLocale( "nodeId"
						, shader::getNodeId( c3d_objectIdsData
							, pipelineID
							, writer.cast< sdw::UInt >( in.drawID ) ) );
					auto modelData = writer.declLocale( "modelData"
						, c3d_modelsData[nodeId - 1u] );
					out.nodeId = writer.cast< sdw::Int >( nodeId );

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
					out.texture0 = vec3( in.texture0, 1.0_f );
					out.texture1 = vec3( in.texture0 * 50.0_f, 1.0_f );
					out.colour = vec3( 1.0_f );
					out.vtx.position = modelData.worldToModel( vec4( worldPos, 1.0_f ) );
				} );
		}
		else
		{
			writer.implementEntryPointT< shader::MeshVertexT, shader::FragmentSurfaceT >( sdw::VertexInT< shader::MeshVertexT >{ writer, submeshShaders }
				, sdw::VertexOutT< shader::FragmentSurfaceT >{ writer, passShaders, flags }
				, [&]( sdw::VertexInT< shader::MeshVertexT > in
					, sdw::VertexOutT< shader::FragmentSurfaceT > out )
				{
					auto nodeId = writer.declLocale( "nodeId"
						, shader::getNodeId( c3d_objectIdsData
							, in
							, pipelineID
							, writer.cast< sdw::UInt >( in.drawID )
							, flags ) );
					out.vtx.position = in.position;
					out.texture0 = in.texture0;
					out.texture1 = in.texture1;
					out.texture2 = in.texture2;
					out.texture3 = in.texture3;
					out.normal = in.normal;
					out.tangent = in.tangent;
					out.bitangent = in.bitangent;
					out.colour = in.colour;
					auto modelData = writer.declLocale( "modelData"
						, c3d_modelsData[nodeId - 1u] );
					auto material = writer.declLocale( "material"
						, materials.getMaterial( modelData.getMaterialId() ) );
					material.getPassMultipliers( flags
						, in.passMasks
						, out.passMultipliers );
					out.nodeId = writer.cast< sdw::Int >( nodeId );
				} );
		}

		writer.implementPatchRoutineT< shader::FragmentSurfaceT, shd::OutputVertices, sdw::VoidT >( sdw::TessControlListInT< shader::FragmentSurfaceT, shd::OutputVertices >{ writer
				, false
				, passShaders
				, flags }
			, sdw::TrianglesTessPatchOutT< sdw::VoidT >{ writer, 9u }
			, [&]( sdw::TessControlPatchRoutineIn in
				, sdw::TessControlListInT< shader::FragmentSurfaceT, shd::OutputVertices > listIn
				, sdw::TrianglesTessPatchOut patchOut )
			{
				patchOut.tessLevelOuter[0] = c3d_wavesData.tessellationFactor();
				patchOut.tessLevelOuter[1] = c3d_wavesData.tessellationFactor();
				patchOut.tessLevelOuter[2] = c3d_wavesData.tessellationFactor();

				patchOut.tessLevelInner[0] = c3d_wavesData.tessellationFactor();
			} );

		writer.implementEntryPointT< shader::FragmentSurfaceT, shd::OutputVertices, shader::FragmentSurfaceT >( sdw::TessControlListInT< shader::FragmentSurfaceT, shd::OutputVertices >{ writer
				, true
				, passShaders
				, flags }
			, sdw::TrianglesTessControlListOutT< shader::FragmentSurfaceT >{ writer
				, ast::type::Partitioning::eFractionalEven
				, ast::type::OutputTopology::eTriangle
				, ast::type::PrimitiveOrdering::eCCW
				, shd::OutputVertices
				, passShaders
				, flags }
			, [&]( sdw::TessControlMainIn in
				, sdw::TessControlListInT< shader::FragmentSurfaceT, shd::OutputVertices > listIn
				, sdw::TrianglesTessControlListOutT< shader::FragmentSurfaceT > listOut )
			{
				listOut.vtx.position = listIn[in.invocationID].vtx.position;
				listOut.nodeId = listIn[in.invocationID].nodeId;
				listOut.texture0 = listIn[in.invocationID].texture0;
				listOut.texture1 = listIn[in.invocationID].texture1;
				listOut.texture2 = listIn[in.invocationID].texture2;
				listOut.texture3 = listIn[in.invocationID].texture3;
				listOut.normal = listIn[in.invocationID].normal;
				listOut.tangent = listIn[in.invocationID].tangent;
				listOut.bitangent = listIn[in.invocationID].bitangent;
				listOut.colour = listIn[in.invocationID].colour;
				listOut.passMultipliers = listIn[in.invocationID].passMultipliers;
			} );

		writer.implementEntryPointT< shader::FragmentSurfaceT, shd::OutputVertices, sdw::VoidT, shader::FragmentSurfaceT >( sdw::TessEvalListInT< shader::FragmentSurfaceT, shd::OutputVertices >{ writer
				, ast::type::PatchDomain::eTriangles
				, ast::type::Partitioning::eFractionalEven
				, ast::type::PrimitiveOrdering::eCCW
				, passShaders
				, flags }
			, sdw::TrianglesTessPatchInT< sdw::VoidT >{ writer, 9u }
			, sdw::TessEvalDataOutT< shader::FragmentSurfaceT >{ writer
				, passShaders
				, flags }
			, [&]( sdw::TessEvalMainIn mainIn
				, sdw::TessEvalListInT< shader::FragmentSurfaceT, shd::OutputVertices > listIn
				, sdw::TrianglesTessPatchInT< sdw::VoidT > patchIn
				, sdw::TessEvalDataOutT< shader::FragmentSurfaceT > out )
			{
				auto position = writer.declLocale( "position"
					, patchIn.tessCoord.x() * listIn[0].vtx.position
					+ patchIn.tessCoord.y() * listIn[1].vtx.position
					+ patchIn.tessCoord.z() * listIn[2].vtx.position );
				auto texcoord = writer.declLocale( "texcoord"
					, patchIn.tessCoord.x() * listIn[0].texture0
					+ patchIn.tessCoord.y() * listIn[1].texture0
					+ patchIn.tessCoord.z() * listIn[2].texture0 );
				auto nodeId = writer.declLocale( "nodeId"
					, listIn[0].nodeId );

				auto dampening = writer.declLocale( "dampening"
					, 1.0_f - pow( utils.saturate( abs( texcoord.x() - 0.5_f ) / 0.5_f ), c3d_wavesData.dampeningFactor() ) );
				dampening *= 1.0_f - pow( utils.saturate( abs( texcoord.y() - 0.5_f ) / 0.5_f ), c3d_wavesData.dampeningFactor() );

				auto finalWaveResult = writer.declLocale< shd::WaveResult >( "finalWaveResult" );
				finalWaveResult.position = vec3( 0.0_f );
				finalWaveResult.normal = vec3( 0.0_f );
				finalWaveResult.tangent = vec3( 0.0_f );
				finalWaveResult.bitangent = vec3( 0.0_f );

				auto numWaves = writer.declLocale( "numWaves"
					, c3d_wavesData.numWaves() );

				IF( writer, numWaves > 0_u )
				{
					FOR( writer, sdw::UInt, waveId, 0_u, waveId < numWaves, waveId++ )
					{
						auto waveResult = writer.declLocale( "waveResult"
							, calculateWave( c3d_wavesData.waves()[waveId]
								, position.xyz()
								, dampening
								, c3d_wavesData.numWaves()
								, c3d_wavesData.time() ) );
						finalWaveResult.position += waveResult.position;
						finalWaveResult.normal += waveResult.normal;
						finalWaveResult.tangent += waveResult.tangent;
						finalWaveResult.bitangent += waveResult.bitangent;
					}
					ROF;

					finalWaveResult.position -= position.xyz() * ( writer.cast< sdw::Float >( c3d_wavesData.numWaves() - 1_u ) );
					finalWaveResult.normal = normalize( finalWaveResult.normal );
					finalWaveResult.tangent = normalize( finalWaveResult.tangent );
					finalWaveResult.bitangent = normalize( finalWaveResult.bitangent );
				}
				ELSE
				{
					finalWaveResult.position = position.xyz();
					finalWaveResult.normal = vec3( 0.0_f, 1.0_f, 0.0_f );
					finalWaveResult.tangent = vec3( 1.0_f, 0.0_f, 0.0_f );
					finalWaveResult.bitangent = vec3( 0.0_f, 0.0_f, 1.0_f );
				}
				FI;

				auto modelData = writer.declLocale( "modelData"
					, c3d_modelsData[writer.cast< sdw::UInt >( nodeId ) - 1u] );
				auto height = writer.declLocale( "height"
					, finalWaveResult.position.y() - position.y() );
				auto curMtxModel = writer.declLocale( "curMtxModel"
					, modelData.getModelMtx() );
				auto prvMtxModel = writer.declLocale( "prvMtxModel"
					, modelData.getPrvModelMtx( flags, curMtxModel ) );

				auto curPosition = writer.declLocale( "curPosition"
					, vec4( finalWaveResult.position, 1.0_f ) );
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
					, finalWaveResult.normal
					, vec4( finalWaveResult.tangent, 1.0_f )
					, finalWaveResult.bitangent );

				out.texture0 = texcoord;
				out.texture1 = patchIn.tessCoord.x() * listIn[0].texture1
					+ patchIn.tessCoord.y() * listIn[1].texture1
					+ patchIn.tessCoord.z() * listIn[2].texture1;
				out.texture2 = patchIn.tessCoord.x() * listIn[0].texture2
					+ patchIn.tessCoord.y() * listIn[1].texture2
					+ patchIn.tessCoord.z() * listIn[2].texture2;
				out.texture3 = patchIn.tessCoord.x() * listIn[0].texture3
					+ patchIn.tessCoord.y() * listIn[1].texture3
					+ patchIn.tessCoord.z() * listIn[2].texture3;
				out.colour = patchIn.tessCoord.x() * listIn[0].colour
					+ patchIn.tessCoord.y() * listIn[1].colour
					+ patchIn.tessCoord.z() * listIn[2].colour;
				out.passMultipliers[0] = patchIn.tessCoord.x() * listIn[0].passMultipliers[0]
					+ patchIn.tessCoord.y() * listIn[1].passMultipliers[0]
					+ patchIn.tessCoord.z() * listIn[2].passMultipliers[0];
				out.passMultipliers[1] = patchIn.tessCoord.x() * listIn[0].passMultipliers[1]
					+ patchIn.tessCoord.y() * listIn[1].passMultipliers[1]
					+ patchIn.tessCoord.z() * listIn[2].passMultipliers[1];
				out.passMultipliers[2] = patchIn.tessCoord.x() * listIn[0].passMultipliers[2]
					+ patchIn.tessCoord.y() * listIn[1].passMultipliers[2]
					+ patchIn.tessCoord.z() * listIn[2].passMultipliers[2];
				out.passMultipliers[3] = patchIn.tessCoord.x() * listIn[0].passMultipliers[3]
					+ patchIn.tessCoord.y() * listIn[1].passMultipliers[3]
					+ patchIn.tessCoord.z() * listIn[2].passMultipliers[3];
				out.nodeId = nodeId;
				out.worldPosition = worldPos;
				//out.worldPosition.w() = height;
				out.viewPosition = c3d_cameraData.worldToCurView( worldPos );
				curPosition = c3d_cameraData.worldToCurProj( worldPos );
				out.computeVelocity( c3d_cameraData
					, curPosition
					, prvPosition );
				out.vtx.position = curPosition;
			} );
	}

	//*********************************************************************************************

	WavesRenderComponent::Plugin::Plugin( castor3d::SubmeshComponentRegister const & submeshComponents )
		: castor3d::SubmeshComponentPlugin{ submeshComponents, &parse::createContext }
	{
	}

	void WavesRenderComponent::Plugin::createParsers( castor::AttributeParsers & parsers )const
	{
		addParser( parsers, uint32_t( castor3d::CSCNSection::eMesh ), cuT( "waves" ), &parse::parserWavesComponent );

		addParser( parsers, uint32_t( parse::WavesSection::eWaves ), cuT( "widthSubdiv" ), &parse::parserWidthSubdiv, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		addParser( parsers, uint32_t( parse::WavesSection::eWaves ), cuT( "depthSubdiv" ), &parse::parserDepthSubdiv, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		addParser( parsers, uint32_t( parse::WavesSection::eWaves ), cuT( "width" ), &parse::parserWidth, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		addParser( parsers, uint32_t( parse::WavesSection::eWaves ), cuT( "depth" ), &parse::parserDepth, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		addParser( parsers, uint32_t( parse::WavesSection::eWaves ), cuT( "tessellationFactor" ), &parse::parserTessellationFactor, { castor::makeParameter< castor::ParameterType::eUInt32 >() } );
		addParser( parsers, uint32_t( parse::WavesSection::eWaves ), cuT( "dampeningFactor" ), &parse::parserDampeningFactor, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( parsers, uint32_t( parse::WavesSection::eWaves ), cuT( "wave" ), &parse::parserWave );
		addParser( parsers, uint32_t( parse::WavesSection::eWaves ), cuT( "}" ), &parse::parserWavesComponentEnd );

		addParser( parsers, uint32_t( parse::WavesSection::eWave ), cuT( "direction" ), &parse::parserWaveDirection, { castor::makeParameter< castor::ParameterType::ePoint3F >() } );
		addParser( parsers, uint32_t( parse::WavesSection::eWave ), cuT( "steepness" ), &parse::parserWaveSteepness, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( parsers, uint32_t( parse::WavesSection::eWave ), cuT( "length" ), &parse::parserWaveLength, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( parsers, uint32_t( parse::WavesSection::eWave ), cuT( "amplitude" ), &parse::parserWaveAmplitude, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( parsers, uint32_t( parse::WavesSection::eWave ), cuT( "speed" ), &parse::parserWaveSpeed, { castor::makeParameter< castor::ParameterType::eFloat >() } );
		addParser( parsers, uint32_t( parse::WavesSection::eWave ), cuT( "}" ), &parse::parserWaveEnd );
	}

	void WavesRenderComponent::Plugin::createSections( castor::StrUInt32Map & sections )const
	{
		sections.emplace( uint32_t( parse::WavesSection::eWaves ), cuT( "waves" ) );
		sections.emplace( uint32_t( parse::WavesSection::eWave ), cuT( "wave" ) );
	}

	//*********************************************************************************************

	castor::String const WavesRenderComponent::TypeName = C3D_PluginMakeSubmeshRenderComponentName( "waves", "waves" );
	castor::String const WavesRenderComponent::FullName = "Waves Rendering";

	WavesRenderComponent::WavesRenderComponent( Submesh & submesh )
		: SubmeshComponent{ submesh, TypeName }
	{
	}

	SubmeshComponentUPtr WavesRenderComponent::clone( Submesh & submesh )const
	{
		auto result = castor::makeUnique< WavesRenderComponent >( submesh );
		result->initialiseRenderData();
		return castor::ptrRefCast< SubmeshComponent >( result );
	}

	//*********************************************************************************************
}
