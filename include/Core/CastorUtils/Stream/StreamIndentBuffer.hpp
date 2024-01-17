/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_STREAM_INDENT_BUFFER_H___
#define ___CASTOR_STREAM_INDENT_BUFFER_H___

#include "CastorUtils/Stream/StreamModule.hpp"

#include "CastorUtils/Config/BeginExternHeaderGuard.hpp"
#include <streambuf>
#include "CastorUtils/Config/EndExternHeaderGuard.hpp"

namespace castor::format
{
	template < typename char_type, char_type fill_char, typename traits >
	struct BasicIndentBuffer
		: public std::basic_streambuf< char_type, traits >
	{
	public:
		using int_type = typename traits::int_type;
		using pos_type = typename traits::pos_type;
		using off_type = typename traits::off_type;

	private:
		BasicIndentBuffer( BasicIndentBuffer const & ) = delete;
		BasicIndentBuffer & operator=( BasicIndentBuffer const & ) = delete;
		BasicIndentBuffer( BasicIndentBuffer && )noexcept = delete;
		BasicIndentBuffer & operator=( BasicIndentBuffer && )noexcept = delete;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		explicit BasicIndentBuffer( std::basic_streambuf< char_type, traits > * sbuf )
			: m_sbuf( sbuf )
			, m_set{ true }
		{
		}
		/**
		 *\~english
		 *\brief		Retrieves current indentation value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la valeur d'indentation courante
		 *\return		La valeur
		 */
		int indent()const
		{
			return m_count;
		}
		/**
		 *\~english
		 *\brief		Defines the indentation value
		 *\param[in]	i	The new value
		 *\~french
		 *\brief		Définit la valeur d'indentation
		 *\param[in]	i	La nouvelle valeur
		 */
		void indent( int i )
		{
			m_count = i;
		}
		/**
		 *\~english
		 *\brief		Retrieves the stream buffer
		 *\return		The buffer
		 *\~french
		 *\brief		Récupère le tampon du flux
		 *\return		Le tampon
		 */
		std::streambuf * sbuf()const
		{
			return m_sbuf;
		}

	private:
		virtual int_type overflow( int_type c = traits::eof() )
		{
			if ( traits::eq_int_type( c, traits::eof() ) )
			{
				return m_sbuf->sputc( static_cast< char_type >( c ) );
			}

			if ( m_set )
			{
				std::fill_n( std::ostreambuf_iterator< char_type >( m_sbuf ), m_count, fill_char );
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
		//!\~english The internal stream buffer	\~french Le tampon interne
		std::basic_streambuf< char_type, traits > * m_sbuf;
		//!\~english The indentation value	\~french La valeur d'indentation
		int m_count{};
		//!\~english Tells the stream has been indented	\~french Dit si le flux a été indenté
		bool m_set{};
	};
}

#endif
