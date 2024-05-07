/*
See LICENSE file in root folder
*/
#ifndef ___GC_ParticleSystemTreeItemProperty_H___
#define ___GC_ParticleSystemTreeItemProperty_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

namespace GuiCommon
{
	/**
	\~english
	\brief		Billboard helper class to communicate between Scene objects or Materials lists and PropertiesContainer.
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les billboards.
	*/
	class ParticleSystemTreeItemProperty
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
		ParticleSystemTreeItemProperty( bool editable
			, castor3d::Engine * engine );

		void setData( castor3d::ParticleSystem & data )noexcept
		{
			clearProperties();
			m_particleSystem = &data;
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		void doCreateProperties( wxPropertyGrid * grid )override;

	private:
		castor3d::ParticleSystem * m_particleSystem{};
		wxArrayString m_materials;
	};
}

#endif
