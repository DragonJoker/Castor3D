/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___GUICOMMON_VIEWPORT_TREE_ITEM_PROPERTY_H___
#define ___GUICOMMON_VIEWPORT_TREE_ITEM_PROPERTY_H___

#include "TreeItemProperty.hpp"

namespace GuiCommon
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		26/08/2015
	\version	0.8.0
	\~english
	\brief		Viewport helper class to communicate between Scene objects or Materials lists and PropertiesHolder
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et PropertiesHolder, pour les viewports
	*/
	class ViewportTreeItemProperty
		: public TreeItemProperty
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_editable	Tells if the properties are modifiable
		 *\param[in]	p_viewport	The target viewport
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_editable	Dit si les propriétés sont modifiables
		 *\param[in]	p_viewport	Le viewport cible
		 */
		ViewportTreeItemProperty( bool p_editable, Castor3D::ViewportSPtr p_viewport );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~ViewportTreeItemProperty();
		/**
		 *\~english
		 *\brief		Creates and fills the viewport properties, in the given wxPropertyGrid
		 *\param[in]	p_grid	The target wxPropertyGrid
		 *\~french
		 *\brief		Construit et remplit les propriétés du viewport, dans la wxPropertyGrid donnée
		 *\param[in]	p_grid	La wxPropertyGrid cible
		 */
		virtual void CreateProperties( wxPGEditor * p_editor, wxPropertyGrid * p_grid );
		/**
		 *\~english
		 *\brief		Call when a property grid property is changed
		 *\param[in]	p_event	The event
		 *\~french
		 *\brief		Appelée lorsqu'une propriété est changée
		 *\param[in]	p_event	L'évènement
		 */
		virtual void OnPropertyChange( wxPropertyGridEvent & p_event );
		/**
		 *\~english
		 *\brief		Retrieves the viewport
		 *\return		The value
		 *\~french
		 *\brief		Récupère le viewport
		 *\return		La valeur
		 */
		inline Castor3D::ViewportSPtr GetViewport()
		{
			return m_viewport.lock();
		}

	private:
		void OnTypeChange( Castor3D::eVIEWPORT_TYPE p_value );
		void OnSizeChange( Castor::Size const & p_value );
		void OnTopChange( double p_value );
		void OnBottomChange( double p_value );
		void OnLeftChange( double p_value );
		void OnRightChange( double p_value );
		void OnNearChange( double p_value );
		void OnFarChange( double p_value );
		void OnFovYChange( double p_value );
		void OnRatioChange( double p_value );

	private:
		Castor3D::ViewportWPtr m_viewport;
	};
}

#endif
