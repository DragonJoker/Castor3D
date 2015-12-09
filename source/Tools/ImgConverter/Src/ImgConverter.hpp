#ifndef ___ImgToIcoApp___
#define ___ImgToIcoApp___

#include <wx/app.h>
#include "MainFrame.hpp"

namespace ImgToIco
{
	class MainFrame;

	class ImgToIcoApp
		: public wxApp
	{
	private:
		MainFrame * m_mainFrame;
		wxLocale * m_pLocale;

	public:
		ImgToIcoApp(){}
		virtual ~ImgToIcoApp(){}
		virtual bool OnInit();
		virtual int OnExit();
	};
}

#endif
