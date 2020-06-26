/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_STREAM_INDENT_H___
#define ___CASTOR_STREAM_INDENT_H___

#include "CastorUtils/Stream/StreamIndentBuffer.hpp"
#include "CastorUtils/Stream/StreamIndentBufferManager.hpp"

namespace castor
{
	namespace format
	{
		struct Indent
		{
			/**
			 *\~english
			 *\brief		Constructor
			 *\param[in]	i	The indentation value
			 *\~french
			 *\brief		Constructor
			 *\param[in]	i	The indentation value
			 */
			explicit Indent( int i )
				: m_indent( i )
			{
			}

			//!\~english The indentation value	\~french La valeur d'indentation
			int m_indent;
		};
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
		inline long & indentValue( std::ios_base & ios )
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
		inline long getIndent( std::ios_base & ios )
		{
			return indentValue( ios );
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
		inline void setIndent( std::ios_base & ios, unsigned int val )
		{
			indentValue( ios ) = val;
		}
		/**
		 *\~english
		 *\brief		Initializes the stream in order to indent it
		 *\param[in]	stream	The stream
		 *\~french
		 *\brief		Initialise le flux afin de pouvoir l'indenter
		 *\param[in]	stream	Le flux
		 */
		template< typename CharType, typename BufferType = BasicIndentBuffer< CharType >, typename BufferManagerType = BasicIndentBufferManager< CharType > >
		inline BufferType * installIndentBuffer( std::basic_ostream< CharType > & stream )
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
			if ( BasicIndentBufferManager< CharType >::instances() )
			{
				if ( ev == std::ios_base::erase_event )
				{
					BasicIndentBufferManager< CharType >::instance()->erase( ios );
				}
				else if ( ev == std::ios_base::copyfmt_event )
				{
#if __GNUC__ && ( __GNUC__ < 3 || (__GNUC__ == 3 && __GNUC_MINOR__ < 3 ) )
#	error Your compiler is too buggy; it is known to miscompile.
#	error Known good compilers: 3.3
#else

					if ( std::basic_ostream< CharType > & o_s = dynamic_cast< std::basic_ostream< CharType > & >( ios ) )
					{
						o_s << Indent( getIndent( ios ) );
					}

#endif
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
	inline std::basic_ostream< CharType > & operator <<( std::basic_ostream< CharType > & stream, format::Indent const & ind )
	{
		format::BasicIndentBuffer< CharType > * sbuf = dynamic_cast< format::BasicIndentBuffer< CharType > * >( stream.rdbuf() );

		if ( !sbuf )
		{
			sbuf = format::installIndentBuffer( stream );
			stream.register_callback( format::callback< CharType >, 0 );
		}

		format::setIndent( stream, ind.m_indent );
		sbuf->indent( ind.m_indent );
		return stream;
	}
}

#endif
