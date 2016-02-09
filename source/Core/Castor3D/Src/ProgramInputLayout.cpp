#include "ProgramInputLayout.hpp"

#include "ShaderProgram.hpp"

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		bool IsLike( String const & p_name, String const & p_ref )
		{
			String l_name = string::lower_case( p_name );
			String l_ref = string::lower_case( p_ref );
			return l_name == l_ref
				|| l_name.find( l_ref ) != String::npos
				|| l_ref.find( l_name ) != String::npos;
		}
	}

	ProgramInputLayout::ProgramInputLayout( RenderSystem & p_renderSystem )
		: OwnedBy< RenderSystem >( p_renderSystem )
	{
	}

	ProgramInputLayout::~ProgramInputLayout()
	{
	}

	void ProgramInputLayout::DoAddAttribute( String const & p_name, eELEMENT_TYPE p_type, uint32_t p_offset )
	{
		auto l_usage = DoGuessUsages( p_name, p_type );
		m_layout.push_back( BufferElementDeclaration{ p_name, l_usage, p_type, p_offset } );
	}

	uint32_t ProgramInputLayout::DoGuessUsages( String const & p_name, eELEMENT_TYPE p_type )
	{
		uint32_t l_return = 0u;

		if ( IsLike( p_name, ShaderProgram::Position ) )
		{
			l_return |= eELEMENT_USAGE_POSITION;
		}

		if ( IsLike( p_name, ShaderProgram::Normal ) )
		{
			l_return |= eELEMENT_USAGE_NORMAL;
		}

		if ( IsLike( p_name, ShaderProgram::Tangent ) )
		{
			l_return |= eELEMENT_USAGE_TANGENT;
		}

		if ( IsLike( p_name, ShaderProgram::Bitangent ) )
		{
			l_return |= eELEMENT_USAGE_BITANGENT;
		}

		if ( IsLike( p_name, ShaderProgram::Texture ) )
		{
			l_return |= eELEMENT_USAGE_TEXCOORDS;
		}

		if ( IsLike( p_name, ShaderProgram::BoneIds0 ) )
		{
			l_return |= eELEMENT_USAGE_BONE_IDS0;
		}

		if ( IsLike( p_name, ShaderProgram::BoneIds1 ) )
		{
			l_return |= eELEMENT_USAGE_BONE_IDS1;
		}

		if ( IsLike( p_name, ShaderProgram::Weights0 ) )
		{
			l_return |= eELEMENT_USAGE_BONE_WEIGHTS0;
		}

		if ( IsLike( p_name, ShaderProgram::Weights1 ) )
		{
			l_return |= eELEMENT_USAGE_BONE_WEIGHTS1;
		}

		if ( IsLike( p_name, ShaderProgram::Transform ) )
		{
			l_return |= eELEMENT_USAGE_TRANSFORM;
		}

		return l_return;
	}

	bool operator==( ProgramInputLayout const & p_lhs, ProgramInputLayout const & p_rhs )
	{
		bool l_return = p_lhs.m_layout.size() == p_rhs.m_layout.size();
		auto l_itl = p_lhs.m_layout.begin();
		auto l_itr = p_rhs.m_layout.begin();

		while ( l_return && l_itl != p_lhs.m_layout.end() )
		{
			l_return = *l_itl == *l_itr;
			++l_itl;
			++l_itr;
		}

		return l_return;
	}

	bool operator!=( ProgramInputLayout const & p_lhs, ProgramInputLayout const & p_rhs )
	{
		return !( p_lhs == p_rhs );
	}
}
