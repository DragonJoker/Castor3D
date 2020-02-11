/* See LICENSE file in root folder */
#ifndef ___C3D_OCEAN_LIGHTING_APPLICATION_H___
#define ___C3D_OCEAN_LIGHTING_APPLICATION_H___

#include "Technique.hpp"
#include "MainFrame.hpp"

#include <wx/app.h>

namespace OceanLighting
{
	class Application
		: public wxApp
	{
	private:
		MainFrame * m_pMainFrame{ nullptr };
		wxLocale * m_pLocale{ nullptr };

	public:
		Application() {}
		virtual ~Application() {}

		virtual bool OnInit();
		virtual int OnExit();
	};
}

#endif
