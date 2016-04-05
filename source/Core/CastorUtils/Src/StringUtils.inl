#include <locale>

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
			StringStream l_out( p_str );
			l_out.imbue( p_locale );
			l_out >> p_val;
		}

		template< typename T >
		inline void extract( String & p_str, T & p_val, std::locale const & p_locale )
		{
			StringStream l_out( p_str );
			l_out.imbue( p_locale );
			l_out >> p_val;
			p_str = l_out.str();
		}

		template< typename T >
		inline String to_string( T const & p_value, int p_base, std::locale const & p_locale )
		{
			StringStream l_return;
			l_return.imbue( p_locale );
			l_return << manip::xbase( p_base ) << p_value;
			return l_return.str();
		}

		template< typename T, typename U >
		std::basic_string< T > string_cast( std::basic_string< U > const & p_src )
		{
			std::basic_string< T > l_return;
			StringConverter< U, T >::Convert( p_src, l_return );
			return l_return;
		}

		template< typename T, typename U >
		std::basic_string< T > string_cast( U const * p_src )
		{
			std::basic_string< T > l_return;
			StringConverter< U, T >::Convert( std::basic_string< U >( p_src ), l_return );
			return l_return;
		}

		template< typename T, typename U >
		std::basic_string< T > string_cast( U const * p_begin, U const * p_end )
		{
			std::basic_string< T > l_return;
			StringConverter< U, T >::Convert( std::basic_string< U >( p_begin, p_end ), l_return );
			return l_return;
		}

		template< typename T, typename U >
		std::basic_string< T > string_cast( std::initializer_list< U > const & p_src )
		{
			std::basic_string< T > l_return;
			StringConverter< U, T >::Convert( std::basic_string< U >( p_src ), l_return );
			return l_return;
		}

		namespace utf8
		{
			//*************************************************************************************************

			template< typename IteratorType >
			inline char32_t to_utf8( IteratorType p_value )
			{
				char32_t l_return;
				char l_firstByte = *p_value;

				if ( l_firstByte & FirstBitMask ) // This means the first byte has a value greater than 127, and so is beyond the ASCII range.
				{
					if ( l_firstByte & ThirdBitMask ) // This means that the first byte has a value greater than 191, and so it must be at least a three-octet code point.
					{
						if ( l_firstByte & FourthBitMask ) // This means that the first byte has a value greater than 224, and so it must be a four-octet code point.
						{
							l_return = ( l_firstByte & 0x07 ) << 18;
							char l_secondByte = *( p_value + 1 );
							l_return += ( l_secondByte & 0x3f ) << 12;
							char l_thirdByte = *( p_value + 2 );
							l_return += ( l_thirdByte & 0x3f ) << 6;;
							char l_fourthByte = *( p_value + 3 );
							l_return += ( l_fourthByte & 0x3f );
						}
						else
						{
							l_return = ( l_firstByte & 0x0f ) << 12;
							char l_secondByte = *( p_value + 1 );
							l_return += ( l_secondByte & 0x3f ) << 6;
							char l_thirdByte = *( p_value + 2 );
							l_return += ( l_thirdByte & 0x3f );
						}
					}
					else
					{
						l_return = ( l_firstByte & 0x1f ) << 6;
						char l_secondByte = *( p_value + 1 );
						l_return += ( l_secondByte & 0x3f );
					}
				}
				else
				{
					l_return = l_firstByte;
				}

				return l_return;
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
				char l_firstByte = *m_it;
				++m_it;

				if ( l_firstByte & FirstBitMask ) // This means the first byte has a value greater than 127, and so is beyond the ASCII range.
				{
					++m_it;

					if ( l_firstByte & ThirdBitMask ) // This means that the first byte has a value greater than 224, and so it must be at least a three-octet code point.
					{
						++m_it;

						if ( l_firstByte & FourthBitMask ) // This means that the first byte has a value greater than 240, and so it must be a four-octet code point.
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
				iterator l_it( p_it );
				l_it += p_offset;
				return l_it;
			}

			iterator operator-( iterator p_it, size_t p_offset )
			{
				iterator l_it( p_it );
				l_it -= p_offset;
				return l_it;
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
				char l_firstByte = *m_it;
				++m_it;

				if ( l_firstByte & FirstBitMask ) // This means the first byte has a value greater than 127, and so is beyond the ASCII range.
				{
					++m_it;

					if ( l_firstByte & ThirdBitMask ) // This means that the first byte has a value greater than 224, and so it must be at least a three-octet code point.
					{
						++m_it;

						if ( l_firstByte & FourthBitMask ) // This means that the first byte has a value greater than 240, and so it must be a four-octet code point.
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
				const_iterator l_it( p_it );
				l_it += p_offset;
				return l_it;
			}

			const_iterator operator-( const_iterator p_it, size_t p_offset )
			{
				const_iterator l_it( p_it );
				l_it -= p_offset;
				return l_it;
			}

			//*************************************************************************************************
		}
	}
}
