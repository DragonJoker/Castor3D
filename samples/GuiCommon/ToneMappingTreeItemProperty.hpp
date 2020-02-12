/*
See LICENSE file in root folder
*/
#ifndef ___GC_ToneMappingTreeItemProperty_HPP___
#define ___GC_ToneMappingTreeItemProperty_HPP___

#include "GuiCommon/TreeItemProperty.hpp"

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
		ToneMappingTreeItemProperty( bool editable
			, castor3d::ToneMapping & toneMapping
			, wxWindow * parent );
		/**
		*\~english
		*\brief		Destructor
		*\~french
		*\brief		Destructeur
		*/
		~ToneMappingTreeItemProperty();

	private:
		/**
		*\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		*/
		virtual void doCreateProperties( wxPGEditor * editor
			, wxPropertyGrid * grid );
		/**
		*\copydoc GuiCommon::TreeItemProperty::doPropertyChange
		*/
		virtual void doPropertyChange( wxPropertyGridEvent & event );

		bool onEditShader( wxPGProperty * p_property );

	private:
		castor3d::ToneMapping & m_toneMapping;
		wxWindow * m_parent;
	};
}

#endif
