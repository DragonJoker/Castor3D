/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_FRAME_VARIABLE_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_FRAME_VARIABLE_TREE_ITEM_PROPERTY_H___

#include "GuiCommon/Shader/ShaderSource.hpp"
#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

namespace GuiCommon
{
	/**
	\author 	Sylvain DOREMUS
	\date 		10/09/2015
	\version	0.8.0
	\~english
	\brief		Frame variable helper class to communicate between Scene objects or Materials lists and PropertiesContainer
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les variables de frame
	*/
	class FrameVariableTreeItemProperty
		: public TreeItemProperty
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	editable	Tells if the properties are modifiable.
		 *\param[in]	uniform		The target variable.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	editable	Dit si les propriétés sont modifiables.
		 *\param[in]	uniform		La variable cible.
		 */
		FrameVariableTreeItemProperty( castor3d::Engine * engine
			, bool editable
			, UniformValueBase & uniform );

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		void doCreateProperties( wxPGEditor * editor, wxPropertyGrid * grid )override;

	private:
		wxPGProperty * doBuildTrackedRangedValueProperty( wxPropertyGrid * grid
			, wxString const & name
			, UniformValueBase & uniform );
		wxPGProperty * doBuildTrackedUnrangedValueProperty( wxPropertyGrid * grid
			, wxString const & name
			, UniformValueBase & uniform );
		wxPGProperty * doBuildTrackedValueProperty( wxPropertyGrid * grid
			, wxString const & name
			, UniformValueBase & uniform );
		wxPGProperty * doBuildUntrackedRangedValueProperty( wxPropertyGrid * grid
			, wxString const & name
			, UniformValueBase & uniform );
		wxPGProperty * doBuildUntrackedUnrangedValueProperty( wxPropertyGrid * grid
			, wxString const & name
			, UniformValueBase & uniform );
		wxPGProperty * doBuildUntrackedValueProperty( wxPropertyGrid * grid
			, wxString const & name
			, UniformValueBase & uniform );

	private:
		UniformValueBase & m_uniform;
	};
}

#endif
