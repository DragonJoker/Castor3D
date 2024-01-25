/*
See LICENSE file in root folder
*/
#ifndef ___CU_LoggerStream_H___
#define ___CU_LoggerStream_H___

#include "CastorUtils/Log/LoggerStreambuf.hpp"

namespace castor
{
	template< typename CharT
		, template< typename T > typename StreambufT >
	struct LoggerStreamT
		: public std::basic_ostringstream< CharT >
	{
		template< typename T >
		using StreambufPtrT = castor::RawUniquePtr< StreambufT< T > >;

	public:
		LoggerStreamT()
			: std::basic_ostringstream< CharT >{}
		{
		}

		void set( LoggerInstance & logger )
		{
			m_streambuf.reset();
			m_streambuf = castor::make_unique< StreambufT< CharT > >( logger
				, static_cast< std::basic_ostream< CharT > & >( *this ) );
			this->imbue( std::locale{ "C" } );
		}

		void reset()
		{
			m_streambuf.reset();
		}

	private:
		StreambufPtrT< CharT > m_streambuf;
	};
}

#endif
