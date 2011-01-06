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
#ifndef ___Importer___
#define ___Importer___

#include "../Prerequisites.h"

namespace Castor3D
{
#	define MAX_TEXTURES 100

	//! External file format importer
	/*!
	Base class for external file importation
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class C3D_API Importer
	{
	protected:
		String m_fileName;
		String m_filePath;
		SceneNodePtrArray m_nodes;
		ScenePtr m_pScene;
		SceneManager * m_pManager;

	public:
		GeometryPtrStrMap m_geometries;

	public:
		/**
		 * Constructor
		 */
		Importer( SceneManager * p_pManager)
			:	m_fileName( C3DEmptyString)
			,	m_pManager( p_pManager)
		{
		}
		/**
		 * Import function
		 *@param p_fileName : [in] The file path
		 *@return true if successful, false if not
		 */
		bool Import( const String & p_fileName);

		/**@name Accessors */
		//@{
		SceneNodePtr GetNode();
		//@}

	protected:
		virtual bool _import()=0;
//		void _convertToMesh( MeshPtr p_mesh, Imported3DModel * p_model);
	};
}

#endif