/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_OVERLAY_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_OVERLAY_TREE_ITEM_PROPERTY_H___

#include "GuiCommon/TreeItemProperty.hpp"

namespace GuiCommon
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.8.0
	\~english
	\brief		Overlay helper class to communicate between Scene objects or Materials lists and PropertiesContainer
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les incrustations
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
