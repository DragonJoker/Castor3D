#include "FrameVariableBuffer.hpp"
#include "FrameVariable.hpp"

using namespace Castor;

//*************************************************************************************************
namespace Castor3D
{
	uint32_t FrameVariableBuffer::sm_uiCount = 0;

	FrameVariableBuffer::FrameVariableBuffer( String const & p_strName, RenderSystem * p_pRenderSystem )
		:	m_pRenderSystem( p_pRenderSystem )
		,	m_strName( p_strName )
		,	m_uiIndex( sm_uiCount++ )
	{
	}

	FrameVariableBuffer::~FrameVariableBuffer()
	{
		CASTOR_ASSERT( m_mapVariables.size() == 0 && m_listVariables.size() == 0 );
	}

	FrameVariableSPtr FrameVariableBuffer::CreateVariable( ShaderProgramBase & p_program, eFRAME_VARIABLE_TYPE p_eType, String const & p_strName, uint32_t p_uiNbOcc )
	{
		FrameVariableSPtr l_pReturn;
		FrameVariablePtrStrMapConstIt l_it = m_mapVariables.find( p_strName );

		if ( l_it == m_mapVariables.end() )
		{
			l_pReturn = DoCreateVariable( &p_program, p_eType, p_strName, p_uiNbOcc );

			if ( l_pReturn )
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

	bool FrameVariableBuffer::Initialise( ShaderProgramBase & p_program )
	{
		bool l_bReturn = true;

		if ( !DoInitialise( &p_program ) )
		{
			uint32_t l_uiTotalSize = 0;

			for ( FrameVariablePtrListIt l_it = m_listVariables.begin(); l_it != m_listVariables.end(); ++l_it )
			{
				if ( ( *l_it )->Initialise() )
				{
					l_uiTotalSize += ( *l_it )->size();
					m_listInitialised.push_back( *l_it );
				}
			}

			m_buffer.resize( l_uiTotalSize );
			l_uiTotalSize = 0;

			for ( FrameVariablePtrListIt l_it = m_listInitialised.begin(); l_it != m_listInitialised.end(); ++l_it )
			{
				( *l_it )->link( &m_buffer[l_uiTotalSize] );
				l_uiTotalSize += ( *l_it )->size();
			}
		}

		return l_bReturn;
	}

	void FrameVariableBuffer::Cleanup()
	{
		DoCleanup();
		m_mapVariables.clear();
		m_listVariables.clear();
	}

	bool FrameVariableBuffer::Bind()
	{
		return DoBind();
	}

	void FrameVariableBuffer::Unbind()
	{
		DoUnbind();
	}
}
