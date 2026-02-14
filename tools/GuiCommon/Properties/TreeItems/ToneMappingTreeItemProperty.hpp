/*
See LICENSE file in root folder
*/
#ifndef ___GC_ToneMappingTreeItemProperty_HPP___
#define ___GC_ToneMappingTreeItemProperty_HPP___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

namespace GuiCommon
{
	/**
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.11.0
	\~english
	\brief		Helper class to communicate between Scene objects or Materials lists and PropertiesContainer.
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer.
	*/
	class ToneMappingTreeItemProperty
		: public TreeItemProperty
		, private wxEvtHandler
	{
	public:
		/**
		*\~english
		*\brief		Constructor
		*\param[in]	editable	Tells if the properties are modifiable.
		*\param[in]	toneMapping	The target object.
		*\~french
		*\brief		Constructeur
		*\param[in]	editable	Dit si les propriétés sont modifiables.
		*\param[in]	toneMapping	L'objet cible.
		*/
		ToneMappingTreeItemProperty( ImagesLoader & imagesLoader
			, bool editable
			, c3d::RenderTarget & target
			, wxWindow * parent );

	private:
		/**
		*\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		*/
		void doCreateProperties( wxPropertyGrid * grid )override;

		void onEditShader( wxVariant const & var );

	private:
		c3d::RenderTarget & m_target;
		wxWindow * m_parent;
		TreeItemPropertyUPtr m_hdrProperties;
		TreeItemPropertyUPtr m_toneMappingProperties;
		wxArrayString m_choices;
		c3d::StringMap< uint32_t > m_nameToChoice;
	};
}

#endif
