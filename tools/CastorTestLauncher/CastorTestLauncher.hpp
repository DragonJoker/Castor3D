/*
See LICENSE file in root folder
*/
#ifndef ___CTL_CastorTestLauncher_HPP___
#define ___CTL_CastorTestLauncher_HPP___

#include <Castor3D/Castor3DModule.hpp>

#pragma warning( push )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4371 )
#include <wx/app.h>
#pragma warning( pop )

namespace test_launcher
{
	class MainFrame;

	class CastorTestLauncher
		: public wxApp
	{
	public:
		CastorTestLauncher();

		inline MainFrame * getMainFrame()const
		{
			return m_mainFrame;
		}

	private:
		bool doParseCommandLine();
		castor3d::EngineSPtr doInitialiseCastor();

		bool OnInit() override;
		int OnRun() override;

	private:
		struct Config
		{
			bool validate{ false };
			bool generate{ false };
			castor::LogType log{ castor::LogType::eInfo };
			castor::String renderer;
			castor::Path fileName;
		};

		MainFrame * m_mainFrame{ nullptr };
		Config m_config;
		castor::String m_outputFileSuffix;
	};
}

wxDECLARE_APP( test_launcher::CastorTestLauncher );

#endif
