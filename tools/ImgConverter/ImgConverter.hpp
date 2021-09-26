/* See LICENSE file in root folder */
#ifndef ___ImgToIcoApp___
#define ___ImgToIcoApp___

#include "MainFrame.hpp"

#pragma warning( push )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4371 )
#include <wx/app.h>
#pragma warning( pop )

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
		bool OnInit()override;
		int OnExit()override;
	};
}

DECLARE_APP( ImgToIco::ImgToIcoApp )

#endif
