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
		struct Properties
		{
			explicit Properties( castor3d::PassComponentUPtr pownComponent = {}
				, castor3d::PassComponentRPtr pcomponent = {}
				, PropertyArray pproperties = {} )
				: ownComponent{ castor::move( pownComponent ) }
				, component{ castor::move( pcomponent ) }
				, properties{ castor::move( pproperties ) }

			{
			}

			castor3d::PassComponentUPtr ownComponent;
			castor3d::PassComponentRPtr component;
			PropertyArray properties;
			wxPGProperty * container{};
		};

		using PropertiesPtr = castor::RawUniquePtr< Properties >;
		using PropertiesArray = castor::Vector< PropertiesPtr >;

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
			, castor3d::Pass & pass
			, castor3d::Scene & scene
			, wxWindow * parent );
		/**
		 *\~english
		 *\brief		Retrieves the geometry
		 *\return		The value
		 *\~french
		 *\brief		Récupère la gàomàtrie
		 *\return		La valeur
		 */
		inline castor3d::PassRPtr getPass()
		{
			return m_pass;
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		void doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid )override;

		void moveComponentsToPass( castor3d::PassComponentUPtr component );
		void moveComponentsToProps( castor::Vector< castor3d::PassComponentUPtr > removed );

	private:
		castor3d::PassRPtr m_pass{};
		castor3d::Scene & m_scene;
		wxWindow * m_parent;
		PropertiesArray m_properties;
	};
}

#endif
