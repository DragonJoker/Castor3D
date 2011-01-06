#ifndef ___CastorViewer___
#define ___CastorViewer___

namespace CastorViewer
{
	class MainFrame;

	class CastorViewerApp : public wxApp
	{
	private:
		MainFrame * m_mainFrame;

	public:
		virtual bool OnInit();
		virtual int OnExit();

		inline MainFrame * GetMainFrame() {return m_mainFrame;}
	};
}

#endif