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
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_MeshManager___
#define ___C3D_MeshManager___

#include "../../main/Module_Main.h"
#include "Mesh.h"

namespace Castor3D
{
	//! The Mesh manager representation
	/*!
	This is the handler of all created meshes, this class has the only way to create a mesh
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class CS3D_API MeshManager : public General::Templates::Manager<Mesh>, public General::Theory::AutoSingleton<MeshManager>
	{
	private:
		MeshPtrMap m_meshMap;	//!< The created meshes map, sorted by name

	public:
		/**
		 * Constructor
		 */
		MeshManager();
		/**
		 * Destructor
		 */
		~MeshManager();
		/**
		 * Flushes all the create meshes
		 */
		void Clear();
		/**
		 * Creates a mesh from the given parameters
		 *@param p_name : [in] The mesh name
		 *@param p_type : [in] The mesh type
		 *@param p_faces : [in] The faces numbers
		 *@param p_size : [in] The sizes values
		 *@return The created mesh, NULL if unknown MeshType (but is it possible ?)
		 */
		Mesh * CreateMesh( const String & p_name, UIntArray p_faces, 
						   FloatArray p_size, Mesh::eTYPE p_type=Mesh::eCustom);
		/**
		 * Writes all the meshes in a file
		 *@param p_file : [in] The file to write in
		 *@return true if successful, false if not
		 */
		bool Write( const String & p_path)const;
		/**
		 * Reads all the meshes from a file
		 *@param p_file : [in] The file to read from
		 *@return true if successful, false if not
		 */
		bool Read( const String & p_path);

	private:
		bool _addElement( Mesh * p_element);
	};
}

#endif

