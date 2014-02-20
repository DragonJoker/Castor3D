#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/ShaderObject.hpp"
#include "Castor3D/ShaderProgram.hpp"
#include "Castor3D/FrameVariable.hpp"

using namespace Castor3D;
using namespace Castor;

//*************************************************************************************************

bool ShaderObjectBinaryParser :: DoFill( ShaderObjectBase const & p_object, stCHUNK & p_chunk )
{
	bool l_bReturn = true;
	stCHUNK l_chunk;
	l_chunk.m_eChunkType = eCHUNK_TYPE_SHADER_PROGRAM;

	if( p_object.GetParent()->GetLanguage() == eSHADER_LANGUAGE_GLSL )
	{
		// One source for each object
		if( l_bReturn )
		{
			l_bReturn = DoFillStringChunk( p_object.GetLoadedSource(), eCHUNK_TYPE_PROGRAM_SOURCE, l_chunk );
		}
	}
	else
	{
		// One source for all program but entries for objects
		if( l_bReturn )
		{
			l_bReturn = DoFillStringChunk( p_object.GetEntryPoint(), eCHUNK_TYPE_PROGRAM_ENTRY, l_chunk );
		}
	}

	if( p_object.GetType() == eSHADER_TYPE_GEOMETRY )
	{
		if( l_bReturn )
		{
			l_bReturn = DoFillValueChunk( p_object.GetInputType(), eCHUNK_TYPE_PROGRAM_INPUT, l_chunk );
		}

		if( l_bReturn )
		{
			l_bReturn = DoFillValueChunk( p_object.GetOutputType(), eCHUNK_TYPE_PROGRAM_OUTPUT, l_chunk );
		}

		if( l_bReturn )
		{
			l_bReturn = DoFillValueChunk( p_object.GetOutputVtxCount(), eCHUNK_TYPE_PROGRAM_OUTCOUNT, l_chunk );
		}
	}

	for( FrameVariablePtrStrMapConstIt l_it = p_object.GetFrameVariablesBegin(); l_it != p_object.GetFrameVariablesEnd() && l_bReturn; ++l_it )
	{
		FrameVariableSPtr l_pVariable = l_it->second.lock();
		stCHUNK l_chunkVariable;
		l_chunkVariable.m_eChunkType = eCHUNK_TYPE_PROGRAM_VARIABLE;

		if( l_bReturn )
		{
			l_bReturn = DoFillValueChunk( l_pVariable->GetOccCount(), eCHUNK_TYPE_VARIABLE_COUNT, l_chunkVariable );
		}

		if( l_bReturn )
		{
			l_bReturn = DoFillValueChunk( l_pVariable->GetFullType(), eCHUNK_TYPE_VARIABLE_TYPE, l_chunkVariable );
		}

		if( l_bReturn )
		{
			l_bReturn = DoFillStringChunk( l_pVariable->GetName(), eCHUNK_TYPE_NAME, l_chunkVariable );
		}

		if( l_bReturn )
		{
			l_bReturn = DoFillValuesChunk( l_pVariable->const_ptr(), l_pVariable->size(), eCHUNK_TYPE_VARIABLE_VALUE, l_chunkVariable );
		}

		if( l_bReturn )
		{
			l_bReturn = DoAddSubChunk( l_chunkVariable, l_chunk );
		}
	}

	if( l_bReturn )
	{
		l_bReturn = DoAddSubChunk( l_chunk, p_chunk );
	}

	return l_bReturn;
}

bool ShaderObjectBinaryParser :: DoParse( ShaderObjectBase & p_object, stCHUNK & p_chunk )
{
	bool l_bReturn = true;
	String l_strText;
	eTOPOLOGY l_ePrimType;
	eFRAME_VARIABLE_TYPE l_eType;
	FrameVariableSPtr l_pVariable;
	uint8_t l_uiCount;

	while( p_chunk.m_uiIndex < p_chunk.m_pData.size() )
	{
		stCHUNK l_chunk;
		l_bReturn = DoGetSubChunk( p_chunk, l_chunk );

		switch( l_chunk.m_eChunkType )
		{
		case eCHUNK_TYPE_PROGRAM_ENTRY:
			l_bReturn = DoParseString( l_strText, l_chunk );
			if( l_bReturn )
			{
				p_object.SetEntryPoint( l_strText );
			}
			break;

		case eCHUNK_TYPE_PROGRAM_SOURCE:
			l_bReturn = DoParseString( l_strText, l_chunk );
			if( l_bReturn )
			{
				p_object.SetSource( eSHADER_MODEL_5, l_strText );
				p_object.SetSource( eSHADER_MODEL_4, l_strText );
				p_object.SetSource( eSHADER_MODEL_3, l_strText );
				p_object.SetSource( eSHADER_MODEL_2, l_strText );
				p_object.SetSource( eSHADER_MODEL_1, l_strText );
			}
			break;

		case eCHUNK_TYPE_PROGRAM_VARIABLE:
			l_uiCount = 0;
			l_eType = eFRAME_VARIABLE_TYPE( -1 );
			l_strText.clear();
			l_pVariable.reset();
			while( l_chunk.m_uiIndex < l_chunk.m_pData.size() )
			{
				stCHUNK l_chunkVariable;
				l_bReturn = DoGetSubChunk( l_chunk, l_chunkVariable );

				switch( l_chunkVariable.m_eChunkType )
				{
				case eCHUNK_TYPE_VARIABLE_COUNT:
					l_bReturn = DoParseValue( l_uiCount, l_chunkVariable );
					break;

				case eCHUNK_TYPE_NAME:
					l_bReturn = DoParseString( l_strText, l_chunkVariable );
					break;

				case eCHUNK_TYPE_VARIABLE_TYPE:
					l_bReturn = DoParseValue( l_eType, l_chunkVariable );
					break;
				}

// 				if( l_bReturn )
// 				{
// 					if( l_uiCount && l_eType != eFRAME_VARIABLE_TYPE( -1 ) && !l_strText.empty() )
// 					{
// 						switch( l_eType )
// 						{
// 						case eFRAME_VARIABLE_TYPE_INT:		l_pVariable = p_object.GetParent()->CreateFrameVariable< OneIntFrameVariable		>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_UINT:		l_pVariable = p_object.GetParent()->CreateFrameVariable< OneUIntFrameVariable		>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_FLOAT:	l_pVariable = p_object.GetParent()->CreateFrameVariable< OneFloatFrameVariable		>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_DOUBLE:	l_pVariable = p_object.GetParent()->CreateFrameVariable< OneDoubleFrameVariable		>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_SAMPLER:	l_pVariable = p_object.GetParent()->CreateFrameVariable< OneTextureFrameVariable	>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_VEC2I:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Point2iFrameVariable		>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_VEC3I:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Point3iFrameVariable		>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_VEC4I:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Point4iFrameVariable		>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_VEC2F:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Point2fFrameVariable		>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_VEC3F:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Point3fFrameVariable		>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_VEC4F:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Point4fFrameVariable		>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_VEC2D:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Point2dFrameVariable		>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_VEC3D:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Point3dFrameVariable		>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_VEC4D:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Point4dFrameVariable		>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_MAT2X2I:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Matrix2x2iFrameVariable	>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_MAT2X3I:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Matrix2x3iFrameVariable	>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_MAT2X4I:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Matrix2x4iFrameVariable	>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_MAT3X2I:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Matrix3x2iFrameVariable	>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_MAT3X3I:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Matrix3x3iFrameVariable	>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_MAT3X4I:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Matrix3x4iFrameVariable	>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_MAT4X2I:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Matrix4x2iFrameVariable	>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_MAT4X3I:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Matrix4x3iFrameVariable	>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_MAT4X4I:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Matrix4x4iFrameVariable	>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_MAT2X2F:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Matrix2x2fFrameVariable	>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_MAT2X3F:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Matrix2x3fFrameVariable	>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_MAT2X4F:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Matrix2x4fFrameVariable	>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_MAT3X2F:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Matrix3x2fFrameVariable	>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_MAT3X3F:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Matrix3x3fFrameVariable	>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_MAT3X4F:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Matrix3x4fFrameVariable	>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_MAT4X2F:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Matrix4x2fFrameVariable	>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_MAT4X3F:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Matrix4x3fFrameVariable	>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_MAT4X4F:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Matrix4x4fFrameVariable	>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_MAT2X2D:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Matrix2x2dFrameVariable	>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_MAT2X3D:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Matrix2x3dFrameVariable	>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_MAT2X4D:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Matrix2x4dFrameVariable	>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_MAT3X2D:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Matrix3x2dFrameVariable	>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_MAT3X3D:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Matrix3x3dFrameVariable	>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_MAT3X4D:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Matrix3x4dFrameVariable	>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_MAT4X2D:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Matrix4x2dFrameVariable	>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_MAT4X3D:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Matrix4x3dFrameVariable	>( l_uiCount, p_object.GetType() );	break;
// 						case eFRAME_VARIABLE_TYPE_MAT4X4D:	l_pVariable = p_object.GetParent()->CreateFrameVariable< Matrix4x4dFrameVariable	>( l_uiCount, p_object.GetType() );	break;
// 						}
// 					}
// 
// 					if( l_pVariable )
// 					{
// 						l_pVariable->SetName( l_strText );
// 					}
// 				}

				if( !l_bReturn )
				{
					p_chunk.m_uiIndex = uint32_t( p_chunk.m_pData.size() );
				}
			}
			break;
		}

		if( p_object.GetType() == eSHADER_TYPE_GEOMETRY )
		{
			switch( l_chunk.m_eChunkType )
			{
			case eCHUNK_TYPE_PROGRAM_INPUT:
				l_bReturn = DoParseValue( l_ePrimType, l_chunk );
				if( l_bReturn )
				{
					p_object.SetInputType( l_ePrimType );
				}
				break;

			case eCHUNK_TYPE_PROGRAM_OUTPUT:
				l_bReturn = DoParseValue( l_ePrimType, l_chunk );
				if( l_bReturn )
				{
					p_object.SetOutputType( l_ePrimType );
				}
				break;

			case eCHUNK_TYPE_PROGRAM_OUTCOUNT:
				l_bReturn = DoParseValue( l_uiCount, l_chunk );
				if( l_bReturn )
				{
					p_object.SetOutputVtxCount( l_uiCount );
				}
				break;
			}
		}

		if( !l_bReturn )
		{
			p_chunk.m_uiIndex = uint32_t( p_chunk.m_pData.size() );
		}
	}

	return l_bReturn;
}

//*************************************************************************************************

ShaderObjectBase::TextLoader :: TextLoader( File::eENCODING_MODE p_eEncodingMode )
	:	Loader< ShaderObjectBase, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_eEncodingMode )
{
}

bool ShaderObjectBase::TextLoader :: operator ()( ShaderObjectBase const & p_shaderObject, TextFile & p_file )
{
	String l_strFile;
	String l_strTabs = cuT( "\t\t\t" );
	bool l_bReturn = p_file.WriteText( l_strTabs + p_shaderObject.GetStrType() ) > 0;

	static std::array< String, eSHADER_MODEL_COUNT > l_arrayModels;
	l_arrayModels[eSHADER_MODEL_1] = cuT( "sm_1" );
	l_arrayModels[eSHADER_MODEL_2] = cuT( "sm_2" );
	l_arrayModels[eSHADER_MODEL_3] = cuT( "sm_3" );
	l_arrayModels[eSHADER_MODEL_4] = cuT( "sm_4" );
	l_arrayModels[eSHADER_MODEL_5] = cuT( "sm_5" );

	if( l_bReturn )
	{
		l_bReturn = p_file.WriteText( l_strTabs + cuT( "{\n" ) ) > 0;
	}

	if( l_bReturn )
	{
		for( int i = 0; i < eSHADER_MODEL_COUNT; i++ )
		{
			l_strFile = p_shaderObject.GetFile( eSHADER_MODEL( i ) );

			if( !l_strFile.empty() )
			{
				l_bReturn = p_file.WriteText( l_strTabs + cuT( "\tfile " ) + l_arrayModels[i] + cuT( " " ) + l_strFile + cuT( "\n" ) ) > 0;
			}
		}
	}

	for( FrameVariablePtrStrMapConstIt l_it = p_shaderObject.GetFrameVariablesBegin(); l_it != p_shaderObject.GetFrameVariablesEnd() && l_bReturn; ++l_it )
	{
		l_bReturn = FrameVariable::TextLoader()( *l_it->second.lock(), p_file );
	}

	if( l_bReturn )
	{
		l_bReturn = p_file.WriteText( l_strTabs + cuT( "}\n" ) ) > 0;
	}

	return l_bReturn;
}

//*************************************************************************************************

const std::array< String, eSHADER_TYPE_COUNT >	ShaderObjectBase :: string_type =
{	cuT( "vertex_program"	)
,	cuT( "hull_program"		)
,	cuT( "domain_program"	)
,	cuT( "geometry_program"	)
,	cuT( "pixel_program"	)
};

ShaderObjectBase :: ShaderObjectBase( ShaderProgramBase * p_pParent, eSHADER_TYPE p_eType)
	:	m_eStatus		( eSHADER_STATUS_NOTCOMPILED	)
	,	m_eType			( p_eType						)
	,	m_pParent		( p_pParent						)
	,	m_eInputType	( eTOPOLOGY_TRIANGLES		)
	,	m_eOutputType	( eTOPOLOGY_TRIANGLES		)
	,	m_eShaderModel	( eSHADER_MODEL_1				)
{
}

ShaderObjectBase :: ~ShaderObjectBase()
{
}

bool ShaderObjectBase :: Compile()
{
	std::for_each( m_listFrameVariables.begin(), m_listFrameVariables.end(), [&]( FrameVariableSPtr p_pVariable )
	{
		p_pVariable->Bind();
	} );

	return true;
}

void ShaderObjectBase :: Bind()
{
	std::for_each( m_listFrameVariables.begin(), m_listFrameVariables.end(), [&]( FrameVariableSPtr p_pVariable )
	{
		p_pVariable->Apply();
	} );
}

void ShaderObjectBase :: Unbind()
{
}

void ShaderObjectBase :: SetFile( eSHADER_MODEL p_eModel, Path const & p_filename)
{
	m_eStatus = eSHADER_STATUS_NOTCOMPILED;
	m_arrayFiles[p_eModel].clear();
	m_arraySources[p_eModel].clear();

	if ( !p_filename.empty() && File::FileExists( p_filename ) )
	{
		TextFile l_file( p_filename.c_str(), File::eOPEN_MODE_READ);

		if (l_file.IsOk())
		{
			if( l_file.GetLength() > 0 )
			{
				m_arrayFiles[p_eModel] = p_filename;
				l_file.CopyToString( m_arraySources[p_eModel] );
			}
		}
	}
}

void ShaderObjectBase :: SetSource( eSHADER_MODEL p_eModel, String const & p_strSource )
{
	m_eStatus = eSHADER_STATUS_NOTCOMPILED;
	m_arraySources[p_eModel] = p_strSource;
}

bool ShaderObjectBase :: HasSource()
{
	bool l_bReturn = false;

	for( int i = 0; i < eSHADER_MODEL_COUNT && !l_bReturn; i++ )
	{
		l_bReturn = !m_arraySources[i].empty();
	}

	return l_bReturn;
}

void ShaderObjectBase :: AddFrameVariable( OneTextureFrameVariableSPtr p_pVariable)
{
	if( p_pVariable )
	{
		m_listFrameVariables.push_back( p_pVariable );
		m_mapFrameVariables.insert( std::make_pair( p_pVariable->GetName(), p_pVariable ) );
	}
}

OneTextureFrameVariableSPtr ShaderObjectBase :: FindFrameVariable( Castor::String const & p_strName )const
{
	OneTextureFrameVariableSPtr l_pReturn;
	FrameVariablePtrStrMapConstIt l_it = m_mapFrameVariables.find( p_strName );

	if( l_it != m_mapFrameVariables.end() )
	{
		FrameVariableSPtr l_pFound = l_it->second.lock();

		if( l_pFound && l_pFound->GetFullType() == OneTextureFrameVariable::GetFrameVariableType() )
		{
			l_pReturn = std::static_pointer_cast< OneTextureFrameVariable >( l_pFound );
		}
	}

	return l_pReturn;
}

void ShaderObjectBase :: FlushFrameVariables()
{
	clear_container( m_mapFrameVariables );
	clear_container( m_listFrameVariables );
}

//*************************************************************************************************
