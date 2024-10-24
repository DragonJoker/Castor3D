/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_STREAM_PREFIX_BUFFER_H___
#define ___CASTOR_STREAM_PREFIX_BUFFER_H___

#include "CastorUtils/Stream/StreamModule.hpp"

#include "CastorUtils/Config/BeginExternHeaderGuard.hpp"
#include <streambuf>
#include <sstream>
#include "CastorUtils/Config/EndExternHeaderGuard.hpp"

#include <string>

namespace castor::format
{
	template< typename PrefixT, typename CharT, typename TraitsT >
	struct BasicPrefixBufferT
		: public std::basic_streambuf< CharT, TraitsT >
		, public PrefixT
	{
	public:
		using traits_type = TraitsT;
		using string_type = std::basic_string< CharT, TraitsT >;
		using ostream_type = std::basic_ostream< CharT, TraitsT >;
		using streambuf_type = std::basic_streambuf< CharT, TraitsT >;
		using int_type = typename TraitsT::int_type;
		using pos_type = typename TraitsT::pos_type;
		using off_type = typename TraitsT::off_type;

	public:
		explicit BasicPrefixBufferT( ostream_type & stream )
			: m_stream{ stream }
		{
			m_old = m_stream.rdbuf( this );
		}

		~BasicPrefixBufferT()noexcept override
		{
			try
			{
				m_stream.rdbuf( m_old );
			}
			catch ( ... )
			{
				// Nothing to do here...
			}
		}

	private:
		BasicPrefixBufferT( const BasicPrefixBufferT< PrefixT, CharT, TraitsT > & ) = delete;
		BasicPrefixBufferT( BasicPrefixBufferT< PrefixT, CharT, TraitsT > && ) = delete;
		BasicPrefixBufferT< PrefixT, CharT, TraitsT > & operator=( const BasicPrefixBufferT< PrefixT, CharT, TraitsT > & ) = delete;
		BasicPrefixBufferT< PrefixT, CharT, TraitsT > & operator=( BasicPrefixBufferT< PrefixT, CharT, TraitsT > && ) = delete;

	private:
		int_type overflow( int_type c = TraitsT::eof() )override
		{
			if ( TraitsT::eq_int_type( c, TraitsT::eof() ) )
			{
				return m_old->sputc( static_cast< CharT >( c ) );
			}

			if ( m_set )
			{
				auto prefix = PrefixT::toString();
				m_old->sputn( prefix.c_str(), std::streamsize( prefix.size() ) );
				m_set = false;
			}

			if ( TraitsT::eq_int_type( m_old->sputc( static_cast< CharT >( c ) ), TraitsT::eof() ) )
			{
				return TraitsT::eof();
			}

			if ( TraitsT::eq_int_type( c, TraitsT::to_char_type( '\n' ) ) )
			{
				m_set = true;
			}

			return TraitsT::not_eof( c );
		}

	private:
		ostream_type & m_stream;
		streambuf_type * m_old{};
		bool m_set{ true };
	};
}

#endif
