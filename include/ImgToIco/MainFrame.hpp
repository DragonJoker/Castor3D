#ifndef ___IX_MainFrame___
#define ___IX_MainFrame___

namespace ImgToIco
{
	class MainFrame : public wxFrame
	{
	private:
		enum eIDs
		{
			eProcess,
			eBrowse,
			eExit,
		};

		wxListBox * m_pListImages;
		wxButton * m_pButtonProcess;
		wxButton * m_pButtonBrowse;
		wxButton * m_pButtonExit;

	public:
		MainFrame();
		~MainFrame();

	protected:
		DECLARE_EVENT_TABLE()
		void _onBrowse( wxCommandEvent & p_event);
		void _onProcess( wxCommandEvent & p_event);
		void _onExit( wxCommandEvent & p_event);
	};
}

#endif
