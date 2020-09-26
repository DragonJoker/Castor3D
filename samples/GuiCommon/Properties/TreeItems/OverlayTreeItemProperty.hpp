/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_OVERLAY_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_OVERLAY_TREE_ITEM_PROPERTY_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

namespace GuiCommon
{
	/**
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
		virtual void doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid );

	private:
		void doCreatePanelOverlayProperties( wxPropertyGrid * grid, castor3d::PanelOverlaySPtr overlay );
		void doCreateBorderPanelOverlayProperties( wxPropertyGrid * grid, castor3d::BorderPanelOverlaySPtr overlay );
		void doCreateTextOverlayProperties( wxPropertyGrid * grid, castor3d::TextOverlaySPtr overlay );

	private:
		castor3d::OverlayCategoryWPtr m_overlay;
	};
}

#endif
