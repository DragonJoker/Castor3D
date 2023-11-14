#include "Castor3D/Shader/Shaders/GlslSubmeshShaders.hpp"

#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/SubmeshComponentRegister.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Writer.hpp>

namespace castor3d::shader
{
	//************************************************************************************************

	SubmeshShaders::SubmeshShaders( SubmeshComponentRegister const & compRegister
		, PipelineFlags const & flags )
		: m_compRegister{ compRegister }
		, m_shaders{ m_compRegister.getSurfaceShaders( flags ) }
		, m_flags{ flags }
	{
	}

	void SubmeshShaders::fillSurface( sdw::type::IOStruct & surface )const
	{
		if ( surface.empty() )
		{
			uint32_t index = 0u;

			for ( auto & shader : m_shaders )
			{
				shader->fillSurfaceType( surface, &index );
			}

			surface.declMember( "velocity", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, ( m_flags.enableVelocity() ? index++ : 0 )
				, m_flags.enableVelocity() );
			surface.declMember( "objectIds", ast::type::Kind::eVec4U
				, ast::type::NotArray
				, ( m_flags.enableInstantiation() ? index++ : 0 )
				, m_flags.enableInstantiation() );
		}
	}

	void SubmeshShaders::fillSurface( sdw::type::BaseStruct & surface )const
	{
		if ( surface.empty() )
		{
			for ( auto & shader : m_shaders )
			{
				shader->fillSurfaceType( surface, nullptr );
			}

			surface.declMember( "velocity", ast::type::Kind::eVec3F
				, ast::type::NotArray
				, m_flags.enableVelocity() );
			surface.declMember( "objectIds", ast::type::Kind::eVec4U
				, ast::type::NotArray
				, m_flags.enableInstantiation() );
		}
	}
}
