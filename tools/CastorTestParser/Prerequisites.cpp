#include "CastorTestParser/Prerequisites.hpp"

#include "CastorTestParser/Database/DbDateTimeHelpers.hpp"

#include <CastorUtils/Data/File.hpp>
#include <CastorUtils/Miscellaneous/Utils.hpp>

#include <wx/filefn.h>

namespace test_parser
{
	namespace
	{
		static const std::string FOLDER_DATETIME = "%Y-%m-%d_%H-%M-%S";
		static constexpr size_t FOLDER_DATETIME_SIZE = 4u + 3u + 3u + 3u + 3u + 3u;
	}

	castor::Path getFolderName( TestStatus value )
	{
		switch ( value )
		{
		case TestStatus::eNotRun:
			return castor::Path{ "NotRun" };
		case TestStatus::eNegligible:
			return castor::Path{ "Negligible" };
		case TestStatus::eAcceptable:
			return castor::Path{ "Acceptable" };
		case TestStatus::eUnacceptable:
			return castor::Path{ "Unacceptable" };
		case TestStatus::eUnprocessed:
			return castor::Path{ "Unprocessed" };
		default:
			assert( false );
			return castor::Path{};
		}
	}

	TestStatus getStatus( std::string const & name )
	{
		static std::map< std::string, TestStatus > const folders
		{
			{ "Negligible", TestStatus::eNegligible },
			{ "Acceptable", TestStatus::eAcceptable },
			{ "Unacceptable", TestStatus::eUnacceptable },
			{ "Unprocessed", TestStatus::eUnprocessed },
		};
		auto it = folders.find( name );
		assert( it != folders.end() );
		return it->second;
	}

	castor::Path getResultFolder( Test const & test, bool useStatus )
	{
		auto result = castor::Path{ "Result" } / test.category;
		return useStatus
			? result / getFolderName( test.status )
			: result;
	}

	castor::Path getResultName( Test const & test )
	{
		return castor::Path{ getFolderName( test.runDate ) + "_" + test.name + "_" + test.renderer + ".png" };
	}

	castor::Path getCompareFolder( Test const & test, bool useStatus )
	{
		auto result = castor::Path{ test.category } / cuT( "Compare" );
		return useStatus
			? result / getFolderName( test.status )
			: result;
	}

	castor::Path getCompareName( Test const & test )
	{
		return castor::Path{ test.name + "_" + test.renderer + ".png" };
	}

	castor::Path getReferenceFolder( Test const & test )
	{
		return castor::Path{ test.category };
	}

	castor::Path getReferenceName( Test const & test )
	{
		return castor::Path{ test.name + "_ref.png" };
	}

	wxString makeWxString( std::string const & in )
	{
		return wxString{ in.c_str(), wxMBConvUTF8{} };
	}

	std::string makeStdString( wxString const & in )
	{
		return in.char_str( wxMBConvUTF8{} ).data();
	}

	wxDateTime makeWxDateTime( db::DateTime const & in )
	{
		int year = int( in.date().year() );
		int month = int( in.date().month() ) - 1;
		int day = int( in.date().day() );
		int hour = int( in.time_of_day().hours() );
		int minute = int( in.time_of_day().minutes() );
		int second = int( in.time_of_day().seconds() );
		return wxDateTime{ wxDateTime::wxDateTime_t( day )
			, wxDateTime::Month( month )
			, wxDateTime::wxDateTime_t( year )
			, wxDateTime::wxDateTime_t( hour )
			, wxDateTime::wxDateTime_t( minute )
			, wxDateTime::wxDateTime_t( second ) };
	}

	db::DateTime makeDbDateTime( wxDateTime const & in )
	{
		int monthDay = in.GetDay();
		int month = in.GetMonth() + 1;
		int year = in.GetYear();
		int hour = in.GetHour();
		int min = in.GetMinute();
		int sec = in.GetSecond();
		return db::DateTime{ db::Date( year, month, monthDay )
			, db::Time( hour, min, sec ) };
	}

	db::DateTime getFileDate( castor::Path const & imgPath )
	{
		wxStructStat strucStat;
		wxStat( makeWxString( imgPath ), &strucStat );
		return makeDbDateTime( wxDateTime{ strucStat.st_mtime } );
	}

	castor::Path getFolderName( db::DateTime const & value )
	{
		return castor::Path{ db::date_time::format( value, FOLDER_DATETIME ) };
	}

	bool isDateTime( castor::String const & value
		, db::DateTime & result )
	{
		return db::date_time::isDateTime( value, FOLDER_DATETIME, result );
	}

	std::string getDetails( Test const & test )
	{
		return test.category
			+ " - " + test.renderer
			+ " - " + test.name
			+ " - " + db::date_time::format( test.runDate, FOLDER_DATETIME );
	}

	castor::PathArray findTestResults( Test const & test
		, castor::Path const & work )
	{
		std::vector< TestStatus > statuses
		{
			TestStatus::eNegligible,
			TestStatus::eAcceptable,
			TestStatus::eUnacceptable,
			TestStatus::eUnprocessed,
		};
		castor::File::FilterFunction filterFile = [&test]( castor::Path const & folder
			, castor::String const & name )
		{
			return name.find( test.name ) == ( FOLDER_DATETIME_SIZE + 1u );
		};
		castor::PathArray result;
		auto baseFolder = work / getResultFolder( test, false );

		for ( auto & status : statuses )
		{
			auto matches = castor::File::filterDirectoryFiles( baseFolder / getFolderName( status )
				, filterFile
				, true );
			result.insert( result.end(), matches.begin(), matches.end() );
		}

		return result;
	}
}
