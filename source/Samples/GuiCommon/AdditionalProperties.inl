namespace GuiCommon
{

	//************************************************************************************************

	template<>
	inline bool getValue< bool >( wxVariant const & p_variant )
	{
		return p_variant.GetBool();
	}

	template<>
	inline int getValue< int >( wxVariant const & p_variant )
	{
		return p_variant.GetInteger();
	}

	template<>
	inline uint32_t getValue< uint32_t >( wxVariant const & p_variant )
	{
		return uint32_t( p_variant.GetInteger() );
	}

	template<>
	inline float getValue< float >( wxVariant const & p_variant )
	{
		return float( p_variant.GetDouble() );
	}

	template<>
	inline double getValue< double >( wxVariant const & p_variant )
	{
		return p_variant.GetDouble();
	}

	//************************************************************************************************

	template<>
	inline wxVariant setValue< bool >( bool const & p_value )
	{
		return wxVariant( p_value );
	}

	template<>
	inline wxVariant setValue< int >( int const & p_value )
	{
		return wxVariant( p_value );
	}

	template<>
	inline wxVariant setValue< uint32_t >( uint32_t const & p_value )
	{
		return wxVariant( int( p_value ) );
	}

	template<>
	inline wxVariant setValue< float >( float const & p_value )
	{
		return wxVariant( p_value );
	}

	template<>
	inline wxVariant setValue< double >( double const & p_value )
	{
		return wxVariant( p_value );
	}
}
