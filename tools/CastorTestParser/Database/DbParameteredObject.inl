#include "DbParameter.hpp"

namespace test_parser::db
{
	uint16_t ParameteredObject::getParametersCount() const
	{
		return uint16_t( m_parameters.size() );
	}

	template< typename T >
	void ParameteredObject::setParameterValue( uint32_t index
		, const T & value )
	{
		getParameter( index )->SetValueFast( value );
	}

	template< typename T >
	void ParameteredObject::setParameterValue( const std::string & name
		, const T & value )
	{
		getParameter( name )->SetValueFast( value );
	}

	const ParameterArray & ParameteredObject::doGetParameters() const
	{
		return m_parameters;
	}
}
