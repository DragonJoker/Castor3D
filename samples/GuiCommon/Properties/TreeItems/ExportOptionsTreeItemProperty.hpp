/*
See LICENSE file in root folder
*/
#ifndef ___GC_ExportOptionsTreeItemProperty_HPP___
#define ___GC_ExportOptionsTreeItemProperty_HPP___

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
	class ExportOptionsTreeItemProperty
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
		ExportOptionsTreeItemProperty( bool editable
			, ExportOptions & options );
		/**
		*\~english
		*\brief		Destructor
		*\~french
		*\brief		Destructeur
		*/
		~ExportOptionsTreeItemProperty();

	private:
		/**
		*\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		*/
		virtual void doCreateProperties( wxPGEditor * editor
			, wxPropertyGrid * grid );

	private:
		ExportOptions & m_options;
	};
}

#endif
