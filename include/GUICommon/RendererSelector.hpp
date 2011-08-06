#ifndef ___GC_RendererSelector___
#define ___GC_RendererSelector___

#define CV_IMG_CASTOR	1268

namespace GuiCommon
{
	class wxRendererSelector : public wxDialog
	{
	private:
		typedef enum
		{	eID_BUTTON_OK
		,	eID_BUTTON_CANCEL
		,	eID_LIST_RENDERERS
		}	eID;

	private:
		wxBitmap * m_pBmpCastor;
		wxListBox * m_pListRenderers;
		wxButton * m_pBtnOk;
		wxButton * m_pBtnCancel;

	public:
		wxRendererSelector( wxWindow * p_pParent, const wxString & p_strTitle);
		virtual ~wxRendererSelector();

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
