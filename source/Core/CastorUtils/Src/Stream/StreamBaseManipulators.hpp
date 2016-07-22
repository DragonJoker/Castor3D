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
#ifndef ___CASTOR_STREAM_BASE_MANIPULATORS_H___
#define ___CASTOR_STREAM_BASE_MANIPULATORS_H___

#include "CastorUtilsPrerequisites.hpp"

#include <streambuf>
#include <cassert>
#include <climits>

namespace Castor
{
	namespace manip
	{
		/*!
		\author		Sylvain DOREMUS
		\version	0.7.0.0
		\date		02/09/2014
		\~english
		\brief		Defines a base manipulator type, its what the built-in stream manipulators do when they take parameters, only they return an opaque type.
		\~french
		\brief		Définit un type manipulateur de base, c'est ce que font les manipulateurs de flux lorsqu'ils prennent des paramètres, sauf qu'ils retournent un type opaque
		*/
		template< typename CharType >
		struct basic_base_manip
		{
			int m_base;

			basic_base_manip( int base )
				: m_base( base )
			{
				assert( base >= 2 );
				assert( base <= 36 );
			}

			static int getIWord()
			{
				// call xalloc once to get an index at which we can store data for this
				// manipulator.
				static int iw = std::ios_base::xalloc();
				return iw;
			}

			void apply( std::basic_ostream< CharType > & os )const
			{
				// store the base value in the manipulator.
				os.iword( getIWord() ) = m_base;
			}
		};

		typedef basic_base_manip< char > base_manip;
		typedef basic_base_manip< wchar_t > wbase_manip;
		typedef basic_base_manip< xchar > xbase_manip;

		/**
		 *\~english
		 *\brief		Stream operator
		 *\remarks		Creates a manipulator, to be able to use it in a stream
		 *\param[in]	b	The base
		 *\~french
		 *\brief		Opérateur de flux
		 *\remarks		Crée un manipulateur, pour l'utiliser dans un flux
		 *\param[in]	b	La base
		 */
		inline base_manip base( int b )
		{
			return base_manip( b );
		}

		/**
		 *\~english
		 *\brief		Stream operator
		 *\remarks		Creates a manipulator, to be able to use it in a stream
		 *\param[in]	b	The base
		 *\~french
		 *\brief		Opérateur de flux
		 *\remarks		Crée un manipulateur, pour l'utiliser dans un flux
		 *\param[in]	b	La base
		 */
		inline wbase_manip wbase( int b )
		{
			return wbase_manip( b );
		}

		/**
		 *\~english
		 *\brief		Stream operator
		 *\remarks		Creates a manipulator, to be able to use it in a stream
		 *\param[in]	b	The base
		 *\~french
		 *\brief		Opérateur de flux
		 *\remarks		Crée un manipulateur, pour l'utiliser dans un flux
		 *\param[in]	b	La base
		 */
		inline xbase_manip xbase( int b )
		{
			return xbase_manip( b );
		}

		/**
		 *\~english
		 *\brief		Stream operator
		 *\remarks		We need this so we can apply our custom stream manipulator to the stream.
		 *\param[in]	stream	The stream
		 *\param[in]	manip	The manipulator
		 *\~french
		 *\brief		Opérateur de flux
		 *\remarks		Applique notre manipulateur au flux
		 *\param[in]	stream	Le flux
		 *\param[in]	manip	Le manipulateur
		 */
		inline std::ostream & operator<<( std::ostream & stream, const manip::base_manip & manip )
		{
			manip.apply( stream );
			return stream;
		}

		/**
		 *\~english
		 *\brief		Stream operator
		 *\remarks		We need this so we can apply our custom stream manipulator to the stream.
		 *\param[in]	stream	The stream
		 *\param[in]	manip	The manipulator
		 *\~french
		 *\brief		Opérateur de flux
		 *\remarks		Applique notre manipulateur au flux
		 *\param[in]	stream	Le flux
		 *\param[in]	manip	Le manipulateur
		 */
		inline std::wostream & operator<<( std::wostream & stream, const manip::wbase_manip & manip )
		{
			manip.apply( stream );
			return stream;
		}
		/*!
		\author		Sylvain DOREMUS
		\version	0.7.0.0
		\date		02/09/2014
		\~english
		\brief		A custom number output facet.
		\remarks	These are used by the std::locale code in streams.
					<br />The num_put facet handles the output of numberic values as characters in the stream.
					<br />Here we create one that knows about our custom manipulator.
		\~french
		\brief		Une facet d'écriture de nombre
		\remarks	Généralement utilisées par le code std::locale dans les flux.
					<br />La facet num_put gère la sortie de valeurs numériques en tant que caractères dans le flux
					<br />Ici nous en créons une qui connait notre manipulateur.
		*/
		template< typename CharType >
		struct base_num_put
			: std::num_put< CharType >
		{
			typedef typename std::num_put< CharType >::iter_type iter_type;
			// These absVal functions are needed as std::abs doesnt support
			// unsigned types, but the templated doPutHelper works on signed and
			// unsigned types.
			unsigned short absVal( unsigned short a )const
			{
				return a;
			}

			unsigned int absVal( unsigned int a )const
			{
				return a;
			}

			unsigned long absVal( unsigned long a )const
			{
				return a;
			}

			unsigned long long absVal( unsigned long long a )const
			{
				return a;
			}

			template< class NumType >
			NumType absVal( NumType a )const
			{
				return std::abs( a );
			}

			template< class NumType >
			iter_type doPutHelper( iter_type out, std::ios_base & str, CharType fill, NumType val )const
			{
				// Read the value stored in our xalloc location.
				const int base = str.iword( basic_base_manip< CharType >::getIWord() );

				// we only want this manipulator to affect the next numeric value, so
				// reset its value.
				str.iword( basic_base_manip< CharType >::getIWord() ) = 0;

				// normal number output, use the built in putter.
				if ( base == 0 || base == 10 )
				{
					return std::num_put< CharType >::do_put( out, str, fill, val );
				}

				// We want to conver the base, so do it and output.

				int digits[CHAR_BIT * sizeof( NumType )] = { 0 };
				int i = 0;
				NumType tempVal = absVal( val );

				while ( tempVal != 0 )
				{
					digits[i++] = tempVal % base;
					tempVal /= base;
				}

				// Get the format flags.
				const std::ios_base::fmtflags flags = str.flags();

				// Add the padding if needs by (i.e. they have used std::setw).
				// Only applies if we are right aligned, or none specified.
				if ( ( flags & std::ios_base::right ) || !( ( flags & std::ios_base::internal ) || ( flags & std::ios_base::left ) ) )
				{
					std::fill_n( out, str.width() - i, fill );
				}

				if ( val < 0 )
				{
					*out++ = '-';
				}

				// Handle the internal adjustment flag.
				if ( flags & std::ios_base::internal )
				{
					std::fill_n( out, str.width() - i, fill );
				}

				char digitCharLc[] = "0123456789abcdefghijklmnopqrstuvwxyz";
				char digitCharUc[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
				const char * digitChar = ( str.flags() & std::ios_base::uppercase ) ? digitCharUc : digitCharLc;

				while ( i > 0 )
				{
					// out is an iterator that accepts characters
					*out++ = digitChar[digits[--i]];
				}

				// Add the padding if needs by (i.e. they have used std::setw).
				// Only applies if we are left aligned.
				if ( str.flags() & std::ios_base::left )
				{
					std::fill_n( out, str.width() - i, fill );
				}

				// clear the width
				str.width( 0 );

				return out;
			}

			// Overrides for the virtual do_put member functions.
			iter_type do_put( iter_type out, std::ios_base & str, CharType fill, long val )const
			{
				return doPutHelper( out, str, fill, val );
			}

			iter_type do_put( iter_type out, std::ios_base & str, CharType fill, unsigned long val )const
			{
				return doPutHelper( out, str, fill, val );
			}
		};
	}
}

#endif
