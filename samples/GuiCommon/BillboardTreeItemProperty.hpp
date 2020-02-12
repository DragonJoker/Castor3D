/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_BILLBOARD_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_BILLBOARD_TREE_ITEM_PROPERTY_H___

#include "GuiCommon/TreeItemProperty.hpp"

namespace GuiCommon
{
	/**
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.8.0
	\~english
	\brief		Billboard helper class to communicate between Scene objects or Materials lists and PropertiesContainer
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les billboards
	*/
	class BillboardTreeItemProperty
		: public TreeItemProperty
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_editable	Tells if the properties are modifiable
		 *\param[in]	p_billboard	The target billboard
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_editable	Dit si les propriétés sont modifiables
		 *\param[in]	p_billboard	Le billboard cible
		 */
		BillboardTreeItemProperty( bool p_editable, castor3d::BillboardList & p_billboard );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~BillboardTreeItemProperty();
		/**
		 *\~english
		 *\brief		Retrieves the billboard
		 *\return		The value
		 *\~french
		 *\brief		Récupère le billboard
		 *\return		La valeur
		 */
		inline castor3d::BillboardList & getBillboard()
		{
			return m_billboard;
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
		void OnSizeChange( castor::Point2f const & p_size );

	private:
		castor3d::BillboardList & m_billboard;
	};
}

#endif
