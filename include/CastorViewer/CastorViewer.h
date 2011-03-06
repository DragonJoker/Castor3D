#ifndef ___CastorViewer___
#define ___CastorViewer___

namespace CastorViewer
{
	class MainFrame;

	class CastorViewerApp : public wxApp
	{
	private:
		MainFrame * m_mainFrame;
		SplashScreen * m_pSplash;

	public:
		virtual bool OnInit();
		virtual int OnExit();
		void DestroySplashScreen();

		inline MainFrame	*	GetMainFrame	()const	{ return m_mainFrame; }
		inline SplashScreen *	GetSplashScreen	()const	{ return m_pSplash; }
	};
}

#endif