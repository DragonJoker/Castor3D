#ifndef ___GC_PassPanel___
#define ___GC_PassPanel___

namespace GuiCommon
{
	class wxPassPanel : public wxScrolledWindow
	{
	public:
		typedef enum eID
		{	eID_BUTTON_DELETE
		,	eID_BUTTON_AMBIENT
		,	eID_BUTTON_DIFFUSE
		,	eID_BUTTON_SPECULAR
		,	eID_BUTTON_EMISSIVE
		,	eID_BUTTON_TEXTURE_IMAGE
		,	eID_BUTTON_HAS_SHADER
		,	eID_EDIT_SHININESS
		,	eID_EDIT_TEXTURE_PATH
		,	eID_COMBO_TEXTURE_UNITS
		,	eID_CHECK_DOUBLE_FACE
		,	eID_RADIO_MAP_MODE
		,	eID_SLIDER_ALPHA
		,	eID_COMBO_TEXTURE_CHANNEL
		}	eID;

	private:
		wxStaticBox *						m_pBoxGeneral;
		wxStaticText *						m_pStaticAmbient;
		wxStaticText *						m_pStaticDiffuse;
		wxStaticText *						m_pStaticSpecular;
		wxStaticText *						m_pStaticEmissive;
		wxStaticText *						m_pStaticExponent;
		wxStaticText *						m_pStaticPathLabel;
		wxStaticBox *						m_pBoxTextures;
		wxPanel *							m_pPanelTextureUnit;
		wxBitmapButton *					m_pButtonTextureImage;
		wxButton *							m_pButtonDeleteMaterial;
		wxButton *							m_pButtonDeleteTextureUnit;
		wxBitmapButton *					m_pButtonAmbient;
		wxBitmapButton *					m_pButtonDiffuse;
		wxBitmapButton *					m_pButtonSpecular;
		wxBitmapButton *					m_pButtonEmissive;
		wxCheckBox *						m_pCheckTwoSided;
		wxTextCtrl *						m_pEditShininess;
		wxSlider *							m_pSliderAlpha;
		wxStaticText *						m_pStaticPath;
		wxComboBox *						m_pComboTextures;
		wxButton *							m_pButtonShader;
		wxColour							m_clrDiffuse;
		wxColour							m_clrAmbient;
		wxColour							m_clrEmissive;
		wxColour							m_clrSpecular;
		wxImage								m_imgSelected;
		Castor3D::PassWPtr					m_wpPass;
		uint32_t							m_uiSelectedUnitIndex;
		Castor3D::TextureUnitWPtr			m_pTextureUnit;
		wxStaticText *						m_pStaticTexChannel;
		wxComboBox *						m_pComboTextureChannel;
		bool								m_bCanEdit;
		wxSize								m_sizeImage;
		Castor3D::Engine *					m_pEngine;

	public:
		wxPassPanel( Castor3D::Engine * p_pEngine, bool p_bCanEdit, wxWindow * p_pParent, wxPoint const & p_ptPos = wxDefaultPosition, wxSize const & p_size = wxSize( 460, 380 ) );
		~wxPassPanel();

		void			SetPass				( Castor3D::PassWPtr p_wpPass );
		void			GetDiffuse			( Castor::Colour & p_clrColour )const;
		void			GetAmbient			( Castor::Colour & p_clrColour )const;
		void			GetEmissive			( Castor::Colour & p_clrColour )const;
		void			GetSpecular			( Castor::Colour & p_clrColour )const;
		Castor::real	GetShininess		()const;
		int				GetTextureUnitIndex	()const;
		void			SetMaterialImage	( uint32_t p_uiIndex, uint32_t p_uiWidth, uint32_t p_uiHeight );
		void			SetMaterialImage	( Castor3D::TextureUnitSPtr p_pTexture, uint32_t p_uiWidth, uint32_t p_uiHeight );

	private:
		void DoSelectTextureUnit		( int p_iIndex );
		void DoShowPassFields			( bool p_bShow );
		void DoShowTextureFields		( bool p_bShow );

		DECLARE_EVENT_TABLE()
		void OnDeleteTextureUnit	( wxCommandEvent & p_event );
		void OnAmbientColour		( wxCommandEvent & p_event );
		void OnDiffuseColour		( wxCommandEvent & p_event );
		void OnSpecularColour		( wxCommandEvent & p_event );
		void OnEmissiveColour		( wxCommandEvent & p_event );
		void OnTextureImage			( wxCommandEvent & p_event );
		void OnShininess			( wxCommandEvent & p_event );
		void OnTexturePath			( wxCommandEvent & p_event );
		void OnTextureSelect		( wxCommandEvent & p_event );
		void OnDoubleFace			( wxCommandEvent & p_event );
		void OnTextureChannel		( wxCommandEvent & p_event );
		void OnAlpha				( wxCommandEvent & p_event );
		void OnHasShader			( wxCommandEvent & p_event );
	};
	/**
	 *\~english
	 *\brief		Copies the buffer into the bitmap
	 *\remark		The buffer must be in BGRA 32bits
	 *\param[in]	p_pBuffer	The buffer
	 *\param[in]	p_uiWidth	The buffer image's width
	 *\param[in]	p_uiHeight	The buffer image's height
	 *\param[out]	p_bitmap	Receives the generated bitmap
	 *\~french
	 *\brief		Copie le buffer donné dans un bitmap
	 *\remark		Le buffer doit être en format BGRA 32bits
	 *\param[in]	p_pBuffer	Le buffer
	 *\param[in]	p_uiWidth	La largeur de l'image
	 *\param[in]	p_uiHeight	La hauteur de l'image
	 *\param[out]	p_bitmap	Reçoit le bitmap généré
	 */
	void wxCreateBitmapFromBuffer( uint8_t const * p_pBuffer, uint32_t p_uiWidth, uint32_t p_uiHeight, wxBitmap & p_bitmap );
	/**
	 *\~english
	 *\brief		Copies the unit texture into the bitmap
	 *\remark		The image buffer will be copied in a BGRA 32bits buffer
	 *\param[in]	p_pUnit		The unit
	 *\param[out]	p_bitmap	Receives the generated bitmap
	 *\~french
	 *\brief		Copie la texture de l'unité dans un bitmap
	 *\remark		Le buffer de l'image va être copié dans un buffer BGRA 32bits
	 *\param[in]	p_pUnit		L'unité
	 *\param[out]	p_bitmap	Reçoit le bitmap généré
	 */
	void wxCreateBitmapFromBuffer( Castor3D::TextureUnitSPtr p_pUnit, wxBitmap & p_bitmap );
}

#endif
