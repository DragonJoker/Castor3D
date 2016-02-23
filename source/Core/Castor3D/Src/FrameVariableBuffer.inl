#include "MatrixFrameVariable.hpp"
#include "OneFrameVariable.hpp"
#include "PointFrameVariable.hpp"

namespace Castor3D
{
	template< typename T >
	std::shared_ptr< OneFrameVariable< T > > FrameVariableBuffer::GetVariable( Castor::String const & p_name, std::shared_ptr< OneFrameVariable< T > > & p_variable )const
	{
		FrameVariablePtrStrMapConstIt l_it = m_mapVariables.find( p_name );

		if ( l_it != m_mapVariables.end() )
		{
			FrameVariableSPtr l_variable = l_it->second.lock();

			if ( l_variable->GetFullType() == OneFrameVariableDefinitions< T >::Full )
			{
				p_variable = std::static_pointer_cast< OneFrameVariable< T > >( l_variable );
			}
		}

		return p_variable;
	}

	template< typename T, uint32_t Count >
	std::shared_ptr< PointFrameVariable< T, Count > > FrameVariableBuffer::GetVariable( Castor::String const & p_name, std::shared_ptr< PointFrameVariable< T, Count > > & p_variable )const
	{
		FrameVariablePtrStrMapConstIt l_it = m_mapVariables.find( p_name );

		if ( l_it != m_mapVariables.end() )
		{
			FrameVariableSPtr l_variable = l_it->second.lock();

			if ( l_variable->GetFullType() == PntFrameVariableDefinitions< T, Count >::Full )
			{
				p_variable = std::static_pointer_cast< PointFrameVariable< T, Count > >( l_variable );
			}
		}

		return p_variable;
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	std::shared_ptr< MatrixFrameVariable< T, Rows, Columns > > FrameVariableBuffer::GetVariable( Castor::String const & p_name, std::shared_ptr< MatrixFrameVariable< T, Rows, Columns > > & p_variable )const
	{
		FrameVariablePtrStrMapConstIt l_it = m_mapVariables.find( p_name );

		if ( l_it != m_mapVariables.end() )
		{
			FrameVariableSPtr l_variable = l_it->second.lock();

			if ( l_variable->GetFullType() == MtxFrameVariableDefinitions< T, Rows, Columns >::Full )
			{
				p_variable = std::static_pointer_cast< MatrixFrameVariable< T, Rows, Columns > >( l_variable );
			}
		}

		return p_variable;
	}
}
