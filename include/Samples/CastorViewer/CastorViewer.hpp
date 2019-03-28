/*
See LICENSE file in root folder
*/
#ifndef ___CV_CASTOR_VIEWER_H___
#define ___CV_CASTOR_VIEWER_H___

#include <GuiCommon/CastorApplication.hpp>

namespace CastorViewer
{
	class MainFrame;

	class CastorViewerApp
		: public GuiCommon::CastorApplication
	{
	public:
		CastorViewerApp();

		inline MainFrame * getMainFrame()const
		{
			return m_mainFrame;
		}

	private:
		virtual void doLoadAppImages();
		virtual wxWindow * doInitialiseMainFrame( GuiCommon::SplashScreen * p_splashScreen );

	private:
		MainFrame * m_mainFrame;
		std::unique_ptr< wxLocale > m_locale;
	};
}

wxDECLARE_APP( CastorViewer::CastorViewerApp );

#endif
