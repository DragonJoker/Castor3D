/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_TEXTURE_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_TEXTURE_TREE_ITEM_PROPERTY_H___

#include "GuiCommon/TreeItemProperty.hpp"

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
			, castor3d::TextureUnitSPtr texture
			, castor3d::MaterialType type );
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
		void addProperty( wxPropertyGrid * grid
			, wxString const & flagName
			, wxString const & isName
			, wxString const & compName
			, castor3d::TextureFlag flag
			, castor::Point2ui & mask
			, uint32_t componentsCount );
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		virtual void doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid );
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doPropertyChange
		 */
		virtual void doPropertyChange( wxPropertyGridEvent & event );

	private:
		void onChange( castor3d::TextureFlag flag
			, uint32_t componentsCount );
		void onNormalFactorChange( double value );
		void onHeightFactorChange( double value );
		void onNormalDirectXChange( bool value );
		void onImageChange( castor::Path const & value );

	private:
		struct PropertyPair
		{
			wxPGProperty * isMap;
			wxPGProperty * components;
			castor::Point2ui & mask;
		};
		castor3d::TextureUnitWPtr m_texture;
		castor3d::TextureConfiguration m_configuration;
		castor3d::MaterialType m_materialType;
		std::map< castor3d::TextureFlag, PropertyPair > m_properties;
	};
}

#endif
