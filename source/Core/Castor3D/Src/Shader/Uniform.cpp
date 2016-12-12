#include "Uniform.hpp"

#include "UniformBuffer.hpp"
#include "OneUniform.hpp"
#include "PointUniform.hpp"
#include "MatrixUniform.hpp"

using namespace Castor;

namespace Castor3D
{
	const Castor::String UniformDataTyper< bool >::DataTypeName = cuT( "bool" );
	const Castor::String UniformDataTyper< int >::DataTypeName = cuT( "int" );
	const Castor::String UniformDataTyper< uint32_t >::DataTypeName = cuT( "uint" );
	const Castor::String UniformDataTyper< float >::DataTypeName = cuT( "float" );
	const Castor::String UniformDataTyper< double >::DataTypeName = cuT( "double" );

	//*************************************************************************************************

	const Castor::String OneUniformDefinitions< bool >::FullTypeName = cuT( "bool" );
	const Castor::String OneUniformDefinitions< int >::FullTypeName = cuT( "int" );
	const Castor::String OneUniformDefinitions< uint32_t >::FullTypeName = cuT( "uint" );
	const Castor::String OneUniformDefinitions< float >::FullTypeName = cuT( "float" );
	const Castor::String OneUniformDefinitions< double >::FullTypeName = cuT( "double" );

	//*************************************************************************************************

	const Castor::String PntUniformDefinitions< bool, 2 >::FullTypeName = cuT( "vec2b" );
	const Castor::String PntUniformDefinitions< bool, 3 >::FullTypeName = cuT( "vec3b" );
	const Castor::String PntUniformDefinitions< bool, 4 >::FullTypeName = cuT( "vec4b" );
	const Castor::String PntUniformDefinitions< int, 2 >::FullTypeName = cuT( "vec2i" );
	const Castor::String PntUniformDefinitions< int, 3 >::FullTypeName = cuT( "vec3i" );
	const Castor::String PntUniformDefinitions< int, 4 >::FullTypeName = cuT( "vec4i" );
	const Castor::String PntUniformDefinitions< uint32_t, 2 >::FullTypeName = cuT( "vec2ui" );
	const Castor::String PntUniformDefinitions< uint32_t, 3 >::FullTypeName = cuT( "vec3ui" );
	const Castor::String PntUniformDefinitions< uint32_t, 4 >::FullTypeName = cuT( "vec4ui" );
	const Castor::String PntUniformDefinitions< float, 2 >::FullTypeName = cuT( "vec2f" );
	const Castor::String PntUniformDefinitions< float, 3 >::FullTypeName = cuT( "vec3f" );
	const Castor::String PntUniformDefinitions< float, 4 >::FullTypeName = cuT( "vec4f" );
	const Castor::String PntUniformDefinitions< double, 2 >::FullTypeName = cuT( "vec2d" );
	const Castor::String PntUniformDefinitions< double, 3 >::FullTypeName = cuT( "vec3d" );
	const Castor::String PntUniformDefinitions< double, 4 >::FullTypeName = cuT( "vec4d" );

	//*************************************************************************************************

	const Castor::String MtxUniformDefinitions< bool, 2, 2 >::FullTypeName = cuT( "mat2x2b" );
	const Castor::String MtxUniformDefinitions< bool, 2, 3 >::FullTypeName = cuT( "mat2x3b" );
	const Castor::String MtxUniformDefinitions< bool, 2, 4 >::FullTypeName = cuT( "mat2x4b" );
	const Castor::String MtxUniformDefinitions< bool, 3, 2 >::FullTypeName = cuT( "mat3x2b" );
	const Castor::String MtxUniformDefinitions< bool, 3, 3 >::FullTypeName = cuT( "mat3x3b" );
	const Castor::String MtxUniformDefinitions< bool, 3, 4 >::FullTypeName = cuT( "mat3x4b" );
	const Castor::String MtxUniformDefinitions< bool, 4, 2 >::FullTypeName = cuT( "mat4x2b" );
	const Castor::String MtxUniformDefinitions< bool, 4, 3 >::FullTypeName = cuT( "mat4x3b" );
	const Castor::String MtxUniformDefinitions< bool, 4, 4 >::FullTypeName = cuT( "mat4x4b" );
	const Castor::String MtxUniformDefinitions< int, 2, 2 >::FullTypeName = cuT( "mat2x2i" );
	const Castor::String MtxUniformDefinitions< int, 2, 3 >::FullTypeName = cuT( "mat2x3i" );
	const Castor::String MtxUniformDefinitions< int, 2, 4 >::FullTypeName = cuT( "mat2x4i" );
	const Castor::String MtxUniformDefinitions< int, 3, 2 >::FullTypeName = cuT( "mat3x2i" );
	const Castor::String MtxUniformDefinitions< int, 3, 3 >::FullTypeName = cuT( "mat3x3i" );
	const Castor::String MtxUniformDefinitions< int, 3, 4 >::FullTypeName = cuT( "mat3x4i" );
	const Castor::String MtxUniformDefinitions< int, 4, 2 >::FullTypeName = cuT( "mat4x2i" );
	const Castor::String MtxUniformDefinitions< int, 4, 3 >::FullTypeName = cuT( "mat4x3i" );
	const Castor::String MtxUniformDefinitions< int, 4, 4 >::FullTypeName = cuT( "mat4x4i" );
	const Castor::String MtxUniformDefinitions< uint32_t, 2, 2 >::FullTypeName = cuT( "mat2x2ui" );
	const Castor::String MtxUniformDefinitions< uint32_t, 2, 3 >::FullTypeName = cuT( "mat2x3ui" );
	const Castor::String MtxUniformDefinitions< uint32_t, 2, 4 >::FullTypeName = cuT( "mat2x4ui" );
	const Castor::String MtxUniformDefinitions< uint32_t, 3, 2 >::FullTypeName = cuT( "mat3x2ui" );
	const Castor::String MtxUniformDefinitions< uint32_t, 3, 3 >::FullTypeName = cuT( "mat3x3ui" );
	const Castor::String MtxUniformDefinitions< uint32_t, 3, 4 >::FullTypeName = cuT( "mat3x4ui" );
	const Castor::String MtxUniformDefinitions< uint32_t, 4, 2 >::FullTypeName = cuT( "mat4x2ui" );
	const Castor::String MtxUniformDefinitions< uint32_t, 4, 3 >::FullTypeName = cuT( "mat4x3ui" );
	const Castor::String MtxUniformDefinitions< uint32_t, 4, 4 >::FullTypeName = cuT( "mat4x4ui" );
	const Castor::String MtxUniformDefinitions< float, 2, 2 >::FullTypeName = cuT( "mat2x2f" );
	const Castor::String MtxUniformDefinitions< float, 2, 3 >::FullTypeName = cuT( "mat2x3f" );
	const Castor::String MtxUniformDefinitions< float, 2, 4 >::FullTypeName = cuT( "mat2x4f" );
	const Castor::String MtxUniformDefinitions< float, 3, 2 >::FullTypeName = cuT( "mat3x2f" );
	const Castor::String MtxUniformDefinitions< float, 3, 3 >::FullTypeName = cuT( "mat3x3f" );
	const Castor::String MtxUniformDefinitions< float, 3, 4 >::FullTypeName = cuT( "mat3x4f" );
	const Castor::String MtxUniformDefinitions< float, 4, 2 >::FullTypeName = cuT( "mat4x2f" );
	const Castor::String MtxUniformDefinitions< float, 4, 3 >::FullTypeName = cuT( "mat4x3f" );
	const Castor::String MtxUniformDefinitions< float, 4, 4 >::FullTypeName = cuT( "mat4x4f" );
	const Castor::String MtxUniformDefinitions< double, 2, 2 >::FullTypeName = cuT( "mat2x2d" );
	const Castor::String MtxUniformDefinitions< double, 2, 3 >::FullTypeName = cuT( "mat2x3d" );
	const Castor::String MtxUniformDefinitions< double, 2, 4 >::FullTypeName = cuT( "mat2x4d" );
	const Castor::String MtxUniformDefinitions< double, 3, 2 >::FullTypeName = cuT( "mat3x2d" );
	const Castor::String MtxUniformDefinitions< double, 3, 3 >::FullTypeName = cuT( "mat3x3d" );
	const Castor::String MtxUniformDefinitions< double, 3, 4 >::FullTypeName = cuT( "mat3x4d" );
	const Castor::String MtxUniformDefinitions< double, 4, 2 >::FullTypeName = cuT( "mat4x2d" );
	const Castor::String MtxUniformDefinitions< double, 4, 3 >::FullTypeName = cuT( "mat4x3d" );
	const Castor::String MtxUniformDefinitions< double, 4, 4 >::FullTypeName = cuT( "mat4x4d" );

	//*************************************************************************************************

	Uniform::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< Uniform >{ p_tabs }
	{
	}

	bool Uniform::TextWriter::operator()( Uniform const & p_variable, TextFile & p_file )
	{
		bool l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "variable\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\ttype " ) + p_variable.GetFullTypeName() + cuT( "\n" ) ) > 0;
			Castor::TextWriter< Uniform >::CheckError( l_return, "Uniform type" );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\tname " ) + p_variable.GetName() + cuT( "\n" ) ) > 0;
			Castor::TextWriter< Uniform >::CheckError( l_return, "Uniform name" );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\tvalue " ) + p_variable.GetStrValue() + cuT( "\n" ) ) > 0;
			Castor::TextWriter< Uniform >::CheckError( l_return, "Uniform value" );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return l_return;
	}

	//*************************************************************************************************

	Uniform::Uniform( ShaderProgram & p_program, uint32_t p_occurences )
		: m_changed( true )
		, m_occurences( p_occurences )
		, m_program( p_program )
	{
	}

	Uniform::~Uniform()
	{
	}

	void Uniform::SetStrValue( String const & p_value, uint32_t p_index )
	{
		REQUIRE( p_index < m_occurences );
		DoSetStrValue( p_value, p_index );
		m_changed = true;
	}

	String Uniform::GetStrValue( uint32_t p_index )const
	{
		REQUIRE( p_index < m_occurences );
		return DoGetStrValue( p_index );
	}

	void Uniform::SetStrValues( String const & p_value )
	{
		StringArray l_values = string::split( p_value, cuT( "|" ) );
		REQUIRE( l_values.size() == m_occurences );
		uint32_t l_index = 0u;

		for ( auto l_value : l_values )
		{
			if ( l_index < m_occurences )
			{
				SetStrValue( l_value, l_index++ );
			}
		}

		m_changed = true;
	}

	String Uniform::GetStrValues()const
	{
		String l_return;
		String l_separator;

		for ( uint32_t i = 0u; i < m_occurences; ++i )
		{
			l_return += l_separator + GetStrValue( i );
			l_separator = "|";
		}

		return l_return;
	}
}
