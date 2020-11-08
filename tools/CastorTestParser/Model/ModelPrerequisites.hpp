/*
See LICENSE file in root folder
*/
#ifndef ___CTP_Prerequisites_HPP___
#define ___CTP_Prerequisites_HPP___

#include "DbPrerequisites.hpp"

#include <wx/datetime.h>
#include <wx/string.h>

namespace test_parser
{
	class TreeModelNode;
	class TreeModel;

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
