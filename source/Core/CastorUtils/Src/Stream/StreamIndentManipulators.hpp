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
#ifndef ___CASTOR_STREAM_INDENT_H___
#define ___CASTOR_STREAM_INDENT_H___

#include "CastorUtilsPrerequisites.hpp"

#include "StreamIndentBuffer.hpp"
#include "StreamIndentBufferManager.hpp"

namespace Castor
{
	namespace format
	{
		/*!
		\author		Sylvain DOREMUS
		\version	0.7.0.0
		\date		02/09/2014
		\~english
		\brief		Holds the indentation value
		\~french
		\brief		Garde la valeur d'intentation
		*/
		struct indent
		{
			/**
			 *\~english
			 *\brief		Constructor
			 *\param[in]	i	The indentation value
			 *\~french
			 *\brief		Constructor
			 *\param[in]	i	The indentation value
			 */
			indent( int i )
				: m_indent( i )
			{
			}

			//!\~english The indentation value	\~french La valeur d'indentation
			int m_indent;
		};

		namespace
		{
			/**
			 *\~english
			 *\brief		Retrieves the indentation value for the given stream
			 *\param[in]	ios	The stream
			 *\return		A reference to the current indentation value
			 *\~french
			 *\brief		Récupère la valeur d'indentation pour le flux donné
			 *\param[in]	ios	Le flux
			 *\return		Une référence sur la valeur d'indentation actuelle
			 */
			inline long & indent_value( std::ios_base & ios )
			{
				static int indentIndex = std::ios_base::xalloc();
				return ios.iword( indentIndex );
			}
			/**
			 *\~english
			 *\brief		Retrieves the indentation value for the given stream
			 *\param[in]	ios	The stream
			 *\return		The indentation value
			 *\~french
			 *\brief		Récupère la valeur d'indentation pour le flux donné
			 *\param[in]	ios	Le flux
			 *\return		La valeur d'indentation
			 */
			inline long get_indent( std::ios_base & ios )
			{
				return indent_value( ios );
			}
			/**
			 *\~english
			 *\brief		Defines the indentation value for given stream
			 *\param[in]	ios	The stream
			 *\param[in]	val	The new indentation value
			 *\~french
			 *\brief		Définit la valeur d'indentation pour le flux donné
			 *\param[in]	ios	Le flux
			 *\param[in]	val	La nouvelle valeur d'indentation
			 */
			inline void set_indent( std::ios_base & ios, unsigned int val )
			{
				indent_value( ios ) = val;
			}
			/**
			 *\~english
			 *\brief		Initializes the stream in order to indent it
			 *\param[in]	stream	The stream
			 *\~french
			 *\brief		Initialise le flux afin de pouvoir l'indenter
			 *\param[in]	stream	Le flux
			 */
			template< typename CharType, typename BufferType = basic_indent_buffer< CharType >, typename BufferManagerType = basic_indent_buffer_manager< CharType > >
			inline BufferType * install_indent_buffer( std::basic_ostream< CharType > & stream )
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
			template< typename CharType >
			inline void callback( std::ios_base::event ev, std::ios_base & ios, int x )
			{
				if ( basic_indent_buffer_manager< CharType >::instances() )
				{
					if ( ev == std::ios_base::erase_event )
					{
						basic_indent_buffer_manager< CharType >::instance()->erase( ios );
					}
					else if ( ev == std::ios_base::copyfmt_event )
					{
#if __GNUC__ && ( __GNUC__ < 3 || (__GNUC__ == 3 && __GNUC_MINOR__ < 3 ) )
#	error Your compiler is too buggy; it is known to miscompile.
#	error Known good compilers: 3.3
#else

						if ( std::basic_ostream< CharType > & o_s = dynamic_cast< std::basic_ostream< CharType > & >( ios ) )
						{
							o_s << indent( get_indent( ios ) );
						}

#endif
					}
				}
			}
		}
	}
	/**
	 *\~english
	 *\brief		Stream operator
	 *\remarks		Initializes the stream in order to indent it
	 *\param[in]	stream	The stream
	 *\param[in]	ind		The indent
	 *\~french
	 *\brief		Opérateur de flux
	 *\remarks		Initialise le flux afin de pouvoir l'indenter
	 *\param[in]	stream	Le flux
	 *\param[in]	ind		La valeur d'indentation
	 */
	template< typename CharType >
	inline std::basic_ostream< CharType > & operator <<( std::basic_ostream< CharType > & stream, format::indent const & ind )
	{
		format::basic_indent_buffer< CharType > * sbuf = dynamic_cast< format::basic_indent_buffer< CharType > * >( stream.rdbuf() );

		if ( !sbuf )
		{
			sbuf = format::install_indent_buffer( stream );
			stream.register_callback( format::callback< CharType >, 0 );
		}

		format::set_indent( stream, ind.m_indent );
		sbuf->indent( ind.m_indent );
		return stream;
	}
}

#endif
