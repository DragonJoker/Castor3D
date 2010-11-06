#ifndef ___CA_EnvironmentFrame___
#define ___CA_EnvironmentFrame___

namespace CastorArchitect
{
	class EnvironmentFrame : public wxFrame
	{
	private:
		enum eIDs
		{
			eOK,
			eCancel,
			eRGBSrc0,
			eRGBSrc1,
			eRGBSrc2,
			eRGBOperand0,
			eRGBOperand1,
			eRGBOperand2,
			eAlphaSrc0,
			eAlphaSrc1,
			eAlphaSrc2,
			eAlphaOperand0,
			eAlphaOperand1,
			eAlphaOperand2,
		};

	private:
		Castor3D::TextureEnvironmentPtr m_environment;
		Castor3D::PassPtr m_pass;

		wxButton * m_cancelButton;
		wxButton * m_OKButton;
		wxComboBox * m_RGBSourcesCB[3];
		Castor3D::CombinationSource m_RGBSources[3];
		int m_RGBTextures[3];
		wxComboBox * m_RGBOperandsCB[3];
		Castor3D::RGBOperand m_RGBOperands[3];

		wxComboBox * m_AlphaSourcesCB[3];
		Castor3D::CombinationSource m_AlphaSources[3];
		int m_AlphaTextures[3];
		wxComboBox * m_AlphaOperandsCB[3];
		Castor3D::AlphaOperand m_AlphaOperands[3];

	public:
		EnvironmentFrame( wxWindow * parent, const wxString & title, Castor3D::PassPtr p_pass,
							Castor3D::TextureEnvironmentPtr p_env, const wxPoint & pos = wxDefaultPosition,
							const wxSize & size = wxSize( 200, 200));
		~EnvironmentFrame();

	private:
		void _createRGBSourceComboBox( unsigned int p_index, int p_currentTop);
		void _createRGBOperandComboBox( unsigned int p_index, int p_currentTop);

		void _createAlphaSourceComboBox( unsigned int p_index, int p_currentTop);
		void _createAlphaOperandComboBox( unsigned int p_index, int p_currentTop);

	private:
		void _onOK							( wxCommandEvent & event);
		void _onCancel						( wxCommandEvent & event);

		void _onRGBSource0Select			( wxCommandEvent & event);
		void _onRGBSource1Select			( wxCommandEvent & event);
		void _onRGBSource2Select			( wxCommandEvent & event);
		void _onRGBOperand0Select			( wxCommandEvent & event);
		void _onRGBOperand1Select			( wxCommandEvent & event);
		void _onRGBOperand2Select			( wxCommandEvent & event);

		void _onAlphaSource0Select			( wxCommandEvent & event);
		void _onAlphaSource1Select			( wxCommandEvent & event);
		void _onAlphaSource2Select			( wxCommandEvent & event);
		void _onAlphaOperand0Select			( wxCommandEvent & event);
		void _onAlphaOperand1Select			( wxCommandEvent & event);
		void _onAlphaOperand2Select			( wxCommandEvent & event);

		DECLARE_EVENT_TABLE()
	};
}

#endif