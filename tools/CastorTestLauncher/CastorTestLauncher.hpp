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
		c3d::EngineUPtr doInitialiseCastor()const;
		void doRunTest( c3d::Engine & engine );

		bool OnInit() override;
		int OnRun() override;

	private:
		struct Config
		{
			bool validate{ false };
			bool generate{ false };
			uint32_t maxFrameCount{ 10u };
			c3d::LogType log{ c3d::LogType::eInfo };
			c3d::String renderer;
			c3d::Path fileName;
			bool disableUpdateOptimisations{ false };
			bool disableRandom{ false };
		};

		MainFrame * m_mainFrame{ nullptr };
		Config m_config;
		c3d::String m_outputFileSuffix;
	};
}

wxDECLARE_APP( test_launcher::CastorTestLauncher );

#endif
