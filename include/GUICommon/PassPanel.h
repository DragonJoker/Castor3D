#ifndef ___GC_PassPanel___
#define ___GC_PassPanel___

namespace GUICommon
{
	class PassPanel : public wxScrolledWindow
	{
	public:
		enum IDs
		{
			eDeleteTextureUnit,
			eAmbient,
			eDiffuse,
			eSpecular,
			eEmissive,
			eShininess,
			eTextureUnits,
			eTexturePath,
			eTextureImage,
			eTextureEnvMode,
			eTextureBlendColour,
			eTextureRGBCombination,
			eTextureAlphaCombination,
			eEnvironmentConfig,
			eDoubleFace,
			eTextureMode,
			eAlpha,
			eHasShader,
		};

	private:
		Castor3D::PassPtr m_pass;
		unsigned int m_selectedUnitIndex;
		Castor3D::TextureUnitPtr m_selectedTextureUnit;
		wxImage m_selectedUnitImage;

		wxPanel * m_textureUnitPanel;
		wxBitmapButton * m_texImageButton;
		wxButton * m_deleteMaterial;
		wxButton * m_deleteTextureUnit;
		wxButton * m_ambientButton;
		wxButton * m_diffuseButton;
		wxButton * m_specularButton;
		wxButton * m_emissiveButton;
		wxButton * m_blendButton;
		wxButton * m_environmentConfigButton;
		wxCheckBox * m_doubleFace;
		wxTextCtrl * m_shininessText;
		wxSlider * m_sliderAlpha;
		wxStaticText * m_texPathText;
		wxColour m_diffuseColour;
		wxColour m_ambientColour;
		wxColour m_emissiveColour;
		wxColour m_specularColour;
		wxColour m_blendColour;

		wxComboBox * m_texturesCubeBox;
		wxComboBox * m_textureEnvMode;
		wxComboBox * m_textureRGBCombination;
		wxComboBox * m_textureAlphaCombination;
		wxStaticText * m_rgbCombinationText;
		wxStaticText * m_alphaCombinationText;
		wxRadioBox * m_texMode;

		wxButton * m_buttonShader;
		Castor3D::MaterialManager * m_pManager;

	public:
		PassPanel( Castor3D::MaterialManager * p_pManager, wxWindow * parent, const wxPoint & pos = wxDefaultPosition, const wxSize & size = wxSize( 460, 380));
		~PassPanel();

		void CreatePassPanel( Castor3D::PassPtr p_pass);

		void	GetDiffuse			( real & red, real & green, real & blue)const;
		void	GetAmbient			( real & red, real & green, real & blue)const;
		void	GetEmissive			( real & red, real & green, real & blue)const;
		void	GetSpecular			( real & red, real & green, real & blue)const;
		void	GetBlendColour		( real & red, real & green, real & blue)const;
		real	GetShininess		()const;
		int		GetTextureUnitIndex	()const;
		void	SetMaterialImage( unsigned int p_index, unsigned int p_width, unsigned int p_height);
		void	SetMaterialImage( Castor3D::TextureUnitPtr p_pTexture, unsigned int p_width, unsigned int p_height);

	private:
		void _createBitmapFromBuffer( wxBitmap & p_bitmap, unsigned char * p_pBuffer, wxCoord p_width, wxCoord p_height);
		void _createPassPanel		();
		void _createTextureUnitPanel( int p_index);

		DECLARE_EVENT_TABLE()
		void _onDeleteTextureUnit				( wxCommandEvent & event);
		void _onAmbientColour					( wxCommandEvent & event);
		void _onDiffuseColour					( wxCommandEvent & event);
		void _onSpecularColour					( wxCommandEvent & event);
		void _onEmissiveColour					( wxCommandEvent & event);
		void _onBlendColour						( wxCommandEvent & event);
		void _onTextureImage					( wxCommandEvent & event);
		void _onEnvironmentConfig				( wxCommandEvent & event);
		void _onShininess						( wxCommandEvent & event);
		void _onTexturePath						( wxCommandEvent & event);
		void _onTextureSelect					( wxCommandEvent & event);
		void _onTextureEnvModeSelect			( wxCommandEvent & event);
		void _onTextureRGBCombinationSelect		( wxCommandEvent & event);
		void _onTextureAlphaCombinationSelect	( wxCommandEvent & event);
		void _onDoubleFace						( wxCommandEvent & event);
		void _onTextureMode						( wxCommandEvent & event);
		void _onAlpha							( wxCommandEvent & event);
		void _onHasShader						( wxCommandEvent & event);

	};
}

#endif