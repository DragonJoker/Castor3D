/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_SCENE_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_SCENE_TREE_ITEM_PROPERTY_H___

#include "TreeItemProperty.hpp"

namespace GuiCommon
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.8.0
	\~english
	\brief		Scene helper class to communicate between Scene objects or Materials lists and PropertiesHolder
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesHolder, pour les scànes
	*/
	class SceneTreeItemProperty
		: public TreeItemProperty
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
		SceneTreeItemProperty( bool editable
			, castor3d::Scene & scene );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~SceneTreeItemProperty();
		/**
		 *\~english
		 *\brief		Retrieves the object
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'objet
		 *\return		La valeur
		 */
		inline castor3d::Scene & getScene()
		{
			return m_scene;
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		virtual void doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid );
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doPropertyChange
		 */
		virtual void doPropertyChange( wxPropertyGridEvent & event );

	private:
		wxPGProperty * doCreateTextureImageProperty( wxString const & name
			, castor3d::TextureLayoutSPtr texture );
		void onDebugOverlaysChange( bool const & value );
		void onAmbientLightChange( castor::RgbColour const & value );
		void onBackgroundColourChange( castor::RgbColour const & value );
		void onBackgroundImageChange( castor::String const & value );

	private:
		castor3d::Scene & m_scene;
	};
}

#endif
