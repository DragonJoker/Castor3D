/*
See LICENSE file in root folder
*/
#ifndef ___GC_SubmeshTreeItemProperty_H___
#define ___GC_SubmeshTreeItemProperty_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

namespace GuiCommon
{
	/**
	\~english
	\brief		Submesh helper class to communicate between Scene objects or Materials lists and PropertiesContainer.
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les maillages.
	*/
	class SubmeshTreeItemProperty
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
		SubmeshTreeItemProperty( bool editable
			, castor3d::Engine * engine );

		void setData( castor3d::Geometry & geometry
			, castor3d::Submesh & submesh )noexcept
		{
			m_geometry = &geometry;
			m_submesh = &submesh;
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		void doCreateProperties( wxPropertyGrid * grid )override;

	private:
		void onMaterialChange( wxVariant const & var );
		void onTopologyChange( wxVariant const & var );

	private:
		castor3d::Geometry * m_geometry{};
		castor3d::Submesh * m_submesh{};
		wxArrayString m_materials;
	};
}

#endif
