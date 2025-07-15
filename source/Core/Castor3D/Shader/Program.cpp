#include "Castor3D/Shader/Program.hpp"

#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleSystem.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Stream/StreamPrefixManipulators.hpp>

CU_ImplementSmartPtr( c3d, ShaderProgram )

namespace c3d
{
	namespace shdprog
	{
		static String getName( VkShaderStageFlagBits value )
		{
			switch ( value )
			{
			case VK_SHADER_STAGE_VERTEX_BIT:
				return cuT( "Vert" );
			case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
				return cuT( "Tesc" );
			case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
				return cuT( "Tese" );
			case VK_SHADER_STAGE_GEOMETRY_BIT:
				return cuT( "Geom" );
			case VK_SHADER_STAGE_FRAGMENT_BIT:
				return cuT( "Frag" );
			case VK_SHADER_STAGE_COMPUTE_BIT:
				return cuT( "Comp" );
#ifdef VK_NV_ray_tracing
			case VK_SHADER_STAGE_RAYGEN_BIT_NV:
				return cuT( "Rgen" );
			case VK_SHADER_STAGE_ANY_HIT_BIT_NV:
				return cuT( "Ahit" );
			case VK_SHADER_STAGE_CLOSEST_HIT_BIT_NV:
				return cuT( "Chit" );
			case VK_SHADER_STAGE_MISS_BIT_NV:
				return cuT( "Rmis" );
			case VK_SHADER_STAGE_INTERSECTION_BIT_NV:
				return cuT( "Rint" );
			case VK_SHADER_STAGE_CALLABLE_BIT_NV:
				return cuT( "Call" );
#endif
#if defined( VK_EXT_mesh_shader )
			case VK_SHADER_STAGE_TASK_BIT_EXT:
				return cuT( "Task" );
			case VK_SHADER_STAGE_MESH_BIT_EXT:
				return cuT( "Mesh" );
#elif defined( VK_NV_mesh_shader )
			case VK_SHADER_STAGE_TASK_BIT_NV:
				return cuT( "Task" );
			case VK_SHADER_STAGE_MESH_BIT_NV:
				return cuT( "Mesh" ); 
#endif
			default:
				assert( false && "Unsupported VkShaderStageFlagBits." );
				return cuT( "Unsupported VkShaderStageFlagBits" );
			}
		}

		static void eraseFile( VkShaderStageFlagBits target
			, Map< VkShaderStageFlagBits, Path > & files )
		{
			auto it = files.find( target );

			if ( it != files.end() )
			{
				files.erase( it );
			}
		}

		static void eraseStage( VkShaderStageFlagBits target
			, ashes::PipelineShaderStageCreateInfoArray & states )
		{
			auto it = std::find_if( states.begin()
				, states.end()
				, [target]( ashes::PipelineShaderStageCreateInfo const & lookup )
				{
					return lookup->stage == target;
				} );

			if ( it != states.end() )
			{
				states.erase( it );
			}
		}

		static CU_ImplementAttributeParserBlock( parserComputeShader, ProgramContext )
		{
			blockContext->shaderStage = VK_SHADER_STAGE_COMPUTE_BIT;
		}
		CU_EndAttributePushBlock( CSCNSection::eShaderStage, blockContext )

		static CU_ImplementAttributeParserBlock( parserShaderProgramEnd, ProgramContext )
		{
			if ( !blockContext->shaderProgram )
			{
				CU_ParsingError( cuT( "No ShaderProgram initialised." ) );
			}
			else
			{
				if ( blockContext->particleSystem )
				{
					blockContext->particleSystem->particleSystem->setCSUpdateProgram( blockContext->shaderProgram );
				}

				blockContext->shaderProgram = {};
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserShaderFile, ProgramContext )
		{
			if ( !blockContext->shaderProgram )
			{
				CU_ParsingError( cuT( "No ShaderProgram initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				if ( blockContext->shaderStage != VkShaderStageFlagBits( 0u ) )
				{
					blockContext->shaderProgram->setFile( blockContext->shaderStage
						, context.file.getPath() / params[0]->get< Path >() );
				}
				else
				{
					CU_ParsingError( cuT( "Shader not initialised" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserShaderGroupSizes, ProgramContext )
		{
			if ( !blockContext->shaderProgram )
			{
				CU_ParsingError( cuT( "No ShaderProgram initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				if ( blockContext->particleSystem
					&& blockContext->shaderStage != VkShaderStageFlagBits( 0u ) )
				{
					blockContext->particleSystem->particleSystem->setCSGroupSizes( params[0]->get< Point3i >() );
				}
				else
				{
					CU_ParsingError( cuT( "Shader not initialised" ) );
				}
			}
		}
		CU_EndAttribute()
	}

	//*************************************************************************************************

	ShaderProgram::ShaderProgram( String const & name
		, RenderSystem & renderSystem )
		: Named{ name }
		, OwnedBy< RenderSystem >{ renderSystem }
		, m_module{ name }
	{
	}

	void ShaderProgram::setFile( VkShaderStageFlagBits target, Path const & pathFile )
	{
		String source;
		{
			TextFile file{ pathFile, File::OpenMode::eRead };
			file.copyToString( source );
		}
		setSource( target, toUtf8( source ) );
		m_files[target] = pathFile;
	}

	void ShaderProgram::setSource( VkShaderStageFlagBits target, MbString const & source )
	{
		shdprog::eraseFile( target, m_files );
		shdprog::eraseStage( target, m_states );
		auto & renderSystem = *getRenderSystem();
		auto & device = renderSystem.getRenderDevice();
		auto const & spirvShader = m_module.compiled.try_emplace( getShaderStage( getOwner()->getRenderDevice(), target )
			, renderSystem.compileShader( target, getName(), source ) ).first->second;
		m_states.push_back( makeShaderState( *device, target, spirvShader, getName() + shdprog::getName( target ) ) );
	}

	void ShaderProgram::setSource( VkShaderStageFlagBits target, ShaderPtr shader )
	{
		shdprog::eraseFile( target, m_files );
		shdprog::eraseStage( target, m_states );
		auto & renderSystem = *getRenderSystem();
		auto & device = renderSystem.getRenderDevice();
		ast::EntryPointConfig entryPoint{ getShaderStage( getOwner()->getRenderDevice(), target ), "main" };
		auto const & spirvShader = m_module.compiled.try_emplace( getShaderStage( getOwner()->getRenderDevice(), target )
			, renderSystem.compileShader( target, getName(), *shader, entryPoint ) ).first->second;
		m_states.push_back( makeShaderState( *device, target, spirvShader, getName() + shdprog::getName( target ) ) );
	}

	void ShaderProgram::setSource( ShaderPtr shader )
	{
		m_files.clear();
		m_module.compiled.clear();
		m_module.shader = c3d::move( shader );
		m_states = makeProgramStates( getRenderSystem()->getRenderDevice(), m_module );
	}

	bool ShaderProgram::hasSource( ast::ShaderStage stage )const
	{
		auto it = m_module.compiled.find( stage );
		return it != m_module.compiled.end()
			&& !it->second.spirv.empty();
	}

	void ShaderProgram::addParsers( AttributeParsers & result )
	{
		BlockParserContextT< ProgramContext > programCtx{ result, CSCNSection::eShaderProgram };
		BlockParserContextT< ProgramContext > stageCtx{ result, CSCNSection::eShaderStage, CSCNSection::eShaderProgram };

		programCtx.addPushParser( cuT( "compute_program" ), CSCNSection::eShaderStage, shdprog::parserComputeShader );
		programCtx.addPopParser( cuT( "}" ), shdprog::parserShaderProgramEnd );

		stageCtx.addParser( cuT( "file" ), shdprog::parserShaderFile, { makeParameter< ParameterType::ePath >() } );
		stageCtx.addParser( cuT( "group_sizes" ), shdprog::parserShaderGroupSizes, { makeParameter< ParameterType::ePoint3I >() } );
		stageCtx.addDefaultPopParser();
	}

	SpirVShader const & compileShader( RenderSystem & renderSystem
		, ShaderModule & shaderModule )
	{
		return renderSystem.compileShader( shaderModule );
	}

	SpirVShader const & compileShader( RenderDevice const & device
		, ShaderModule & shaderModule )
	{
		return compileShader( device.renderSystem, shaderModule );
	}

	SpirVShader const & compileShader( RenderSystem & renderSystem
		, ProgramModule & shaderModule
		, ast::EntryPointConfig const & entryPoint )
	{
		return renderSystem.compileShader( shaderModule
			, entryPoint );
	}

	SpirVShader const & compileShader( RenderDevice const & device
		, ProgramModule & shaderModule
		, ast::EntryPointConfig const & entryPoint )
	{
		return compileShader( device.renderSystem, shaderModule, entryPoint );
	}

	ashes::PipelineShaderStageCreateInfoArray makeProgramStates( RenderDevice const & device
		, ProgramModule & programModule
		, ashes::Optional< ashes::SpecializationInfo > const & specialization )
	{
		ashes::PipelineShaderStageCreateInfoArray result;
		{
			auto entryPoints = ast::listEntryPoints( *programModule.shader->getStatements() );

			for ( auto const & entryPoint : entryPoints )
			{
				result.push_back( makeShaderState( *device
					, getVkShaderStage( entryPoint.stage )
					, compileShader( device, programModule, entryPoint )
					, programModule.name
					, cuT( "main" )
					, specialization ) );
			}
		}

		programModule.shader.reset();
		return result;
	}

	ast::ShaderStage getShaderStage( RenderDevice const & device
		, VkShaderStageFlagBits value )
	{
		switch ( value )
		{
		case VK_SHADER_STAGE_VERTEX_BIT:
			return ast::ShaderStage::eVertex;
		case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
			return ast::ShaderStage::eTessellationControl;
		case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
			return ast::ShaderStage::eTessellationEvaluation;
		case VK_SHADER_STAGE_GEOMETRY_BIT:
			return ast::ShaderStage::eGeometry;
#if defined( VK_NV_mesh_shader ) || defined( VK_EXT_mesh_shader )
#	if defined( VK_NV_mesh_shader ) && defined( VK_EXT_mesh_shader )
		case VK_SHADER_STAGE_MESH_BIT_EXT:
			return device.prefersMeshShaderEXT() ? ast::ShaderStage::eMesh : ast::ShaderStage::eMeshNV;
		case VK_SHADER_STAGE_TASK_BIT_EXT:
			return device.prefersMeshShaderEXT() ? ast::ShaderStage::eTask : ast::ShaderStage::eTaskNV;
#	elif defined( VK_EXT_mesh_shader )
		case VK_SHADER_STAGE_MESH_BIT_EXT:
			return ast::ShaderStage::eMesh;
		case VK_SHADER_STAGE_TASK_BIT_EXT:
			return ast::ShaderStage::eTask;
#	else
		case VK_SHADER_STAGE_MESH_BIT_NV:
			return ast::ShaderStage::eMeshNV;
		case VK_SHADER_STAGE_TASK_BIT_NV:
			return ast::ShaderStage::eTaskNV;
#	endif
#endif
		case VK_SHADER_STAGE_FRAGMENT_BIT:
			return ast::ShaderStage::eFragment;
		case VK_SHADER_STAGE_RAYGEN_BIT_KHR:
			return ast::ShaderStage::eRayGeneration;
		case VK_SHADER_STAGE_ANY_HIT_BIT_KHR:
			return ast::ShaderStage::eRayAnyHit;
		case VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR:
			return ast::ShaderStage::eRayClosestHit;
		case VK_SHADER_STAGE_MISS_BIT_KHR:
			return ast::ShaderStage::eRayMiss;
		case VK_SHADER_STAGE_INTERSECTION_BIT_KHR:
			return ast::ShaderStage::eRayIntersection;
		case VK_SHADER_STAGE_CALLABLE_BIT_KHR:
			return ast::ShaderStage::eCallable;
		case VK_SHADER_STAGE_COMPUTE_BIT:
			return ast::ShaderStage::eCompute;
		default:
			CU_Failure( "Unsupported ShaderStage" );
			return ast::ShaderStage::eCompute;
		}
	}

	VkShaderStageFlagBits getVkShaderStage( sdw::ShaderStage value )
	{
		switch ( value )
		{
		case ast::ShaderStage::eVertex:
			return VK_SHADER_STAGE_VERTEX_BIT;
		case ast::ShaderStage::eTessellationControl:
			return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		case ast::ShaderStage::eTessellationEvaluation:
			return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		case ast::ShaderStage::eGeometry:
			return VK_SHADER_STAGE_GEOMETRY_BIT;
		case ast::ShaderStage::eMeshNV:
			return VK_SHADER_STAGE_MESH_BIT_NV;
		case ast::ShaderStage::eTaskNV:
			return VK_SHADER_STAGE_TASK_BIT_NV;
		case ast::ShaderStage::eMesh:
			return VK_SHADER_STAGE_MESH_BIT_EXT;
		case ast::ShaderStage::eTask:
			return VK_SHADER_STAGE_TASK_BIT_EXT;
		case ast::ShaderStage::eFragment:
			return VK_SHADER_STAGE_FRAGMENT_BIT;
		case ast::ShaderStage::eRayGeneration:
			return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
		case ast::ShaderStage::eRayAnyHit:
			return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
		case ast::ShaderStage::eRayClosestHit:
			return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
		case ast::ShaderStage::eRayMiss:
			return VK_SHADER_STAGE_MISS_BIT_KHR;
		case ast::ShaderStage::eRayIntersection:
			return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
		case ast::ShaderStage::eCallable:
			return VK_SHADER_STAGE_CALLABLE_BIT_KHR;
		case ast::ShaderStage::eCompute:
			return VK_SHADER_STAGE_COMPUTE_BIT;
		default:
			CU_Failure( "Unsupported ShaderStage" );
			return VK_SHADER_STAGE_COMPUTE_BIT;
		}
	}

	VkShaderStageFlagBits getVkShaderStage( sdw::EntryPoint value )
	{
		return getVkShaderStage( getShaderStage( value ) );
	}

	ast::EntryPoint getEntryPointType( RenderDevice const & device
		, VkShaderStageFlagBits value )
	{
		switch ( value )
		{
		case  VK_SHADER_STAGE_VERTEX_BIT:
			return ast::EntryPoint::eVertex;
		case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
			return ast::EntryPoint::eTessellationControl;
		case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
			return ast::EntryPoint::eTessellationEvaluation;
		case VK_SHADER_STAGE_GEOMETRY_BIT:
			return ast::EntryPoint::eGeometry;
#if defined( VK_NV_mesh_shader ) || defined( VK_EXT_mesh_shader )
#	if defined( VK_NV_mesh_shader ) && defined( VK_EXT_mesh_shader )
		case VK_SHADER_STAGE_MESH_BIT_EXT:
			return device.prefersMeshShaderEXT() ? ast::EntryPoint::eMesh : ast::EntryPoint::eMeshNV;
		case VK_SHADER_STAGE_TASK_BIT_EXT:
			return device.prefersMeshShaderEXT() ? ast::EntryPoint::eTask : ast::EntryPoint::eTaskNV;
#	elif defined( VK_EXT_mesh_shader )
		case VK_SHADER_STAGE_MESH_BIT_EXT:
			return ast::EntryPoint::eMesh;
		case VK_SHADER_STAGE_TASK_BIT_EXT:
			return ast::EntryPoint::eTask;
#	else
		case VK_SHADER_STAGE_MESH_BIT_NV:
			return ast::EntryPoint::eMeshNV;
		case VK_SHADER_STAGE_TASK_BIT_NV:
			return ast::EntryPoint::eTaskNV;
#	endif
#endif
		case VK_SHADER_STAGE_FRAGMENT_BIT:
			return ast::EntryPoint::eFragment;
		case VK_SHADER_STAGE_RAYGEN_BIT_KHR:
			return ast::EntryPoint::eRayGeneration;
		case VK_SHADER_STAGE_ANY_HIT_BIT_KHR:
			return ast::EntryPoint::eRayAnyHit;
		case VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR:
			return ast::EntryPoint::eRayClosestHit;
		case VK_SHADER_STAGE_MISS_BIT_KHR:
			return ast::EntryPoint::eRayMiss;
		case VK_SHADER_STAGE_INTERSECTION_BIT_KHR:
			return ast::EntryPoint::eRayIntersection;
		case VK_SHADER_STAGE_CALLABLE_BIT_KHR:
			return ast::EntryPoint::eCallable;
		case VK_SHADER_STAGE_COMPUTE_BIT:
			return ast::EntryPoint::eCompute;
		default:
			CU_Failure( "Unsupported VkShaderStageFlagBits" );
			return ast::EntryPoint::eCompute;
		}
	}
}
