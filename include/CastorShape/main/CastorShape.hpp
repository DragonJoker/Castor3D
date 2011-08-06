#ifndef ___CASTORSHAPE___
#define ___CASTORSHAPE___

#include "Module_GUI.hpp"

namespace CastorShape
{
	class CastorShapeApp : public wxApp
	{
	private:
		MainFrame * m_mainFrame;
		GuiCommon::wxSplashScreen * m_pSplash;

	public:
		virtual bool OnInit();
		virtual int OnExit();
		void DestroySplashScreen();

		inline MainFrame	*	GetMainFrame	()const	{ return m_mainFrame; }
		inline GuiCommon::wxSplashScreen *	GetSplashScreen	()const	{ return m_pSplash; }
	};
}

#endif
