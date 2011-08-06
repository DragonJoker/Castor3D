#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/ShaderObject.hpp"
#include "Castor3D/ShaderProgram.hpp"

using namespace Castor3D;

//*************************************************************************************************

struct VariableBind
{
	template <class T> void operator()(T & p) const
	{
		CASTOR_TRACK;
		p->Bind();
	}
};

//*************************************************************************************************

ShaderObjectBase :: ShaderObjectBase( ShaderProgramBase * p_pParent, eSHADER_TYPE p_eType)
	:	m_bCompiled( false)
	,	m_eType( p_eType)
	,	m_bError( false)
	,	m_pParent( p_pParent)
{
	CASTOR_TRACK;
}

ShaderObjectBase :: ~ShaderObjectBase()
{
	CASTOR_TRACK;
}

bool ShaderObjectBase :: Compile()
{
	CASTOR_TRACK;
	std::for_each( m_listFrameVariables.begin(), m_listFrameVariables.end(), VariableBind());
	return true;
}

void ShaderObjectBase :: SetFile( Path const & p_filename)
{
	CASTOR_TRACK;
	m_bError = false;
	m_pathFile.clear();
	m_strSource.clear();

	if ( ! p_filename.empty())
	{
		File l_file( p_filename.c_str(), File::eOPEN_MODE_READ);

		if (l_file.IsOk())
		{
			size_t l_len = size_t( l_file.GetLength());

			if (l_len > 0)
			{
				m_pathFile = p_filename;
				l_file.CopyToString( m_strSource);
			}
		}
	}
}

void ShaderObjectBase :: SetSource( String const & p_strSource)
{
	CASTOR_TRACK;
	m_bError = false;
	m_strSource = p_strSource;
}

void ShaderObjectBase :: AddFrameVariable( FrameVariablePtr p_pVariable)
{
	CASTOR_TRACK;

	if (p_pVariable)
	{
		m_listFrameVariables.push_back( p_pVariable);
	}
}

//*************************************************************************************************
