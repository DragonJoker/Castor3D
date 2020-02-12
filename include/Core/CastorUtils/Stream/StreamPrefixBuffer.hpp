/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_STREAM_PREFIX_BUFFER_H___
#define ___CASTOR_STREAM_PREFIX_BUFFER_H___

#include "CastorUtils/CastorUtilsPrerequisites.hpp"

#include <streambuf>
#include <sstream>
#include <string>

namespace castor
{
	namespace format
	{
		/**
		\author		Sylvain DOREMUS
		\version	0.7.0.0
		\date		02/09/2014
		\~english
		\brief		Stream buffer used by streams to indent lines
		\~french
		\brief		Tampon de flux utilisé pour indenter les lignes
		*/
		template < typename prefix_type, typename char_type, typename traits = std::char_traits< char_type > >
		struct BasicPrefixBuffer
			: public std::basic_streambuf< char_type, traits >
			, public prefix_type
		{
		public:
			typedef typename traits::int_type int_type;
			typedef typename traits::pos_type pos_type;
			typedef typename traits::off_type off_type;

		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\~french
			 *\brief		Constructeur.
			 */
			explicit BasicPrefixBuffer( std::basic_streambuf< char_type, traits > * sbuf )
				: m_sbuf( sbuf )
				, m_set( true )
			{
			}
			/**
			 *\~english
			 *\brief		Destructor.
			 *\~french
			 *\brief		Destructeur.
			 */
			~BasicPrefixBuffer()
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
			bool m_set;
		};
	}
}

#endif
