/*
See LICENSE file in root folder
*/
#ifndef ___CTP_Aria_HPP___
#define ___CTP_Aria_HPP___

#include <CastorUtils/Data/Path.hpp>

#include <wx/app.h>

namespace aria
{
	class MainFrame;
	struct Config;

	class Aria
		: public wxApp
	{
	public:
		Aria();

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

wxDECLARE_APP( aria::Aria );

#endif
