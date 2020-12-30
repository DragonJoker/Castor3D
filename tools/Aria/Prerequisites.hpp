/*
See LICENSE file in root folder
*/
#ifndef ___CTP_Prerequisites_HPP___
#define ___CTP_Prerequisites_HPP___

#include "Database/DbPrerequisites.hpp"

#include <wx/colour.h>
#include <wx/datetime.h>
#include <wx/string.h>

namespace aria
{
	class CategoryPanel;
	class LayeredPanel;
	class MainFrame;
	class TestDatabase;
	class TestPanel;
	class TreeModelNode;
	class TreeModel;

	static const wxColour PANEL_BACKGROUND_COLOUR = wxColour( 30, 30, 30 );
	static const wxColour PANEL_FOREGROUND_COLOUR = wxColour( 220, 220, 220 );
	static const wxColour BORDER_COLOUR = wxColour( 90, 90, 90 );
	static const wxColour INACTIVE_TAB_COLOUR = wxColour( 60, 60, 60 );
	static const wxColour INACTIVE_TEXT_COLOUR = wxColour( 200, 200, 200 );
	static const wxColour ACTIVE_TAB_COLOUR = wxColour( 51, 153, 255, 255 );
	static const wxColour ACTIVE_TEXT_COLOUR = wxColour( 255, 255, 255, 255 );

	typedef std::vector< TreeModelNode * > TreeModelNodePtrArray;

	struct Config
	{
		castor::Path test;
		castor::Path work;
		castor::Path database;
		castor::Path launcher;
		castor::Path viewer;
		castor::Path differ;
		castor::Path castor;
		db::DateTime castorRefDate;
		std::vector< castor::String > renderers{ cuT( "vk" ), cuT( "gl" ), cuT( "d3d11" ) };
		bool skip{};
	};

	enum class TestStatus
	{
		eNotRun,
		eNegligible,
		eAcceptable,
		eUnacceptable,
		eUnprocessed,
		eRunning_0,
		eRunning_1,
		eRunning_2,
		eRunning_3,
		eRunning_4,
		eRunning_5,
		eRunning_6,
		eRunning_7,
		eRunning_8,
		eRunning_9,
		eRunning_10,
		eRunning_11,
		eCount,
		eRunning_Begin = eRunning_0,
		eRunning_End = eRunning_11,
	};
	static bool isRunning( TestStatus value )
	{
		return value >= TestStatus::eRunning_Begin
			&& value <= TestStatus::eRunning_End;
	}
	castor::Path getFolderName( TestStatus value );
	TestStatus getStatus( std::string const & name );

	struct Test
	{
		uint32_t id;
		std::string name;
		db::DateTime runDate;
		TestStatus status;
		std::string renderer;
		std::string category;
		bool ignoreResult;
		db::DateTime castorDate;
		db::DateTime sceneDate;
	};
	static constexpr size_t IgnoredIndex = 0u;
	static constexpr size_t AdditionalIndices = 1u;

	db::DateTime getSceneDate( Config const & config, Test const & test );
	bool isOutOfCastorDate( Config const & config, Test const & test );
	bool isOutOfSceneDate( Config const & config, Test const & test );
	bool isOutOfDate( Config const & config, Test const & test );
	void updateCastorRefDate( Config & config );
	uint32_t getTestStatusIndex( Config const & config
		, Test const & test );
	castor::Path getSceneFile( Test const & test );
	castor::Path getResultFolder( Test const & test, bool useStatus = true );
	castor::Path getResultName( Test const & test );
	castor::Path getCompareFolder( Test const & test, bool useStatus = true );
	castor::Path getCompareName( Test const & test );
	castor::Path getReferenceFolder( Test const & test );
	castor::Path getReferenceName( Test const & test );

	using TestArray = std::vector< Test >;
	using TestCategoryMap = std::map< std::string, TestArray >;
	using TestMap = std::map< std::string, TestCategoryMap >;

	struct TestNode
	{
		Test * test;
		TestStatus status;
		TreeModelNode * node;
	};

	wxString makeWxString( std::string const & in );
	std::string makeStdString( wxString const & in );
	wxDateTime makeWxDateTime( db::DateTime const & in );
	db::DateTime makeDbDateTime( wxDateTime const & in );

	db::DateTime getFileDate( castor::Path const & imgPath );
	castor::Path getFolderName( db::DateTime const & value );
	bool isDateTime( castor::String const & value
		, db::DateTime & result );
	castor::PathArray findTestResults( Test const & test
		, castor::Path const & work );
	std::string getDetails( Test const & test );
}

#endif
