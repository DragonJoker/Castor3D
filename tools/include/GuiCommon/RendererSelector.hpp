#ifndef ___GC_RendererSelector___
#define ___GC_RendererSelector___

#define CV_IMG_CASTOR	1268

namespace GuiCommon
{
	class wxRendererSelector : public wxDialog
	{
	private:
		typedef enum eID
		{	eID_BUTTON_OK
		,	eID_BUTTON_CANCEL
		,	eID_LIST_RENDERERS
		}	eID;

	private:
		wxImage *			m_pImgCastor;
		wxListBox *			m_pListRenderers;
		wxButton *			m_pBtnOk;
		wxButton *			m_pBtnCancel;
		Castor3D::Engine *	m_pEngine;

	public:
		wxRendererSelector( Castor3D::Engine * p_pEngine, wxWindow * p_pParent, wxString const & p_strTitle );
		virtual ~wxRendererSelector();

		Castor3D::eRENDERER_TYPE GetSelectedRenderer()const;

	private:
		void DoDraw( wxDC * p_pDC);
		void DoSelect();

	protected:
		DECLARE_EVENT_TABLE()
		void OnPaint(			wxPaintEvent &		p_event );
		void OnKeyUp(			wxKeyEvent &		p_event );
		void OnButtonOk(		wxCommandEvent &	p_event );
		void OnButtonCancel(	wxCommandEvent &	p_event );
	};
}

#endif
