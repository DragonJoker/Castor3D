#include "FrameVariable.hpp"

#include "FrameVariableBuffer.hpp"
#include "OneFrameVariable.hpp"
#include "PointFrameVariable.hpp"
#include "MatrixFrameVariable.hpp"

using namespace Castor;

namespace Castor3D
{
	const xchar * FrameVariableDataTyper< bool >::DataTypeName = cuT( "bool" );
	const xchar * FrameVariableDataTyper< int >::DataTypeName = cuT( "int" );
	const xchar * FrameVariableDataTyper< uint32_t >::DataTypeName = cuT( "uint" );
	const xchar * FrameVariableDataTyper< float >::DataTypeName = cuT( "float" );
	const xchar * FrameVariableDataTyper< double >::DataTypeName = cuT( "double" );

	//*************************************************************************************************

	const xchar * OneFrameVariableDefinitions< bool >::FullTypeName = cuT( "bool" );
	const xchar * OneFrameVariableDefinitions< int >::FullTypeName = cuT( "int" );
	const xchar * OneFrameVariableDefinitions< uint32_t >::FullTypeName = cuT( "uint" );
	const xchar * OneFrameVariableDefinitions< float >::FullTypeName = cuT( "float" );
	const xchar * OneFrameVariableDefinitions< double >::FullTypeName = cuT( "double" );

	//*************************************************************************************************

	const xchar * PntFrameVariableDefinitions< bool, 2 >::FullTypeName = cuT( "bvec2" );
	const xchar * PntFrameVariableDefinitions< bool, 3 >::FullTypeName = cuT( "bvec3" );
	const xchar * PntFrameVariableDefinitions< bool, 4 >::FullTypeName = cuT( "bvec4" );
	const xchar * PntFrameVariableDefinitions< int, 2 >::FullTypeName = cuT( "ivec2" );
	const xchar * PntFrameVariableDefinitions< int, 3 >::FullTypeName = cuT( "ivec3" );
	const xchar * PntFrameVariableDefinitions< int, 4 >::FullTypeName = cuT( "ivec4" );
	const xchar * PntFrameVariableDefinitions< uint32_t, 2 >::FullTypeName = cuT( "uivec2" );
	const xchar * PntFrameVariableDefinitions< uint32_t, 3 >::FullTypeName = cuT( "uivec3" );
	const xchar * PntFrameVariableDefinitions< uint32_t, 4 >::FullTypeName = cuT( "uivec4" );
	const xchar * PntFrameVariableDefinitions< float, 2 >::FullTypeName = cuT( "vec2" );
	const xchar * PntFrameVariableDefinitions< float, 3 >::FullTypeName = cuT( "vec3" );
	const xchar * PntFrameVariableDefinitions< float, 4 >::FullTypeName = cuT( "vec4" );
	const xchar * PntFrameVariableDefinitions< double, 2 >::FullTypeName = cuT( "dvec2" );
	const xchar * PntFrameVariableDefinitions< double, 3 >::FullTypeName = cuT( "dvec3" );
	const xchar * PntFrameVariableDefinitions< double, 4 >::FullTypeName = cuT( "dvec4" );

	//*************************************************************************************************

	const xchar * MtxFrameVariableDefinitions< bool, 2, 2 >::FullTypeName = cuT( "bmat2x2" );
	const xchar * MtxFrameVariableDefinitions< bool, 2, 3 >::FullTypeName = cuT( "bmat2x3" );
	const xchar * MtxFrameVariableDefinitions< bool, 2, 4 >::FullTypeName = cuT( "bmat2x4" );
	const xchar * MtxFrameVariableDefinitions< bool, 3, 2 >::FullTypeName = cuT( "bmat3x2" );
	const xchar * MtxFrameVariableDefinitions< bool, 3, 3 >::FullTypeName = cuT( "bmat3x3" );
	const xchar * MtxFrameVariableDefinitions< bool, 3, 4 >::FullTypeName = cuT( "bmat3x4" );
	const xchar * MtxFrameVariableDefinitions< bool, 4, 2 >::FullTypeName = cuT( "bmat4x2" );
	const xchar * MtxFrameVariableDefinitions< bool, 4, 3 >::FullTypeName = cuT( "bmat4x3" );
	const xchar * MtxFrameVariableDefinitions< bool, 4, 4 >::FullTypeName = cuT( "bmat4x4" );
	const xchar * MtxFrameVariableDefinitions< int, 2, 2 >::FullTypeName = cuT( "imat2x2" );
	const xchar * MtxFrameVariableDefinitions< int, 2, 3 >::FullTypeName = cuT( "imat2x3" );
	const xchar * MtxFrameVariableDefinitions< int, 2, 4 >::FullTypeName = cuT( "imat2x4" );
	const xchar * MtxFrameVariableDefinitions< int, 3, 2 >::FullTypeName = cuT( "imat3x2" );
	const xchar * MtxFrameVariableDefinitions< int, 3, 3 >::FullTypeName = cuT( "imat3x3" );
	const xchar * MtxFrameVariableDefinitions< int, 3, 4 >::FullTypeName = cuT( "imat3x4" );
	const xchar * MtxFrameVariableDefinitions< int, 4, 2 >::FullTypeName = cuT( "imat4x2" );
	const xchar * MtxFrameVariableDefinitions< int, 4, 3 >::FullTypeName = cuT( "imat4x3" );
	const xchar * MtxFrameVariableDefinitions< int, 4, 4 >::FullTypeName = cuT( "imat4x4" );
	const xchar * MtxFrameVariableDefinitions< uint32_t, 2, 2 >::FullTypeName = cuT( "uimat2x2" );
	const xchar * MtxFrameVariableDefinitions< uint32_t, 2, 3 >::FullTypeName = cuT( "uimat2x3" );
	const xchar * MtxFrameVariableDefinitions< uint32_t, 2, 4 >::FullTypeName = cuT( "uimat2x4" );
	const xchar * MtxFrameVariableDefinitions< uint32_t, 3, 2 >::FullTypeName = cuT( "uimat3x2" );
	const xchar * MtxFrameVariableDefinitions< uint32_t, 3, 3 >::FullTypeName = cuT( "uimat3x3" );
	const xchar * MtxFrameVariableDefinitions< uint32_t, 3, 4 >::FullTypeName = cuT( "uimat3x4" );
	const xchar * MtxFrameVariableDefinitions< uint32_t, 4, 2 >::FullTypeName = cuT( "uimat4x2" );
	const xchar * MtxFrameVariableDefinitions< uint32_t, 4, 3 >::FullTypeName = cuT( "uimat4x3" );
	const xchar * MtxFrameVariableDefinitions< uint32_t, 4, 4 >::FullTypeName = cuT( "uimat4x4" );
	const xchar * MtxFrameVariableDefinitions< float, 2, 2 >::FullTypeName = cuT( "mat2x2" );
	const xchar * MtxFrameVariableDefinitions< float, 2, 3 >::FullTypeName = cuT( "mat2x3" );
	const xchar * MtxFrameVariableDefinitions< float, 2, 4 >::FullTypeName = cuT( "mat2x4" );
	const xchar * MtxFrameVariableDefinitions< float, 3, 2 >::FullTypeName = cuT( "mat3x2" );
	const xchar * MtxFrameVariableDefinitions< float, 3, 3 >::FullTypeName = cuT( "mat3x3" );
	const xchar * MtxFrameVariableDefinitions< float, 3, 4 >::FullTypeName = cuT( "mat3x4" );
	const xchar * MtxFrameVariableDefinitions< float, 4, 2 >::FullTypeName = cuT( "mat4x2" );
	const xchar * MtxFrameVariableDefinitions< float, 4, 3 >::FullTypeName = cuT( "mat4x3" );
	const xchar * MtxFrameVariableDefinitions< float, 4, 4 >::FullTypeName = cuT( "mat4x4" );
	const xchar * MtxFrameVariableDefinitions< double, 2, 2 >::FullTypeName = cuT( "dmat2x2" );
	const xchar * MtxFrameVariableDefinitions< double, 2, 3 >::FullTypeName = cuT( "dmat2x3" );
	const xchar * MtxFrameVariableDefinitions< double, 2, 4 >::FullTypeName = cuT( "dmat2x4" );
	const xchar * MtxFrameVariableDefinitions< double, 3, 2 >::FullTypeName = cuT( "dmat3x2" );
	const xchar * MtxFrameVariableDefinitions< double, 3, 3 >::FullTypeName = cuT( "dmat3x3" );
	const xchar * MtxFrameVariableDefinitions< double, 3, 4 >::FullTypeName = cuT( "dmat3x4" );
	const xchar * MtxFrameVariableDefinitions< double, 4, 2 >::FullTypeName = cuT( "dmat4x2" );
	const xchar * MtxFrameVariableDefinitions< double, 4, 3 >::FullTypeName = cuT( "dmat4x3" );
	const xchar * MtxFrameVariableDefinitions< double, 4, 4 >::FullTypeName = cuT( "dmat4x4" );

	//*************************************************************************************************

	FrameVariable::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< FrameVariable >{ p_tabs }
	{
	}

	bool FrameVariable::TextWriter::operator()( FrameVariable const & p_variable, TextFile & p_file )
	{
		bool l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "variable\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\ttype " ) + p_variable.GetFullTypeName() + cuT( "\n" ) ) > 0;
			Castor::TextWriter< FrameVariable >::CheckError( l_return, "FrameVariable type" );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\tname " ) + p_variable.GetName() + cuT( "\n" ) ) > 0;
			Castor::TextWriter< FrameVariable >::CheckError( l_return, "FrameVariable name" );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\tvalue " ) + p_variable.GetStrValue() + cuT( "\n" ) ) > 0;
			Castor::TextWriter< FrameVariable >::CheckError( l_return, "FrameVariable value" );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return l_return;
	}

	//*************************************************************************************************

	FrameVariable::FrameVariable( ShaderProgram & p_program, uint32_t p_occurences )
		: m_changed( true )
		, m_occurences( p_occurences )
		, m_program( p_program )
	{
	}

	FrameVariable::~FrameVariable()
	{
	}

	void FrameVariable::SetStrValue( String const & p_value, uint32_t p_index )
	{
		REQUIRE( p_index < m_occurences );
		DoSetStrValue( p_value, p_index );
		m_changed = true;
	}

	String FrameVariable::GetStrValue( uint32_t p_index )const
	{
		REQUIRE( p_index < m_occurences );
		return DoGetStrValue( p_index );
	}

	void FrameVariable::SetStrValues( String const & p_value )
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

	String FrameVariable::GetStrValues()const
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
