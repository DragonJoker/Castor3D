#ifndef ___CASTORSHAPE___
#define ___CASTORSHAPE___

#include "Module_GUI.hpp"

namespace CastorShape
{
	class CastorShapeApp : public wxApp
	{
	private:
		MainFrame * m_pMainFrame;
		wxLocale *	m_pLocale;

	public:
		virtual bool OnInit();
		virtual int OnExit();

		inline MainFrame	*	GetMainFrame	()const	{ return m_pMainFrame; }
	};
}

#endif
