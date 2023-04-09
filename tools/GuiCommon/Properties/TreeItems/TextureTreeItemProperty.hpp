/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_TEXTURE_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_TEXTURE_TREE_ITEM_PROPERTY_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

#include <Castor3D/Material/Texture/TextureConfiguration.hpp>

namespace GuiCommon
{
	/**
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
		struct PropertyPair
		{
			wxPGProperty * isMap;
			wxPGProperty * components;
			castor3d::TextureFlagConfiguration & configuration;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	editable	Tells if the properties are modifiable
		 *\param[in]	texture		The target texture
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	editable	Dit si les propriétés sont modifiables
		 *\param[in]	texture		La texture cible
		 */
		TextureTreeItemProperty( bool editable
			, castor3d::Pass & pass
			, castor3d::TextureUnit & texture );

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		void doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid )override;

	private:
		void onChange( wxVariant const & var
			, castor3d::PassComponentTextureFlag flag
			, uint32_t componentsCount );
		void onImageChange( wxVariant const & var );

	private:
		castor3d::Pass & m_pass;
		castor3d::TextureUnit & m_texture;
		castor3d::TextureConfiguration m_configuration;
		castor::Point2f m_translate;
		castor::Angle m_rotate;
		castor::Point2f m_scale;
		std::map< castor3d::PassComponentTextureFlag, PropertyPair > m_properties;
		castor::Path m_path;
	};
}

#endif
