#ifndef ___ImgToXpmApp___
#define ___ImgToXpmApp___

namespace ImgToXpm
{
	class MainFrame;

	class ImgToXpmApp : public wxApp
	{
	private:
		MainFrame * m_mainFrame;

	public:
		virtual bool OnInit();
	};
}

#endif
