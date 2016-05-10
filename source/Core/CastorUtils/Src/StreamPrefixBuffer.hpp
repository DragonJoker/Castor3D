/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
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
