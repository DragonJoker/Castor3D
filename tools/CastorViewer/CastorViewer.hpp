/*
See LICENSE file in root folder
*/
#ifndef ___CV_CASTOR_VIEWER_H___
#define ___CV_CASTOR_VIEWER_H___

#include <GuiCommon/System/CastorApplication.hpp>

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
		void doLoadAppImages()override;
		wxWindow * doInitialiseMainFrame( GuiCommon::SplashScreen & splashScreen )override;

	private:
		MainFrame * m_mainFrame;
		castor::RawUniquePtr< wxLocale > m_locale;
	};
}

wxDECLARE_APP( CastorViewer::CastorViewerApp );

#endif
