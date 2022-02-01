/* See LICENSE file in root folder */
#ifndef ___IX_MainFrame___
#define ___IX_MainFrame___

#pragma warning( push )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4371 )
#pragma warning( disable: 5054 )
#include <wx/frame.h>
#include <wx/listbox.h>
#include <wx/button.h>
#pragma warning( pop )

namespace ImgToIco
{
	class MainFrame
		: public wxFrame
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

	protected:
		DECLARE_EVENT_TABLE()
		void _onBrowse( wxCommandEvent & p_event );
		void _onProcess( wxCommandEvent & p_event );
		void _onExit( wxCommandEvent & p_event );
	};
}

#endif
