/*
See LICENSE file in root folder
*/
#ifndef ___GC_AnimationTreeItemProperty_H___
#define ___GC_AnimationTreeItemProperty_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

namespace GuiCommon
{
	/**
	\~english
	\brief		Animation helper class to communicate between Scene objects or Materials lists and PropertiesContainer.
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les Animation.
	*/
	class AnimationTreeItemProperty
		: public TreeItemProperty
		, private wxEvtHandler
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	editable	Tells if the properties are modifiable.
		 *\param[in]	engine		The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	editable	Dit si les propriétés sont modifiables.
		 *\param[in]	engine		Le moteur.
		 */
		AnimationTreeItemProperty( bool editable
			, c3d::Engine * engine );

		void setData( c3d::AnimatedObjectGroup & group
			, c3d::GroupAnimation anim )noexcept
		{
			clearProperties();
			m_group = &group;
			m_groupAnim = std::move( anim );
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		void doCreateProperties( wxPropertyGrid * grid )override;

	private:
		c3d::AnimatedObjectGroup * m_group{};
		c3d::GroupAnimation m_groupAnim{ c3d::cuEmptyString };
	};
}

#endif
