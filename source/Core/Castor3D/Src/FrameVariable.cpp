#include "FrameVariable.hpp"
#include "FrameVariableBuffer.hpp"

using namespace Castor;
using namespace Castor3D;

//*************************************************************************************************

const String FrameVariable::Names[] =
{
	cuT( "bool" )
	,	cuT( "int" )
	,	cuT( "uint" )
	,	cuT( "float" )
	,	cuT( "double" )
	,	cuT( "sampler" )
	,	cuT( "vec2b" )
	,	cuT( "vec3b" )
	,	cuT( "vec4b" )
	,	cuT( "vec2i" )
	,	cuT( "vec3i" )
	,	cuT( "vec4i" )
	,	cuT( "vec2ui" )
	,	cuT( "vec3ui" )
	,	cuT( "vec4ui" )
	,	cuT( "vec2f" )
	,	cuT( "vec3f" )
	,	cuT( "vec4f" )
	,	cuT( "vec2d" )
	,	cuT( "vec3d" )
	,	cuT( "vec4d" )
	,	cuT( "mat2x2b" )
	,	cuT( "mat2x3b" )
	,	cuT( "mat2x4b" )
	,	cuT( "mat3x2b" )
	,	cuT( "mat3x3b" )
	,	cuT( "mat3x4b" )
	,	cuT( "mat4x2b" )
	,	cuT( "mat4x3b" )
	,	cuT( "mat4x4b" )
	,	cuT( "mat2x2i" )
	,	cuT( "mat2x3i" )
	,	cuT( "mat2x4i" )
	,	cuT( "mat3x2i" )
	,	cuT( "mat3x3i" )
	,	cuT( "mat3x4i" )
	,	cuT( "mat4x2i" )
	,	cuT( "mat4x3i" )
	,	cuT( "mat4x4i" )
	,	cuT( "mat2x2ui" )
	,	cuT( "mat2x3ui" )
	,	cuT( "mat2x4ui" )
	,	cuT( "mat3x2ui" )
	,	cuT( "mat3x3ui" )
	,	cuT( "mat3x4ui" )
	,	cuT( "mat4x2ui" )
	,	cuT( "mat4x3ui" )
	,	cuT( "mat4x4ui" )
	,	cuT( "mat2x2f" )
	,	cuT( "mat2x3f" )
	,	cuT( "mat2x4f" )
	,	cuT( "mat3x2f" )
	,	cuT( "mat3x3f" )
	,	cuT( "mat3x4f" )
	,	cuT( "mat4x2f" )
	,	cuT( "mat4x3f" )
	,	cuT( "mat4x4f" )
	,	cuT( "mat2x2d" )
	,	cuT( "mat2x3d" )
	,	cuT( "mat2x4d" )
	,	cuT( "mat3x2d" )
	,	cuT( "mat3x3d" )
	,	cuT( "mat3x4d" )
	,	cuT( "mat4x2d" )
	,	cuT( "mat4x3d" )
	,	cuT( "mat4x4d" )
};

//*************************************************************************************************

FrameVariable::TextLoader::TextLoader( File::eENCODING_MODE p_eEncodingMode )
	:	Loader< FrameVariable, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
{
}

bool FrameVariable::TextLoader::operator()( FrameVariable const & p_variable, TextFile & p_file )
{
	String l_strTabs = cuT( "\t\t\t\t" );
	bool l_bReturn = p_file.WriteText( l_strTabs + cuT( "variable\n" ) ) > 0;

	if ( l_bReturn )
	{
		l_bReturn = p_file.WriteText( l_strTabs + cuT( "{\n" ) ) > 0;
	}

	if ( l_bReturn )
	{
		l_bReturn = p_file.WriteText( l_strTabs + cuT( "\ttype " ) + FrameVariable::Names[p_variable.GetFullType()] + cuT( "\n" ) ) > 0;
	}

	if ( l_bReturn )
	{
		l_bReturn = p_file.WriteText( l_strTabs + cuT( "\tname " ) + p_variable.GetName() + cuT( "\n" ) ) > 0;
	}

	if ( l_bReturn )
	{
		l_bReturn = p_file.WriteText( l_strTabs + cuT( "\tvalue " ) + p_variable.GetStrValue() + cuT( "\n" ) ) > 0;
	}

	if ( l_bReturn )
	{
		l_bReturn = p_file.WriteText( l_strTabs + cuT( "}\n" ) ) > 0;
	}

	return l_bReturn;
}

//*************************************************************************************************

FrameVariable::FrameVariable( ShaderProgramBase * p_pProgram )
	:	m_bChanged( true )
	,	m_uiOcc( 1 )
	,	m_strValue( 1 )
	,	m_pProgram( p_pProgram )
{
}

FrameVariable::FrameVariable( ShaderProgramBase * p_pProgram, uint32_t p_uiOcc )
	:	m_bChanged( true )
	,	m_uiOcc( p_uiOcc )
	,	m_strValue( m_uiOcc )
	,	m_pProgram( p_pProgram )
{
}

FrameVariable::FrameVariable( FrameVariable const & p_object )
	:	m_strName( p_object.m_strName )
	,	m_uiOcc( p_object.m_uiOcc )
	,	m_strValue( p_object.m_strValue )
	,	m_pProgram( p_object.m_pProgram )
{
}

FrameVariable::FrameVariable( FrameVariable && p_object )
	:	m_strName( std::move( p_object.m_strName ) )
	,	m_uiOcc( std::move( p_object.m_uiOcc ) )
	,	m_strValue( std::move( p_object.m_strValue ) )
	,	m_pProgram( std::move( p_object.m_pProgram ) )
{
	p_object.m_strName		.clear();
	p_object.m_uiOcc		= 0;
	p_object.m_strValue		.clear();
	p_object.m_pProgram		 = NULL;
}

FrameVariable & FrameVariable::operator =( FrameVariable const & p_object )
{
	m_strName		= p_object.m_strName	;
	m_uiOcc			= p_object.m_uiOcc		;
	m_strValue		= p_object.m_strValue	;
	m_pProgram		= p_object.m_pProgram	;
	return *this;
}

FrameVariable & FrameVariable::operator =( FrameVariable && p_object )
{
	if ( this != &p_object )
	{
		m_strName		= std::move( p_object.m_strName );
		m_uiOcc			= std::move( p_object.m_uiOcc );
		m_strValue		= std::move( p_object.m_strValue );
		m_pProgram		= std::move( p_object.m_pProgram );
		p_object.m_strName		.clear();
		p_object.m_uiOcc		= 0;
		p_object.m_strValue		.clear();
		p_object.m_pProgram		 = NULL;
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

//*************************************************************************************************
