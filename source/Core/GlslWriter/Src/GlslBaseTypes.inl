#include <iomanip>

namespace GLSL
{
	//*****************************************************************************************

	Int::Int()
		: Type( cuT( "int " ) )
	{
	}

	Int::Int( Int && p_value )
		: Type( std::move( p_value ) )
	{
	}

	Int::Int( Int const & p_value )
		: Type( cuT( "int " ), p_value.m_writer )
	{
		m_value << castor::String( p_value );
	}

	Int::Int( Type const & p_value )
		: Type( cuT( "int " ), p_value.m_writer )
	{
		m_value << castor::String( p_value );
	}

	Int::Int( int p_value )
		: Type( cuT( "int " ) )
	{
		m_value << p_value;
	}

	Int::Int( float p_value )
		: Type( cuT( "int " ) )
	{
		m_value << p_value;
	}

	Int::Int( GlslWriter * p_writer, int p_value )
		: Type( cuT( "int " ), p_writer, castor::String() )
	{
		m_value << p_value;
	}

	Int::Int( GlslWriter * p_writer, float p_value )
		: Type( cuT( "int " ), p_writer, castor::String() )
	{
		m_value << p_value;
	}

	Int::Int( GlslWriter * p_writer, castor::String const & p_name )
		: Type( cuT( "int " ), p_writer, p_name )
	{
	}

	Int & Int::operator=( Int const & p_rhs )
	{
		if ( m_writer )
		{
			m_writer->writeAssign( *this, p_rhs );
		}
		else
		{
			Type::operator=( p_rhs );
			m_writer = p_rhs.m_writer;
		}

		return *this;
	}

	template< typename T >
	Int & Int::operator=( T const & p_rhs )
	{
		updateWriter( p_rhs );
		m_writer->writeAssign( *this, p_rhs );
		return *this;
	}

	template< typename T >
	Int & Int::operator=( int p_rhs )
	{
		m_writer->writeAssign( *this, p_rhs );
		return *this;
	}

	Int::operator uint32_t()
	{
		return 0u;
	}

	Int & Int::operator++()
	{
		m_value << cuT( "++" ) << castor::String( *this );
		return *this;
	}

	Int Int::operator++( int )
	{
		Int result;
		result.m_value << castor::String( *this ) << cuT( "++" );
		return result;
	}

	inline Int & Int::operator<<=( int i )
	{
		*m_writer << castor::String{ *this } << cuT( " <<= " ) << castor::string::toString( i ) << cuT( ";" ) << Endl();
		return *this;
	}

	inline Int & Int::operator>>=( int i )
	{
		*m_writer << castor::String{ *this } << cuT( " >>= " ) << castor::string::toString( i ) << cuT( ";" ) << Endl();
		return *this;
	}

	inline Int & Int::operator<<=( Int const & i )
	{
		*m_writer << castor::String{ *this } << cuT( " <<= " ) << castor::String( i ) << cuT( ";" ) << Endl();
		return *this;
	}

	inline Int & Int::operator>>=( Int const & i )
	{
		*m_writer << castor::String{ *this } << cuT( " >>= " ) << castor::String( i ) << cuT( ";" ) << Endl();
		return *this;
	}

	inline Int & Int::operator<<=( UInt const & i )
	{
		*m_writer << castor::String{ *this } << cuT( " <<= " ) << castor::String( i ) << cuT( ";" ) << Endl();
		return *this;
	}

	inline Int & Int::operator>>=( UInt const & i )
	{
		*m_writer << castor::String{ *this } << cuT( " >>= " ) << castor::String( i ) << cuT( ";" ) << Endl();
		return *this;
	}

	inline Int & Int::operator&=( int i )
	{
		*m_writer << castor::String{ *this } << cuT( " &= " ) << castor::string::toString( i ) << cuT( ";" ) << Endl();
		return *this;
	}

	inline Int & Int::operator|=( int i )
	{
		*m_writer << castor::String{ *this } << cuT( " |= " ) << castor::string::toString( i ) << cuT( ";" ) << Endl();
		return *this;
	}

	inline Int & Int::operator&=( Int const & i )
	{
		*m_writer << castor::String{ *this } << cuT( " &= " ) << castor::String( i ) << cuT( ";" ) << Endl();
		return *this;
	}

	inline Int & Int::operator|=( Int const & i )
	{
		*m_writer << castor::String{ *this } << cuT( " |= " ) << castor::String( i ) << cuT( ";" ) << Endl();
		return *this;
	}

	inline Int & Int::operator&=( UInt const & i )
	{
		*m_writer << castor::String{ *this } << cuT( " &= " ) << castor::String( i ) << cuT( ";" ) << Endl();
		return *this;
	}

	inline Int & Int::operator|=( UInt const & i )
	{
		*m_writer << castor::String{ *this } << cuT( " |= " ) << castor::String( i ) << cuT( ";" ) << Endl();
		return *this;
	}

	inline Int operator<<( Int const & p_value, int i )
	{
		Int result( p_value.m_writer );
		result.m_value << castor::String( p_value ) << " << " << toString( i );
		return result;
	}

	inline Int operator>>( Int const & p_value, int i )
	{
		Int result( p_value.m_writer );
		result.m_value << castor::String( p_value ) << " >> " << toString( i );
		return result;
	}

	inline Int operator<<( Int const & p_value, Int const & i )
	{
		Int result( p_value.m_writer );
		result.m_value << castor::String( p_value ) << " << " << castor::String( i );
		return result;
	}

	inline Int operator>>( Int const & p_value, Int const & i )
	{
		Int result( p_value.m_writer );
		result.m_value << castor::String( p_value ) << " >> " << castor::String( i );
		return result;
	}

	inline Int operator<<( Int const & p_value, UInt const & i )
	{
		Int result( p_value.m_writer );
		result.m_value << castor::String( p_value ) << " << " << castor::String( i );
		return result;
	}

	inline Int operator>>( Int const & p_value, UInt const & i )
	{
		Int result( p_value.m_writer );
		result.m_value << castor::String( p_value ) << " >> " << castor::String( i );
		return result;
	}

	inline Int operator&( Int const & p_value, int i )
	{
		Int result( p_value.m_writer );
		result.m_value << castor::String( p_value ) << " & " << toString( i );
		return result;
	}

	inline Int operator|( Int const & p_value, int i )
	{
		Int result( p_value.m_writer );
		result.m_value << castor::String( p_value ) << " | " << toString( i );
		return result;
	}

	inline Int operator&( Int const & p_value, Int const & i )
	{
		Int result( p_value.m_writer );
		result.m_value << castor::String( p_value ) << " & " << castor::String( i );
		return result;
	}

	inline Int operator|( Int const & p_value, Int const & i )
	{
		Int result( p_value.m_writer );
		result.m_value << castor::String( p_value ) << " | " << castor::String( i );
		return result;
	}

	inline Int operator&( Int const & p_value, UInt const & i )
	{
		Int result( p_value.m_writer );
		result.m_value << castor::String( p_value ) << " & " << castor::String( i );
		return result;
	}

	inline Int operator|( Int const & p_value, UInt const & i )
	{
		Int result( p_value.m_writer );
		result.m_value << castor::String( p_value ) << " | " << castor::String( i );
		return result;
	}

	//*****************************************************************************************

	UInt::UInt()
		: Type( cuT( "uint " ) )
	{
	}

	UInt::UInt( UInt && p_value )
		: Type( std::move( p_value ) )
	{
	}

	UInt::UInt( UInt const & p_value )
		: Type( cuT( "uint " ), p_value.m_writer )
	{
		m_value << castor::String( p_value );
	}

	UInt::UInt( Type const & p_value )
		: Type( cuT( "uint " ), p_value.m_writer )
	{
		m_value << castor::String( p_value );
	}

	UInt::UInt( unsigned int p_value )
		: Type( cuT( "uint " ) )
	{
		m_value << p_value;
	}

	UInt::UInt( float p_value )
		: Type( cuT( "uint " ) )
	{
		m_value << p_value;
	}

	UInt::UInt( GlslWriter * p_writer, unsigned int p_value )
		: Type( cuT( "uint " ), p_writer, castor::String() )
	{
		m_value << p_value;
	}

	UInt::UInt( GlslWriter * p_writer, float p_value )
		: Type( cuT( "uint " ), p_writer, castor::String() )
	{
		m_value << p_value;
	}

	UInt::UInt( GlslWriter * p_writer, castor::String const & p_name )
		: Type( cuT( "uint " ), p_writer, p_name )
	{
	}

	UInt & UInt::operator=( UInt const & p_rhs )
	{
		if ( m_writer )
		{
			m_writer->writeAssign( *this, p_rhs );
		}
		else
		{
			Type::operator=( p_rhs );
			m_writer = p_rhs.m_writer;
		}

		return *this;
	}

	template< typename T >
	UInt & UInt::operator=( T const & p_rhs )
	{
		updateWriter( p_rhs );
		m_writer->writeAssign( *this, p_rhs );
		return *this;
	}

	template< typename T >
	UInt & UInt::operator=( unsigned int p_rhs )
	{
		m_writer->writeAssign( *this, p_rhs );
		return *this;
	}

	UInt::operator uint32_t()
	{
		return 0u;
	}

	UInt & UInt::operator++()
	{
		m_value << cuT( "++" ) << castor::String( *this );
		return *this;
	}

	UInt UInt::operator++( int )
	{
		UInt result;
		result.m_value << castor::String( *this ) << cuT( "++" );
		return result;
	}

	inline UInt & UInt::operator<<=( int i )
	{
		*m_writer << castor::String{ *this } << cuT( " <<= " ) << castor::string::toString( i ) << cuT( ";" ) << Endl();
		return *this;
	}

	inline UInt & UInt::operator>>=( int i )
	{
		*m_writer << castor::String{ *this } << cuT( " >>= " ) << castor::string::toString( i ) << cuT( ";" ) << Endl();
		return *this;
	}

	inline UInt & UInt::operator<<=( Int const & i )
	{
		*m_writer << castor::String{ *this } << cuT( " <<= " ) << castor::String( i ) << cuT( ";" ) << Endl();
		return *this;
	}

	inline UInt & UInt::operator>>=( Int const & i )
	{
		*m_writer << castor::String{ *this } << cuT( " >>= " ) << castor::String( i ) << cuT( ";" ) << Endl();
		return *this;
	}

	inline UInt & UInt::operator<<=( UInt const & i )
	{
		*m_writer << castor::String{ *this } << cuT( " <<= " ) << castor::String( i ) << cuT( ";" ) << Endl();
		return *this;
	}

	inline UInt & UInt::operator>>=( UInt const & i )
	{
		*m_writer << castor::String{ *this } << cuT( " >>= " ) << castor::String( i ) << cuT( ";" ) << Endl();
		return *this;
	}

	inline UInt & UInt::operator&=( int i )
	{
		*m_writer << castor::String{ *this } << cuT( " &= " ) << castor::string::toString( i ) << cuT( ";" ) << Endl();
		return *this;
	}

	inline UInt & UInt::operator|=( int i )
	{
		*m_writer << castor::String{ *this } << cuT( " |= " ) << castor::string::toString( i ) << cuT( ";" ) << Endl();
		return *this;
	}

	inline UInt & UInt::operator&=( Int const & i )
	{
		*m_writer << castor::String{ *this } << cuT( " &= " ) << castor::String( i ) << cuT( ";" ) << Endl();
		return *this;
	}

	inline UInt & UInt::operator|=( Int const & i )
	{
		*m_writer << castor::String{ *this } << cuT( " |= " ) << castor::String( i ) << cuT( ";" ) << Endl();
		return *this;
	}

	inline UInt & UInt::operator&=( UInt const & i )
	{
		*m_writer << castor::String{ *this } << cuT( " &= " ) << castor::String( i ) << cuT( ";" ) << Endl();
		return *this;
	}

	inline UInt & UInt::operator|=( UInt const & i )
	{
		*m_writer << castor::String{ *this } << cuT( " |= " ) << castor::String( i ) << cuT( ";" ) << Endl();
		return *this;
	}

	inline UInt operator<<( UInt const & p_value, int i )
	{
		UInt result( p_value.m_writer );
		result.m_value << castor::String( p_value ) << " << " << toString( i );
		return result;
	}

	inline UInt operator>>( UInt const & p_value, int i )
	{
		UInt result( p_value.m_writer );
		result.m_value << castor::String( p_value ) << " >> " << toString( i );
		return result;
	}

	inline UInt operator<<( UInt const & p_value, Int const & i )
	{
		UInt result( p_value.m_writer );
		result.m_value << castor::String( p_value ) << " << " << castor::String( i );
		return result;
	}

	inline UInt operator>>( UInt const & p_value, Int const & i )
	{
		UInt result( p_value.m_writer );
		result.m_value << castor::String( p_value ) << " >> " << castor::String( i );
		return result;
	}

	inline UInt operator<<( UInt const & p_value, UInt const & i )
	{
		UInt result( p_value.m_writer );
		result.m_value << castor::String( p_value ) << " << " << castor::String( i );
		return result;
	}

	inline UInt operator>>( UInt const & p_value, UInt const & i )
	{
		UInt result( p_value.m_writer );
		result.m_value << castor::String( p_value ) << " >> " << castor::String( i );
		return result;
	}

	inline UInt operator&( UInt const & p_value, int i )
	{
		UInt result( p_value.m_writer );
		result.m_value << castor::String( p_value ) << " & " << toString( i );
		return result;
	}

	inline UInt operator|( UInt const & p_value, int i )
	{
		UInt result( p_value.m_writer );
		result.m_value << castor::String( p_value ) << " | " << toString( i );
		return result;
	}

	inline UInt operator&( UInt const & p_value, Int const & i )
	{
		UInt result( p_value.m_writer );
		result.m_value << castor::String( p_value ) << " & " << castor::String( i );
		return result;
	}

	inline UInt operator|( UInt const & p_value, Int const & i )
	{
		UInt result( p_value.m_writer );
		result.m_value << castor::String( p_value ) << " | " << castor::String( i );
		return result;
	}

	inline UInt operator&( UInt const & p_value, UInt const & i )
	{
		UInt result( p_value.m_writer );
		result.m_value << castor::String( p_value ) << " & " << castor::String( i );
		return result;
	}

	inline UInt operator|( UInt const & p_value, UInt const & i )
	{
		UInt result( p_value.m_writer );
		result.m_value << castor::String( p_value ) << " | " << castor::String( i );
		return result;
	}

	//*****************************************************************************************

	Float::Float()
		: Type( cuT( "float " ) )
	{
	}

	Float::Float( Float && p_value )
		: Type( std::move( p_value ) )
	{
	}

	Float::Float( Float const & p_value )
		: Type( cuT( "float " ) )
	{
		m_value << castor::String( p_value );
	}

	Float::Float( Type const & p_value )
		: Type( cuT( "float " ) )
	{
		m_value << castor::String( p_value );
	}

	Float::Float( int p_value )
		: Type( cuT( "float " ) )
	{
		m_value << p_value << cuT( ".0" );
	}

	Float::Float( float p_value )
		: Type( cuT( "float " ) )
	{
		if ( abs( p_value - int( p_value ) ) <= std::numeric_limits< float >::epsilon() )
		{
			m_value << int( p_value ) << cuT( ".0" );
		}
		else
		{
			m_value << p_value;
		}
	}

	Float::Float( double p_value )
		: Type( cuT( "float " ) )
	{
		if ( abs( p_value - int( p_value ) ) <= std::numeric_limits< double >::epsilon() )
		{
			m_value << int( p_value ) << cuT( ".0" );
		}
		else
		{
			m_value << std::setprecision( 15u ) << p_value;
		}
	}

	Float::Float( GlslWriter * p_writer, int p_value )
		: Type( cuT( "float " ), p_writer, castor::String() )
	{
		m_value << p_value << cuT( ".0" );
	}

	Float::Float( GlslWriter * p_writer, float p_value )
		: Type( cuT( "float " ), p_writer, castor::String() )
	{
		if ( abs( p_value - int( p_value ) ) <= std::numeric_limits< float >::epsilon() )
		{
			m_value << int( p_value ) << cuT( ".0" );
		}
		else
		{
			m_value << p_value;
		}
	}

	Float::Float( GlslWriter * p_writer, double p_value )
		: Type( cuT( "float " ), p_writer, castor::String() )
	{

		if ( abs( p_value - int( p_value ) ) <= std::numeric_limits< double >::epsilon() )
		{
			m_value << int( p_value ) << cuT( ".0" );
		}
		else
		{
			m_value << std::setprecision( 15u ) << p_value;
		}
	}

	Float::Float( GlslWriter * p_writer, castor::String const & p_name )
		: Type( cuT( "float " ), p_writer, p_name )
	{
	}

	Float & Float::operator=( Float const & p_rhs )
	{
		if ( m_writer )
		{
			m_writer->writeAssign( *this, p_rhs );
		}
		else
		{
			Type::operator=( p_rhs );
			m_writer = p_rhs.m_writer;
		}

		return *this;
	}

	template< typename T >
	Float & Float::operator=( T const & p_rhs )
	{
		updateWriter( p_rhs );
		m_writer->writeAssign( *this, p_rhs );
		return *this;
	}

	template< typename T >
	Float & Float::operator=( float p_rhs )
	{
		*m_writer << castor::String( *this ) << cuT( " = " ) << castor::string::toString( p_rhs ) << cuT( ";" ) << Endl();
		return *this;
	}

	Float::operator float()
	{
		return 0.0f;
	}

	//*****************************************************************************************
}
