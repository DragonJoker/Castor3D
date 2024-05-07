/*
See LICENSE file in root folder
*/
#ifndef ___GC_GeometryTreeItemProperty_H___
#define ___GC_GeometryTreeItemProperty_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

namespace GuiCommon
{
	/**
	\~english
	\brief		Geometry helper class to communicate between Scene objects or Materials lists and PropertiesContainer.
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les gàomàtries.
	*/
	class GeometryTreeItemProperty
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
		GeometryTreeItemProperty( bool editable
			, castor3d::Engine * engine );
		/**
		 *\~english
		 *\brief		Retrieves the geometry
		 *\return		The value
		 *\~french
		 *\brief		Récupère la géométrie
		 *\return		La valeur
		 */
		void setData( castor3d::Geometry & data )noexcept
		{
			clearProperties();
			m_geometry = &data;
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		void doCreateProperties( wxPropertyGrid * grid )override;

	private:
		castor3d::Geometry * m_geometry{};
	};
}

#endif
