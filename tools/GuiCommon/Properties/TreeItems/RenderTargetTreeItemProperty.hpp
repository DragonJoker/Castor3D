/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_RENDER_TARGET_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_RENDER_TARGET_TREE_ITEM_PROPERTY_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

class wxTreeListCtrl;
class wxTreeListItem;

namespace GuiCommon
{
	/**
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.8.0
	\~english
	\brief		Render target helper class to communicate between Scene objects or Materials lists and PropertiesContainer
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les cibles de rendu
	*/
	class RenderTargetTreeItemProperty
		: public TreeItemProperty
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	editable	Tells if the properties are modifiable
		 *\param[in]	target	The target object
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	editable	Dit si les propriétés sont modifiables
		 *\param[in]	target	L'objet cible
		 */
		RenderTargetTreeItemProperty( bool editable
			, castor3d::RenderTarget & target );
		/**
		 *\~english
		 *\brief		Retrieves the object
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'objet
		 *\return		La valeur
		 */
		inline castor3d::RenderTarget & getRenderTarget()
		{
			return m_target;
		}

	private:
		/**
		 *\copydoc GuiCommon::TreeItemProperty::doCreateProperties
		 */
		void doCreateProperties( wxPropertyGrid * grid )override;

	private:
		castor3d::RenderTarget & m_target;
	};

	void appendRenderTarget( wxTreeCtrlBase * list
		, bool editable
		, wxTreeItemId id
		, castor3d::RenderTarget & target );
	void appendRenderTarget( wxTreeListCtrl * list
		, bool editable
		, wxTreeListItem const & id
		, castor3d::RenderTarget & target );
}

#endif
