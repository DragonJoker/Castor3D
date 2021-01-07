#include "Aria/Prerequisites.hpp"

#include "Aria/Database/DbDateTimeHelpers.hpp"
#include "Aria/TestDatabase.hpp"

#include <CastorUtils/Data/File.hpp>
#include <CastorUtils/Miscellaneous/Utils.hpp>

#include <wx/filefn.h>

namespace aria
{
	namespace
	{
		static const std::string FOLDER_DATETIME = "%Y-%m-%d_%H-%M-%S";
		static constexpr size_t FOLDER_DATETIME_SIZE = 4u + 3u + 3u + 3u + 3u + 3u;
	}

	size_t HashNoCase::operator()( std::string const & v )const
	{
		return std::hash< std::string >{}( castor::string::lowerCase( v ) );
	}

	bool LessNoCase::operator()( const char lhs, const char rhs )const
	{
		return operator()( &lhs, &rhs, 1 );
	}

	bool LessNoCase::operator()( const char * lhs, const char * rhs, size_t minSize )const
	{
		return strnicmp( lhs, rhs, minSize ) < 0;
	}

	bool LessNoCase::operator()( const char * lhs, const char * rhs )const
	{
		return operator()( lhs, rhs, std::min( strlen( lhs ), strlen( rhs ) ) );
	}

	bool LessNoCase::operator()( std::string const & lhs, std::string const & rhs )const
	{
		return operator()( lhs.data(), rhs.data(), std::min( lhs.size(), rhs.size() ) );
	}

	bool EqualNoCase::operator()( const char lhs, const char rhs )const
	{
		return operator()( &lhs, &rhs, 1 );
	}
	
	bool EqualNoCase::operator()( const char * lhs, const char * rhs, size_t minSize )const
	{
		return strnicmp( lhs, rhs, minSize ) == 0;
	}

	bool EqualNoCase::operator()( const char * lhs, const char * rhs )const
	{
		return operator()( lhs, rhs, std::min( strlen( lhs ), strlen( rhs ) ) );
	}

	bool EqualNoCase::operator()( std::string const & lhs, std::string const & rhs )const
	{
		return operator()( lhs.data(), rhs.data(), std::min( lhs.size(), rhs.size() ) );
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

		if ( it == folders.end() )
		{
			return TestStatus::eUnprocessed;
		}

		return it->second;
	}

	uint32_t StatusName::getStatusIndex( bool ignoreResult
		, TestStatus status )
	{
		uint32_t result{};

		if ( ( !ignoreResult )
			|| ( !isPending( status ) && !isRunning( status ) ) )
		{
			result = size_t( status ) + AdditionalIndices;
		}
		else if ( ignoreResult )
		{
			result = IgnoredIndex;
		}

		return ( result << 2 );
	}

	uint32_t StatusName::getTestStatusIndex( Config const & config
		, TestRun const & test )
	{
		return getStatusIndex( test.test->ignoreResult, test.status )
			| ( isOutOfCastorDate( config, test )
				? 0x01u
				: 0x00u )
			| ( isOutOfSceneDate( config, test )
				? 0x02u
				: 0x00u );
	}

	db::DateTime getSceneDate( Config const & config, Test const & test )
	{
		return getFileDate( config.test / getSceneFile( test ) );
	}

	db::DateTime getSceneDate( Config const & config, TestRun const & test )
	{
		return getFileDate( config.test / getSceneFile( test ) );
	}

	bool isOutOfCastorDate( Config const & config, TestRun const & test )
	{
		return ( !db::date_time::isValid( test.castorDate ) )
			|| test.castorDate < getFileDate( config.castor );
	}

	bool isOutOfSceneDate( Config const & config, TestRun const & test )
	{
		return ( !db::date_time::isValid( test.sceneDate ) )
			|| test.sceneDate < getSceneDate( config, test );
	}

	bool isOutOfDate( Config const & config, TestRun const & test )
	{
		return isOutOfSceneDate( config, test )
			|| isOutOfCastorDate( config, test );
	}

	void updateCastorRefDate( Config & config )
	{
		config.castorRefDate = getFileDate( config.castor );
		assert( db::date_time::isValid( config.castorRefDate ) );
	}

	castor::Path getSceneFile( Test const & test )
	{
		return castor::Path{ test.category->name } / ( test.name + ".cscn" );
	}

	castor::Path getSceneFile( TestRun const & test )
	{
		return getSceneFile( *test.test );
	}

	castor::Path getResultFolder( Test const & test )
	{
		return castor::Path{ "Result" } / test.category->name;
	}

	castor::Path getResultFolder( TestRun const & test )
	{
		return getResultFolder( *test.test ) / getFolderName( test.status );
	}

	castor::Path getResultName( TestRun const & test )
	{
		return castor::Path{ getFolderName( test.runDate ) + "_" + test.test->name + "_" + test.renderer->name + ".png" };
	}

	castor::Path getCompareFolder( Test const & test )
	{
		return castor::Path{ test.category->name } / cuT( "Compare" );
	}

	castor::Path getCompareFolder( TestRun const & test )
	{
		return getCompareFolder( *test.test ) / getFolderName( test.status );
	}

	castor::Path getCompareName( TestRun const & test )
	{
		return castor::Path{ test.test->name + "_" + test.renderer->name + ".png" };
	}

	castor::Path getReferenceFolder( Test const & test )
	{
		return castor::Path{ test.category->name };
	}

	castor::Path getReferenceFolder( TestRun const & test )
	{
		return getReferenceFolder( *test.test );
	}

	castor::Path getReferenceName( Test const & test )
	{
		return castor::Path{ test.name + "_ref.png" };
	}

	castor::Path getReferenceName( TestRun const & test )
	{
		return getReferenceName( *test.test );
	}

	std::string getDetails( Test const & test )
	{
		return test.category->name
			+ " - " + test.name;
	}

	wxString getProgressDetails( Test const & test )
	{
		return getProgressDetails( test.category->name, test.name );
	}

	wxString getProgressDetails( wxString const & catName
		, wxString const & testName )
	{
		return wxT( "- Category: " ) + catName
			+ wxT( "\n" ) + wxT( "- Test: " ) + testName;
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
		auto baseFolder = work / getResultFolder( test );

		for ( auto & status : statuses )
		{
			auto matches = castor::File::filterDirectoryFiles( baseFolder / getFolderName( status )
				, filterFile
				, true );
			result.insert( result.end(), matches.begin(), matches.end() );
		}

		return result;
	}

	std::string getDetails( TestRun const & test )
	{
		return test.renderer->name
			+ " - " + getDetails( *test.test )
			+ " - " + ( db::date_time::isValid( test.runDate )
				? db::date_time::format( test.runDate, DISPLAY_DATETIME )
				: std::string{} );
	}

	wxString getProgressDetails( DatabaseTest const & test )
	{
		return getProgressDetails( makeWxString( test.getCategory()->name )
			, makeWxString( test.getName() )
			, makeWxString( test->renderer->name )
			, test->runDate );
	}

	wxString getProgressDetails( wxString const & catName
		, wxString const & testName
		, wxString const & rendName
		, db::DateTime const & runDate )
	{
		return wxT( "- Renderer: " ) + rendName
			+ wxT( "\n" ) + getProgressDetails( catName, testName )
			+ wxT( "\n- Run date: " ) + ( db::date_time::isValid( runDate )
				? db::date_time::format( runDate, DISPLAY_DATETIME )
				: std::string{} );
	}

	castor::PathArray findTestResults( TestRun const & test
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
			return name.find( test.test->name ) == ( FOLDER_DATETIME_SIZE + 1u );
		};
		castor::PathArray result;
		auto baseFolder = work / getResultFolder( *test.test );

		for ( auto & status : statuses )
		{
			auto matches = castor::File::filterDirectoryFiles( baseFolder / getFolderName( status )
				, filterFile
				, true );
			result.insert( result.end(), matches.begin(), matches.end() );
		}

		return result;
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
		if ( !db::date_time::isValid( in ) )
		{
			return wxDateTime::Today();
		}

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
		if ( !in.IsValid() )
		{
			return db::DateTime{};
		}

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
		return db::date_time::isValid( value )
			? castor::Path{ db::date_time::format( value, FOLDER_DATETIME ) }
			: castor::Path{};
	}

	bool isDateTime( castor::String const & value
		, db::DateTime & result )
	{
		return db::date_time::isDateTime( value, FOLDER_DATETIME, result );
	}
}
