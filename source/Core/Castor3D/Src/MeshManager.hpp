/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___C3D_MESH_MANAGER_H___
#define ___C3D_MESH_MANAGER_H___

#include "Manager/ResourceManager.hpp"

#include "Mesh/Mesh.hpp"
#include "Mesh/MeshFactory.hpp"

#include <OwnedBy.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to get an object type name.
	\~french
	\brief		Structure permettant de récupérer le nom du type d'un objet.
	*/
	template<> struct ManagedObjectNamer< Mesh >
	{
		C3D_API static const Castor::String Name;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Meshes manager.
	\~french
	\brief		Gestionnaire de maillages.
	*/
	class MeshManager
		: public ResourceManager< Castor::String, Mesh >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_engine	Le moteur.
		 */
		C3D_API MeshManager( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~MeshManager();
		/**
		 *\~english
		 *\brief		Creates a mesh with the given informations.
		 *\remarks		If a mesh with the given name already exists, it is returned and no mesh is created.
		 *\param[in]	p_type		Mesh type.
		 *\param[in]	p_name		The wanted name.
		 *\return		The created mesh.
		 *\~french.=
		 *\brief		Crée un mesh avec les informations données.
		 *\remarks		Si un mesh avec le nom donné existe déjà, il est retourné et aucun mesh n'est créé.
		 *\param[in]	p_type		Le type de mesh.
		 *\param[in]	p_name		Le nom du mesh.
		 *\return		Le mesh créé.
		 */
		C3D_API MeshSPtr Create( Castor::String const & p_name, eMESH_TYPE p_type );
		/**
		 *\~english
		 *\brief		Creates a mesh with the given informations.
		 *\remarks		If a mesh with the given name already exists, it is returned and no mesh is created.
		 *\param[in]	p_type			Mesh type.
		 *\param[in]	p_name			The wanted name.
		 *\param[in]	p_arrayFaces	The array of faces (for non custom meshes).
		 *\return		The created mesh.
		 *\~french
		 *\brief		Crée un mesh avec les informations données.
		 *\remarks		Si un mesh avec le nom donné existe déjà, il est retourné et aucun mesh n'est créé.
		 *\param[in]	p_type			Le type de mesh.
		 *\param[in]	p_name			Le nom du mesh.
		 *\param[in]	p_arrayFaces	Le tableau de faces (pour les mesh non custom).
		 *\return		Le mesh créé.
		 */
		C3D_API MeshSPtr Create( Castor::String const & p_name, eMESH_TYPE p_type, UIntArray const & p_arrayFaces );
		/**
		 *\~english
		 *\brief		Creates a mesh with the given informations.
		 *\remarks		If a mesh with the given name already exists, it is returned and no mesh is created.
		 *\param[in]	p_type			Mesh type.
		 *\param[in]	p_name			The wanted name.
		 *\param[in]	p_arrayFaces	The array of faces (for non custom meshes).
		 *\param[in]	p_arraySizes	The array of dimensions (for non custom meshes).
		 *\return		The created mesh.
		 *\~french
		 *\brief		Crée un mesh avec les informations données.
		 *\remarks		Si un mesh avec le nom donné existe déjà, il est retourné et aucun mesh n'est créé.
		 *\param[in]	p_type			Le type de mesh.
		 *\param[in]	p_name			Le nom du mesh.
		 *\param[in]	p_arrayFaces	Le tableau de faces (pour les mesh non custom).
		 *\param[in]	p_arraySizes	Le tableau de dimensions (pour les mesh non custom).
		 *\return		Le mesh créé
		 */
		C3D_API MeshSPtr Create( Castor::String const & p_name, eMESH_TYPE p_type, UIntArray const & p_arrayFaces, RealArray const & p_arraySizes );

	private:
		//!\~english The MeshGenerator factory	\~french La fabrique de MeshGenerator
		MeshFactory m_factory;
	};
}

#endif
