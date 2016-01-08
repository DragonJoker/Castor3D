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
#ifndef ___CASTOR_STREAM_PREFIX_H___
#define ___CASTOR_STREAM_PREFIX_H___

#include "CastorUtilsPrerequisites.hpp"

#include "StreamPrefixBuffer.hpp"
#include "StreamPrefixBufferManager.hpp"

namespace Castor
{
	namespace format
	{
		/*!
		\author		Sylvain DOREMUS
		\version	0.8.0.0
		\date		10/11/2015
		\~english
		\brief		Contains an std::basic_string< char_type > operator()() member function which generates the prefix.
		\~french
		\brief		Contient une méthode std::basic_string< char_type > operator()() qui génère le préfixe.
		*/
		template < typename char_type, typename prefix_traits >
		struct base_prefixer
			: public prefix_traits
		{
			std::basic_string< char_type > to_string()
			{
				return prefix_traits::to_string();
			}
		};

		/*!
		\author		Sylvain DOREMUS
		\version	0.8.0.0
		\date		10/11/2015
		\~english
		\brief		Default prefix traits.
		\~french
		\brief		Classe de traits par défaut pour les préfixes.
		*/
		template < typename char_type >
		struct basic_prefix_traits
		{
			std::basic_string< char_type > to_string()
			{
				return std::basic_string< char_type >();
			}
		};

		using prefix = base_prefixer< char, basic_prefix_traits< char > >;
		using wprefix = base_prefixer< wchar_t, basic_prefix_traits< wchar_t > >;

		/*!
		\author		Sylvain DOREMUS
		\version	0.8.0.0
		\date		10/11/2015
		\~english
		\brief		Prefixes lines with their index.
		\~french
		\brief		Préfixe les lignes par leur index.
		*/
		template < typename char_type >
		struct basic_line_prefix_traits
		{
			std::basic_string< char_type > to_string()
			{
				std::basic_stringstream< char_type > l_prefix;
				l_prefix.width( 8 );
				l_prefix.fill( ' ' );
				l_prefix << std::left << ++m_line;
				return l_prefix.str();
			}

			uint32_t m_line = 0;
		};

		using line_prefix = base_prefixer< char, basic_line_prefix_traits< char > >;
		using wline_prefix = base_prefixer< wchar_t, basic_line_prefix_traits< wchar_t > >;

		using prefix_buffer = basic_prefix_buffer< prefix, char >;
		using wprefix_buffer = basic_prefix_buffer< wprefix, wchar_t >;

		using line_prefix_buffer = basic_prefix_buffer< line_prefix, char >;
		using wline_prefix_buffer = basic_prefix_buffer< wline_prefix, wchar_t >;

		namespace
		{
			/**
			 *\~english
			 *\brief		Initializes the stream in order to indent it
			 *\param[in]	stream	The stream
			 *\~french
			 *\brief		Initialise le flux afin de pouvoir l'indenter
			 *\param[in]	stream	Le flux
			 */
			template< typename PrefixType, typename CharType, typename BufferType = basic_prefix_buffer< base_prefixer< CharType, PrefixType >, CharType >, typename BufferManagerType = basic_prefix_buffer_manager< base_prefixer< CharType, PrefixType >, CharType > >
			inline BufferType * install_prefix_buffer( std::basic_ostream< CharType > & stream )
			{
				BufferType * sbuf( new BufferType( stream.rdbuf() ) );
				BufferManagerType::instance()->insert( stream, sbuf );
				stream.rdbuf( sbuf );
				return sbuf;
			}

			/**
			 *\~english
			 *\brief		The stream events callback
			 *\~french
			 *\brief		Le callback des évènements du flux
			 */
			template< typename PrefixType, typename CharType >
			inline void callback( std::ios_base::event ev, std::ios_base & ios, int x )
			{
				if ( basic_prefix_buffer_manager< base_prefixer< CharType, PrefixType >, CharType >::instances() )
				{
					if ( ev == std::ios_base::erase_event )
					{
						basic_prefix_buffer_manager< base_prefixer< CharType, PrefixType >, CharType >::instance()->erase( ios );
					}
					else if ( ev == std::ios_base::copyfmt_event )
					{
#if __GNUC__ && ( __GNUC__ < 3 || (__GNUC__ == 3 && __GNUC_MINOR__ < 3 ) )
#	error Your compiler is too buggy; it is known to miscompile.
#	error Known good compilers: 3.3
#else

						if ( std::basic_ostream< CharType > & o_s = dynamic_cast< std::basic_ostream< CharType > & >( ios ) )
						{
							o_s << base_prefixer< CharType, PrefixType >();
						}

#endif
					}
				}
			}
		}
	}

	/**
	*\~english
	*\brief		Stream operator.
	*\remarks		Initializes the stream in order to prefix it.
	*\param[in]	stream	The stream.
	*\param[in]	prefix	The prefix.
	*\~french
	*\brief		Opérateur de flux.
	*\remarks		Initialise le flux afin de pouvoir le préfixer.
	*\param[in]	stream	Le flux.
	*\param[in]	prefix	Le préfixe.
	*/
	template< typename CharType, typename PrefixType >
	inline std::basic_ostream< CharType > & operator<<( std::basic_ostream< CharType > & stream, format::base_prefixer< CharType, PrefixType > const & prefix )
	{
		format::basic_prefix_buffer< format::base_prefixer< CharType, PrefixType >, CharType > * sbuf = dynamic_cast< format::basic_prefix_buffer< format::base_prefixer< CharType, PrefixType >, CharType > * >( stream.rdbuf() );

		if ( !sbuf )
		{
			sbuf = format::install_prefix_buffer< PrefixType >( stream );
			stream.register_callback( format::callback< PrefixType, CharType >, 0 );
		}

		return stream;
	}
}

#endif
