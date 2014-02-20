#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/FrameVariable.hpp"

using namespace Castor;
using namespace Castor3D;

//*************************************************************************************************

const String FrameVariable::Names[] =
{	cuT( "bool"		)
,	cuT( "int"		)
,	cuT( "uint"		)
,	cuT( "float"	)
,	cuT( "double"	)
,	cuT( "sampler"	)
,	cuT( "vec2b"	)
,	cuT( "vec3b"	)
,	cuT( "vec4b"	)
,	cuT( "vec2i"	)
,	cuT( "vec3i"	)
,	cuT( "vec4i"	)
,	cuT( "vec2ui"	)
,	cuT( "vec3ui"	)
,	cuT( "vec4ui"	)
,	cuT( "vec2f"	)
,	cuT( "vec3f"	)
,	cuT( "vec4f"	)
,	cuT( "vec2d"	)
,	cuT( "vec3d"	)
,	cuT( "vec4d"	)
,	cuT( "mat2x2b"	)
,	cuT( "mat2x3b"	)
,	cuT( "mat2x4b"	)
,	cuT( "mat3x2b"	)
,	cuT( "mat3x3b"	)
,	cuT( "mat3x4b"	)
,	cuT( "mat4x2b"	)
,	cuT( "mat4x3b"	)
,	cuT( "mat4x4b"	)
,	cuT( "mat2x2i"	)
,	cuT( "mat2x3i"	)
,	cuT( "mat2x4i"	)
,	cuT( "mat3x2i"	)
,	cuT( "mat3x3i"	)
,	cuT( "mat3x4i"	)
,	cuT( "mat4x2i"	)
,	cuT( "mat4x3i"	)
,	cuT( "mat4x4i"	)
,	cuT( "mat2x2ui"	)
,	cuT( "mat2x3ui"	)
,	cuT( "mat2x4ui"	)
,	cuT( "mat3x2ui"	)
,	cuT( "mat3x3ui"	)
,	cuT( "mat3x4ui"	)
,	cuT( "mat4x2ui"	)
,	cuT( "mat4x3ui"	)
,	cuT( "mat4x4ui"	)
,	cuT( "mat2x2f"	)
,	cuT( "mat2x3f"	)
,	cuT( "mat2x4f"	)
,	cuT( "mat3x2f"	)
,	cuT( "mat3x3f"	)
,	cuT( "mat3x4f"	)
,	cuT( "mat4x2f"	)
,	cuT( "mat4x3f"	)
,	cuT( "mat4x4f"	)
,	cuT( "mat2x2d"	)
,	cuT( "mat2x3d"	)
,	cuT( "mat2x4d"	)
,	cuT( "mat3x2d"	)
,	cuT( "mat3x3d"	)
,	cuT( "mat3x4d"	)
,	cuT( "mat4x2d"	)
,	cuT( "mat4x3d"	)
,	cuT( "mat4x4d"	)
};

//*************************************************************************************************

FrameVariable::TextLoader :: TextLoader( File::eENCODING_MODE p_eEncodingMode )
	:	Loader< FrameVariable, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
{
}

bool FrameVariable::TextLoader :: operator ()( FrameVariable const & p_variable, TextFile & p_file )
{
	String l_strTabs = cuT( "\t\t\t\t" );
	bool l_bReturn = p_file.WriteText( l_strTabs + cuT( "variable\n" ) ) > 0;

	if( l_bReturn )
	{
		l_bReturn = p_file.WriteText( l_strTabs + cuT( "{\n" ) ) > 0;
	}

	if( l_bReturn )
	{
		l_bReturn = p_file.WriteText( l_strTabs + cuT( "\ttype " ) + FrameVariable::Names[p_variable.GetFullType()] + cuT( "\n" ) ) > 0;
	}

	if( l_bReturn )
	{
		l_bReturn = p_file.WriteText( l_strTabs + cuT( "\tname " ) + p_variable.GetName() + cuT( "\n" ) ) > 0;
	}

	if( l_bReturn )
	{
		l_bReturn = p_file.WriteText( l_strTabs + cuT( "\tvalue " ) + p_variable.GetStrValue() + cuT( "\n" ) ) > 0;
	}

	if( l_bReturn )
	{
		l_bReturn = p_file.WriteText( l_strTabs + cuT( "}\n" ) ) > 0;
	}

	return l_bReturn;
}

//*************************************************************************************************

uint32_t FrameVariableBuffer :: sm_uiCount = 0;

FrameVariableBuffer :: FrameVariableBuffer( String const & p_strName, RenderSystem * p_pRenderSystem )
	:	m_pRenderSystem	( p_pRenderSystem	)
	,	m_strName		( p_strName			)
	,	m_uiIndex		( sm_uiCount++		)
{
}

FrameVariableBuffer :: ~FrameVariableBuffer()
{
	CASTOR_ASSERT( m_mapVariables.size() == 0 && m_listVariables.size() == 0 );
}

FrameVariableSPtr FrameVariableBuffer :: CreateVariable( ShaderProgramBase & p_program, eFRAME_VARIABLE_TYPE p_eType, String const & p_strName, uint32_t p_uiNbOcc )
{
	FrameVariableSPtr l_pReturn;
	FrameVariablePtrStrMapConstIt l_it = m_mapVariables.find( p_strName );

	if( l_it == m_mapVariables.end() )
	{
		l_pReturn = DoCreateVariable( &p_program, p_eType, p_strName, p_uiNbOcc );

		if( l_pReturn )
		{
			m_mapVariables.insert( std::make_pair( p_strName, l_pReturn ) );
			m_listVariables.push_back( l_pReturn );
		}
	}
	else
	{
		l_pReturn = l_it->second.lock();
	}

	return l_pReturn;
}

bool FrameVariableBuffer :: Initialise( ShaderProgramBase & p_program )
{
	bool l_bReturn = true;

	if( !DoInitialise( &p_program ) )
	{
		uint32_t l_uiTotalSize = 0;

		for( FrameVariablePtrListIt l_it = m_listVariables.begin(); l_it != m_listVariables.end(); ++l_it )
		{
			if( (*l_it)->Initialise() )
			{
				l_uiTotalSize += (*l_it)->size();
				m_listInitialised.push_back( *l_it );
			}
		}
		
		m_buffer.resize( l_uiTotalSize );
		l_uiTotalSize = 0;

		for( FrameVariablePtrListIt l_it = m_listInitialised.begin(); l_it != m_listInitialised.end(); ++l_it )
		{
			(*l_it)->link( &m_buffer[l_uiTotalSize] );
			l_uiTotalSize += (*l_it)->size();
		}
	}

	return l_bReturn;
}

void FrameVariableBuffer :: Cleanup()
{
	DoCleanup();
	m_mapVariables.clear();
	m_listVariables.clear();
}

bool FrameVariableBuffer :: Bind()
{
	return DoBind();
}

void FrameVariableBuffer :: Unbind()
{
	DoUnbind();
}

//*************************************************************************************************

FrameVariable :: FrameVariable( ShaderProgramBase * p_pProgram )
	:	m_bChanged		( true			)
	,	m_uiOcc			( 1				)
	,	m_strValue		( 1				)
	,	m_pProgram		( p_pProgram	)
{
}

FrameVariable :: FrameVariable( ShaderProgramBase * p_pProgram, uint32_t p_uiOcc )
	:	m_bChanged		( true			)
	,	m_uiOcc			( p_uiOcc		)
	,	m_strValue		( m_uiOcc		)
	,	m_pProgram		( p_pProgram	)
{
}

FrameVariable :: FrameVariable( FrameVariable const & p_object )
	:	m_strName		( p_object.m_strName	)
	,	m_uiOcc			( p_object.m_uiOcc		)
	,	m_strValue		( p_object.m_strValue	)
	,	m_pProgram		( p_object.m_pProgram	)
{
}

FrameVariable :: FrameVariable( FrameVariable && p_object )
	:	m_strName		( std::move( p_object.m_strName		) )
	,	m_uiOcc			( std::move( p_object.m_uiOcc		) )
	,	m_strValue		( std::move( p_object.m_strValue	) )
	,	m_pProgram		( std::move( p_object.m_pProgram	) )
{
	p_object.m_strName		.clear();
	p_object.m_uiOcc		= 0;
	p_object.m_strValue		.clear();
	p_object.m_pProgram		 = NULL;
}

FrameVariable & FrameVariable :: operator =( FrameVariable const & p_object )
{
	m_strName		= p_object.m_strName	;
	m_uiOcc			= p_object.m_uiOcc		;
	m_strValue		= p_object.m_strValue	;
	m_pProgram		= p_object.m_pProgram	;

	return *this;
}

FrameVariable & FrameVariable :: operator =( FrameVariable && p_object )
{
	if( this != &p_object )
	{
		m_strName		= std::move( p_object.m_strName		);
		m_uiOcc			= std::move( p_object.m_uiOcc		);
		m_strValue		= std::move( p_object.m_strValue	);
		m_pProgram		= std::move( p_object.m_pProgram	);

		p_object.m_strName		.clear();
		p_object.m_uiOcc		= 0;
		p_object.m_strValue		.clear();
		p_object.m_pProgram		 = NULL;
	}

	return *this;
}

FrameVariable :: ~FrameVariable()
{
}

void FrameVariable :: SetValueStr( String const & p_strValue )
{
	SetValueStr( p_strValue, 0 );
}

void FrameVariable :: SetValueStr( String const & p_strValue, uint32_t p_uiIndex )
{
	if( p_uiIndex < m_uiOcc )
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