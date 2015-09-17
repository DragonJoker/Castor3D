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
	const xchar * FrameVariableDataTyper< TextureBase * >::DataTypeName = cuT( "sampler" );

	//*************************************************************************************************

	const xchar * OneFrameVariableDefinitions< bool >::FullTypeName = cuT( "bool" );
	const xchar * OneFrameVariableDefinitions< int >::FullTypeName = cuT( "int" );
	const xchar * OneFrameVariableDefinitions< uint32_t >::FullTypeName = cuT( "uint" );
	const xchar * OneFrameVariableDefinitions< float >::FullTypeName = cuT( "float" );
	const xchar * OneFrameVariableDefinitions< double >::FullTypeName = cuT( "double" );
	const xchar * OneFrameVariableDefinitions< TextureBase * >::FullTypeName = cuT( "sampler" );

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

	FrameVariable::TextLoader::TextLoader( File::eENCODING_MODE p_eEncodingMode )
		:	Loader< FrameVariable, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
	{
	}

	bool FrameVariable::TextLoader::operator()( FrameVariable const & p_variable, TextFile & p_file )
	{
		String l_strTabs = cuT( "\t\t\t\t" );
		bool l_return = p_file.WriteText( l_strTabs + cuT( "variable\n" ) ) > 0;

		if ( l_return )
		{
			l_return = p_file.WriteText( l_strTabs + cuT( "{\n" ) ) > 0;
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( l_strTabs + cuT( "\ttype " ) + p_variable.GetFullTypeName() + cuT( "\n" ) ) > 0;
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( l_strTabs + cuT( "\tname " ) + p_variable.GetName() + cuT( "\n" ) ) > 0;
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( l_strTabs + cuT( "\tvalue " ) + p_variable.GetStrValue() + cuT( "\n" ) ) > 0;
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( l_strTabs + cuT( "}\n" ) ) > 0;
		}

		return l_return;
	}

	//*************************************************************************************************

	FrameVariable::FrameVariable( ShaderProgramBase * p_pProgram )
		: m_bChanged( true )
		, m_uiOcc( 1 )
		, m_strValue( 1 )
		, m_pProgram( p_pProgram )
	{
	}

	FrameVariable::FrameVariable( ShaderProgramBase * p_pProgram, uint32_t p_uiOcc )
		: m_bChanged( true )
		, m_uiOcc( p_uiOcc )
		, m_strValue( m_uiOcc )
		, m_pProgram( p_pProgram )
	{
	}

	FrameVariable::FrameVariable( FrameVariable const & p_object )
		: m_strName( p_object.m_strName )
		, m_uiOcc( p_object.m_uiOcc )
		, m_strValue( p_object.m_strValue )
		, m_pProgram( p_object.m_pProgram )
	{
	}

	FrameVariable::FrameVariable( FrameVariable && p_object )
		: m_strName( std::move( p_object.m_strName ) )
		, m_uiOcc( std::move( p_object.m_uiOcc ) )
		, m_strValue( std::move( p_object.m_strValue ) )
		, m_pProgram( std::move( p_object.m_pProgram ) )
	{
		p_object.m_strName.clear();
		p_object.m_uiOcc = 0;
		p_object.m_strValue.clear();
		p_object.m_pProgram = NULL;
	}

	FrameVariable & FrameVariable::operator =( FrameVariable const & p_object )
	{
		m_strName = p_object.m_strName;
		m_uiOcc = p_object.m_uiOcc;
		m_strValue = p_object.m_strValue;
		m_pProgram = p_object.m_pProgram;
		return *this;
	}

	FrameVariable & FrameVariable::operator =( FrameVariable && p_object )
	{
		if ( this != &p_object )
		{
			m_strName = std::move( p_object.m_strName );
			m_uiOcc = std::move( p_object.m_uiOcc );
			m_strValue = std::move( p_object.m_strValue );
			m_pProgram = std::move( p_object.m_pProgram );
			p_object.m_strName.clear();
			p_object.m_uiOcc = 0;
			p_object.m_strValue.clear();
			p_object.m_pProgram = NULL;
		}

		return *this;
	}

	FrameVariable::~FrameVariable()
	{
	}

	void FrameVariable::SetValueStr( String const & p_strValue )
	{
		SetValueStr( p_strValue, 0 );
	}

	void FrameVariable::SetValueStr( String const & p_strValue, uint32_t p_uiIndex )
	{
		if ( p_uiIndex < m_uiOcc )
		{
			m_strValue[p_uiIndex] = p_strValue;
			m_bChanged = true;
			DoSetValueStr( p_strValue, p_uiIndex );
		}
		else
		{
			CASTOR_ASSERT( false );
		}
	}
}
