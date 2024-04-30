/*
See LICENSE file in root folder
*/
#ifndef ___GC_OverlayTreeItemProperty_H___
#define ___GC_OverlayTreeItemProperty_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

namespace GuiCommon
{
	/**
	\~english
	\brief		Overlay helper class to communicate between Scene objects or Materials lists and PropertiesContainer.
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les incrustations.
	*/
	class OverlayTreeItemProperty
		: public TreeItemProperty
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	editable	Tells if the properties are modifiable.
		 *\param[in]	engine		The engine.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	editable	Dit si les propriétés sont modifiables.
		 *\param[in]	engine		Le moteur.
		 */
		OverlayTreeItemProperty( bool editable
			, castor3d::Engine * engine );

		void setData( castor3d::OverlayCategory & data )noexcept
		{
			m_overlay = &data;
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
		castor3d::OverlayCategory * m_overlay{};
		wxArrayString m_materials;
	};
}

#endif
