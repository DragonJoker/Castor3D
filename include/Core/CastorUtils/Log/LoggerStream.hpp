/*
See LICENSE file in root folder
*/
#ifndef ___CU_LoggerStream_H___
#define ___CU_LoggerStream_H___

#include "CastorUtils/Log/LoggerStreambuf.hpp"

namespace castor
{
	template< typename CharT
		, template< typename CharT > typename StreambufT >
	struct LoggerStreamT
		: public std::basic_ostringstream< CharT >
	{
		template< typename CharT >
		using StreambufPtrT = std::unique_ptr< StreambufT< CharT > >;

	public:
		LoggerStreamT( std::basic_ostream< CharT > & stream )
			: std::basic_ostringstream< CharT >{}
		{
			reset( stream );
		}
		
		LoggerStreamT()
			: std::basic_ostringstream< CharT >{}
		{
		}

		LoggerStreamT( LoggerInstance & logger )
			: std::basic_ostringstream< CharT >{}
		{
			reset( logger );
		}

		~LoggerStreamT()
		{
		}

		void reset( LoggerInstance & logger )
		{
			m_streambuf.reset();
			m_streambuf = std::make_unique< StreambufT< CharT > >( logger
				, static_cast< std::basic_ostream< CharT > & >( *this ) );
			this->imbue( std::locale{ "C" } );
		}

		void reset( std::basic_ostream< CharT > & stream )
		{
			m_streambuf.reset();
			m_streambuf = std::make_unique< StreambufT< CharT > >( stream );
			this->imbue( std::locale{ "C" } );
		}

	private:
		StreambufPtrT< CharT > m_streambuf;
	};
}

#endif
