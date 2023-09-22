#include "Castor3D/Cache/ShaderCache.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <ShaderWriter/Source.hpp>

#include <ashespp/Core/Device.hpp>

namespace castor3d
{
	ShaderProgramCache::ShaderProgramCache( Engine & engine )
		: OwnedBy< Engine >( engine )
	{
	}

	ShaderProgramCache::~ShaderProgramCache()
	{
	}

	void ShaderProgramCache::clear()
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		m_autoGenerated.clear();
		m_programs.clear();
	}

	ShaderProgramRPtr ShaderProgramCache::getNewProgram( castor::String const & name
		, bool initialise )
	{
		auto result = castor::makeUnique< ShaderProgram >( name , *getEngine()->getRenderSystem() );
		auto lock( castor::makeUniqueLock( m_mutex ) );
		auto ret = result.get();
		doAddProgram( std::move( result ) );
		return ret;
	}

	ShaderProgramRPtr ShaderProgramCache::getAutomaticProgram( RenderNodesPass const & renderPass
		, PipelineFlags const & flags )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );

		if ( auto result = doFindAutomaticProgram( renderPass, flags ) )
		{
			return result;
		}

		auto result = doCreateAutomaticProgram( renderPass, flags );
		CU_Require( result );
		auto ret = result.get();
		doAddAutomaticProgram( std::move( result ), renderPass, flags );
		return ret;
	}

	ShaderProgramRPtr ShaderProgramCache::doFindAutomaticProgram( RenderNodesPass const & renderPass
		, PipelineFlags const & flags )
	{
		auto it = std::find_if( m_autoGenerated.begin()
			, m_autoGenerated.end()
			, [&flags, &renderPass]( AutoGeneratedProgram const & lookup )
			{
				return  renderPass.getDeferredLightingMode() == lookup.deferredLighting
					&& lookup.flags == flags;
			} );

		if ( it != m_autoGenerated.end() )
		{
			return it->program;
		}

		return nullptr;
	}

	ShaderProgramUPtr ShaderProgramCache::doCreateAutomaticProgram( RenderNodesPass const & renderPass
		, PipelineFlags const & flags )const
	{
		auto result = castor::makeUnique< ShaderProgram >( renderPass.getName(), *getEngine()->getRenderSystem() );

		if ( getEngine()->hasMeshShaders()
			&& flags.usesMesh() )
		{
			if ( flags.usesTask() )
			{
				auto task = renderPass.getTaskShaderSource( flags );

				if ( task )
				{
					result->setSource( VK_SHADER_STAGE_TASK_BIT_NV
						, std::move( task ) );
				}
			}

			result->setSource( VK_SHADER_STAGE_MESH_BIT_NV
				, renderPass.getMeshShaderSource( flags ) );
		}
		else
		{
			result->setSource( VK_SHADER_STAGE_VERTEX_BIT
				, renderPass.getVertexShaderSource( flags ) );

			auto hull = renderPass.getHullShaderSource( flags );

			if ( hull )
			{
				result->setSource( VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT
					, std::move( hull ) );
			}

			auto domain = renderPass.getDomainShaderSource( flags );

			if ( domain )
			{
				result->setSource( VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
					, std::move( domain ) );
			}

			auto geometry = renderPass.getGeometryShaderSource( flags );

			if ( geometry )
			{
				result->setSource( VK_SHADER_STAGE_GEOMETRY_BIT
					, std::move( geometry ) );
			}
		}

		result->setSource( VK_SHADER_STAGE_FRAGMENT_BIT
			, renderPass.getPixelShaderSource( flags ) );
		return result;
	}

	void ShaderProgramCache::doAddAutomaticProgram( ShaderProgramUPtr program
		, RenderNodesPass const & renderPass
		, PipelineFlags const & flags )
	{
		m_autoGenerated.push_back( { flags, renderPass.getDeferredLightingMode(), program.get() } );
		doAddProgram( std::move( program ) );
	}

	void ShaderProgramCache::doAddProgram( ShaderProgramUPtr program )
	{
		m_programs.push_back( std::move( program ) );
	}
}
