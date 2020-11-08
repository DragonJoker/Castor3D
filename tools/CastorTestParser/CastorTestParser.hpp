/*
See LICENSE file in root folder
*/
#ifndef ___CTP_CastorTestParser_HPP___
#define ___CTP_CastorTestParser_HPP___

#include <CastorUtils/Data/Path.hpp>

#include <wx/app.h>

namespace test_parser
{
	class MainFrame;
	struct Config;

	class CastorTestParser
		: public wxApp
	{
	public:
		CastorTestParser();

		inline MainFrame * getMainFrame()const
		{
			return m_mainFrame;
		}

	private:
		bool doParseCommandLine( Config & config );

		bool OnInit()override;
		int OnExit()override;

	private:
		MainFrame * m_mainFrame{ nullptr };
	};
}

wxDECLARE_APP( test_parser::CastorTestParser );

#endif
