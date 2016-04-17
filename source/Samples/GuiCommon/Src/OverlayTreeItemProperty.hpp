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
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesHolder, pour les incrustations
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
		 *\param[in]	p_editable	Dit si les propriétés sont modifiables
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
		 *\brief		Récupère l'incrustation
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
