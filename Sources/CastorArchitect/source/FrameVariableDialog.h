#ifndef ___CA_FrameVariableDialog___
#define ___CA_FrameVariableDialog___

namespace CastorArchitect
{
	class FrameVariableDialog : public wxDialog
	{
	private:
		enum eIDs
		{
			eOK,
			eCancel,
			eType
		};

	protected:
		Castor3D::FrameVariablePtr m_pFrameVariable;
		bool m_bOwn;

		wxComboBox * m_pComboType;
		wxTextCtrl * m_pEditName;
		wxTextCtrl * m_pEditValue;

	public:
		FrameVariableDialog( wxWindow * p_pParent, Castor3D::FrameVariablePtr p_pFrameVariable=Castor3D::FrameVariablePtr());
		~FrameVariableDialog();

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