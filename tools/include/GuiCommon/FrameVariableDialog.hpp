#ifndef ___GC_FrameVariableDialog___
#define ___GC_FrameVariableDialog___

namespace GuiCommon
{
	class wxFrameVariableDialog : public wxDialog
	{
	private:
		typedef enum eID
		{	eID_BUTTON_OK
		,	eID_BUTTON_CANCEL
		,	eID_COMBO_TYPE
		}	eID;

	protected:
		Castor3D::FrameVariableWPtr		m_pFrameVariable;
		Castor3D::ShaderProgramBaseWPtr	m_pProgram;
		bool							m_bOwn;
		wxComboBox *					m_pComboType;
		wxTextCtrl *					m_pEditName;
		wxTextCtrl *					m_pEditValue;
		Castor3D::eSHADER_TYPE			m_eTargetStage;

	public:
		wxFrameVariableDialog( wxWindow * p_pParent, Castor3D::ShaderProgramBaseSPtr p_pProgram, Castor3D::eSHADER_TYPE p_eTargetStage, Castor3D::FrameVariableSPtr p_pFrameVariable=nullptr );
		~wxFrameVariableDialog();

		inline Castor3D::FrameVariableSPtr GetFrameVariable() { return m_pFrameVariable.lock(); }

	private:
		DECLARE_EVENT_TABLE()
		void OnClose		( wxCloseEvent &	p_event );
		void OnSelectType	( wxCommandEvent &	p_event );
		void OnOk			( wxCommandEvent &	p_event );
		void OnCancel		( wxCommandEvent &	p_event );
	};
}

#endif
