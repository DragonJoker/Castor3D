namespace Castor
{
	namespace string
	{
		namespace detail
		{
			template< typename T, typename U > struct StringCaster;

			template< typename T >
			struct StringCaster< T, T >
			{
				static std::basic_string< T > const & Cast( std::basic_string< T > const & p_src, std::locale const & p_locale = std::locale() )
				{
					return p_src;
				}
			};

			template<>
			struct StringCaster< char, wchar_t >
			{
				static std::string Cast( std::wstring const & p_src, std::locale const & p_locale = std::locale() )
				{
					std::string l_return;

					if ( !p_src.empty() )
					{
						typedef std::codecvt< wchar_t, char, std::mbstate_t > facet_type;
						typedef facet_type::result result_type;
						const facet_type & l_facet = std::use_facet< facet_type >( p_locale );
						std::mbstate_t l_state = std::mbstate_t();
						std::vector< char > l_dst( p_src.size() * l_facet.max_length(), 0 );
						const wchar_t * l_endSrc = NULL;
						char * l_endDst = NULL;
						l_facet.out( l_state,
									 p_src.data(), p_src.data() + p_src.size(), l_endSrc,
									 &l_dst[0], &l_dst[0] + l_dst.size(), l_endDst
									 );
						l_return = std::string( &l_dst.front(), l_endDst );
					}

					return l_return;
				}
			};

			template<>
			struct StringCaster< wchar_t, char >
			{
				static std::wstring Cast( std::string const & p_src, std::locale const & p_locale = std::locale() )
				{
					std::wstring l_return;

					if ( !p_src.empty() )
					{
						typedef std::codecvt< wchar_t, char, std::mbstate_t > facet_type;
						typedef facet_type::result result_type;
						const facet_type & l_facet = std::use_facet< facet_type >( p_locale );
						std::mbstate_t l_state = std::mbstate_t();
						std::vector< wchar_t > l_dst( p_src.size() * l_facet.max_length(), 0 );
						const char * l_endSrc = NULL;
						wchar_t * l_endDst = NULL;
						l_facet.in( l_state,
									p_src.data(), p_src.data() + p_src.size(), l_endSrc,
									&l_dst[0], &l_dst[0] + l_dst.size(), l_endDst
									);
						l_return = std::wstring( &l_dst.front(), l_endDst );
					}

					return l_return;
				}
			};
		}

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
		std::basic_string< T > string_cast( std::basic_string< U > const & p_src, std::locale const & p_locale )
		{
			return detail::StringCaster< T, U >::Cast( p_src );
		}

		template< typename T, typename U >
		std::basic_string< T > string_cast( U const * p_src, std::locale const & p_locale )
		{
			return detail::StringCaster< T, U >::Cast( p_src );
		}
	};
}
