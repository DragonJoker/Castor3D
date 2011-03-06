#ifndef ___CASTORSHAPE___
#define ___CASTORSHAPE___

#include "Module_GUI.h"

namespace CastorShape
{
	class CastorShapeApp : public wxApp
	{
	private:
		MainFrame * m_mainFrame;
		GuiCommon::SplashScreen * m_pSplash;

	public:
		virtual bool OnInit();
		void DestroySplashScreen();

		inline MainFrame	*	GetMainFrame	()const	{ return m_mainFrame; }
		inline GuiCommon::SplashScreen *	GetSplashScreen	()const	{ return m_pSplash; }
	};
}

#endif
