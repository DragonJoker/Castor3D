#ifndef ___ImgToIcoApp___
#define ___ImgToIcoApp___

namespace ImgToIco
{
	class MainFrame;

	class ImgToIcoApp : public wxApp
	{
	private:
		MainFrame * m_mainFrame;
		wxLocale * m_pLocale;

	public:
		virtual bool OnInit();
		virtual int OnExit();
	};
}

#endif
