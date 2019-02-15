/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_TEXTURE_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_TEXTURE_TREE_ITEM_PROPERTY_H___

#include "TreeItemProperty.hpp"

namespace GuiCommon
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.8.0
	\~english
	\brief		Texture helper class to communicate between Scene objects or Materials lists and PropertiesContainer
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les textures
	*/
	class TextureTreeItemProperty
		: public TreeItemProperty
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_editable	Tells if the properties are modifiable
		 *\param[in]	p_texture	The target texture
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_editable	Dit si les propriétés sont modifiables
		 *\param[in]	p_texture	La texture cible
		 */
		TextureTreeItemProperty( bool p_editable
			, castor3d::TextureUnitSPtr p_texture
			, castor3d::MaterialType p_type );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~TextureTreeItemProperty();
		/**
		 *\~english
		 *\brief		Retrieves the texture
		 *\return		The value
		 *\~french
		 *\brief		Récupère la texture
		 *\return		La valeur
		 */
		inline castor3d::TextureUnitSPtr getTexture()const
		{
			return m_texture.lock();
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		virtual void doCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid );
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doPropertyChange
		 */
		virtual void doPropertyChange( wxPropertyGridEvent & p_event );

	private:
		void OnChannelChange( castor3d::TextureChannel p_value );
		void OnImageChange( castor::String const & p_value );

	private:
		castor3d::TextureUnitWPtr m_texture;
		castor3d::MaterialType m_materialType;
	};
}

#endif
