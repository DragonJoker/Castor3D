#include "CastorUtils/Design/FlagCombination.hpp"

#include "CastorUtils/Config/BeginExternHeaderGuard.hpp"
#include <locale>
#include "CastorUtils/Config/EndExternHeaderGuard.hpp"

namespace castor::string
{
	//*************************************************************************************************

	namespace details
	{
		template< typename InChar, typename OutChar > struct StringConverter;

		template<> struct StringConverter< char, wchar_t >
		{
			static void convert( std::basic_string_view< char > strIn
				, std::basic_string< wchar_t > & strOut
				, std::locale const & locale = std::locale( "C" ) )
			{
				if ( !strIn.empty() )
				{
					using facet_type = std::codecvt< wchar_t, char, std::mbstate_t >;
					auto const & facet = std::use_facet< facet_type >( locale );
					std::mbstate_t state{};
					std::vector< wchar_t > dst( strIn.size() * facet.max_length(), 0 );
					const char * endSrc = nullptr;
					wchar_t * endDst = nullptr;
					facet.in( state
						, strIn.data(), strIn.data() + strIn.size(), endSrc
						, &dst[0], &dst[0] + dst.size(), endDst );
					strOut = std::wstring( &dst.front(), endDst );
				}
			}
		};

		template<> struct StringConverter< char, char32_t >
		{
			static void convert( std::basic_string_view< char > strIn
				, std::basic_string< char32_t > & strOut
				, std::locale const & locale = std::locale( "C" ) )
			{
				if ( !strIn.empty() )
				{
					using Char8CPtr = char8_t const *;
					using facet_type = std::codecvt< char32_t, char8_t, std::mbstate_t >;
					auto const & facet = std::use_facet< facet_type >( locale );
					std::mbstate_t state{};
					std::vector< char32_t > dst( strIn.size() * facet.max_length(), 0 );
					const char8_t * endSrc = nullptr;
					char32_t * endDst = nullptr;
					facet.in( state
						, Char8CPtr( strIn.data() )
						, Char8CPtr( strIn.data() ) + strIn.size()
						, endSrc
						, &dst[0], &dst[0] + dst.size(), endDst );
					strOut = std::u32string( &dst.front(), endDst );
				}
			}
		};

		template<> struct StringConverter< wchar_t, char >
		{
			static void convert( std::basic_string_view< wchar_t > strIn
				, std::basic_string< char > & strOut
				, std::locale const & locale = std::locale() )
			{
				if ( !strIn.empty() )
				{
					using facet_type = std::codecvt< wchar_t, char, std::mbstate_t >;
					auto const & facet = std::use_facet< facet_type >( locale );
					std::mbstate_t state{};
					std::vector< char > dst( strIn.size() * facet.max_length(), 0 );
					const wchar_t * endSrc = nullptr;
					char * endDst = nullptr;
					facet.out( state,
						strIn.data(), strIn.data() + strIn.size(), endSrc
						, &dst[0], &dst[0] + dst.size(), endDst );
					strOut = std::string( &dst.front(), endDst );
				}
			}
		};

		template<> struct StringConverter< char32_t, char >
		{
			static void convert( std::basic_string_view< char32_t > strIn
				, std::basic_string< char > & strOut
				, std::locale const & locale = std::locale() )
			{
				if ( !strIn.empty() )
				{
					using Char8Ptr = char8_t *;
					using CharPtr = char *;
					using facet_type = std::codecvt< char32_t, char8_t, std::mbstate_t >;
					auto const & facet = std::use_facet< facet_type >( locale );
					std::mbstate_t state{};
					std::vector< char > dst( strIn.size() * facet.max_length(), 0 );
					const char32_t * endSrc = nullptr;
					char8_t * endDst = nullptr;
					facet.out( state,
						strIn.data(), strIn.data() + strIn.size(), endSrc
							, Char8Ptr( &dst[0] )
							, Char8Ptr( &dst[0] ) + dst.size()
							, endDst );
					strOut = std::string( &dst.front(), CharPtr( endDst ) );
				}
			}
		};

		template< typename InChar >
		struct StringConverter< InChar, InChar >
		{
			static void convert( std::basic_string_view< InChar > strIn
				, std::basic_string< InChar > & strOut
				, CU_UnusedParam( std::locale const &, locale ) = std::locale() )
			{
				strOut = strIn;
			}
		};
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
	inline U32String toU32String( T const & value, std::locale const & locale )
	{
		return toU32String( toString( value, locale ) );
	}

	template< typename T, typename U >
	std::basic_string< T > stringCast( std::basic_string< U > const & src )
	{
		std::basic_string< T > result;
		details::StringConverter< U, T >::convert( src, result );
		return result;
	}

	template< typename T, typename U >
	std::basic_string< T > stringCast( U const * src )
	{
		std::basic_string< T > result;

		if ( src )
		{
			details::StringConverter< U, T >::convert( std::basic_string< U >( src ), result );
		}

		return result;
	}

	template< typename T, typename U >
	std::basic_string< T > stringCast( U const * begin, U const * end )
	{
		std::basic_string< T > result;
		details::StringConverter< U, T >::convert( std::basic_string< U >( begin, end ), result );
		return result;
	}

	template< typename T, typename U >
	std::basic_string< T > stringCast( std::initializer_list< U > const & src )
	{
		std::basic_string< T > result;
		details::StringConverter< U, T >::convert( std::basic_string< U >( src ), result );
		return result;
	}

	namespace utf8
	{
		//*************************************************************************************************

		namespace details
		{
			static constexpr std::byte FirstBitMask = std::byte( 0x80 );
			static constexpr std::byte SecondBitMask = std::byte( 0x40 );
			static constexpr std::byte ThirdBitMask = std::byte( 0x20 );
			static constexpr std::byte FourthBitMask = std::byte( 0x10 );
			static constexpr std::byte FifthBitMask = std::byte( 0x08 );
		}

		template< typename IteratorType >
		inline char32_t toUtf8( IteratorType value )
		{
			char32_t result;

			if ( auto firstByte = std::byte( *value );
				checkFlag( firstByte, details::FirstBitMask ) ) // This means the first byte has a value greater than 127, and so is beyond the ASCII range.
			{
				if ( checkFlag( firstByte, details::ThirdBitMask ) ) // This means that the first byte has a value greater than 191, and so it must be at least a three-octet code point.
				{
					if ( checkFlag( firstByte, details::FourthBitMask ) ) // This means that the first byte has a value greater than 224, and so it must be a four-octet code point.
					{
						result = char32_t( ( firstByte & std::byte( 0x07 ) ) << 18 );
						auto secondByte = std::byte( *( value + 1 ) );
						result += char32_t( ( secondByte & std::byte( 0x3f ) ) << 12 );
						auto thirdByte = std::byte( *( value + 2 ) );
						result += char32_t( ( thirdByte & std::byte( 0x3f ) ) << 6 );
						auto fourthByte = std::byte( *( value + 3 ) );
						result += char32_t( fourthByte & std::byte( 0x3f ) );
					}
					else
					{
						result = char32_t( ( firstByte & std::byte( 0x0f ) ) << 12 );
						auto secondByte = std::byte( *( value + 1 ) );
						result += char32_t( ( secondByte & std::byte( 0x3f ) ) << 6 );
						auto thirdByte = std::byte( *( value + 2 ) );
						result += char32_t( thirdByte & std::byte( 0x3f ) );
					}
				}
				else
				{
					result = char32_t( ( firstByte & std::byte( 0x1f ) ) << 6 );
					auto secondByte = std::byte( *( value + 1 ) );
					result += char32_t( secondByte & std::byte( 0x3f ) );
				}
			}
			else
			{
				result = char32_t( firstByte );
			}

			return result;
		}

		//*************************************************************************************************

		inline iterator::iterator( std::string::iterator const & rhs )
			: m_it( rhs )
			, m_lastCodePoint( 0 )
			, m_dirty( true )
		{
		}

		inline iterator::iterator( iterator const & rhs )
			: m_it( rhs.m_it )
			, m_lastCodePoint( rhs.m_lastCodePoint )
			, m_dirty( rhs.m_dirty )
		{
		}

		inline iterator::iterator( iterator && rhs )noexcept
			: m_it( rhs.m_it )
			, m_lastCodePoint( rhs.m_lastCodePoint )
			, m_dirty( rhs.m_dirty )
		{
		}

		inline iterator & iterator::operator=( std::string::iterator const & rhs )
		{
			m_it = rhs;
			m_lastCodePoint = 0;
			m_dirty = true;
			return *this;
		}

		inline iterator & iterator::operator=( iterator const & rhs )
		{
			m_it = rhs.m_it;
			m_lastCodePoint = rhs.m_lastCodePoint;
			m_dirty = rhs.m_dirty;
			return *this;
		}

		inline iterator & iterator::operator=( iterator && rhs )noexcept
		{
			m_it = rhs.m_it;
			m_lastCodePoint = rhs.m_lastCodePoint;
			m_dirty = rhs.m_dirty;
			return *this;
		}

		inline iterator & iterator::operator+=( size_t rhs )
		{
			while ( rhs-- )
			{
				++( *this );
			}

			return *this;
		}

		inline iterator & iterator::operator-=( size_t rhs )
		{
			while ( rhs-- )
			{
				--( *this );
			}

			return *this;
		}

		inline iterator & iterator::operator++()
		{
			auto firstByte = std::byte( *m_it );
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

		inline iterator iterator::operator++( int )
		{
			iterator temp = *this;
			++( *this );
			return temp;
		}

		inline iterator & iterator::operator--()
		{
			--m_it;

			if ( checkFlag( std::byte( *m_it ), details::FirstBitMask ) ) // This means that the previous byte is not an ASCII character.
			{
				--m_it;

				if ( !checkFlag( std::byte( *m_it ), details::SecondBitMask ) )
				{
					--m_it;

					if ( !checkFlag( std::byte( *m_it ), details::SecondBitMask ) )
					{
						--m_it;
					}
				}
			}

			m_dirty = true;
			return *this;
		}

		inline iterator iterator::operator--( int )
		{
			iterator temp = *this;
			--( *this );
			return temp;
		}

		inline char32_t iterator::operator*()const
		{
			doCalculateCurrentCodePoint();
			return m_lastCodePoint;
		}

		inline bool iterator::operator==( const iterator & rhs )const
		{
			return m_it == rhs.m_it;
		}

		inline bool iterator::operator==( const std::string::iterator & rhs )const
		{
			return m_it == rhs;
		}

		inline bool iterator::operator!=( const iterator & rhs )const
		{
			return m_it != rhs.m_it;
		}

		inline bool iterator::operator!=( const std::string::iterator & rhs )const
		{
			return m_it != rhs;
		}

		inline std::string::iterator iterator::internal()const
		{
			return m_it;
		}

		inline void iterator::doCalculateCurrentCodePoint()const
		{
			if ( m_dirty )
			{
				m_lastCodePoint = toUtf8( m_it );
				m_dirty = false;
			}
		}

		//*************************************************************************************************

		iterator operator+( iterator lhs, size_t rhs )
		{
			iterator it( lhs );
			it += rhs;
			return it;
		}

		iterator operator-( iterator lhs, size_t rhs )
		{
			iterator it( lhs );
			it -= rhs;
			return it;
		}

		//*************************************************************************************************

		inline const_iterator::const_iterator( std::string::const_iterator const & rhs )
			: m_it( rhs )
			, m_lastCodePoint( 0 )
			, m_dirty( true )
		{
		}

		inline const_iterator::const_iterator( const_iterator const & rhs )
			: m_it( rhs.m_it )
			, m_lastCodePoint( rhs.m_lastCodePoint )
			, m_dirty( rhs.m_dirty )
		{
		}

		inline const_iterator::const_iterator( const_iterator && rhs )noexcept
			: m_it( rhs.m_it )
			, m_lastCodePoint( rhs.m_lastCodePoint )
			, m_dirty( rhs.m_dirty )
		{
		}

		inline const_iterator & const_iterator::operator=( std::string::const_iterator const & rhs )
		{
			m_it = rhs;
			m_lastCodePoint = 0;
			m_dirty = true;
			return *this;
		}

		inline const_iterator & const_iterator::operator=( const_iterator const & rhs )
		{
			m_it = rhs.m_it;
			m_lastCodePoint = rhs.m_lastCodePoint;
			m_dirty = rhs.m_dirty;
			return *this;
		}

		inline const_iterator & const_iterator::operator=( const_iterator && rhs )noexcept
		{
			m_it = rhs.m_it;
			m_lastCodePoint = rhs.m_lastCodePoint;
			m_dirty = rhs.m_dirty;
			return *this;
		}

		inline const_iterator & const_iterator::operator+=( size_t rhs )
		{
			while ( rhs-- )
			{
				++( *this );
			}

			return *this;
		}

		inline const_iterator & const_iterator::operator-=( size_t rhs )
		{
			while ( rhs-- )
			{
				--( *this );
			}

			return *this;
		}

		inline const_iterator & const_iterator::operator++()
		{
			auto firstByte = std::byte( *m_it );
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

		inline const_iterator const_iterator::operator++( int )
		{
			const_iterator temp = *this;
			++( *this );
			return temp;
		}

		inline const_iterator & const_iterator::operator--()
		{
			--m_it;

			if ( checkFlag( std::byte( *m_it ), details::FirstBitMask ) ) // This means that the previous byte is not an ASCII character.
			{
				--m_it;

				if ( !checkFlag( std::byte( *m_it ), details::SecondBitMask ) )
				{
					--m_it;

					if ( !checkFlag( std::byte( *m_it ), details::SecondBitMask ) )
					{
						--m_it;
					}
				}
			}

			m_dirty = true;
			return *this;
		}

		inline const_iterator const_iterator::operator--( int )
		{
			const_iterator temp = *this;
			--( *this );
			return temp;
		}

		inline char32_t const_iterator::operator*()const
		{
			doCalculateCurrentCodePoint();
			return m_lastCodePoint;
		}

		inline bool const_iterator::operator==( const const_iterator & rhs )const
		{
			return m_it == rhs.m_it;
		}

		inline bool const_iterator::operator==( const std::string::const_iterator & rhs )const
		{
			return m_it == rhs;
		}

		inline bool const_iterator::operator!=( const const_iterator & rhs )const
		{
			return m_it != rhs.m_it;
		}

		inline bool const_iterator::operator!=( const std::string::const_iterator & rhs )const
		{
			return m_it != rhs;
		}

		inline std::string::const_iterator const_iterator::internal()const
		{
			return m_it;
		}

		inline void const_iterator::doCalculateCurrentCodePoint()const
		{
			if ( m_dirty )
			{
				m_lastCodePoint = toUtf8( m_it );
				m_dirty = false;
			}
		}

		//*************************************************************************************************

		const_iterator operator+( const_iterator lhs, size_t rhs )
		{
			const_iterator it( lhs );
			it += rhs;
			return it;
		}

		const_iterator operator-( const_iterator lhs, size_t rhs )
		{
			const_iterator it( lhs );
			it -= rhs;
			return it;
		}

		//*************************************************************************************************
	}
}
