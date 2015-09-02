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
#ifndef ___GUICOMMON_TREE_ITEM_PROPERTY_DATA_H___
#define ___GUICOMMON_TREE_ITEM_PROPERTY_DATA_H___

#include "GuiCommonPrerequisites.hpp"

#include <wx/treectrl.h>
#include <wx/propgrid/propgrid.h>

namespace GuiCommon
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.8.0
	\~english
	\brief		The supported object types, for display, and properties
	\~french
	\brief		Les type d'objets supportés, dans les propriétés, et à l'affichage
	*/
	typedef enum ePROPERTY_DATA_TYPE
	{
		ePROPERTY_DATA_TYPE_SCENE,
		ePROPERTY_DATA_TYPE_RENDER_TARGET,
		ePROPERTY_DATA_TYPE_VIEWPORT,
		ePROPERTY_DATA_TYPE_RENDER_WINDOW,
		ePROPERTY_DATA_TYPE_CAMERA,
		ePROPERTY_DATA_TYPE_GEOMETRY,
		ePROPERTY_DATA_TYPE_SUBMESH,
		ePROPERTY_DATA_TYPE_LIGHT,
		ePROPERTY_DATA_TYPE_NODE,
		ePROPERTY_DATA_TYPE_OVERLAY,
		ePROPERTY_DATA_TYPE_MATERIAL,
		ePROPERTY_DATA_TYPE_PASS,
		ePROPERTY_DATA_TYPE_TEXTURE,
	}	ePROPERTY_DATA_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\date 		24/08/2015
	\version	0.8.0
	\~english
	\brief		Helper class to communicate between Scene objects or Materials lists and wxPropertiesHolder
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et wxPropertiesHolder
	*/
	class wxTreeItemProperty
		: public wxTreeItemData
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_type	 The object type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_type	Le type d'objet
		 */
		wxTreeItemProperty( ePROPERTY_DATA_TYPE p_type );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~wxTreeItemProperty();
		/**
		 *\~english
		 *\brief		Creates and fills the overlay properties, in the given wxPropertyGrid
		 *\param[in]	p_grid	The target wxPropertyGrid
		 *\~french
		 *\brief		Construit et remplit les propriétés de l'incrustation, dans la wxPropertyGrid donnée
		 *\param[in]	p_grid	La wxPropertyGrid cible
		 */
		virtual void CreateProperties( wxPropertyGrid * p_grid ) = 0;
		/**
		 *\~english
		 *\brief		Call when a property grid property is changed
		 *\param[in]	p_event	The event
		 *\~french
		 *\brief		Appelée lorsqu'une propriété est changée
		 *\param[in]	p_event	L'évènement
		 */
		virtual void OnPropertyChange( wxPropertyGridEvent & p_event ) = 0;
		/**
		 *\~english
		 *\brief		Retrieves the object type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type d'objet
		 *\return		La valeur
		 */
		inline ePROPERTY_DATA_TYPE GetType()const
		{
			return m_type;
		}

	protected:
		/**
		 *\~english
		 *\brief		Creates a material selector property
		 *\param[in]	p_name		The property name
		 *\param[in]	p_engine	The engine, to retrieve the materials
		 *\return		The created property
		 *\~french
		 *\brief		Crée une propriété de sélection de matériau
		 *\param[in]	p_name		Le nom de la propriété
		 *\param[in]	p_engine	Le moteur, pour récupérer les matériaux
		 *\return		La propriété créée.
		 */
		wxEnumProperty * DoCreateMaterialProperty( wxString const & p_name, Castor3D::Engine * p_engine );

	private:
		ePROPERTY_DATA_TYPE m_type;
	};
}

#endif
