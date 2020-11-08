#include "CastorTestParser/Database/DbDateTimeHelpers.hpp"

namespace test_parser::db
{
	namespace
	{
		static int MonthMaxDays[13] = { -1, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

		void str_formalize( std::string & formattedString
			, int maxSize
			, const char * format
			, va_list vaList )
		{
			static const std::string ERROR_DB_FORMALIZE = "Error while formatting: ";
			std::vector< char > strText( maxSize + 1, 0 );

			try
			{

				if ( format )
				{
					size_t written = vsnprintf( strText.data(), maxSize + 1, format, vaList );
					formattedString.assign( strText.data(), strText.data() + std::min( written, size_t( maxSize ) ) );
				}
			}
			catch ( ... )
			{
				std::stringstream message;
				message << ERROR_DB_FORMALIZE << formattedString.c_str();
				throw std::runtime_error{ message.str() };
			}
		}

		void formalize( std::string & formattedString, int maxSize, const char * format, ... )
		{
			formattedString.clear();

			if ( format )
			{
				va_list vaList;
				va_start( vaList, format );
				str_formalize( formattedString, maxSize, format, vaList );
				va_end( vaList );
			}
		}

		bool isLeap( int year )
		{
			return ( year % 4 == 0 ) && ( year % 100 != 0 || year % 400 == 0 );
		}

		template< typename CharType >
		int stoi( CharType const *& in, size_t count )
		{
			if ( *in == '-' )
			{
				++count;
			}

			int result = std::stoi( std::basic_string< CharType >( in, in + count ) );
			in += count;
			return result;
		}
	}

	//*********************************************************************************************

	namespace time
	{
		namespace utils
		{
			static const int TIME_MAX_SIZE = 100;

			template< typename Char >
			bool isTime( const std::basic_string< Char > & time
				, const std::basic_string< Char > & format
				, int & hours
				, int & minutes
				, int & seconds )
			{
				bool bReturn = !format.empty();

				hours = 0;
				minutes = 0;
				seconds = 0;

				if ( bReturn )
				{
					Char const * fc = format.data();
					Char const * dc = time.data();

					while ( bReturn && *fc )
					{
						if ( *fc == '%' )
						{
							bReturn = ++fc != NULL;

							if ( bReturn )
							{
								switch ( *fc++ )
								{
								case 'H':
									hours = stoi( dc, 2 );
									break;

								case 'M':
									minutes = stoi( dc, 2 );
									break;

								case 'S':
									seconds = stoi( dc, 2 );
									break;

								case '%':
									break;

								default:
									bReturn = false;
									break;
								}
							}
						}
						else if ( *fc == *dc )
						{
							++fc;
							++dc;
						}
						else
						{
							bReturn = false;
						}
					}
				}

				if ( bReturn )
				{
					bReturn = hours >= 0 && hours <= 23 && minutes >= 0 && minutes <= 59 && seconds >= 0 && seconds <= 59;
				}

				return bReturn;
			}
		}

		std::string format( const Time & time, const std::string & format )
		{
			if ( time.is_not_a_date_time() )
			{
				throw std::out_of_range( "Time::Format - Not a date time" );
			}

			auto hours = time.hours();
			auto minutes = time.minutes();
			auto seconds = time.seconds();

			if ( hours < 0 || hours > 23 )
			{
				throw std::out_of_range( "Time::Format - Invalid hours value" );
			}

			if ( minutes < 0 || minutes > 59 )
			{
				throw std::out_of_range( "Time::Format - Invalid minutes value" );
			}

			if ( seconds < 0 || seconds > 59 )
			{
				throw std::out_of_range( "Time::Format - Invalid seconds value" );
			}

			std::tm tmbuf = boost::posix_time::to_tm( time );
			char buffer[utils::TIME_MAX_SIZE];
			size_t length = strftime( buffer, utils::TIME_MAX_SIZE, format.c_str(), &tmbuf );
			assert( length < utils::TIME_MAX_SIZE );
			return std::string( buffer, buffer + length );
		}

		std::string print( const Time & time, const std::string & format )
		{
			if ( time.is_not_a_date_time() )
			{
				throw std::out_of_range( "Time::Print - Not a date time" );
			}

			auto hours = int( time.hours() );
			auto minutes = int( time.minutes() );
			auto seconds = int( time.seconds() );

			if ( hours < 0 || hours > 23 )
			{
				throw std::out_of_range( "Time::Print - Invalid hours value" );
			}

			if ( minutes < 0 || minutes > 59 )
			{
				throw std::out_of_range( "Time::Print - Invalid minutes value" );
			}

			if ( seconds < 0 || seconds > 59 )
			{
				throw std::out_of_range( "Time::Print - Invalid seconds value" );
			}

			std::string result;
			formalize( result, 1024, format.c_str(), hours, minutes, seconds );
			return result;
		}

		bool isTime( const std::string & time, const std::string & format )
		{
			int hours = 0;
			int minutes = 0;
			int seconds = 0;
			return utils::isTime( time, format, hours, minutes, seconds );
		}

		bool isTime( const std::string & time, const std::string & format, Time & result )
		{
			int hours = 0;
			int minutes = 0;
			int seconds = 0;
			bool bReturn = utils::isTime( time, format, hours, minutes, seconds );

			if ( bReturn )
			{
				result = Time( hours, minutes, seconds );
			}
			else
			{
				result = Time( boost::posix_time::not_a_date_time );
			}

			return bReturn;
		}

		bool isValid( const Time & time )
		{
			return !time.is_negative() && !time.is_not_a_date_time();
		}
	}

	//*********************************************************************************************

	namespace date_time
	{
		namespace utils
		{
			static const size_t DATE_TIME_MAX_SIZE = 100;

			template< typename Char >
			bool isDateTime( const  std::basic_string< Char > & date
				, const  std::basic_string< Char > & format
				, int & year
				, int & month
				, int & monthDay
				, int & hours
				, int & minutes
				, int & seconds )
			{
				bool bReturn = !format.empty();

				monthDay = 0;
				month = 0;
				year = -1;
				hours = 0;
				minutes = 0;
				seconds = 0;

				if ( bReturn )
				{
					Char const * fc = format.data();
					Char const * dc = date.data();

					while ( bReturn && *fc )
					{
						if ( *fc == '%' )
						{
							bReturn = ++fc != NULL;

							if ( bReturn )
							{
								switch ( *fc++ )
								{
								case 'H':
									hours = stoi( dc, 2 );
									break;

								case 'M':
									minutes = stoi( dc, 2 );
									break;

								case 'S':
									seconds = stoi( dc, 2 );
									break;

								case 'Y':
									year = stoi( dc, 4 );
									break;

								case 'd':
									monthDay = stoi( dc, 2 );
									break;

								case 'm':
									month = stoi( dc, 2 );
									break;

								case 'y':
									year = stoi( dc, 2 ) + 1900;
									break;

								case '%':
									break;

								default:
									bReturn = false;
									break;
								}
							}
						}
						else if ( *fc == *dc )
						{
							++fc;
							++dc;
						}
						else
						{
							bReturn = false;
						}
					}
				}

				if ( bReturn )
				{
					bReturn = year >= boost::gregorian::date( boost::gregorian::min_date_time ).year() && year <= boost::gregorian::date( boost::gregorian::max_date_time ).year();
				}

				if ( bReturn )
				{
					bReturn = month >= boost::gregorian::Jan && month <= boost::gregorian::Dec;
				}

				if ( bReturn )
				{
					if ( month != boost::gregorian::Feb )
					{
						bReturn = monthDay <= MonthMaxDays[month];
					}
					else
					{
						bReturn = monthDay <= ( MonthMaxDays[month] + isLeap( year ) );
					}
				}

				if ( bReturn )
				{
					bReturn = hours >= 0 && hours <= 23 && minutes >= 0 && minutes <= 59 && seconds >= 0 && seconds <= 59;
				}

				return bReturn;
			}
		}

		std::string format( const DateTime & dateTime, const std::string & format )
		{
			char buffer[utils::DATE_TIME_MAX_SIZE + 1] = { 0 };
			std::tm tm = boost::posix_time::to_tm( dateTime );
			size_t length = strftime( buffer, utils::DATE_TIME_MAX_SIZE, format.c_str(), &tm );
			assert( length < utils::DATE_TIME_MAX_SIZE );
			return std::string( buffer, buffer + length );
		}

		std::string print( const DateTime & dateTime, const std::string & format )
		{
			int year = int( dateTime.date().year() );
			int month = int( dateTime.date().month() );
			int day = int( dateTime.date().day() );
			int hours = int( dateTime.time_of_day().hours() );
			int minutes = int( dateTime.time_of_day().minutes() );
			int seconds = int( dateTime.time_of_day().seconds() );
			std::string result;
			formalize( result, 1024, format.c_str(), year, month, day, hours, minutes, seconds );
			return result;
		}

		bool isDateTime( const std::string & date, const std::string & format )
		{
			int monthDay = 0;
			int month = 0;
			int year = -1;
			int hour = 0;
			int min = 0;
			int sec = 0;
			return utils::isDateTime( date, format, year, month, monthDay, hour, min, sec );
		}

		bool isDateTime( const std::string & date, const std::string & format, DateTime & result )
		{
			int monthDay = 0;
			int month = 0;
			int year = -1;
			int hour = 0;
			int min = 0;
			int sec = 0;
			bool bReturn = utils::isDateTime( date, format, year, month, monthDay, hour, min, sec );

			if ( bReturn )
			{
				result = DateTime( Date( year, month, monthDay ), Time( hour, min, sec ) );
			}
			else
			{
				result = DateTime( boost::posix_time::not_a_date_time );
			}

			return bReturn;
		}

		bool isValid( const DateTime & dateTime )
		{
			return !dateTime.date().is_not_a_date()
				&& !dateTime.time_of_day().is_negative()
				&& !dateTime.time_of_day().is_not_a_date_time();
		}
	}

	//*********************************************************************************************

	namespace date
	{
		namespace utils
		{
			static const size_t DATE_MAX_SIZE = 100;

			template< typename Char >
			bool isDate( const  std::basic_string< Char > & date
				, const  std::basic_string< Char > & format
				, int & year
				, int & month
				, int & monthDay )
			{
				bool bReturn = !format.empty();

				monthDay = 0;
				month = 0;
				year = -1;

				if ( bReturn )
				{
					Char const * fc = format.data();
					Char const * dc = date.data();

					while ( bReturn && *fc )
					{
						if ( *fc == '%' )
						{
							bReturn = ++fc != NULL;

							if ( bReturn )
							{
								switch ( *fc++ )
								{
								case 'Y':
									year = stoi( dc, 4 );
									break;

								case 'd':
									monthDay = stoi( dc, 2 );
									break;

								case 'm':
									month = stoi( dc, 2 );
									break;

								case 'y':
									year = stoi( dc, 2 ) + 1900;
									break;

								case '%':
									++dc;
									break;

								default:
									bReturn = false;
									break;
								}
							}
						}
						else if ( *fc == *dc )
						{
							++fc;
							++dc;
						}
						else
						{
							bReturn = false;
						}
					}
				}

				if ( bReturn )
				{
					bReturn = year >= boost::gregorian::date( boost::gregorian::min_date_time ).year() && year <= boost::gregorian::date( boost::gregorian::max_date_time ).year();
				}

				if ( bReturn )
				{
					bReturn = month >= boost::gregorian::Jan && month <= boost::gregorian::Dec;
				}

				if ( bReturn )
				{
					if ( month != boost::gregorian::Feb )
					{
						bReturn = monthDay <= MonthMaxDays[month];
					}
					else
					{
						bReturn = monthDay <= ( MonthMaxDays[month] + isLeap( year ) );
					}
				}

				return bReturn;
			}
		}

		std::string format( const Date & date, const std::string & format )
		{
			std::string result;

			if ( !date::isValid( date ) )
			{
				std::stringstream stream;
				stream << "invalid: (" << date.year() << "-" << ( date.month() ) << "-" << date.day() << ")";
				result = stream.str();
			}
			else
			{
				char buffer[utils::DATE_MAX_SIZE + 1] = { 0 };
				std::tm tm = boost::gregorian::to_tm( date );
				size_t length = strftime( buffer, utils::DATE_MAX_SIZE, format.c_str(), &tm );
				assert( length < utils::DATE_MAX_SIZE );
				result = std::string( buffer, buffer + length );
			}

			return result;
		}

		std::string print( const Date & date, const std::string & format )
		{
			int year = int( date.year() );
			int month = int( date.month() );
			int day = int( date.day() );
			std::string result;
			formalize( result, 1024, format.c_str(), year, month, day );
			return result;
		}

		bool isDate( const std::string & date, const std::string & format )
		{
			int iMonthDay = 0;
			int iMonth = 0;
			int iYear = -1;
			return utils::isDate( date, format, iYear, iMonth, iMonthDay );
		}

		bool isDate( const std::string & date, const std::string & format, Date & result )
		{
			int iMonthDay = 0;
			int iMonth = 0;
			int iYear = -1;
			bool bReturn = utils::isDate( date, format, iYear, iMonth, iMonthDay );

			if ( bReturn )
			{
				result = Date( iYear, iMonth, iMonthDay );
			}

			return bReturn;
		}

		bool isValid( const Date & date )
		{
			return !date.is_not_a_date();
		}
	}

	//*********************************************************************************************
}
