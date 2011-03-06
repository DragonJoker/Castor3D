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

#include "../../Prerequisites.h"
#include "Mesh.h"

namespace Castor3D
{
	//! The Mesh manager representation
	/*!
	This is the handler of all created meshes, you can create a mesh only from here
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class C3D_API MeshManager : public Serialisable, public Castor::Templates::Manager<Mesh>
	{
	private:
		friend class Castor::Templates::Manager<Mesh>;

		SceneManager			*	m_pParent;

	public:
		/**@name Construction / Destruction */
		//@{
		/**
		 * Constructor
		 */
		MeshManager( SceneManager * p_pParent);
		/**
		 * Destructor
		 */
		~MeshManager();
		//@}

		/**
		 * Flushes all the create meshes
		 */
		void Clear();
		/**
		 * Creates a mesh from the given parameters
		 *@param p_name : [in] The mesh name
		 *@param p_faces : [in] The faces numbers
		 *@param p_size : [in] The sizes values
		 *@param p_type : [in] The mesh type
		 *@return The created mesh, NULL if unknown MeshType (but is it possible ?)
		 */
		MeshPtr CreateMesh( const String & p_name, const UIntArray & p_faces, 
			const FloatArray & p_size, eMESH_TYPE p_type=eCustom);

		/**@name Inherited methods from Serialisable */
		//@{
		virtual bool Save( File & p_file)const;
		virtual bool Load( File & p_file);
		//@}

	private:
		bool _addElement( MeshPtr p_element);
	};
}

#endif

