#include "DbParameter.hpp"

namespace aria::db
{
	uint16_t ParameteredObject::getParametersCount() const
	{
		return uint16_t( m_parameters.size() );
	}

	template< typename T >
	void ParameteredObject::setParameterValue( uint32_t index
		, const T & value )
	{
		getParameter( index )->setValue( value );
	}

	template< typename T >
	void ParameteredObject::setParameterValue( const std::string & name
		, const T & value )
	{
		getParameter( name )->setValue( value );
	}

	const ParameterArray & ParameteredObject::doGetParameters() const
	{
		return m_parameters;
	}
}
