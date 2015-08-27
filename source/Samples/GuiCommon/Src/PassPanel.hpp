/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___GUICOMMON_PASS_PANEL_H___
#define ___GUICOMMON_PASS_PANEL_H___

#include "GuiCommonPrerequisites.hpp"

#include <Colour.hpp>

#include <wx/scrolwin.h>

namespace GuiCommon
{
	class wxPassPanel
		:	public wxScrolledWindow
	{
	public:
		typedef enum eID
		{
			eID_BUTTON_DELETE,
			eID_BUTTON_AMBIENT,
			eID_BUTTON_DIFFUSE,
			eID_BUTTON_SPECULAR,
			eID_BUTTON_EMISSIVE,
			eID_BUTTON_TEXTURE_IMAGE,
			eID_BUTTON_HAS_SHADER,
			eID_EDIT_SHININESS,
			eID_EDIT_TEXTURE_PATH,
			eID_COMBO_TEXTURE_UNITS,
			eID_CHECK_DOUBLE_FACE,
			eID_RADIO_MAP_MODE,
			eID_SLIDER_ALPHA,
			eID_COMBO_TEXTURE_CHANNEL,
		}	eID;

	private:
		wxStaticBox * m_pBoxGeneral;
		wxStaticText * m_pStaticAmbient;
		wxStaticText * m_pStaticDiffuse;
		wxStaticText * m_pStaticSpecular;
		wxStaticText * m_pStaticEmissive;
		wxStaticText * m_pStaticExponent;
		wxStaticText * m_pStaticPathLabel;
		wxStaticBox * m_pBoxTextures;
		wxPanel * m_pPanelTextureUnit;
		wxBitmapButton * m_pButtonTextureImage;
		wxButton * m_pButtonDeleteMaterial;
		wxButton * m_pButtonDeleteTextureUnit;
		wxBitmapButton * m_pButtonAmbient;
		wxBitmapButton * m_pButtonDiffuse;
		wxBitmapButton * m_pButtonSpecular;
		wxBitmapButton * m_pButtonEmissive;
		wxCheckBox * m_pCheckTwoSided;
		wxTextCtrl * m_pEditShininess;
		wxSlider * m_pSliderAlpha;
		wxStaticText * m_pStaticPath;
		wxComboBox * m_pComboTextures;
		wxButton * m_pButtonShader;
		wxColour m_clrDiffuse;
		wxColour m_clrAmbient;
		wxColour m_clrEmissive;
		wxColour m_clrSpecular;
		wxImage m_imgSelected;
		Castor3D::PassWPtr m_wpPass;
		uint32_t m_uiSelectedUnitIndex;
		Castor3D::TextureUnitWPtr m_pTextureUnit;
		wxStaticText * m_pStaticTexChannel;
		wxComboBox * m_pComboTextureChannel;
		bool m_bCanEdit;
		wxSize m_sizeImage;
		Castor3D::Engine * m_pEngine;

	public:
		wxPassPanel( Castor3D::Engine * p_pEngine, bool p_bCanEdit, wxWindow * p_pParent, wxPoint const & p_ptPos = wxDefaultPosition, wxSize const & p_size = wxSize( 460, 380 ) );
		~wxPassPanel();

		void SetPass( Castor3D::PassWPtr p_wpPass );
		void GetDiffuse( Castor::Colour & p_clrColour )const;
		void GetAmbient( Castor::Colour & p_clrColour )const;
		void GetEmissive( Castor::Colour & p_clrColour )const;
		void GetSpecular( Castor::Colour & p_clrColour )const;
		Castor::real GetShininess()const;
		int GetTextureUnitIndex()const;
		void SetMaterialImage( uint32_t p_uiIndex, uint32_t p_uiWidth, uint32_t p_uiHeight );
		void SetMaterialImage( Castor3D::TextureUnitSPtr p_pTexture, uint32_t p_uiWidth, uint32_t p_uiHeight );

	private:
		void DoSelectTextureUnit( int p_iIndex );
		void DoShowPassFields( bool p_bShow );
		void DoShowTextureFields( bool p_bShow );

		DECLARE_EVENT_TABLE()
		void OnDeleteTextureUnit( wxCommandEvent & p_event );
		void OnAmbientColour( wxCommandEvent & p_event );
		void OnDiffuseColour( wxCommandEvent & p_event );
		void OnSpecularColour( wxCommandEvent & p_event );
		void OnEmissiveColour( wxCommandEvent & p_event );
		void OnTextureImage( wxCommandEvent & p_event );
		void OnShininess( wxCommandEvent & p_event );
		void OnTexturePath( wxCommandEvent & p_event );
		void OnTextureSelect( wxCommandEvent & p_event );
		void OnDoubleFace( wxCommandEvent & p_event );
		void OnTextureChannel( wxCommandEvent & p_event );
		void OnAlpha( wxCommandEvent & p_event );
		void OnHasShader( wxCommandEvent & p_event );
	};
}

#endif
