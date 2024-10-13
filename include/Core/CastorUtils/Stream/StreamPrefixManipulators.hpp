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
		template < typename CharT, typename TraitsT >
		struct BasePrefixerT
			: public TraitsT
		{
			std::basic_string< CharT > toString()const
			{
				return TraitsT::toString();
			}
		};

		template < typename CharT >
		struct BasicPrefixTraitsT
		{
			std::basic_string< CharT > toString()const
			{
				return prefix;
			}

			String prefix;
		};

		template < typename CharT >
		struct BasicLinePrefixTraitsT
		{
			std::basic_string< CharT > toString()const
			{
				std::basic_stringstream< CharT > prefix;
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
		template< typename PrefixT, typename CharT, typename BufferT = BasicPrefixBufferT< BasePrefixerT< CharT, PrefixT >, CharT >, typename BufferManagerType = BasicPrefixBufferManagerT< BasePrefixerT< CharT, PrefixT >, CharT > >
		inline BufferT * installPrefixBuffer( std::basic_ostream< CharT > & stream )
		{
			BufferT * sbuf( new BufferT( stream.rdbuf() ) );
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
		template< typename PrefixT, typename CharT >
		inline void callback( std::ios_base::event ev, std::ios_base & ios, CU_UnusedParam( int, x ) )
		{
			if ( BasicPrefixBufferManagerT< BasePrefixerT< CharT, PrefixT >, CharT >::instances() )
			{
				if ( ev == std::ios_base::erase_event )
				{
					BasicPrefixBufferManagerT< BasePrefixerT< CharT, PrefixT >, CharT >::instance()->erase( ios );
				}
				else if ( ev == std::ios_base::copyfmt_event )
				{
#if __GNUC__ && ( __GNUC__ < 3 || (__GNUC__ == 3 && __GNUC_MINOR__ < 3 ) )
#	error Your compiler is too buggy; it is known to miscompile.
#	error Known good compilers: 3.3
#else

					if ( auto & o_s = dynamic_cast< std::basic_ostream< CharT > & >( ios ) )
					{
						o_s << BasePrefixerT< CharT, PrefixT >();
					}

#endif
				}
			}
		}
	}
	/**
	 *\~english
	 *\brief		Stream operator.
	 *\remarks		Initializes the stream in order to prefix it.
	 *\param[in]	stream	The stream.
	 *\~french
	 *\brief		Opérateur de flux.
	 *\remarks		Initialise le flux afin de pouvoir le préfixer.
	 *\param[in]	stream	Le flux.
	 */
	template< typename CharT, typename PrefixT >
	inline std::basic_ostream< CharT > & operator<<( std::basic_ostream< CharT > & stream, format::BasePrefixerT< CharT, PrefixT > const & )
	{
		if ( auto * sbuf = dynamic_cast< format::BasicPrefixBufferT< format::BasePrefixerT< CharT, PrefixT >, CharT > * >( stream.rdbuf() );
			!sbuf )
		{
			format::installPrefixBuffer< PrefixT >( stream );
			stream.register_callback( format::callback< PrefixT, CharT >, 0 );
		}

		return stream;
	}
}

#endif
