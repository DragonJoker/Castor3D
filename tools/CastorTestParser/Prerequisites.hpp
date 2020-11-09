/*
See LICENSE file in root folder
*/
#ifndef ___CTP_Prerequisites_HPP___
#define ___CTP_Prerequisites_HPP___

#include "Database/DbPrerequisites.hpp"

#include <wx/colour.h>
#include <wx/datetime.h>
#include <wx/string.h>

namespace test_parser
{
	class CategoryPanel;
	class LayeredPanel;
	class MainFrame;
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
		bool init{};
	};

	enum class TestStatus
	{
		eNotRun,
		eNegligible,
		eAcceptable,
		eUnacceptable,
		eUnprocessed,
		eCount,
	};
	std::string getFolderName( TestStatus value );

	struct Test
	{
		uint32_t id;
		std::string name;
		db::DateTime runDate;
		TestStatus status;
		std::string renderer;
		std::string category;
	};

	using TestArray = std::vector< Test >;
	using TestMap = std::map< std::string, TestArray >;

	wxString makeWxString( std::string const & in );
	std::string makeStdString( wxString const & in );
	wxDateTime makeWxDateTime( db::DateTime const & in );
	db::DateTime makeDbDateTime( wxDateTime const & in );

}

#endif
