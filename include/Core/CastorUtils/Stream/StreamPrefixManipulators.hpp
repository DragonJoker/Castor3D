/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_STREAM_PREFIX_H___
#define ___CASTOR_STREAM_PREFIX_H___

#include "CastorUtils/Stream/StreamPrefixBuffer.hpp"
#include "CastorUtils/Stream/StreamPrefixBufferManager.hpp"

namespace castor
{
	namespace format
	{
		template < typename char_type, typename prefix_traits >
		struct BasePrefixer
			: public prefix_traits
		{
			std::basic_string< char_type > toString()
			{
				return prefix_traits::toString();
			}
		};

		template < typename char_type >
		struct BasicPrefixTraits
		{
			std::basic_string< char_type > toString()
			{
				return std::basic_string< char_type >();
			}
		};

		template < typename char_type >
		struct BasicLinePrefixTraits
		{
			std::basic_string< char_type > toString()
			{
				std::basic_stringstream< char_type > prefix;
				prefix.width( 8 );
				prefix.fill( ' ' );
				prefix << std::left << ++m_line;
				return prefix.str();
			}

			uint32_t m_line = 0;
		};
		/**
		 *\~english
		 *\brief		Initializes the stream in order to indent it
		 *\param[in]	stream	The stream
		 *\~french
		 *\brief		Initialise le flux afin de pouvoir l'indenter
		 *\param[in]	stream	Le flux
		 */
		template< typename PrefixType, typename CharType, typename BufferType = BasicPrefixBuffer< BasePrefixer< CharType, PrefixType >, CharType >, typename BufferManagerType = BasicPrefixBufferManager< BasePrefixer< CharType, PrefixType >, CharType > >
		inline BufferType * installPrefixBuffer( std::basic_ostream< CharType > & stream )
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
			if ( BasicPrefixBufferManager< BasePrefixer< CharType, PrefixType >, CharType >::instances() )
			{
				if ( ev == std::ios_base::erase_event )
				{
					BasicPrefixBufferManager< BasePrefixer< CharType, PrefixType >, CharType >::instance()->erase( ios );
				}
				else if ( ev == std::ios_base::copyfmt_event )
				{
#if __GNUC__ && ( __GNUC__ < 3 || (__GNUC__ == 3 && __GNUC_MINOR__ < 3 ) )
#	error Your compiler is too buggy; it is known to miscompile.
#	error Known good compilers: 3.3
#else

					if ( std::basic_ostream< CharType > & o_s = dynamic_cast< std::basic_ostream< CharType > & >( ios ) )
					{
						o_s << BasePrefixer< CharType, PrefixType >();
					}

#endif
				}
			}
		}
	}
	/**
	 *\~english
	 *\brief		Stream operator.
	 *\remarks		Initializes the stream in order to Prefix it.
	 *\param[in]	stream	The stream.
	 *\param[in]	Prefix	The Prefix.
	 *\~french
	 *\brief		Opérateur de flux.
	 *\remarks		Initialise le flux afin de pouvoir le préfixer.
	 *\param[in]	stream	Le flux.
	 *\param[in]	Prefix	Le préfixe.
	 */
	template< typename CharType, typename PrefixType >
	inline std::basic_ostream< CharType > & operator<<( std::basic_ostream< CharType > & stream, format::BasePrefixer< CharType, PrefixType > const & Prefix )
	{
		auto * sbuf = dynamic_cast< format::BasicPrefixBuffer< format::BasePrefixer< CharType, PrefixType >, CharType > * >( stream.rdbuf() );

		if ( !sbuf )
		{
			format::installPrefixBuffer< PrefixType >( stream );
			stream.register_callback( format::callback< PrefixType, CharType >, 0 );
		}

		return stream;
	}
}

#endif
