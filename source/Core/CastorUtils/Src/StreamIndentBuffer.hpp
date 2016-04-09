/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___CASTOR_STREAM_INDENT_BUFFER_H___
#define ___CASTOR_STREAM_INDENT_BUFFER_H___

#include "CastorUtilsPrerequisites.hpp"

#include <streambuf>

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
		template < typename char_type, char_type fill_char = ' ', typename traits = std::char_traits< char_type > >
		struct basic_indent_buffer
				: public std::basic_streambuf< char_type, traits >
		{
		public:
			typedef typename traits::int_type int_type;
			typedef typename traits::pos_type pos_type;
			typedef typename traits::off_type off_type;

		private:
			/**
			 *\~english
			 *\brief		Copy constructor
			 *\remarks		Not implemented to deactivate it
			 *\~french
			 *\brief		Constructeur par copie
			 *\remarks		Non implémenté afin de le désactiver
			 */
			basic_indent_buffer( const basic_indent_buffer< char_type, fill_char, traits > & ) = delete;

			/**
			 *\~english
			 *\brief		Copy assignment operator
			 *\remarks		Not implemented to deactivate it
			 *\~french
			 *\brief		Opérateur d'affectation par copie
			 *\remarks		Non implémenté afin de le désactiver
			 */
			basic_indent_buffer< char_type, fill_char, traits > & operator =( const basic_indent_buffer< char_type, fill_char, traits > & ) = delete;

		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			basic_indent_buffer( std::basic_streambuf< char_type, traits > * sbuf )
				: m_sbuf( sbuf )
				, m_count( 0 )
				, m_set( true )
			{
			}

			/**
			 *\~english
			 *\brief		Destructor
			 *\~french
			 *\brief		Destructeur
			 */
			~basic_indent_buffer()
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
			int indent() const
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
			std::streambuf * sbuf() const
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
			int m_count;
			//!\~english Tells the stream has been indented	\~french Dit si le flux a été indenté
			bool m_set;
		};

		typedef basic_indent_buffer< char > spc_indent_buffer;
		typedef basic_indent_buffer< wchar_t > wspc_indent_buffer;

		typedef basic_indent_buffer< char, '\t' > tab_indent_buffer;
		typedef basic_indent_buffer< wchar_t, L'\t' > wtab_indent_buffer;
	}
}

#endif
