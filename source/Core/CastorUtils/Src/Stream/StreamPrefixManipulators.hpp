/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_STREAM_PREFIX_H___
#define ___CASTOR_STREAM_PREFIX_H___

#include "CastorUtilsPrerequisites.hpp"

#include "StreamPrefixBuffer.hpp"
#include "StreamPrefixBufferManager.hpp"

namespace castor
{
	namespace format
	{
		/*!
		\author		Sylvain DOREMUS
		\version	0.8.0.0
		\date		10/11/2015
		\~english
		\brief		Contains an std::basic_string< char_type > operator()() member function which generates the Prefix.
		\~french
		\brief		Contient une méthode std::basic_string< char_type > operator()() qui génère le préfixe.
		*/
		template < typename char_type, typename prefix_traits >
		struct BasePrefixer
			: public prefix_traits
		{
			std::basic_string< char_type > toString()
			{
				return prefix_traits::toString();
			}
		};

		/*!
		\author		Sylvain DOREMUS
		\version	0.8.0.0
		\date		10/11/2015
		\~english
		\brief		Default Prefix traits.
		\~french
		\brief		Classe de traits par défaut pour les préfixes.
		*/
		template < typename char_type >
		struct BasicPrefixTraits
		{
			std::basic_string< char_type > toString()
			{
				return std::basic_string< char_type >();
			}
		};

		using Prefix = BasePrefixer< char, BasicPrefixTraits< char > >;
		using WPrefix = BasePrefixer< wchar_t, BasicPrefixTraits< wchar_t > >;

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

		using LinePrefix = BasePrefixer< char, BasicLinePrefixTraits< char > >;
		using WLinePrefix = BasePrefixer< wchar_t, BasicLinePrefixTraits< wchar_t > >;

		using PrefixBuffer = BasicPrefixBuffer< Prefix, char >;
		using WPrefixBuffer = BasicPrefixBuffer< WPrefix, wchar_t >;

		using LinePrefixBuffer = BasicPrefixBuffer< LinePrefix, char >;
		using WLinePrefixBuffer = BasicPrefixBuffer< WLinePrefix, wchar_t >;

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
		format::BasicPrefixBuffer< format::BasePrefixer< CharType, PrefixType >, CharType > * sbuf = dynamic_cast< format::BasicPrefixBuffer< format::BasePrefixer< CharType, PrefixType >, CharType > * >( stream.rdbuf() );

		if ( !sbuf )
		{
			sbuf = format::installPrefixBuffer< PrefixType >( stream );
			stream.register_callback( format::callback< PrefixType, CharType >, 0 );
		}

		return stream;
	}
}

#endif
