#include "FrameVariableBuffer.hpp"
#include "FrameVariable.hpp"

using namespace Castor;

//*************************************************************************************************
namespace Castor3D
{
	uint32_t FrameVariableBuffer::sm_uiCount = 0;

	FrameVariableBuffer::FrameVariableBuffer( String const & p_strName, RenderSystem * p_pRenderSystem )
		:	m_renderSystem( p_pRenderSystem )
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

	void FrameVariableBuffer::RemoveVariable( String const & p_name )
	{
		FrameVariablePtrStrMapConstIt l_itMap = m_mapVariables.find( p_name );

		if ( l_itMap != m_mapVariables.end() )
		{
			m_mapVariables.erase( l_itMap );
		}

		auto && l_itList = std::find_if( m_listVariables.begin(), m_listVariables.end(), [&p_name]( FrameVariableSPtr p_variable )
		{
			return p_name == p_variable->GetName();
		} );

		if ( l_itList != m_listVariables.end() )
		{
			m_listVariables.erase( l_itList );
		}
	}

	bool FrameVariableBuffer::Initialise( ShaderProgramBase & p_program )
	{
		bool l_return = true;

		if ( !DoInitialise( &p_program ) )
		{
			uint32_t l_uiTotalSize = 0;

			for ( auto && l_variable : m_listVariables )
			{
				if ( l_variable->Initialise() )
				{
					l_uiTotalSize += l_variable->size();
					m_listInitialised.push_back( l_variable );
				}
			}

			m_buffer.resize( l_uiTotalSize );
			uint8_t * l_buffer = m_buffer.data();

			for ( auto && l_variable : m_listInitialised )
			{
				l_variable->link( l_buffer );
				l_buffer += l_variable->size();
			}
		}

		return l_return;
	}

	void FrameVariableBuffer::Cleanup()
	{
		DoCleanup();
		m_mapVariables.clear();
		m_listVariables.clear();
	}

	bool FrameVariableBuffer::Bind( uint32_t p_index )
	{
		return DoBind( p_index );
	}

	void FrameVariableBuffer::Unbind( uint32_t p_index )
	{
		DoUnbind( p_index );
	}
}
