/*
See LICENSE file in root folder
*/
#ifndef ___CTL_CastorTestLauncher_HPP___
#define ___CTL_CastorTestLauncher_HPP___

#include <Castor3D/Castor3DPrerequisites.hpp>
#include <wx/app.h>

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

		bool OnInit()override;

	private:
		MainFrame * m_mainFrame{ nullptr };
		castor::String m_rendererType;
		castor::String m_outputFileSuffix;
		castor::Path m_fileName;
	};
}

wxDECLARE_APP( test_launcher::CastorTestLauncher );

#endif
