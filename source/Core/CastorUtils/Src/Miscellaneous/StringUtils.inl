﻿#include <locale>

namespace Castor
{
	namespace string
	{
		//*************************************************************************************************

		namespace
		{
			template< typename InChar, typename OutChar > struct StringConverter;

			template<> struct StringConverter< char, wchar_t >
			{
				static void Convert( std::basic_string< char > const & p_strIn, std::basic_string< wchar_t > & p_strOut, std::locale const & p_locale = std::locale() )
				{
					if ( !p_strIn.empty() )
					{
						typedef std::codecvt< wchar_t, char, std::mbstate_t > facet_type;
						const facet_type & facet = std::use_facet< facet_type >( p_locale );
						std::mbstate_t state = std::mbstate_t();
						std::vector< wchar_t > dst( p_strIn.size() * facet.max_length(), 0 );
						const char * endSrc = nullptr;
						wchar_t * endDst = nullptr;
						facet.in( state,
								  p_strIn.data(), p_strIn.data() + p_strIn.size(), endSrc,
								  &dst[0], &dst[0] + dst.size(), endDst
								);
						p_strOut = std::wstring( &dst.front(), endDst );
					}
				}
			};

			template<> struct StringConverter< wchar_t, char >
			{
				static void Convert( std::basic_string< wchar_t > const & p_strIn, std::basic_string< char > & p_strOut, std::locale const & p_locale = std::locale() )
				{
					if ( !p_strIn.empty() )
					{
						typedef std::codecvt< wchar_t, char, std::mbstate_t > facet_type;
						const facet_type & facet = std::use_facet< facet_type >( p_locale );
						std::mbstate_t state = std::mbstate_t();
						std::vector< char > dst( p_strIn.size() * facet.max_length(), 0 );
						const wchar_t * endSrc = nullptr;
						char * endDst = nullptr;
						facet.out( state,
								   p_strIn.data(), p_strIn.data() + p_strIn.size(), endSrc,
								   &dst[0], &dst[0] + dst.size(), endDst
								 );
						p_strOut = std::string( &dst.front(), endDst );
					}
				}
			};

			template< typename InChar >
			struct StringConverter< InChar, InChar >
			{
				static void Convert( std::basic_string< InChar > const & p_strIn, std::basic_string< InChar > & p_strOut, std::locale const & p_locale = std::locale() )
				{
					p_strOut = p_strIn;
				}
			};
		}

		//*************************************************************************************************

		template< typename T >
		inline void parse( String const & p_str, T & p_val, std::locale const & p_locale )
		{
			StringStream out( p_str );
			out.imbue( p_locale );
			out >> p_val;
		}

		template< typename T >
		inline void extract( String & p_str, T & p_val, std::locale const & p_locale )
		{
			StringStream out( p_str );
			out.imbue( p_locale );
			out >> p_val;
			p_str = out.str();
		}

		template< typename T >
		inline String to_string( T const & p_value, int p_base, std::locale const & p_locale )
		{
			StringStream result;
			result.imbue( p_locale );
			result << manip::xbase( p_base ) << p_value;
			return result.str();
		}

		template< typename T, typename U >
		std::basic_string< T > string_cast( std::basic_string< U > const & p_src )
		{
			std::basic_string< T > result;
			StringConverter< U, T >::Convert( p_src, result );
			return result;
		}

		template< typename T, typename U >
		std::basic_string< T > string_cast( U const * p_src )
		{
			std::basic_string< T > result;

			if ( p_src )
			{
				StringConverter< U, T >::Convert( std::basic_string< U >( p_src ), result );
			}

			return result;
		}

		template< typename T, typename U >
		std::basic_string< T > string_cast( U const * p_begin, U const * p_end )
		{
			std::basic_string< T > result;
			StringConverter< U, T >::Convert( std::basic_string< U >( p_begin, p_end ), result );
			return result;
		}

		template< typename T, typename U >
		std::basic_string< T > string_cast( std::initializer_list< U > const & p_src )
		{
			std::basic_string< T > result;
			StringConverter< U, T >::Convert( std::basic_string< U >( p_src ), result );
			return result;
		}

		namespace utf8
		{
			//*************************************************************************************************

			template< typename IteratorType >
			inline char32_t to_utf8( IteratorType p_value )
			{
				static const unsigned char FirstBitMask = 0x80; // 1000000
				static const unsigned char SecondBitMask = 0x40; // 0100000
				static const unsigned char ThirdBitMask = 0x20; // 0010000
				static const unsigned char FourthBitMask = 0x10; // 0001000
				static const unsigned char FifthBitMask = 0x08; // 0000100

				char32_t result;
				char firstByte = *p_value;

				if ( firstByte & FirstBitMask ) // This means the first byte has a value greater than 127, and so is beyond the ASCII range.
				{
					if ( firstByte & ThirdBitMask ) // This means that the first byte has a value greater than 191, and so it must be at least a three-octet code point.
					{
						if ( firstByte & FourthBitMask ) // This means that the first byte has a value greater than 224, and so it must be a four-octet code point.
						{
							result = ( firstByte & 0x07 ) << 18;
							char secondByte = *( p_value + 1 );
							result += ( secondByte & 0x3f ) << 12;
							char thirdByte = *( p_value + 2 );
							result += ( thirdByte & 0x3f ) << 6;;
							char fourthByte = *( p_value + 3 );
							result += ( fourthByte & 0x3f );
						}
						else
						{
							result = ( firstByte & 0x0f ) << 12;
							char secondByte = *( p_value + 1 );
							result += ( secondByte & 0x3f ) << 6;
							char thirdByte = *( p_value + 2 );
							result += ( thirdByte & 0x3f );
						}
					}
					else
					{
						result = ( firstByte & 0x1f ) << 6;
						char secondByte = *( p_value + 1 );
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

			iterator::iterator( std::string::iterator const & p_it )
				: m_it( p_it )
				, m_lastCodePoint( 0 )
				, m_dirty( true )
			{
			}

			iterator::iterator( iterator const & p_it )
				: m_it( p_it.m_it )
				, m_lastCodePoint( p_it.m_lastCodePoint )
				, m_dirty( p_it.m_dirty )
			{
			}

			iterator::~iterator()
			{
			}

			iterator & iterator::operator=( std::string::iterator const & p_it )
			{
				m_it = p_it;
				m_lastCodePoint = 0;
				m_dirty = true;
				return *this;
			}
			iterator & iterator::operator=( iterator const & p_it )
			{
				m_it = p_it.m_it;
				m_lastCodePoint = p_it.m_lastCodePoint;
				m_dirty = p_it.m_dirty;
				return *this;
			}

			iterator & iterator::operator+=( size_t p_offset )
			{
				while ( p_offset-- )
				{
					++( *this );
				}

				return *this;
			}

			iterator & iterator::operator-=( size_t p_offset )
			{
				while ( p_offset-- )
				{
					--( *this );
				}

				return *this;
			}

			iterator & iterator::operator++()
			{
				static const unsigned char FirstBitMask = 0x80; // 1000000
				static const unsigned char SecondBitMask = 0x40; // 0100000
				static const unsigned char ThirdBitMask = 0x20; // 0010000
				static const unsigned char FourthBitMask = 0x10; // 0001000
				static const unsigned char FifthBitMask = 0x08; // 0000100

				char firstByte = *m_it;
				++m_it;

				if ( firstByte & FirstBitMask ) // This means the first byte has a value greater than 127, and so is beyond the ASCII range.
				{
					++m_it;

					if ( firstByte & ThirdBitMask ) // This means that the first byte has a value greater than 224, and so it must be at least a three-octet code point.
					{
						++m_it;

						if ( firstByte & FourthBitMask ) // This means that the first byte has a value greater than 240, and so it must be a four-octet code point.
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
				static const unsigned char FirstBitMask = 0x80; // 1000000
				static const unsigned char SecondBitMask = 0x40; // 0100000
				static const unsigned char ThirdBitMask = 0x20; // 0010000
				static const unsigned char FourthBitMask = 0x10; // 0001000
				static const unsigned char FifthBitMask = 0x08; // 0000100

				--m_it;

				if ( *m_it & FirstBitMask ) // This means that the previous byte is not an ASCII character.
				{
					--m_it;

					if ( ( *m_it & SecondBitMask ) == 0 )
					{
						--m_it;

						if ( ( *m_it & SecondBitMask ) == 0 )
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
				DoCalculateCurrentCodePoint();
				return m_lastCodePoint;
			}

			bool iterator::operator==( const iterator & p_it )const
			{
				return m_it == p_it.m_it;
			}

			bool iterator::operator==( const std::string::iterator & p_it )const
			{
				return m_it == p_it;
			}

			bool iterator::operator!=( const iterator & p_it )const
			{
				return m_it != p_it.m_it;
			}

			bool iterator::operator!=( const std::string::iterator & p_it )const
			{
				return m_it != p_it;
			}

			std::string::iterator iterator::internal()const
			{
				return m_it;
			}

			void iterator::DoCalculateCurrentCodePoint()const
			{
				if ( m_dirty )
				{
					m_lastCodePoint = to_utf8( m_it );
					m_dirty = false;
				}
			}

			//*************************************************************************************************

			inline iterator operator+( iterator p_it, size_t p_offset )
			{
				iterator it( p_it );
				it += p_offset;
				return it;
			}

			iterator operator-( iterator p_it, size_t p_offset )
			{
				iterator it( p_it );
				it -= p_offset;
				return it;
			}

			//*************************************************************************************************

			const_iterator::const_iterator( std::string::const_iterator const & p_it )
				: m_it( p_it )
				, m_lastCodePoint( 0 )
				, m_dirty( true )
			{
			}

			const_iterator::const_iterator( const_iterator const & p_it )
				: m_it( p_it.m_it )
				, m_lastCodePoint( p_it.m_lastCodePoint )
				, m_dirty( p_it.m_dirty )
			{
			}

			const_iterator::~const_iterator()
			{
			}

			const_iterator & const_iterator::operator=( std::string::const_iterator const & p_it )
			{
				m_it = p_it;
				m_lastCodePoint = 0;
				m_dirty = true;
				return *this;
			}

			const_iterator & const_iterator::operator=( const_iterator const & p_it )
			{
				m_it = p_it.m_it;
				m_lastCodePoint = p_it.m_lastCodePoint;
				m_dirty = p_it.m_dirty;
				return *this;
			}

			const_iterator & const_iterator::operator+=( size_t p_offset )
			{
				while ( p_offset-- )
				{
					++( *this );
				}

				return *this;
			}

			const_iterator & const_iterator::operator-=( size_t p_offset )
			{
				while ( p_offset-- )
				{
					--( *this );
				}

				return *this;
			}

			const_iterator & const_iterator::operator++()
			{
				static const unsigned char FirstBitMask = 0x80; // 1000000
				static const unsigned char SecondBitMask = 0x40; // 0100000
				static const unsigned char ThirdBitMask = 0x20; // 0010000
				static const unsigned char FourthBitMask = 0x10; // 0001000
				static const unsigned char FifthBitMask = 0x08; // 0000100

				char firstByte = *m_it;
				++m_it;

				if ( firstByte & FirstBitMask ) // This means the first byte has a value greater than 127, and so is beyond the ASCII range.
				{
					++m_it;

					if ( firstByte & ThirdBitMask ) // This means that the first byte has a value greater than 224, and so it must be at least a three-octet code point.
					{
						++m_it;

						if ( firstByte & FourthBitMask ) // This means that the first byte has a value greater than 240, and so it must be a four-octet code point.
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
				static const unsigned char FirstBitMask = 0x80; // 1000000
				static const unsigned char SecondBitMask = 0x40; // 0100000
				static const unsigned char ThirdBitMask = 0x20; // 0010000
				static const unsigned char FourthBitMask = 0x10; // 0001000
				static const unsigned char FifthBitMask = 0x08; // 0000100

				--m_it;

				if ( *m_it & FirstBitMask ) // This means that the previous byte is not an ASCII character.
				{
					--m_it;

					if ( ( *m_it & SecondBitMask ) == 0 )
					{
						--m_it;

						if ( ( *m_it & SecondBitMask ) == 0 )
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
				DoCalculateCurrentCodePoint();
				return m_lastCodePoint;
			}

			bool const_iterator::operator==( const const_iterator & p_it )const
			{
				return m_it == p_it.m_it;
			}

			bool const_iterator::operator==( const std::string::const_iterator & p_it )const
			{
				return m_it == p_it;
			}

			bool const_iterator::operator!=( const const_iterator & p_it )const
			{
				return m_it != p_it.m_it;
			}

			bool const_iterator::operator!=( const std::string::const_iterator & p_it )const
			{
				return m_it != p_it;
			}

			std::string::const_iterator const_iterator::internal()const
			{
				return m_it;
			}

			void const_iterator::DoCalculateCurrentCodePoint()const
			{
				if ( m_dirty )
				{
					m_lastCodePoint = to_utf8( m_it );
					m_dirty = false;
				}
			}

			//*************************************************************************************************

			inline const_iterator operator+( const_iterator p_it, size_t p_offset )
			{
				const_iterator it( p_it );
				it += p_offset;
				return it;
			}

			const_iterator operator-( const_iterator p_it, size_t p_offset )
			{
				const_iterator it( p_it );
				it -= p_offset;
				return it;
			}

			//*************************************************************************************************
		}
	}
}
