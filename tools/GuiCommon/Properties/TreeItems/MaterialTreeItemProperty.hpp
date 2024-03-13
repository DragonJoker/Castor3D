/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_MATERIAL_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_MATERIAL_TREE_ITEM_PROPERTY_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

namespace GuiCommon
{
	/**
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.8.0
	\~english
	\brief		Material helper class to communicate between Scene objects or Materials lists and PropertiesContainer
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les matériaux
	*/
	class MaterialTreeItemProperty
		: public TreeItemProperty
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	editable	Tells if the properties are modifiable
		 *\param[in]	material	The target material
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	editable	Dit si les propriétés sont modifiables
		 *\param[in]	material	Le matàriau cible
		 */
		MaterialTreeItemProperty( bool editable, castor3d::Material & material );
		/**
		 *\~english
		 *\brief		Retrieves the material
		 *\return		The value
		 *\~french
		 *\brief		Récupère le matàriau
		 *\return		La valeur
		 */
		inline castor3d::Material & getMaterial()
		{
			return *m_material;
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		void doCreateProperties( wxPropertyGrid * grid )override;

	private:
		castor3d::MaterialObs m_material{};
	};
}

#endif
