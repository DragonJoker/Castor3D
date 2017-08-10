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
		OverlayTreeItemProperty( bool p_editable, castor3d::OverlayCategorySPtr p_overlay );
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
		inline castor3d::OverlayCategorySPtr getOverlay()
		{
			return m_overlay.lock();
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		virtual void doCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid );
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doPropertyChange
		 */
		virtual void doPropertyChange( wxPropertyGridEvent & p_event );

	private:
		void doCreatePanelOverlayProperties( wxPropertyGrid * p_grid, castor3d::PanelOverlaySPtr p_overlay );
		void doCreateBorderPanelOverlayProperties( wxPropertyGrid * p_grid, castor3d::BorderPanelOverlaySPtr p_overlay );
		void doCreateTextOverlayProperties( wxPropertyGrid * p_grid, castor3d::TextOverlaySPtr p_overlay );
		void OnPanelOverlayPropertyChanged( wxPropertyGridEvent & p_event );
		void OnBorderPanelOverlayPropertyChanged( wxPropertyGridEvent & p_event );
		void OnTextOverlayPropertyChanged( wxPropertyGridEvent & p_event );
		void OnMaterialChange( castor::String const & p_value );
		void OnPositionChange( castor::Position const & p_value );
		void OnSizeChange( castor::Size const & p_value );
		void OnBorderMaterialChange( castor::String const & p_value );
		void OnBorderSizeChange( castor::Rectangle const & p_value );
		void OnBorderInnerUVChange( castor::Point4d const & p_value );
		void OnBorderOuterUVChange( castor::Point4d const & p_value );
		void OnBorderPositionChange( castor3d::BorderPosition p_value );
		void OnCaptionChange( castor::String const & p_value );
		void OnFontChange( castor::FontSPtr p_value );
		void OnHAlignChange( castor3d::HAlign p_value );
		void OnVAlignChange( castor3d::VAlign p_value );
		void OnWrappingChange( castor3d::TextWrappingMode p_value );
		void OnSpacingChange( castor3d::TextLineSpacingMode p_value );
		void OnTexturingChange( castor3d::TextTexturingMode p_value );

	private:
		castor3d::OverlayCategoryWPtr m_overlay;
	};
}

#endif
