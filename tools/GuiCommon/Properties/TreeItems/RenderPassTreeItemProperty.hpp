/*
See LICENSE file in root folder
*/
#ifndef ___GC_RenderPassTreeItemProperty_HPP___
#define ___GC_RenderPassTreeItemProperty_HPP___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

#include <Castor3D/Render/Technique/TechniqueModule.hpp>

namespace GuiCommon
{
	/**
	\~english
	\brief		Helper class to communicate between Scene objects or Materials lists and PropertiesContainer.
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer.
	*/
	class RenderPassTreeItemProperty
		: public TreeItemProperty
		, private wxEvtHandler
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	editable	Tells if the properties are modifiable.
		 *\param[in]	rendePass	The target object.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	editable	Dit si les propriétés sont modifiables.
		 *\param[in]	rendePass	L'objet cible.
		 */
		RenderPassTreeItemProperty( bool editable
			, castor3d::RenderTechniquePass & renderPass
			, wxWindow * parent );

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		void doCreateProperties( wxPGEditor * editor
			, wxPropertyGrid * grid )override;

	private:
		castor3d::RenderTechniquePass & m_renderPass;
		wxWindow * m_parent;
	};

	void fillRenderPassConfiguration( wxPropertyGrid * grid
		, TreeItemProperty & properties
		, castor3d::RenderTechniquePass & renderPass );
}

#endif
