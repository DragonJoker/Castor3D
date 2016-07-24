/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___GUICOMMON_OVERLAY_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_OVERLAY_TREE_ITEM_PROPERTY_H___

#include "TreeItemProperty.hpp"

namespace GuiCommon
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.8.0
	\~english
	\brief		Overlay helper class to communicate between Scene objects or Materials lists and PropertiesHolder
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de sc�ne, ou la liste de mat�riaux, et PropertiesHolder, pour les incrustations
	*/
	class OverlayTreeItemProperty
		: public TreeItemProperty
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_editable	Tells if the properties are modifiable
		 *\param[in]	p_overlay	The target overlay
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_editable	Dit si les propri�t�s sont modifiables
		 *\param[in]	p_overlay	L'incrustation cible
		 */
		OverlayTreeItemProperty( bool p_editable, Castor3D::OverlayCategorySPtr p_overlay );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~OverlayTreeItemProperty();
		/**
		 *\~english
		 *\brief		Retrieves the overlay
		 *\return		The value
		 *\~french
		 *\brief		R�cup�re l'incrustation
		 *\return		La valeur
		 */
		inline Castor3D::OverlayCategorySPtr GetOverlay()
		{
			return m_overlay.lock();
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::DoCreateProperties
		 */
		virtual void DoCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid );
		/**
		 *\copydoc GuiCommon::TreeItemProperty::DoPropertyChange
		 */
		virtual void DoPropertyChange( wxPropertyGridEvent & p_event );

	private:
		void DoCreatePanelOverlayProperties( wxPropertyGrid * p_grid, Castor3D::PanelOverlaySPtr p_overlay );
		void DoCreateBorderPanelOverlayProperties( wxPropertyGrid * p_grid, Castor3D::BorderPanelOverlaySPtr p_overlay );
		void DoCreateTextOverlayProperties( wxPropertyGrid * p_grid, Castor3D::TextOverlaySPtr p_overlay );
		void OnPanelOverlayPropertyChanged( wxPropertyGridEvent & p_event );
		void OnBorderPanelOverlayPropertyChanged( wxPropertyGridEvent & p_event );
		void OnTextOverlayPropertyChanged( wxPropertyGridEvent & p_event );
		void OnMaterialChange( Castor::String const & p_value );
		void OnPositionChange( Castor::Position const & p_value );
		void OnSizeChange( Castor::Size const & p_value );
		void OnBorderMaterialChange( Castor::String const & p_value );
		void OnBorderSizeChange( Castor::Rectangle const & p_value );
		void OnBorderInnerUVChange( Castor::Point4d const & p_value );
		void OnBorderOuterUVChange( Castor::Point4d const & p_value );
		void OnBorderPositionChange( Castor3D::eBORDER_POSITION p_value );
		void OnCaptionChange( Castor::String const & p_value );
		void OnFontChange( Castor::FontSPtr p_value );
		void OnHAlignChange( Castor3D::eHALIGN p_value );
		void OnVAlignChange( Castor3D::eVALIGN p_value );
		void OnWrappingChange( Castor3D::eTEXT_WRAPPING_MODE p_value );
		void OnSpacingChange( Castor3D::eTEXT_LINE_SPACING_MODE p_value );
		void OnTexturingChange( Castor3D::eTEXT_TEXTURING_MODE p_value );

	private:
		Castor3D::OverlayCategoryWPtr m_overlay;
	};
}

#endif
