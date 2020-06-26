/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_STREAM_BASE_MANIPULATORS_H___
#define ___CASTOR_STREAM_BASE_MANIPULATORS_H___

#include "CastorUtils/Stream/StreamModule.hpp"

#include <streambuf>
#include <cassert>
#include <climits>

namespace castor
{
	namespace manip
	{
		template< typename CharType >
		struct BasicBaseManip
		{
			int m_base;

			explicit BasicBaseManip( int base )
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

		typedef BasicBaseManip< char > BaseManip;
		typedef BasicBaseManip< wchar_t > WBaseManip;
		typedef BasicBaseManip< xchar > XBaseManip;

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
		inline BaseManip base( int b )
		{
			return BaseManip( b );
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
		inline WBaseManip wbase( int b )
		{
			return WBaseManip( b );
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
		inline XBaseManip xbase( int b )
		{
			return XBaseManip( b );
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
		inline std::ostream & operator<<( std::ostream & stream, const manip::BaseManip & manip )
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
		inline std::wostream & operator<<( std::wostream & stream, const manip::WBaseManip & manip )
		{
			manip.apply( stream );
			return stream;
		}
		/**
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
		struct BaseNumPut
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
				// read the value stored in our xalloc location.
				const int base = str.iword( BasicBaseManip< CharType >::getIWord() );

				// we only want this manipulator to affect the next numeric value, so
				// reset its value.
				str.iword( BasicBaseManip< CharType >::getIWord() ) = 0;

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

				// get the format flags.
				const std::ios_base::fmtflags flags = str.flags();

				// add the padding if needs by (i.e. they have used std::setw).
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

				// add the padding if needs by (i.e. they have used std::setw).
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
