/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_PASS_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_PASS_TREE_ITEM_PROPERTY_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>

namespace GuiCommon
{
	/**
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.8.0
	\~english
	\brief		Geometry helper class to communicate between Scene objects or Materials lists and PropertiesContainer
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les gàomàtries
	*/
	class PassTreeItemProperty
		: public TreeItemProperty
		, private wxEvtHandler
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	editable	Tells if the properties are modifiable.
		 *\param[in]	pass		The target pass.
		 *\param[in]	scene		The scene.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	editable	Dit si les propriétés sont modifiables.
		 *\param[in]	pass		La passe cible.
		 *\param[in]	scene		La scène.
		 */
		PassTreeItemProperty( bool editable
			, castor3d::PassSPtr pass
			, castor3d::Scene & scene
			, wxWindow * parent );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~PassTreeItemProperty();
		/**
		 *\~english
		 *\brief		Retrieves the geometry
		 *\return		The value
		 *\~french
		 *\brief		Récupère la gàomàtrie
		 *\return		La valeur
		 */
		inline castor3d::PassSPtr getPass()
		{
			return m_pass.lock();
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		virtual void doCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid );

	private:
		castor3d::PassWPtr m_pass;
		castor3d::Scene & m_scene;
		wxWindow * m_parent;
	};
}

#endif
