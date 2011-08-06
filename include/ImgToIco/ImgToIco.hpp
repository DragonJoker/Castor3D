#ifndef ___ImgToIcoApp___
#define ___ImgToIcoApp___

namespace ImgToIco
{
	class MainFrame;

	class ImgToIcoApp : public wxApp
	{
	private:
		MainFrame * m_mainFrame;

	public:
		virtual bool OnInit();
		virtual int OnExit();
	};
}

#endif
