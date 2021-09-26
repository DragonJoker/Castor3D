/*
See LICENSE file in root folder
*/
#ifndef ___GC_ExportOptionsTreeItemProperty_HPP___
#define ___GC_ExportOptionsTreeItemProperty_HPP___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

#include <SceneExporter/SceneExporter.hpp>

namespace GuiCommon
{
	/**
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
			, castor3d::exporter::ExportOptions & options );

	private:
		/**
		*\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		*/
		void doCreateProperties( wxPGEditor * editor
			, wxPropertyGrid * grid )override;

	private:
		castor3d::exporter::ExportOptions & m_options;
	};
}

#endif
