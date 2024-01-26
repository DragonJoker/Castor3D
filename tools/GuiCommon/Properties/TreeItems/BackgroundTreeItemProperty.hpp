/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_BackgroundTreeItemProperty_HPP___
#define ___GUICOMMON_BackgroundTreeItemProperty_HPP___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

namespace GuiCommon
{
	/**
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.8.0
	\~english
	\brief		Scene helper class to communicate between Scene objects or Materials lists and PropertiesContainer
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les scànes
	*/
	class BackgroundTreeItemProperty
		: public TreeItemProperty
		, public wxEvtHandler
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	editable	Tells if the properties are modifiable
		 *\param[in]	scene		The target object
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	editable	Dit si les propriétés sont modifiables
		 *\param[in]	scene		L'objet cible
		 */
		BackgroundTreeItemProperty( bool editable
			, castor3d::SceneBackground & background );

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		void doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid )override;

	private:
		wxPGProperty * doCreateTextureImageProperty( wxString const & name
			, castor3d::TextureLayout const & texture );

	private:
		castor3d::SceneBackground & m_background;
	};
}

#endif
