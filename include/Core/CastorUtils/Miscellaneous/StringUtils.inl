#include <locale>

namespace castor
{
	namespace string
	{
		//*************************************************************************************************

		namespace details
		{
			template< typename InChar, typename OutChar > struct StringConverter;

			template<> struct StringConverter< char, wchar_t >
			{
				static void convert( std::basic_string< char > const & strIn
					, std::basic_string< wchar_t > & strOut
					, std::locale const & locale = std::locale( "C" ) )
				{
					if ( !strIn.empty() )
					{
						typedef std::codecvt< wchar_t, char, std::mbstate_t > facet_type;
						const facet_type & facet = std::use_facet< facet_type >( locale );
						std::mbstate_t state = std::mbstate_t();
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

			template<> struct StringConverter< wchar_t, char >
			{
				static void convert( std::basic_string< wchar_t > const & strIn
					, std::basic_string< char > & strOut
					, std::locale const & locale = std::locale() )
				{
					if ( !strIn.empty() )
					{
						typedef std::codecvt< wchar_t, char, std::mbstate_t > facet_type;
						const facet_type & facet = std::use_facet< facet_type >( locale );
						std::mbstate_t state = std::mbstate_t();
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

			template< typename InChar >
			struct StringConverter< InChar, InChar >
			{
				static void convert( std::basic_string< InChar > const & strIn
					, std::basic_string< InChar > & strOut
					, std::locale const & CU_UnusedParam( locale ) = std::locale() )
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
				static constexpr unsigned char FirstBitMask = 0x80;
				static constexpr unsigned char SecondBitMask = 0x40;
				static constexpr unsigned char ThirdBitMask = 0x20;
				static constexpr unsigned char FourthBitMask = 0x10;
				static constexpr unsigned char FifthBitMask = 0x08;
			}

			template< typename IteratorType >
			inline char32_t toUtf8( IteratorType value )
			{
				char32_t result;
				char firstByte = *value;

				if ( firstByte & details::FirstBitMask ) // This means the first byte has a value greater than 127, and so is beyond the ASCII range.
				{
					if ( firstByte & details::ThirdBitMask ) // This means that the first byte has a value greater than 191, and so it must be at least a three-octet code point.
					{
						if ( firstByte & details::FourthBitMask ) // This means that the first byte has a value greater than 224, and so it must be a four-octet code point.
						{
							result = ( firstByte & 0x07 ) << 18;
							char secondByte = *( value + 1 );
							result += ( secondByte & 0x3f ) << 12;
							char thirdByte = *( value + 2 );
							result += ( thirdByte & 0x3f ) << 6;;
							char fourthByte = *( value + 3 );
							result += ( fourthByte & 0x3f );
						}
						else
						{
							result = ( firstByte & 0x0f ) << 12;
							char secondByte = *( value + 1 );
							result += ( secondByte & 0x3f ) << 6;
							char thirdByte = *( value + 2 );
							result += ( thirdByte & 0x3f );
						}
					}
					else
					{
						result = ( firstByte & 0x1f ) << 6;
						char secondByte = *( value + 1 );
						result += ( secondByte & 0x3f );
					}
				}
				else
				{
					result = firstByte;
				}

				return result;
			}

			//*************************************************************************************************

			iterator::iterator( std::string::iterator const & rhs )
				: m_it( rhs )
				, m_lastCodePoint( 0 )
				, m_dirty( true )
			{
			}

			iterator::iterator( iterator const & rhs )
				: m_it( rhs.m_it )
				, m_lastCodePoint( rhs.m_lastCodePoint )
				, m_dirty( rhs.m_dirty )
			{
			}

			iterator::~iterator()
			{
			}

			iterator & iterator::operator=( std::string::iterator const & rhs )
			{
				m_it = rhs;
				m_lastCodePoint = 0;
				m_dirty = true;
				return *this;
			}
			iterator & iterator::operator=( iterator const & rhs )
			{
				m_it = rhs.m_it;
				m_lastCodePoint = rhs.m_lastCodePoint;
				m_dirty = rhs.m_dirty;
				return *this;
			}

			iterator & iterator::operator+=( size_t rhs )
			{
				while ( rhs-- )
				{
					++( *this );
				}

				return *this;
			}

			iterator & iterator::operator-=( size_t rhs )
			{
				while ( rhs-- )
				{
					--( *this );
				}

				return *this;
			}

			iterator & iterator::operator++()
			{
				char firstByte = *m_it;
				++m_it;

				if ( firstByte & details::FirstBitMask ) // This means the first byte has a value greater than 127, and so is beyond the ASCII range.
				{
					++m_it;

					if ( firstByte & details::ThirdBitMask ) // This means that the first byte has a value greater than 224, and so it must be at least a three-octet code point.
					{
						++m_it;

						if ( firstByte & details::FourthBitMask ) // This means that the first byte has a value greater than 240, and so it must be a four-octet code point.
						{
							++m_it;
						}
					}
				}

				m_dirty = true;
				return *this;
			}

			iterator iterator::operator++( int )
			{
				iterator temp = *this;
				++( *this );
				return temp;
			}

			iterator & iterator::operator--()
			{
				--m_it;

				if ( *m_it & details::FirstBitMask ) // This means that the previous byte is not an ASCII character.
				{
					--m_it;

					if ( ( *m_it & details::SecondBitMask ) == 0 )
					{
						--m_it;

						if ( ( *m_it & details::SecondBitMask ) == 0 )
						{
							--m_it;
						}
					}
				}

				m_dirty = true;
				return *this;
			}

			iterator iterator::operator--( int )
			{
				iterator temp = *this;
				--( *this );
				return temp;
			}

			char32_t iterator::operator*()const
			{
				doCalculateCurrentCodePoint();
				return m_lastCodePoint;
			}

			bool iterator::operator==( const iterator & rhs )const
			{
				return m_it == rhs.m_it;
			}

			bool iterator::operator==( const std::string::iterator & rhs )const
			{
				return m_it == rhs;
			}

			bool iterator::operator!=( const iterator & rhs )const
			{
				return m_it != rhs.m_it;
			}

			bool iterator::operator!=( const std::string::iterator & rhs )const
			{
				return m_it != rhs;
			}

			std::string::iterator iterator::internal()const
			{
				return m_it;
			}

			void iterator::doCalculateCurrentCodePoint()const
			{
				if ( m_dirty )
				{
					m_lastCodePoint = toUtf8( m_it );
					m_dirty = false;
				}
			}

			//*************************************************************************************************

			inline iterator operator+( iterator lhs, size_t rhs )
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

			const_iterator::const_iterator( std::string::const_iterator const & rhs )
				: m_it( rhs )
				, m_lastCodePoint( 0 )
				, m_dirty( true )
			{
			}

			const_iterator::const_iterator( const_iterator const & rhs )
				: m_it( rhs.m_it )
				, m_lastCodePoint( rhs.m_lastCodePoint )
				, m_dirty( rhs.m_dirty )
			{
			}

			const_iterator::~const_iterator()
			{
			}

			const_iterator & const_iterator::operator=( std::string::const_iterator const & rhs )
			{
				m_it = rhs;
				m_lastCodePoint = 0;
				m_dirty = true;
				return *this;
			}

			const_iterator & const_iterator::operator=( const_iterator const & rhs )
			{
				m_it = rhs.m_it;
				m_lastCodePoint = rhs.m_lastCodePoint;
				m_dirty = rhs.m_dirty;
				return *this;
			}

			const_iterator & const_iterator::operator+=( size_t rhs )
			{
				while ( rhs-- )
				{
					++( *this );
				}

				return *this;
			}

			const_iterator & const_iterator::operator-=( size_t rhs )
			{
				while ( rhs-- )
				{
					--( *this );
				}

				return *this;
			}

			const_iterator & const_iterator::operator++()
			{
				char firstByte = *m_it;
				++m_it;

				if ( firstByte & details::FirstBitMask ) // This means the first byte has a value greater than 127, and so is beyond the ASCII range.
				{
					++m_it;

					if ( firstByte & details::ThirdBitMask ) // This means that the first byte has a value greater than 224, and so it must be at least a three-octet code point.
					{
						++m_it;

						if ( firstByte & details::FourthBitMask ) // This means that the first byte has a value greater than 240, and so it must be a four-octet code point.
						{
							++m_it;
						}
					}
				}

				m_dirty = true;
				return *this;
			}

			const_iterator const_iterator::operator++( int )
			{
				const_iterator temp = *this;
				++( *this );
				return temp;
			}

			const_iterator & const_iterator::operator--()
			{
				--m_it;

				if ( *m_it & details::FirstBitMask ) // This means that the previous byte is not an ASCII character.
				{
					--m_it;

					if ( ( *m_it & details::SecondBitMask ) == 0 )
					{
						--m_it;

						if ( ( *m_it & details::SecondBitMask ) == 0 )
						{
							--m_it;
						}
					}
				}

				m_dirty = true;
				return *this;
			}

			const_iterator const_iterator::operator--( int )
			{
				const_iterator temp = *this;
				--( *this );
				return temp;
			}

			char32_t const_iterator::operator*()const
			{
				doCalculateCurrentCodePoint();
				return m_lastCodePoint;
			}

			bool const_iterator::operator==( const const_iterator & rhs )const
			{
				return m_it == rhs.m_it;
			}

			bool const_iterator::operator==( const std::string::const_iterator & rhs )const
			{
				return m_it == rhs;
			}

			bool const_iterator::operator!=( const const_iterator & rhs )const
			{
				return m_it != rhs.m_it;
			}

			bool const_iterator::operator!=( const std::string::const_iterator & rhs )const
			{
				return m_it != rhs;
			}

			std::string::const_iterator const_iterator::internal()const
			{
				return m_it;
			}

			void const_iterator::doCalculateCurrentCodePoint()const
			{
				if ( m_dirty )
				{
					m_lastCodePoint = toUtf8( m_it );
					m_dirty = false;
				}
			}

			//*************************************************************************************************

			inline const_iterator operator+( const_iterator lhs, size_t rhs )
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
}
