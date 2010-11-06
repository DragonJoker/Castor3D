/*
This source file is part of Castor3D (http://dragonjoker.co.cc

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
#ifndef ___C3D_MeshManager___
#define ___C3D_MeshManager___

#include "../../main/Module_Main.h"
#include "Mesh.h"
#include <CastorUtils/UniqueManager.h>

namespace Castor3D
{
	//! The Mesh manager representation
	/*!
	This is the handler of all created meshes, you can create a mesh only from here
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class CS3D_API MeshManager : public Castor::Templates::UniqueManager<Mesh, MeshManager>
	{
	private:
		friend class Castor::Templates::UniqueManager<Mesh, MeshManager>;

	private:
		MeshPtrStrMap m_meshMap;	//!< The created meshes map, sorted by name

	private:
		/**
		 * Constructor
		 */
		MeshManager();
		/**
		 * Destructor
		 */
		~MeshManager();

	public:
		/**
		 * Flushes all the create meshes
		 */
		static void Clear();
		/**
		 * Creates a mesh from the given parameters
		 *@param p_name : [in] The mesh name
		 *@param p_faces : [in] The faces numbers
		 *@param p_size : [in] The sizes values
		 *@param p_type : [in] The mesh type
		 *@return The created mesh, NULL if unknown MeshType (but is it possible ?)
		 */
		static MeshPtr CreateMesh( const String & p_name, const UIntArray & p_faces, 
								   const FloatArray & p_size, Mesh::eTYPE p_type=Mesh::eCustom);
		/**
		 * Writes all the meshes in a file
		 *@param p_path : [in] The path of the file to write in
		 *@return true if successful, false if not
		 */
		static bool Write( const String & p_path);
		/**
		 * Reads all the meshes from a file
		 *@param p_path : [in] The path of the file to read from
		 *@return true if successful, false if not
		 */
		static bool Read( const String & p_path);

	private:
		bool _addElement( MeshPtr p_element);
	};
}

#endif

