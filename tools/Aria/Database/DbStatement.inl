namespace aria::db
{
	template <typename T>
	const T & Statement::getOutputValue( uint32_t index )
	{
		return getParameter( index )->getValue< T >();
	}

	template <typename T>
	const T & Statement::getOutputValue( const std::string & name )
	{
		return getParameter( name )->getValue< T >();
	}

	template <typename T>
	void Statement::getOutputValue( uint32_t index, T & value )
	{
		getParameter( index )->getValue( value );
	}

	template <typename T>
	void Statement::getOutputValue( const std::string & name, T & value )
	{
		getParameter( name )->getValue( value );
	}
}
