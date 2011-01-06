#ifndef ___GC_EnvironmentFrame___
#define ___GC_EnvironmentFrame___

namespace GUICommon
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
		Array <Castor3D::TextureEnvironment::eCOMBINATION_SOURCE, 3>::Value m_RGBSources;
		Array <int, 3>::Value m_RGBTextures;
		wxComboBox * m_RGBOperandsCB[3];
		Array <Castor3D::TextureEnvironment::eRGB_OPERAND, 3>::Value m_RGBOperands;

		wxComboBox * m_AlphaSourcesCB[3];
		Array<Castor3D::TextureEnvironment::eCOMBINATION_SOURCE, 3>::Value m_AlphaSources;
		Array <int, 3>::Value m_AlphaTextures;
		wxComboBox * m_AlphaOperandsCB[3];
		Array <Castor3D::TextureEnvironment::eALPHA_OPERAND, 3>::Value m_AlphaOperands;

	public:
		EnvironmentFrame( wxWindow * parent, const wxString & title, Castor3D::PassPtr p_pass,
							Castor3D::TextureEnvironmentPtr p_env, const wxPoint & pos = wxDefaultPosition,
							const wxSize & size = wxSize( 200, 200));
		~EnvironmentFrame();

	private:
		void _createRGBSourceCubeBox( unsigned int p_index, int p_currentTop);
		void _createRGBOperandCubeBox( unsigned int p_index, int p_currentTop);

		void _createAlphaSourceCubeBox( unsigned int p_index, int p_currentTop);
		void _createAlphaOperandCubeBox( unsigned int p_index, int p_currentTop);

		void _onRGBSourceSelect( size_t p_uiIndex);
		void _onRGBOperandSelect( size_t p_uiIndex);
		void _onAlphaSourceSelect( size_t p_uiIndex);
		void _onAlphaOperandSelect( size_t p_uiIndex);

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