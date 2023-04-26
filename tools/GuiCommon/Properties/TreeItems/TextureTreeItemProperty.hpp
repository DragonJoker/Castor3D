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
	\~english
	\brief		Texture helper class to communicate between Scene objects or Materials lists and PropertiesContainer
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les textures
	*/
	class TextureTreeItemProperty
		: public TreeItemProperty
	{
	public:
		struct Properties
		{
			Properties( castor3d::PassComponentTextureFlag pflag
				, castor3d::TextureFlagConfiguration pconfiguration
				, castor3d::PassMapComponentUPtr pownComponent = {}
				, castor3d::PassMapComponentRPtr pcomponent = {}
				, PropertyArray pproperties = {} )
				: flag{ std::move( pflag ) }
				, configuration{ std::move( pconfiguration ) }
				, ownComponent{ std::move( pownComponent ) }
				, component{ std::move( pcomponent ) }
				, properties{ std::move( pproperties ) }

			{
			}

			wxPGProperty * container{};
			wxPGProperty * isEnabled{};
			wxPGProperty * components{};
			castor3d::PassComponentTextureFlag flag;
			castor3d::TextureFlagConfiguration configuration;
			castor3d::PassMapComponentUPtr ownComponent;
			castor3d::PassMapComponentRPtr component;
			PropertyArray properties;
			wxArrayString choices;
			uint32_t componentsCount{};
			bool isSetting{};
		};

		using PropertiesPtr = std::unique_ptr< Properties >;
		using PropertiesArray = std::vector< PropertiesPtr >;

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

		void moveComponentsToPass( castor3d::PassComponentUPtr component );
		void moveComponentsToProps( std::vector< castor3d::PassComponentUPtr > removed );

	private:
		castor3d::Pass & m_pass;
		castor3d::TextureUnit & m_texture;
		castor3d::TextureConfiguration m_configuration;
		castor::Point2f m_translate;
		castor::Angle m_rotate;
		castor::Point2f m_scale;
		PropertiesArray m_properties;
		castor::Path m_path;
	};
}

#endif
