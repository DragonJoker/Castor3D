#ifndef ___CastorViewer___
#define ___CastorViewer___

namespace CastorViewer
{
	class MainFrame;

	class CastorViewerApp : public wxApp
	{
	private:
		MainFrame *	m_pMainFrame;
		wxLocale *	m_pLocale;

	public:
		virtual bool OnInit();
		virtual int OnExit();

		inline MainFrame *	GetMainFrame	()const	{ return m_pMainFrame; }
	};
}

#endif