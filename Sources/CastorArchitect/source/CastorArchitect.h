#ifndef ___CastorArchitect___
#define ___CastorArchitect___

namespace CastorArchitect
{
	class MainFrame;

	class CastorArchitectApp : public wxApp
	{
	private:
		MainFrame * m_mainFrame;

	public:
		virtual bool OnInit();

		inline MainFrame * GetMainFrame() {return m_mainFrame;}
	};
}

#endif