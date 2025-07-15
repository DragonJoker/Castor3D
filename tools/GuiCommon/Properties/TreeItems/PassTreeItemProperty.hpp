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
			explicit Properties( c3d::PassComponentUPtr pownComponent = {}
				, c3d::PassComponentRPtr pcomponent = {}
				, PropertyArray pproperties = {} )
				: ownComponent{ c3d::move( pownComponent ) }
				, component{ c3d::move( pcomponent ) }
				, properties{ c3d::move( pproperties ) }

			{
			}

			c3d::PassComponentUPtr ownComponent;
			c3d::PassComponentRPtr component;
			PropertyArray properties;
			wxPGProperty * container{};
		};

		using PropertiesPtr = c3d::RawUniquePtr< Properties >;
		using PropertiesArray = c3d::Vector< PropertiesPtr >;

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
			, c3d::Scene & scene
			, wxWindow * parent );

		void setData( c3d::Pass & data )noexcept
		{
			clearProperties();
			m_pass = &data;
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		void doCreateProperties( wxPropertyGrid * grid )override;

		void moveComponentsToPass( c3d::PassComponentUPtr component );
		void moveComponentsToProps( c3d::Vector< c3d::PassComponentUPtr > removed );

	private:
		c3d::PassRPtr m_pass{};
		c3d::Scene & m_scene;
		wxWindow * m_parent;
		PropertiesArray m_properties;
	};
}

#endif
