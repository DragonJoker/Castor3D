/*
See LICENSE file in root folder
*/
#ifndef ___GC_TextureTreeItemProperty_H___
#define ___GC_TextureTreeItemProperty_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

#include <Castor3D/Material/Texture/TextureConfiguration.hpp>

namespace GuiCommon
{
	/**
	\~english
	\brief		Texture helper class to communicate between Scene objects or Materials lists and PropertiesContainer.
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les textures.
	*/
	class TextureTreeItemProperty
		: public TreeItemProperty
	{
	public:
		struct Properties
		{
			Properties( c3d::PassComponentTextureFlag pflag
				, c3d::TextureFlagConfiguration pconfiguration
				, c3d::PassMapComponentUPtr pownComponent = {}
				, c3d::PassMapComponentRPtr pcomponent = {}
				, PropertyArray pproperties = {} )
				: flag{ pflag }
				, configuration{ c3d::move( pconfiguration ) }
				, ownComponent{ c3d::move( pownComponent ) }
				, component{ pcomponent }
				, properties{ c3d::move( pproperties ) }

			{
			}

			wxPGProperty * container{};
			wxPGProperty * isEnabled{};
			wxPGProperty * components{};
			c3d::PassComponentTextureFlag flag;
			c3d::TextureFlagConfiguration configuration;
			c3d::PassMapComponentUPtr ownComponent;
			c3d::PassMapComponentRPtr component;
			PropertyArray properties;
			wxArrayString choices;
			uint32_t componentsCount{};
			bool isSetting{};
		};

		using PropertiesPtr = c3d::RawUniquePtr< Properties >;
		using PropertiesArray = c3d::Vector< PropertiesPtr >;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	editable	Tells if the properties are modifiable.
		 *\param[in]	engine		The engine.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	editable	Dit si les propriétés sont modifiables.
		 *\param[in]	engine		Le moteur.
		 */
		TextureTreeItemProperty( bool editable
			, c3d::Engine * engine );

		void setData( c3d::Pass & pass
			, c3d::TextureUnit & texture )noexcept;

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		void doCreateProperties( wxPropertyGrid * grid )override;

		void moveComponentsToPass( c3d::PassComponentUPtr component );
		void moveComponentsToProps( c3d::Vector< c3d::PassComponentUPtr > removed );

	private:
		c3d::Pass * m_pass{};
		c3d::TextureUnit * m_texture{};
		c3d::TextureConfiguration m_configuration;
		c3d::Point2f m_translate;
		c3d::Angle m_rotate;
		c3d::Point2f m_scale;
		PropertiesArray m_properties;
		c3d::TextureSourceInfo m_textureSource;
	};
}

#endif
