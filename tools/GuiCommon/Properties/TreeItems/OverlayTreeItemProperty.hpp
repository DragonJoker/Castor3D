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
		 *\param[in]	editable	Tells if the properties are modifiable
		 *\param[in]	overlay	The target overlay
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	editable	Dit si les propriétés sont modifiables
		 *\param[in]	overlay	L'incrustation cible
		 */
		OverlayTreeItemProperty( bool editable
			, castor3d::OverlayCategory & overlay );
		/**
		 *\~english
		 *\brief		Retrieves the overlay
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'incrustation
		 *\return		La valeur
		 */
		inline castor3d::OverlayCategory & getOverlay()const noexcept
		{
			return m_overlay;
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		void doCreateProperties( wxPropertyGrid * grid )override;

	private:
		void doCreateBorderPanelOverlayProperties( wxPropertyGrid * grid
			, castor3d::BorderPanelOverlay & overlay );
		void doCreateTextOverlayProperties( wxPropertyGrid * grid
			, castor3d::TextOverlay & overlay );

	private:
		castor3d::OverlayCategory & m_overlay;
		wxArrayString m_materials;
	};
}

#endif
