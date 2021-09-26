/*
See LICENSE file in root folder
*/
#ifndef ___GC_SkeletonAnimationTreeItemProperty_H___
#define ___GC_SkeletonAnimationTreeItemProperty_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationModule.hpp>

namespace GuiCommon
{
	/**
	\author 	Sylvain DOREMUS
	\date 		18/05/2018
	\version	0.11.0
	\~english
	\brief		Helper class to communicate between Scene objects or Materials lists and PropertiesContainer
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer.
	*/
	class SkeletonAnimationTreeItemProperty
		: public TreeItemProperty
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	editable	Tells if the properties are modifiable.
		 *\param[in]	animation	The target.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	editable	Dit si les propriétés sont modifiables.
		 *\param[in]	animation	La cible.
		 */
		SkeletonAnimationTreeItemProperty( bool editable, castor3d::SkeletonAnimation & animation );

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		void doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid )override;

	private:
		castor3d::SkeletonAnimation & m_animation;
	};
}

#endif
