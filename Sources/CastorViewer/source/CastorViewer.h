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

		inline MainFrame * GetMainFrame() {return m_mainFrame;}
	};
}

#endif