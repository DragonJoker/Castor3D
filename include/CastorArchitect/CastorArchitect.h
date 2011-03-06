#ifndef ___CastorArchitect___
#define ___CastorArchitect___

namespace CastorArchitect
{
	class MainFrame;

	class CastorArchitectApp : public wxApp
	{
	private:
		MainFrame * m_mainFrame;
		SplashScreen * m_pSplash;

	public:
		virtual bool OnInit();
		void DestroySplashScreen();

		inline MainFrame	*	GetMainFrame	()const	{return m_mainFrame;}
		inline SplashScreen *	GetSplashScreen	()const	{ return m_pSplash; }
	};
}

#endif