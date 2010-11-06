#ifndef ___CA_UniformVariableDialog___
#define ___CA_UniformVariableDialog___

namespace CastorArchitect
{
	class UniformVariableDialog : public wxDialog
	{
	private:
		enum eIDs
		{
			eOK,
			eCancel,
			eType
		};

	protected:
		Castor3D::UniformVariablePtr m_pUniformVariable;
		bool m_bOwn;

		wxComboBox * m_pComboType;
		wxTextCtrl * m_pEditName;
		wxTextCtrl * m_pEditValue;

	public:
		UniformVariableDialog( wxWindow * p_pParent, Castor3D::UniformVariablePtr p_pUniformVariable=NULL);
		~UniformVariableDialog();

		inline Castor3D::UniformVariablePtr GetUniformVariable() { return m_pUniformVariable; }

	private:
		DECLARE_EVENT_TABLE()
		void _onClose			( wxCloseEvent & event);
		void _onSelectType		( wxCommandEvent & event);
		void _onOk				( wxCommandEvent & event);
		void _onCancel			( wxCommandEvent & event);
	};
}

#endif