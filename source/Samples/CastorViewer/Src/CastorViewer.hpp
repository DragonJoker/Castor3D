#ifndef ___CastorViewer___
#define ___CastorViewer___

#include <wx/app.h>

namespace CastorViewer
{
	class MainFrame;

	class CastorViewerApp
		: public wxApp
	{
	private:
		MainFrame * m_pMainFrame;
		std::unique_ptr< wxLocale > m_pLocale;

	public:
		virtual bool OnInit();
		virtual int OnExit();

		inline MainFrame * GetMainFrame()const
		{
			return m_pMainFrame;
		}
	};
}

#endif
