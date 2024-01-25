#include "CastorUtils/Design/FlagCombination.hpp"

#include "CastorUtils/Config/BeginExternHeaderGuard.hpp"
#include <locale>
#include "CastorUtils/Config/EndExternHeaderGuard.hpp"

namespace castor::string
{
	//*************************************************************************************************

	namespace details
	{
		template< typename StringT, typename StringViewT >
		Vector< StringT > split( StringT const & text
			, StringViewT delims
			, uint32_t maxSplits
			, bool keepEmpty )
		{
			Vector< StringT > result;

			if ( !text.empty() && !delims.empty() && maxSplits > 0 )
			{
				result.reserve( maxSplits + 1 );
				std::size_t pos = 0;
				std::size_t start = 0;

				do
				{
					pos = text.find_first_of( delims, start );

					if ( pos == start )
					{
						start = pos + 1;

						if ( keepEmpty )
						{
							result.emplace_back();
						}
					}
					else if ( pos == StringT::npos || result.size() == maxSplits )
					{
						if ( StringT remnants = text.substr( start );
							!remnants.empty() || keepEmpty )
						{
							result.push_back( remnants );
						}

						pos = StringT::npos;
					}
					else
					{
						result.push_back( text.substr( start, pos - start ) );
						start = pos + 1;
					}

				}
				while ( pos != StringT::npos );
			}

			return result;
		}

		template< typename StringT, typename StringViewT >
		StringT & trim( StringT & text
			, bool left
			, bool right
			, StringViewT seps )
		{
			if ( !text.empty() )
			{
				if ( left )
				{
					if ( auto index = text.find_first_not_of( seps );
						index > 0 )
					{
						if ( index != StringT::npos )
						{
							text = text.substr( index, StringT::npos );
						}
						else
						{
							text = text.substr( 0, 0 );
						}
					}
				}

				if ( right && !text.empty() )
				{
					if ( auto index = text.find_last_not_of( seps );
						index < text.size() - 1 )
					{
						if ( index != StringT::npos )
						{
							text = text.substr( 0, index + 1 );
						}
						else
						{
							text = text.substr( 0, 0 );
						}
					}
				}
			}

			return text;
		}
	}

	//*************************************************************************************************

	template< typename T >
	inline void parse( String const & str, T & val, std::locale const & locale )
	{
		StringStream out( str );
		out.imbue( locale );
		out >> val;
	}

	template< typename T >
	inline void extract( String & str, T & val, std::locale const & locale )
	{
		StringStream out( str );
		out.imbue( locale );
		out >> val;
		str = out.str();
	}

	template< typename T >
	inline String toString( T const & value, std::locale const & locale )
	{
		StringStream result;
		result.imbue( locale );
		result << value;
		return result.str();
	}

	template< typename T >
	inline MbString toMbString( T const & value, std::locale const & locale )
	{
		MbStringStream result;
		result.imbue( locale );
		result << value;
		return result.str();
	}

	template< typename CharT >
	Vector< std::basic_string< CharT > > split( std::basic_string< CharT > const & text
		, std::basic_string_view< CharT > delims
		, uint32_t maxSplits
		, bool keepEmpty )
	{
		return details::split( text, delims, maxSplits, keepEmpty );
	}

	template< typename CharT >
	Vector< std::basic_string< CharT > > split( std::basic_string< CharT > const & text
		, std::basic_string< CharT > delims
		, uint32_t maxSplits
		, bool keepEmpty )
	{
		return details::split( text, delims, maxSplits, keepEmpty );
	}

	template< typename CharT >
	Vector< std::basic_string< CharT > > split( std::basic_string< CharT > const & text
		, CharT const * delims
		, uint32_t maxSplits
		, bool keepEmpty )
	{
		return details::split( text, std::basic_string_view< CharT >{ delims }, maxSplits, keepEmpty );
	}

	template< typename CharT >
	Vector< std::basic_string_view< CharT > > split( std::basic_string_view< CharT > text
		, std::basic_string_view< CharT > delims
		, uint32_t maxSplits
		, bool keepEmpty )
	{
		return details::split( text, delims, maxSplits, keepEmpty );
	}

	template< typename CharT >
	Vector< std::basic_string_view< CharT > > split( std::basic_string_view< CharT > text
		, std::basic_string< CharT > delims
		, uint32_t maxSplits
		, bool keepEmpty )
	{
		return details::split( text, delims, maxSplits, keepEmpty );
	}

	template< typename CharT >
	Vector< std::basic_string_view< CharT > > split( std::basic_string_view< CharT > text
		, CharT const * delims
		, uint32_t maxSplits
		, bool keepEmpty )
	{
		return details::split( text, std::basic_string_view< CharT >{ delims }, maxSplits, keepEmpty );
	}

	template< typename CharT >
	std::basic_string< CharT > & replace( std::basic_string< CharT > & text
		, CharT lookup
		, CharT replacement )
	{
		return replace( text
			, std::basic_string< CharT >{ lookup, CharT( '\0' ) }
			, std::basic_string< CharT >{ replacement, CharT( '\0' ) } );
	}

	template< typename CharT >
	std::basic_string< CharT > & replace( std::basic_string< CharT > & text
		, CharT const * lookup
		, CharT const * replacement )
	{
		return replace( text
			, std::basic_string_view< CharT >{ lookup }
			, std::basic_string_view< CharT >{ replacement } );
	}

	template< typename CharT >
	std::basic_string< CharT > & replace( std::basic_string< CharT > & text
		, std::basic_string_view< CharT > lookup
		, CharT replacement )
	{
		return replace( text
			, lookup
			, std::basic_string< CharT >{ replacement, CharT( '\0' ) } );
	}

	template< typename CharT >
	std::basic_string< CharT > & replace( std::basic_string< CharT > & text
		, CharT lookup
		, std::basic_string_view< CharT > replacement )
	{
		return replace( text
			, std::basic_string< CharT >{ lookup, CharT( '\0' ) }
			, replacement );
	}

	template< typename CharT >
	std::basic_string< CharT > & replace( std::basic_string< CharT > & text
		, std::basic_string< CharT > lookup
		, std::basic_string< CharT > replacement )
	{
		return replace( text
			, std::basic_string_view< CharT >{ lookup }
			, std::basic_string_view< CharT >{ replacement } );
	}

	template< typename CharT >
	std::basic_string< CharT > & replace( std::basic_string< CharT > & text
		, std::basic_string_view< CharT > lookup
		, std::basic_string_view< CharT > replacement )
	{
		std::basic_string< CharT > result;
		std::size_t currentPos = 0;
		std::size_t pos = 0;

		while ( ( pos = text.find( lookup, currentPos ) ) != std::basic_string< CharT >::npos )
		{
			result.append( text.substr( currentPos, pos - currentPos ) );
			result.append( replacement );
			currentPos = pos + lookup.size();
		}

		if ( currentPos != text.size() )
		{
			result.append( text.substr( currentPos, pos - currentPos ) );
		}

		text = result;
		return text;
	}

	template< typename CharT >
	std::basic_string< CharT > & trim( std::basic_string< CharT > & text
		, bool left
		, bool right
		, std::basic_string_view< CharT > seps )
	{
		return details::trim( text, left, right, seps );
	}

	template< typename CharT >
	std::basic_string_view< CharT > & trim( std::basic_string_view< CharT > & text
		, bool left
		, bool right
		, std::basic_string_view< CharT > seps )
	{
		return details::trim( text, left, right, seps );
	}

	template< typename CharT >
	std::basic_string< CharT > getLongestCommonSubstring( std::basic_string< CharT > const & lhs
		, std::basic_string< CharT > const & rhs )
	{
		if ( lhs.empty() )
		{
			return rhs;
		}

		if ( rhs.empty() )
		{
			return lhs;
		}

		Vector< std::basic_string< CharT > > substrings;

		for ( auto beg = lhs.begin(); beg != std::prev( lhs.end() ); ++beg )
		{
			for ( auto end = beg; end != lhs.end(); ++end )
			{
				if ( rhs.find( std::basic_string< CharT >{ beg, std::next( end ) } ) != std::basic_string< CharT >::npos )
				{
					substrings.emplace_back( beg, std::next( end ) );
				}
			}
		}

		if ( substrings.empty() )
		{
			return std::basic_string< CharT >{};
		}

		return *std::max_element( substrings.begin(), substrings.end(),
			[]( auto & elem1, auto & elem2 )
			{
				return elem1.length() < elem2.length();
			} );
	}

	namespace utf8
	{
		//*************************************************************************************************

		namespace details
		{
			static constexpr auto FirstBitMask = uint8_t( 0x80 );
			static constexpr auto SecondBitMask = uint8_t( 0x40 );
			static constexpr auto ThirdBitMask = uint8_t( 0x20 );
			static constexpr auto FourthBitMask = uint8_t( 0x10 );
			static constexpr auto FifthBitMask = uint8_t( 0x08 );

			static constexpr auto ThreeBitsMask = uint8_t( 0b0000'0111 );
			static constexpr auto FourBitsMask = uint8_t( 0b0000'1111 );
			static constexpr auto FiveBitsMask = uint8_t( 0b0001'1111 );
			static constexpr auto SixBitsMask = uint8_t( 0b0011'1111 );
		}

		template< typename IteratorType >
		inline char32_t toUtf8( IteratorType first, IteratorType end )
		{
			char32_t result{};

			if ( auto firstByte = uint8_t( *first );
				checkFlag( firstByte, details::FirstBitMask ) ) // This means the first byte has a value greater than 127, and so is beyond the ASCII range.
			{
				if ( auto second = std::next( first );
					second != end )
				{
					if ( checkFlag( firstByte, details::ThirdBitMask ) ) // This means that the first byte has a value greater than 191, and so it must be at least a three-octet code point.
					{
						if ( auto third = std::next( second );
							third != end )
						{
							if ( checkFlag( firstByte, details::FourthBitMask ) ) // This means that the first byte has a value greater than 224, and so it must be a four-octet code point.
							{
								if ( auto fourth = std::next( third );
									fourth != end )
								{
									result = char32_t( ( firstByte & details::ThreeBitsMask ) << 18 );
									auto secondByte = uint8_t( *second );
									result += char32_t( ( secondByte & details::SixBitsMask ) << 12 );
									auto thirdByte = uint8_t( *third );
									result += char32_t( ( thirdByte & details::SixBitsMask ) << 6 );
									auto fourthByte = uint8_t( *fourth );
									result += char32_t( fourthByte & details::SixBitsMask );
								}
							}
							else
							{
								result = char32_t( ( firstByte & details::FourBitsMask ) << 12 );
								auto secondByte = uint8_t( *second );
								result += char32_t( ( secondByte & details::SixBitsMask ) << 6 );
								auto thirdByte = uint8_t( *third );
								result += char32_t( thirdByte & details::SixBitsMask );
							}
						}
					}
					else
					{
						result = char32_t( ( firstByte & details::FiveBitsMask ) << 6 );
						auto secondByte = uint8_t( *second );
						result += char32_t( secondByte & details::SixBitsMask );
					}
				}
			}
			else
			{
				result = char32_t( firstByte );
			}

			return result;
		}

		inline Array< char, 5u > fromUtf8( char32_t c )
		{
			Array< char, 5u > result{};
			auto it = result.begin();

			if ( c < 0x7f )
			{
				*( it++ ) = char( c );
			}
			else if ( c < 0x7ff )
			{
				*( it++ ) = char( 0b1100'0000 + uint8_t( c >> 6 ) );
				*( it++ ) = char( 0b1000'0000 + uint8_t( c & details::SixBitsMask ) );
			}
			else if ( c < 0x10000 )
			{
				*( it++ ) = char( 0b1110'0000 + uint8_t( c >> 12 ) );
				*( it++ ) = char( 0b1000'0000 + uint8_t( ( c >> 6 ) & details::SixBitsMask ) );
				*( it++ ) = char( 0b1000'0000 + uint8_t( c & details::SixBitsMask ) );
			}
			else if ( c < 0x110000 )
			{
				*( it++ ) = char( 0b1111'0000 + uint8_t( c >> 18 ) );
				*( it++ ) = char( 0b1000'0000 + uint8_t( ( c >> 12 ) & details::SixBitsMask ) );
				*( it++ ) = char( 0b1000'0000 + uint8_t( ( c >> 6 ) & details::SixBitsMask ) );
				*( it++ ) = char( 0b1000'0000 + uint8_t( c & details::SixBitsMask ) );
			}

			return result;
		}

		//*************************************************************************************************

		template< typename CharT, typename StringT >
		inline iterator< CharT, StringT >::iterator( typename iterator::iterator_type const & rhs )
			: m_it( rhs )
			, m_lastCodePoint( 0 )
			, m_dirty( true )
		{
		}

		template< typename CharT, typename StringT >
		inline iterator< CharT, StringT >::iterator( iterator< CharT, StringT > const & rhs )
			: m_it( rhs.m_it )
			, m_lastCodePoint( rhs.m_lastCodePoint )
			, m_dirty( rhs.m_dirty )
		{
		}

		template< typename CharT, typename StringT >
		inline iterator< CharT, StringT >::iterator( iterator< CharT, StringT > && rhs )noexcept
			: m_it( rhs.m_it )
			, m_lastCodePoint( rhs.m_lastCodePoint )
			, m_dirty( rhs.m_dirty )
		{
		}

		template< typename CharT, typename StringT >
		inline iterator< CharT, StringT > & iterator< CharT, StringT >::operator=( typename iterator::iterator_type const & rhs )
		{
			m_it = rhs;
			m_lastCodePoint = 0;
			m_dirty = true;
			return *this;
		}

		template< typename CharT, typename StringT >
		inline iterator< CharT, StringT > & iterator< CharT, StringT >::operator=( iterator< CharT, StringT > const & rhs )
		{
			m_it = rhs.m_it;
			m_lastCodePoint = rhs.m_lastCodePoint;
			m_dirty = rhs.m_dirty;
			return *this;
		}

		template< typename CharT, typename StringT >
		inline iterator< CharT, StringT > & iterator< CharT, StringT >::operator=( iterator< CharT, StringT > && rhs )noexcept
		{
			m_it = rhs.m_it;
			m_lastCodePoint = rhs.m_lastCodePoint;
			m_dirty = rhs.m_dirty;
			return *this;
		}

		template< typename CharT, typename StringT >
		inline iterator< CharT, StringT > & iterator< CharT, StringT >::operator+=( size_t rhs )
		{
			while ( rhs-- )
			{
				++( *this );
			}

			return *this;
		}

		template< typename CharT, typename StringT >
		inline iterator< CharT, StringT > & iterator< CharT, StringT >::operator-=( size_t rhs )
		{
			while ( rhs-- )
			{
				--( *this );
			}

			return *this;
		}

		template< typename CharT, typename StringT >
		inline iterator< CharT, StringT > & iterator< CharT, StringT >::operator++()
		{
			auto firstByte = uint8_t( *m_it );
			++m_it;

			if ( checkFlag( firstByte, details::FirstBitMask ) ) // This means the first byte has a value greater than 127, and so is beyond the ASCII range.
			{
				++m_it;

				if ( checkFlag( firstByte, details::ThirdBitMask ) ) // This means that the first byte has a value greater than 224, and so it must be at least a three-octet code point.
				{
					++m_it;

					if ( checkFlag( firstByte, details::FourthBitMask ) ) // This means that the first byte has a value greater than 240, and so it must be a four-octet code point.
					{
						++m_it;
					}
				}
			}

			m_dirty = true;
			return *this;
		}

		template< typename CharT, typename StringT >
		inline iterator< CharT, StringT > iterator< CharT, StringT >::operator++( int )
		{
			iterator< CharT, StringT > temp = *this;
			++( *this );
			return temp;
		}

		template< typename CharT, typename StringT >
		inline iterator< CharT, StringT > & iterator< CharT, StringT >::operator--()
		{
			--m_it;

			if ( checkFlag( uint8_t( *m_it ), details::FirstBitMask ) ) // This means that the previous byte is not an ASCII character.
			{
				--m_it;

				if ( !checkFlag( uint8_t( *m_it ), details::SecondBitMask ) )
				{
					--m_it;

					if ( !checkFlag( uint8_t( *m_it ), details::SecondBitMask ) )
					{
						--m_it;
					}
				}
			}

			m_dirty = true;
			return *this;
		}

		template< typename CharT, typename StringT >
		inline iterator< CharT, StringT > iterator< CharT, StringT >::operator--( int )
		{
			iterator< CharT, StringT > temp = *this;
			--( *this );
			return temp;
		}

		template< typename CharT, typename StringT >
		inline char32_t iterator< CharT, StringT >::operator*()const
		{
			doCalculateCurrentCodePoint();
			return m_lastCodePoint;
		}

		template< typename CharT, typename StringT >
		inline bool iterator< CharT, StringT >::operator==( const iterator< CharT, StringT > & rhs )const
		{
			return m_it == rhs.m_it;
		}

		template< typename CharT, typename StringT >
		inline bool iterator< CharT, StringT >::operator==( const typename iterator::iterator_type & rhs )const
		{
			return m_it == rhs;
		}

		template< typename CharT, typename StringT >
		inline bool iterator< CharT, StringT >::operator!=( const iterator< CharT, StringT > & rhs )const
		{
			return m_it != rhs.m_it;
		}

		template< typename CharT, typename StringT >
		inline bool iterator< CharT, StringT >::operator!=( const typename iterator::iterator_type & rhs )const
		{
			return m_it != rhs;
		}

		template< typename CharT, typename StringT >
		inline typename iterator< CharT, StringT >::iterator_type iterator< CharT, StringT >::internal()const
		{
			return m_it;
		}

		template< typename CharT, typename StringT >
		inline void iterator< CharT, StringT >::doCalculateCurrentCodePoint()const
		{
			if ( m_dirty )
			{
				m_lastCodePoint = toUtf8( m_it, m_it );
				m_dirty = false;
			}
		}

		//*************************************************************************************************

		template< typename CharT, typename StringT >
		iterator< CharT, StringT > operator+( iterator< CharT, StringT > lhs, size_t rhs )
		{
			iterator< CharT, StringT > it( lhs );
			it += rhs;
			return it;
		}

		template< typename CharT, typename StringT >
		iterator< CharT, StringT > operator-( iterator< CharT, StringT > lhs, size_t rhs )
		{
			iterator< CharT, StringT > it( lhs );
			it -= rhs;
			return it;
		}

		//*************************************************************************************************

		template< typename CharT, typename StringT >
		inline const_iterator< CharT, StringT >::const_iterator( typename const_iterator::iterator_type const & rhs )
			: m_it( rhs )
			, m_lastCodePoint( 0 )
			, m_dirty( true )
		{
		}

		template< typename CharT, typename StringT >
		inline const_iterator< CharT, StringT >::const_iterator( const_iterator< CharT, StringT > const & rhs )
			: m_it( rhs.m_it )
			, m_lastCodePoint( rhs.m_lastCodePoint )
			, m_dirty( rhs.m_dirty )
		{
		}

		template< typename CharT, typename StringT >
		inline const_iterator< CharT, StringT >::const_iterator( const_iterator< CharT, StringT > && rhs )noexcept
			: m_it( rhs.m_it )
			, m_lastCodePoint( rhs.m_lastCodePoint )
			, m_dirty( rhs.m_dirty )
		{
		}

		template< typename CharT, typename StringT >
		inline const_iterator< CharT, StringT > & const_iterator< CharT, StringT >::operator=( typename const_iterator::iterator_type const & rhs )
		{
			m_it = rhs;
			m_lastCodePoint = 0;
			m_dirty = true;
			return *this;
		}

		template< typename CharT, typename StringT >
		inline const_iterator< CharT, StringT > & const_iterator< CharT, StringT >::operator=( const_iterator< CharT, StringT > const & rhs )
		{
			m_it = rhs.m_it;
			m_lastCodePoint = rhs.m_lastCodePoint;
			m_dirty = rhs.m_dirty;
			return *this;
		}

		template< typename CharT, typename StringT >
		inline const_iterator< CharT, StringT > & const_iterator< CharT, StringT >::operator=( const_iterator< CharT, StringT > && rhs )noexcept
		{
			m_it = rhs.m_it;
			m_lastCodePoint = rhs.m_lastCodePoint;
			m_dirty = rhs.m_dirty;
			return *this;
		}

		template< typename CharT, typename StringT >
		inline const_iterator< CharT, StringT > & const_iterator< CharT, StringT >::operator+=( size_t rhs )
		{
			while ( rhs-- )
			{
				++( *this );
			}

			return *this;
		}

		template< typename CharT, typename StringT >
		inline const_iterator< CharT, StringT > & const_iterator< CharT, StringT >::operator-=( size_t rhs )
		{
			while ( rhs-- )
			{
				--( *this );
			}

			return *this;
		}

		template< typename CharT, typename StringT >
		inline const_iterator< CharT, StringT > & const_iterator< CharT, StringT >::operator++()
		{
			auto firstByte = uint8_t( *m_it );
			++m_it;

			if ( checkFlag( firstByte, details::FirstBitMask ) ) // This means the first byte has a value greater than 127, and so is beyond the ASCII range.
			{
				++m_it;

				if ( checkFlag( firstByte, details::ThirdBitMask ) ) // This means that the first byte has a value greater than 224, and so it must be at least a three-octet code point.
				{
					++m_it;

					if ( checkFlag( firstByte, details::FourthBitMask ) ) // This means that the first byte has a value greater than 240, and so it must be a four-octet code point.
					{
						++m_it;
					}
				}
			}

			m_dirty = true;
			return *this;
		}

		template< typename CharT, typename StringT >
		inline const_iterator< CharT, StringT > const_iterator< CharT, StringT >::operator++( int )
		{
			const_iterator< CharT, StringT > temp = *this;
			++( *this );
			return temp;
		}

		template< typename CharT, typename StringT >
		inline const_iterator< CharT, StringT > & const_iterator< CharT, StringT >::operator--()
		{
			--m_it;

			if ( checkFlag( uint8_t( *m_it ), details::FirstBitMask ) ) // This means that the previous byte is not an ASCII character.
			{
				--m_it;

				if ( !checkFlag( uint8_t( *m_it ), details::SecondBitMask ) )
				{
					--m_it;

					if ( !checkFlag( uint8_t( *m_it ), details::SecondBitMask ) )
					{
						--m_it;
					}
				}
			}

			m_dirty = true;
			return *this;
		}

		template< typename CharT, typename StringT >
		inline const_iterator< CharT, StringT > const_iterator< CharT, StringT >::operator--( int )
		{
			const_iterator< CharT, StringT > temp = *this;
			--( *this );
			return temp;
		}

		template< typename CharT, typename StringT >
		inline char32_t const_iterator< CharT, StringT >::operator*()const
		{
			doCalculateCurrentCodePoint();
			return m_lastCodePoint;
		}

		template< typename CharT, typename StringT >
		inline bool const_iterator< CharT, StringT >::operator==( const const_iterator< CharT, StringT > & rhs )const
		{
			return m_it == rhs.m_it;
		}

		template< typename CharT, typename StringT >
		inline bool const_iterator< CharT, StringT >::operator==( const const_iterator::iterator_type & rhs )const
		{
			return m_it == rhs;
		}

		template< typename CharT, typename StringT >
		inline bool const_iterator< CharT, StringT >::operator!=( const const_iterator< CharT, StringT > & rhs )const
		{
			return m_it != rhs.m_it;
		}

		template< typename CharT, typename StringT >
		inline bool const_iterator< CharT, StringT >::operator!=( const typename const_iterator::iterator_type & rhs )const
		{
			return m_it != rhs;
		}

		template< typename CharT, typename StringT >
		inline typename const_iterator< CharT, StringT >::iterator_type const_iterator< CharT, StringT >::internal()const
		{
			return m_it;
		}

		template< typename CharT, typename StringT >
		inline void const_iterator< CharT, StringT >::doCalculateCurrentCodePoint()const
		{
			if ( m_dirty )
			{
				m_lastCodePoint = toUtf8( m_it, m_it );
				m_dirty = false;
			}
		}

		//*************************************************************************************************

		template< typename CharT, typename StringT >
		const_iterator< CharT, StringT >  operator+( const_iterator< CharT, StringT > lhs, size_t rhs )
		{
			const_iterator< CharT, StringT > it( lhs );
			it += rhs;
			return it;
		}

		template< typename CharT, typename StringT >
		const_iterator< CharT, StringT > operator-( const_iterator< CharT, StringT > lhs, size_t rhs )
		{
			const_iterator< CharT, StringT > it( lhs );
			it -= rhs;
			return it;
		}

		//*************************************************************************************************
	}
}
