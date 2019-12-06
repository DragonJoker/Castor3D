/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_BackgroundTreeItemProperty_HPP___
#define ___GUICOMMON_BackgroundTreeItemProperty_HPP___

#include "GuiCommon/TreeItemProperty.hpp"

namespace GuiCommon
{
	/*!
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
		BackgroundTreeItemProperty( wxWindow * parent
			, bool editable
			, castor3d::SceneBackground & background );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~BackgroundTreeItemProperty();

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
			, castor3d::TextureLayout const & texture );
		void onBackgroundColourChange( castor::RgbColour const & value );
		void onBackgroundImageChange( castor::String const & value );
		void onBackgroundLeftImageChange( castor::String const & value );
		void onBackgroundRightImageChange( castor::String const & value );
		void onBackgroundTopImageChange( castor::String const & value );
		void onBackgroundBottomImageChange( castor::String const & value );
		void onBackgroundFrontImageChange( castor::String const & value );
		void onBackgroundBackImageChange( castor::String const & value );
		void onBackgroundCrossImageChange( castor::String const & value );
		void onBackgroundEquirectangularImageChange( castor::String const & value );
		void onBackgroundEquirectangularDimensionsChange( uint32_t value );

	private:
		castor3d::SceneBackground & m_background;
		wxWindow * m_parent;
		wxPGProperty * m_backgroundProperty{ nullptr };
	};
}

#endif
