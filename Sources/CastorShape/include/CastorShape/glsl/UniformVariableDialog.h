#ifndef ___UniformVariableDialog___
#define ___UniformVariableDialog___

namespace CastorShape
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
		Castor3D::UniformVariable * m_pUniformVariable;
		bool m_bOwn;

		wxComboBox * m_pComboType;
		wxTextCtrl * m_pEditName;
		wxTextCtrl * m_pEditValue;

	public:
		UniformVariableDialog( wxWindow * p_pParent, Castor3D::UniformVariable * p_pUniformVariable=NULL);
		~UniformVariableDialog();

		inline Castor3D::UniformVariable * GetUniformVariable() { return m_pUniformVariable; }

	private:
		DECLARE_EVENT_TABLE()
		void _onClose			( wxCloseEvent & event);
		void _onSelectType		( wxCommandEvent & event);
		void _onOk				( wxCommandEvent & event);
		void _onCancel			( wxCommandEvent & event);
	};
}

#endif