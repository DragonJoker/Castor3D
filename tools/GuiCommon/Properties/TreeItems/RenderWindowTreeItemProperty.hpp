/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_RENDER_WINDOW_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_RENDER_WINDOW_TREE_ITEM_PROPERTY_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

namespace GuiCommon
{
	/**
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.8.0
	\~english
	\brief		Render window helper class to communicate between Scene objects or Materials lists and PropertiesContainer
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les fenàtres de rendu
	*/
	class RenderWindowTreeItemProperty
		: public TreeItemProperty
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_editable	Tells if the properties are modifiable
		 *\param[in]	p_target	The target object
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_editable	Dit si les propriétés sont modifiables
		 *\param[in]	p_target	L'objet cible
		 */
		RenderWindowTreeItemProperty( bool editable
			, castor3d::RenderWindow & target );

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		void doCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid )override;

	private:
		castor3d::RenderWindow & m_window;
	};
}

#endif
