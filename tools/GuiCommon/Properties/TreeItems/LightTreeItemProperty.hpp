/*
See LICENSE file in root folder
*/
#ifndef ___GC_LightTreeItemProperty_H___
#define ___GC_LightTreeItemProperty_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

#include <Castor3D/Render/GlobalIllumination/GlobalIlluminationModule.hpp>

namespace GuiCommon
{
	enum class GIType
	{
		eNone,
		eLPV,
		eLPVG,
		eLLPV,
		eLLPVG,
	};
	/**
	\~english
	\brief		Helper class to communicate between Scene objects or Materials lists and PropertiesContainer, for Light.
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour Light.
	*/
	class LightTreeItemProperty
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
		LightTreeItemProperty( bool editable, castor3d::Engine * engine );

		void setData( castor3d::Light & data )noexcept
		{
			m_light = &data;
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		void doCreateProperties( wxPropertyGrid * grid )override;

	private:
		castor3d::Light * m_light{};
	};
}

#endif
