/*
See LICENSE file in root folder
*/
#ifndef ___CTL_CastorTestLauncher_HPP___
#define ___CTL_CastorTestLauncher_HPP___

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <wx/app.h>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

#include <Castor3D/Castor3DModule.hpp>

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
		castor3d::EngineUPtr doInitialiseCastor();

		bool OnInit() override;
		int OnRun() override;

	private:
		struct Config
		{
			bool validate{ false };
			bool generate{ false };
			uint32_t maxFrameCount{ 10u };
			castor::LogType log{ castor::LogType::eInfo };
			castor::String renderer;
			castor::Path fileName;
			bool disableUpdateOptimisations{ false };
			bool disableRandom{ false };
		};

		MainFrame * m_mainFrame{ nullptr };
		Config m_config;
		castor::String m_outputFileSuffix;
	};
}

wxDECLARE_APP( test_launcher::CastorTestLauncher );

#endif
