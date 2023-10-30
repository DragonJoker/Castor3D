#include "Castor3D/Shader/Program.hpp"

#include "Castor3D/Render/RenderSystem.hpp"

#include <CastorUtils/Stream/StreamPrefixManipulators.hpp>

CU_ImplementSmartPtr( castor3d, ShaderProgram )

namespace castor3d
{
	namespace shdprog
	{
		template< typename CharType, typename PrefixType >
		inline std::basic_ostream< CharType > & operator<<( std::basic_ostream< CharType > & stream
			, castor::format::BasePrefixer< CharType, PrefixType > const & prefix )
		{
			auto * sbuf = dynamic_cast< castor::format::BasicPrefixBuffer< castor::format::BasePrefixer< CharType, PrefixType >, CharType > * >( stream.rdbuf() );

			if ( !sbuf )
			{
				castor::format::installPrefixBuffer< PrefixType >( stream );
				stream.register_callback( castor::format::callback< PrefixType, CharType >, 0 );
			}

			return stream;
		}

		static bool doAddModule( VkShaderStageFlagBits stage
			, std::string const & name
			, std::map< VkShaderStageFlagBits, ShaderModule > & modules )
		{
			bool added = false;

			if ( modules.find( stage ) == modules.end() )
			{
				modules.emplace( stage, ShaderModule{ stage, name } );
				added = true;
			}

			return added;
		}

		static ashes::PipelineShaderStageCreateInfo loadShader( RenderDevice const & device
			, ShaderProgram::CompiledShader const & compiled
			, VkShaderStageFlagBits stage )
		{
			return makeShaderState( *device
				, stage
				, *compiled.shader
				, compiled.name );
		}
	}

	//*************************************************************************************************

	ShaderProgram::ShaderProgram( castor::String const & name
		, RenderSystem & renderSystem )
		: castor::Named( name )
		, OwnedBy< RenderSystem >( renderSystem )
	{
	}

	ShaderProgram::~ShaderProgram()
	{
	}

	void ShaderProgram::setFile( VkShaderStageFlagBits target, castor::Path const & pathFile )
	{
		m_files[target] = pathFile;
		castor::TextFile file{ pathFile, castor::File::OpenMode::eRead };
		castor::String source;
		file.copyToString( source );
		auto added = shdprog::doAddModule( target, getName(), m_modules );

		if ( added )
		{
			auto & renderSystem = *getRenderSystem();
			auto it = m_modules.find( target );
			it->second.source = source;
			m_compiled.emplace( target
				, CompiledShader{ getName()
					, &renderSystem.compileShader( it->second ) } );
			m_states.push_back( shdprog::loadShader( renderSystem.getRenderDevice()
				, m_compiled[target]
				, target ) );
		}
	}

	castor::Path ShaderProgram::getFile( VkShaderStageFlagBits target )const
	{
		auto it = m_files.find( target );
		CU_Require( it != m_files.end() );
		return it->second;
	}

	bool ShaderProgram::hasFile( VkShaderStageFlagBits target )const
	{
		auto it = m_files.find( target );
		return it != m_files.end()
			&& !it->second.empty();
	}

	void ShaderProgram::setSource( VkShaderStageFlagBits target, castor::String const & source )
	{
		m_files[target].clear();
		auto added = shdprog::doAddModule( target, getName(), m_modules );

		if ( added )
		{
			auto & renderSystem = *getRenderSystem();
			auto it = m_modules.find( target );
			it->second.source = source;
			m_compiled.emplace( target
				, CompiledShader{ getName()
					, &renderSystem.compileShader( it->second ) } );
			m_states.push_back( shdprog::loadShader( renderSystem.getRenderDevice()
				, m_compiled[target]
				, target ) );
		}
	}

	void ShaderProgram::setSource( VkShaderStageFlagBits target, ShaderPtr shader )
	{
		m_files[target].clear();
		auto added = shdprog::doAddModule( target, getName(), m_modules );

		if ( added )
		{
			auto & renderSystem = *getRenderSystem();
			auto it = m_modules.find( target );
			it->second.shader = std::move( shader );
			m_compiled.emplace( target
				, CompiledShader{ getName()
					, &renderSystem.compileShader( it->second ) } );
			m_states.push_back( shdprog::loadShader( renderSystem.getRenderDevice()
				, m_compiled[target]
				, target ) );
		}
	}

	ShaderModule const & ShaderProgram::getSource( VkShaderStageFlagBits target )const
	{
		auto it = m_modules.find( target );
		CU_Require( it != m_modules.end() );
		return it->second;
	}

	bool ShaderProgram::hasSource( VkShaderStageFlagBits target )const
	{
		auto it = m_compiled.find( target );
		return it != m_compiled.end()
			&& !it->second.shader->spirv.empty();
	}

	SpirVShader const & compileShader( RenderSystem & renderSystem
		, ShaderModule & module )
	{
		return renderSystem.compileShader( module );
	}

	SpirVShader const & compileShader( RenderDevice const & device
		, ShaderModule & module )
	{
		return compileShader( device.renderSystem, module );
	}

	SpirVShader const & compileShader( RenderSystem & renderSystem
		, ProgramModule & module
		, ast::EntryPointConfig const & entryPoint )
	{
		return renderSystem.compileShader( module
			, entryPoint );
	}

	SpirVShader const & compileShader( RenderDevice const & device
		, ProgramModule & module
		, ast::EntryPointConfig const & entryPoint )
	{
		return compileShader( device.renderSystem, module, entryPoint );
	}

	ashes::PipelineShaderStageCreateInfoArray makeProgramStates( RenderDevice const & device
		, ProgramModule & programModule
		, ashes::Optional< ashes::SpecializationInfo > specialization )
	{
		ashes::PipelineShaderStageCreateInfoArray result;
		{
			auto entryPoints = ast::listEntryPoints( programModule.shader->getContainer() );

			for ( auto entryPoint : entryPoints )
			{
				result.push_back( makeShaderState( *device
					, getShaderStage( entryPoint.stage )
					, compileShader( device, programModule, entryPoint )
					, programModule.name
					, "main"
					, std::move( specialization ) ) );
			}
		}

		programModule.shader.reset();
		return result;
	}

	VkShaderStageFlagBits getShaderStage( sdw::ShaderStage value )
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
}
