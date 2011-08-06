#ifndef ___GC_FrameVariableDialog___
#define ___GC_FrameVariableDialog___

namespace GuiCommon
{
	class wxFrameVariableDialog : public wxDialog
	{
	private:
		typedef enum
		{	eID_BUTTON_OK
		,	eID_BUTTON_CANCEL
		,	eID_COMBO_TYPE
		}	eID;

	protected:
		Castor3D::FrameVariablePtr m_pFrameVariable;
		Castor3D::ShaderProgramPtr m_pProgram;
		Castor3D::ShaderObjectPtr m_pObject;
		bool m_bOwn;

		wxComboBox * m_pComboType;
		wxTextCtrl * m_pEditName;
		wxTextCtrl * m_pEditValue;

	public:
		wxFrameVariableDialog( wxWindow * p_pParent, Castor3D::ShaderProgramPtr p_pProgram, Castor3D::ShaderObjectPtr p_pObject, Castor3D::FrameVariablePtr p_pFrameVariable=Castor3D::FrameVariablePtr());
		~wxFrameVariableDialog();

		inline Castor3D::FrameVariablePtr GetFrameVariable() { return m_pFrameVariable; }

	private:
		DECLARE_EVENT_TABLE()
		void _onClose			( wxCloseEvent & event);
		void _onSelectType		( wxCommandEvent & event);
		void _onOk				( wxCommandEvent & event);
		void _onCancel			( wxCommandEvent & event);
	};
}

#endif
