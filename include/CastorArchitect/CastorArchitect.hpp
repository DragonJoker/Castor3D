#ifndef ___CastorArchitect___
#define ___CastorArchitect___

namespace CastorArchitect
{
	class MainFrame;

	class CastorArchitectApp : public wxApp
	{
	private:
		MainFrame * m_mainFrame;
		wxSplashScreen * m_pSplash;

	public:
		virtual bool OnInit();
		virtual int OnExit();
		void DestroySplashScreen();

		inline MainFrame	*	GetMainFrame	()const	{return m_mainFrame;}
		inline wxSplashScreen *	GetSplashScreen	()const	{ return m_pSplash; }
	};
}

#endif