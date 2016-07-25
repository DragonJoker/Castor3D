/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___CASTOR_STREAM_PREFIX_BUFFER_H___
#define ___CASTOR_STREAM_PREFIX_BUFFER_H___

#include "CastorUtilsPrerequisites.hpp"

#include <streambuf>
#include <sstream>
#include <string>

namespace Castor
{
	namespace format
	{
		/*!
		\author		Sylvain DOREMUS
		\version	0.7.0.0
		\date		02/09/2014
		\~english
		\brief		Stream buffer used by streams to indent lines
		\~french
		\brief		Tampon de flux utilisé pour indenter les lignes
		*/
		template < typename prefix_type, typename char_type, typename traits = std::char_traits< char_type > >
		struct basic_prefix_buffer
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
			basic_prefix_buffer( std::basic_streambuf< char_type, traits > * sbuf )
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
			~basic_prefix_buffer()
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
			basic_prefix_buffer( const basic_prefix_buffer< prefix_type, char_type, traits > & ) = delete;
			basic_prefix_buffer< prefix_type, char_type, traits > & operator=( const basic_prefix_buffer< prefix_type, char_type, traits > & ) = delete;

		private:
			virtual int_type overflow( int_type c = traits::eof() )
			{
				if ( traits::eq_int_type( c, traits::eof() ) )
				{
					return m_sbuf->sputc( static_cast< char_type >( c ) );
				}

				if ( m_set )
				{
					auto l_prefix = prefix_type::to_string();
					m_sbuf->sputn( l_prefix.c_str(), l_prefix.size() );
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
			//!\~english Tells the stream it to prefix.	\~french Dit si le flux doit être préfixé.
			bool m_set;
		};
	}
}

#endif
