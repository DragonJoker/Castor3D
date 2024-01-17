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
	template < typename prefix_type, typename char_type, typename traits >
	struct BasicPrefixBuffer
		: public std::basic_streambuf< char_type, traits >
		, public prefix_type
	{
	public:
		using int_type = typename traits::int_type;
		using pos_type = typename traits::pos_type;
		using off_type = typename traits::off_type;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		explicit BasicPrefixBuffer( std::basic_streambuf< char_type, traits > * sbuf )
			: m_sbuf( sbuf )
			, m_set{ true }
		{
		}
		/**
		 *\~english
		 *\brief		Retrieves the stream buffer
		 *\return		The buffer
		 *\~french
		 *\brief		Récupère le tampon du flux
		 *\return		Le tampon
		 */
		std::streambuf * sbuf() const
		{
			return m_sbuf;
		}

	private:
		BasicPrefixBuffer( const BasicPrefixBuffer< prefix_type, char_type, traits > & ) = delete;
		BasicPrefixBuffer< prefix_type, char_type, traits > & operator=( const BasicPrefixBuffer< prefix_type, char_type, traits > & ) = delete;

	private:
		virtual int_type overflow( int_type c = traits::eof() )
		{
			if ( traits::eq_int_type( c, traits::eof() ) )
			{
				return m_sbuf->sputc( static_cast< char_type >( c ) );
			}

			if ( m_set )
			{
				auto Prefix = prefix_type::toString();
				m_sbuf->sputn( Prefix.c_str(), Prefix.size() );
				m_set = false;
			}

			if ( traits::eq_int_type( m_sbuf->sputc( static_cast< char_type >( c ) ), traits::eof() ) )
			{
				return traits::eof();
			}

			if ( traits::eq_int_type( c, traits::to_char_type( '\n' ) ) )
			{
				m_set = true;
			}

			return traits::not_eof( c );
		}

	private:
		//!\~english The internal stream buffer.	\~french Le tampon interne.
		std::basic_streambuf< char_type, traits > * m_sbuf;
		//!\~english Tells the stream it to Prefix.	\~french Dit si le flux doit être préfixé.
		bool m_set{};
	};
}

#endif
