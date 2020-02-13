/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_VIEWPORT_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_VIEWPORT_TREE_ITEM_PROPERTY_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

namespace GuiCommon
{
	/**
	\author 	Sylvain DOREMUS
	\date 		26/08/2015
	\version	0.8.0
	\~english
	\brief		Viewport helper class to communicate between Scene objects or Materials lists and PropertiesContainer
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesContainer, pour les viewports
	*/
	class ViewportTreeItemProperty
		: public TreeItemProperty
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_editable	Tells if the properties are modifiable
		 *\param[in]	engine	The engine
		 *\param[in]	p_viewport	The target viewport
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_editable	Dit si les propriétés sont modifiables
		 *\param[in]	engine	Le moteur
		 *\param[in]	p_viewport	Le viewport cible
		 */
		ViewportTreeItemProperty( bool p_editable, castor3d::Engine & engine, castor3d::Viewport & p_viewport );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~ViewportTreeItemProperty();
		/**
		 *\~english
		 *\brief		Retrieves the viewport
		 *\return		The value
		 *\~french
		 *\brief		Récupère le viewport
		 *\return		La valeur
		 */
		inline castor3d::Viewport & getViewport()
		{
			return m_viewport;
		}
		/**
		 *\~english
		 *\brief		Retrieves the viewport
		 *\return		The value
		 *\~french
		 *\brief		Récupère le viewport
		 *\return		La valeur
		 */
		inline castor3d::Viewport const & getViewport()const
		{
			return m_viewport;
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
		void OnTypeChange( castor3d::ViewportType p_value );
		void OnSizeChange( castor::Size const & p_value );
		void OnTopChange( double p_value );
		void OnBottomChange( double p_value );
		void OnLeftChange( double p_value );
		void OnRightChange( double p_value );
		void OnNearChange( double p_value );
		void OnFarChange( double p_value );
		void OnFovYChange( double p_value );
		void OnRatioChange( double p_value );

	private:
		castor3d::Viewport & m_viewport;
	};
}

#endif
