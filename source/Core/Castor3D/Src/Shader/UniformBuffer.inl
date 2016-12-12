#include "MatrixUniform.hpp"
#include "OneUniform.hpp"
#include "PointUniform.hpp"

namespace Castor3D
{
	template< typename T >
	std::shared_ptr< OneUniform< T > > UniformBuffer::GetVariable( Castor::String const & p_name, std::shared_ptr< OneUniform< T > > & p_variable )const
	{
		auto l_it = m_mapVariables.find( p_name );

		if ( l_it != m_mapVariables.end() )
		{
			UniformSPtr l_variable = l_it->second.lock();

			if ( l_variable->GetFullType() == OneUniformDefinitions< T >::Full )
			{
				p_variable = std::static_pointer_cast< OneUniform< T > >( l_variable );
			}
		}

		return p_variable;
	}

	template< typename T, uint32_t Count >
	std::shared_ptr< PointUniform< T, Count > > UniformBuffer::GetVariable( Castor::String const & p_name, std::shared_ptr< PointUniform< T, Count > > & p_variable )const
	{
		auto l_it = m_mapVariables.find( p_name );

		if ( l_it != m_mapVariables.end() )
		{
			UniformSPtr l_variable = l_it->second.lock();

			if ( l_variable->GetFullType() == PntUniformDefinitions< T, Count >::Full )
			{
				p_variable = std::static_pointer_cast< PointUniform< T, Count > >( l_variable );
			}
		}

		return p_variable;
	}

	template< typename T, uint32_t Rows, uint32_t Columns >
	std::shared_ptr< MatrixUniform< T, Rows, Columns > > UniformBuffer::GetVariable( Castor::String const & p_name, std::shared_ptr< MatrixUniform< T, Rows, Columns > > & p_variable )const
	{
		auto l_it = m_mapVariables.find( p_name );

		if ( l_it != m_mapVariables.end() )
		{
			UniformSPtr l_variable = l_it->second.lock();

			if ( l_variable->GetFullType() == MtxUniformDefinitions< T, Rows, Columns >::Full )
			{
				p_variable = std::static_pointer_cast< MatrixUniform< T, Rows, Columns > >( l_variable );
			}
		}

		return p_variable;
	}
}
