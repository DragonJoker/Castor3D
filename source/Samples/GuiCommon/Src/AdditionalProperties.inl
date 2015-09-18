namespace GuiCommon
{

	//************************************************************************************************

	template<>
	inline bool GetValue< bool >( wxVariant const & p_variant )
	{
		return p_variant.GetBool();
	}

	template<>
	inline int GetValue< int >( wxVariant const & p_variant )
	{
		return p_variant.GetInteger();
	}

	template<>
	inline uint32_t GetValue< uint32_t >( wxVariant const & p_variant )
	{
		return uint32_t( p_variant.GetInteger() );
	}

	template<>
	inline float GetValue< float >( wxVariant const & p_variant )
	{
		return p_variant.GetDouble();
	}

	template<>
	inline double GetValue< double >( wxVariant const & p_variant )
	{
		return p_variant.GetDouble();
	}

	//************************************************************************************************

	template<>
	inline wxVariant SetValue< bool >( bool const & p_value )
	{
		return wxVariant( p_value );
	}

	template<>
	inline wxVariant SetValue< int >( int const & p_value )
	{
		return wxVariant( p_value );
	}

	template<>
	inline wxVariant SetValue< uint32_t >( uint32_t const & p_value )
	{
		return wxVariant( int( p_value ) );
	}

	template<>
	inline wxVariant SetValue< float >( float const & p_value )
	{
		return wxVariant( p_value );
	}

	template<>
	inline wxVariant SetValue< double >( double const & p_value )
	{
		return wxVariant( p_value );
	}
}
