#include "MatrixFrameVariable.hpp"
#include "OneFrameVariable.hpp"
#include "PointFrameVariable.hpp"

namespace Castor3D
{
	template< typename T >
	bool FrameVariableBuffer::GetVariable( Castor::String const & p_name, std::shared_ptr< OneFrameVariable< T > > & p_pVariable )const
	{
		bool l_return = false;
		FrameVariablePtrStrMapConstIt l_it = m_mapVariables.find( p_name );

		if ( l_it != m_mapVariables.end() )
		{
			FrameVariableSPtr l_pVariable = l_it->second.lock();

			if ( l_pVariable->GetFullType() == OneFrameVariableDefinitions< T >::Full )
			{
				p_pVariable = std::static_pointer_cast< OneFrameVariable< T > >( l_pVariable );
				l_return = true;
			}
		}

		return l_return;
	}

	template< typename T, uint32_t Count >
	bool FrameVariableBuffer::GetVariable( Castor::String const & p_name, std::shared_ptr< PointFrameVariable< T, Count > > & p_pVariable )const
	{
		bool l_return = false;
		FrameVariablePtrStrMapConstIt l_it = m_mapVariables.find( p_name );

		if ( l_it != m_mapVariables.end() )
		{
			FrameVariableSPtr l_pVariable = l_it->second.lock();

			if ( l_pVariable->GetFullType() == PntFrameVariableDefinitions< T, Count >::Full )
			{
				p_pVariable = std::static_pointer_cast< PointFrameVariable< T, Count > >( l_pVariable );
				l_return = true;
			}
		}

		return l_return;
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	bool FrameVariableBuffer::GetVariable( Castor::String const & p_name, std::shared_ptr< MatrixFrameVariable< T, Rows, Columns > > & p_pVariable )const
	{
		bool l_return = false;
		FrameVariablePtrStrMapConstIt l_it = m_mapVariables.find( p_name );

		if ( l_it != m_mapVariables.end() )
		{
			FrameVariableSPtr l_pVariable = l_it->second.lock();

			if ( l_pVariable->GetFullType() == MtxFrameVariableDefinitions< T, Rows, Columns >::Full )
			{
				p_pVariable = std::static_pointer_cast< MatrixFrameVariable< T, Rows, Columns > >( l_pVariable );
				l_return = true;
			}
		}

		return l_return;
	}
}
