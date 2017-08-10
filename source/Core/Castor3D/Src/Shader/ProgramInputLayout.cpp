#include "ProgramInputLayout.hpp"

#include "ShaderProgram.hpp"

using namespace castor;

namespace castor3d
{
	namespace
	{
		bool IsLike( String const & p_name, String const & p_ref )
		{
			String name = string::lowerCase( p_name );
			String ref = string::lowerCase( p_ref );
			return name == ref
				   || name.find( ref ) != String::npos
				   || ref.find( name ) != String::npos;
		}
	}

	ProgramInputLayout::ProgramInputLayout( RenderSystem & renderSystem )
		: OwnedBy< RenderSystem >( renderSystem )
	{
	}

	ProgramInputLayout::~ProgramInputLayout()
	{
	}

	void ProgramInputLayout::doAddAttribute( String const & p_name, ElementType p_type, uint32_t p_offset )
	{
		auto usage = doGuessUsages( p_name, p_type );
		m_layout.push_back( BufferElementDeclaration { p_name, usage, p_type, p_offset } );
	}

	FlagCombination< ElementUsage > ProgramInputLayout::doGuessUsages( String const & p_name, ElementType p_type )
	{
		FlagCombination< ElementUsage > result = 0u;

		if ( IsLike( p_name, ShaderProgram::Position ) )
		{
			result |= uint32_t( ElementUsage::ePosition );
		}

		if ( IsLike( p_name, ShaderProgram::Normal ) )
		{
			result |= uint32_t( ElementUsage::eNormal );
		}

		if ( IsLike( p_name, ShaderProgram::Tangent ) )
		{
			result |= uint32_t( ElementUsage::eTangent );
		}

		if ( IsLike( p_name, ShaderProgram::Bitangent ) )
		{
			result |= uint32_t( ElementUsage::eBitangent );
		}

		if ( IsLike( p_name, ShaderProgram::Texture ) )
		{
			result |= uint32_t( ElementUsage::eTexCoords );
		}

		if ( IsLike( p_name, ShaderProgram::BoneIds0 ) )
		{
			result |= uint32_t( ElementUsage::eBoneIds0 );
		}

		if ( IsLike( p_name, ShaderProgram::BoneIds1 ) )
		{
			result |= uint32_t( ElementUsage::eBoneIds1 );
		}

		if ( IsLike( p_name, ShaderProgram::Weights0 ) )
		{
			result |= uint32_t( ElementUsage::eBoneWeights0 );
		}

		if ( IsLike( p_name, ShaderProgram::Weights1 ) )
		{
			result |= uint32_t( ElementUsage::eBoneWeights1 );
		}

		if ( IsLike( p_name, ShaderProgram::Transform ) )
		{
			result |= uint32_t( ElementUsage::eTransform );
		}

		return result;
	}

	bool operator==( ProgramInputLayout const & p_lhs, ProgramInputLayout const & p_rhs )
	{
		bool result = p_lhs.m_layout.size() == p_rhs.m_layout.size();
		auto itl = p_lhs.m_layout.begin();
		auto itr = p_rhs.m_layout.begin();

		while ( result && itl != p_lhs.m_layout.end() )
		{
			result = *itl == *itr;
			++itl;
			++itr;
		}

		return result;
	}

	bool operator!=( ProgramInputLayout const & p_lhs, ProgramInputLayout const & p_rhs )
	{
		return !( p_lhs == p_rhs );
	}
}
