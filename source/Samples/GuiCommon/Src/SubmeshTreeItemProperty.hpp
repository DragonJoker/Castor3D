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
	\brief		Submesh helper class to communicate between Scene objects or Materials lists and wxPropertiesHolder
	\~french
	\brief		Classe d'aide facilitant la communication entre la liste des objets de scène, ou la liste de matériaux, et wxPropertiesHolder, pour les maillages
	*/
	class wxSubmeshTreeItemProperty
		: public wxTreeItemProperty
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pGeometry	 The parent geometry
		 *\param[in]	p_pSubmesh	 The target submesh
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pGeometry	La géométrie parente
		 *\param[in]	p_pSubmesh	Le maillage cible
		 */
		wxSubmeshTreeItemProperty( Castor3D::GeometrySPtr p_pGeometry, Castor3D::SubmeshSPtr p_pSubmesh );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~wxSubmeshTreeItemProperty();
		/**
		 *\~english
		 *\brief		Creates and fills the submesh properties, in the given wxPropertyGrid
		 *\param[in]	p_grid	The target wxPropertyGrid
		 *\~french
		 *\brief		Construit et remplit les propriétés du maillage, dans la wxPropertyGrid donnée
		 *\param[in]	p_grid	La wxPropertyGrid cible
		 */
		virtual void CreateProperties( wxPropertyGrid * p_grid );
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
		 *\brief		Retrieves the submesh
		 *\return		The value
		 *\~french
		 *\brief		Récupère le maillage
		 *\return		La valeur
		 */
		inline Castor3D::SubmeshSPtr GetSubmesh()
		{
			return m_pSubmesh.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the geometry
		 *\return		The value
		 *\~french
		 *\brief		Récupère la géométrie
		 *\return		La valeur
		 */
		inline Castor3D::GeometrySPtr GetGeometry()
		{
			return m_pGeometry.lock();
		}

	private:
		void OnMaterialChange( Castor::String const & p_name );

	private:
		Castor3D::GeometryWPtr m_pGeometry;
		Castor3D::SubmeshWPtr m_pSubmesh;
	};
}

#endif
