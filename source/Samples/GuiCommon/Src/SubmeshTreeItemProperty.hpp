/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_SUBMESH_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_SUBMESH_TREE_ITEM_PROPERTY_H___

#include "TreeItemProperty.hpp"

namespace GuiCommon
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.8.0
	\~english
	\brief		Submesh helper class to communicate between Scene objects or Materials lists and PropertiesContainer
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les maillages
	*/
	class SubmeshTreeItemProperty
		: public TreeItemProperty
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_editable	Tells if the properties are modifiable
		 *\param[in]	p_geometry	The parent geometry
		 *\param[in]	p_submesh	The target submesh
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_editable	Dit si les propriétés sont modifiables
		 *\param[in]	p_geometry	La géométrie parente
		 *\param[in]	p_submesh	Le maillage cible
		 */
		SubmeshTreeItemProperty( bool p_editable, castor3d::Geometry & p_geometry, castor3d::Submesh & p_submesh );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~SubmeshTreeItemProperty();
		/**
		 *\~english
		 *\brief		Retrieves the submesh
		 *\return		The value
		 *\~french
		 *\brief		Récupère le maillage
		 *\return		La valeur
		 */
		inline castor3d::Submesh & getSubmesh()
		{
			return m_submesh;
		}
		/**
		 *\~english
		 *\brief		Retrieves the geometry
		 *\return		The value
		 *\~french
		 *\brief		Récupère la géométrie
		 *\return		La valeur
		 */
		inline castor3d::Geometry & getGeometry()
		{
			return m_geometry;
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		virtual void doCreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid );
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doPropertyChange
		 */
		virtual void doPropertyChange( wxPropertyGridEvent & p_event );

	private:
		void OnMaterialChange( castor::String const & p_name );
		void OnTopologyChange( castor3d::Topology p_value );

	private:
		castor3d::Geometry & m_geometry;
		castor3d::Submesh & m_submesh;
	};
}

#endif
