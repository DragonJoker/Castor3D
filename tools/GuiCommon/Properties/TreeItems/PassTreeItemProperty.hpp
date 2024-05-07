/*
See LICENSE file in root folder
*/
#ifndef ___GC_PassTreeItemProperty_H___
#define ___GC_PassTreeItemProperty_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>

namespace GuiCommon
{
	/**
	\~english
	\brief		Helper class to communicate between Scene objects or Materials lists and PropertiesContainer, for Pass.
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour Pass.
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
		 *\param[in]	scene		The scene.
		 *\param[in]	parent		The parent window.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	editable	Dit si les propriétés sont modifiables.
		 *\param[in]	scene		La scène.
		 *\param[in]	parent		La fenêtre parent.
		 */
		PassTreeItemProperty( bool editable
			, castor3d::Scene & scene
			, wxWindow * parent );

		void setData( castor3d::Pass & data )noexcept
		{
			clearProperties();
			m_pass = &data;
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		void doCreateProperties( wxPropertyGrid * grid )override;

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
