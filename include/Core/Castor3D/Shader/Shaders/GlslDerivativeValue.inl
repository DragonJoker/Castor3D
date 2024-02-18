namespace castor3d::shader
{
	template< typename ValueT, sdw::StringLiteralT StructNameT >
	DerivativeValueT< ValueT, StructNameT > DerivativeValueT< ValueT, StructNameT >::operator-()const
	{
		return negate( *this );
	}

	template< typename ValueT, sdw::StringLiteralT StructNameT >
	DerivativeValueT< ValueT, StructNameT > & DerivativeValueT< ValueT, StructNameT >::operator+=( DerivativeValueT< ValueT, StructNameT > const & rhs )
	{
		value() = value() + rhs.value();
		dPdx() = dPdx() + rhs.dPdx();
		dPdy() = dPdy() + rhs.dPdy();
		return *this;
	}

	template< typename ValueT, sdw::StringLiteralT StructNameT >
	DerivativeValueT< ValueT, StructNameT > & DerivativeValueT< ValueT, StructNameT >::operator-=( DerivativeValueT< ValueT, StructNameT > const & rhs )
	{
		value() = value() - rhs.value();
		dPdx() = dPdx() - rhs.dPdx();
		dPdy() = dPdy() - rhs.value();
		return *this;
	}

	template< typename ValueT, sdw::StringLiteralT StructNameT >
	DerivativeValueT< ValueT, StructNameT > & DerivativeValueT< ValueT, StructNameT >::operator*=( DerivativeValueT< ValueT, StructNameT > const & rhs )
	{
		value() = value() * rhs.value();
		dPdx() = dPdx() * rhs.dPdx();
		dPdy() = dPdy() * rhs.dPdy();
		return *this;
	}

	template< typename ValueT, sdw::StringLiteralT StructNameT >
	DerivativeValueT< ValueT, StructNameT > & DerivativeValueT< ValueT, StructNameT >::operator+=( ValueT const & rhs )
	{
		value() = value() + rhs.value();
		return *this;
	}

	template< typename ValueT, sdw::StringLiteralT StructNameT >
	DerivativeValueT< ValueT, StructNameT > & DerivativeValueT< ValueT, StructNameT >::operator-=( ValueT const & rhs )
	{
		value() = value() - rhs.value();
		return *this;
	}

	template< typename ValueT, sdw::StringLiteralT StructNameT >
	DerivativeValueT< ValueT, StructNameT > & DerivativeValueT< ValueT, StructNameT >::operator*=( ValueT const & rhs )
	{
		value() = value() * rhs.value();
		return *this;
	}
}
