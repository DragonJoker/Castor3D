#ifndef ___CASTORSHAPE___
#define ___CASTORSHAPE___

#include "Module_GUI.h"

namespace CastorShape
{
	class CastorShapeApp : public wxApp
	{
	private:
		MainFrame * m_mainFrame;

	public:
		virtual bool OnInit();

		inline MainFrame * GetMainFrame() {return m_mainFrame;}
	};
}

#endif
