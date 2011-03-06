#ifndef ___GC_RendererSelector___
#define ___GC_RendererSelector___

#define CV_IMG_CASTOR	1268

namespace GuiCommon
{
	class RendererSelector : public wxDialog
	{
	private:
		enum
		{
			eOk,
			eCancel,
			eList,
		};

	private:
		wxBitmap * m_pBmpCastor;
		wxPanel * m_pPanel;
		wxListBox * m_pListRenderers;
		wxButton * m_pBtnOk;
		wxButton * m_pBtnCancel;

	public:
		RendererSelector( wxWindow * p_pParent, const wxString & p_strTitle);
		virtual ~RendererSelector();

	private:
		void _draw( wxDC * p_pDC);

	protected:
		DECLARE_EVENT_TABLE()
		void _onPaint( wxPaintEvent & p_event);
		void _onButtonOk( wxCommandEvent & p_event);
		void _onButtonCancel( wxCommandEvent & p_event);
	};
}

#endif