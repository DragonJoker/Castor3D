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
#ifndef ___C3D_ObjImporter___
#define ___C3D_ObjImporter___

#include <Castor3D/Importer.hpp>
#include <Castor3D/SmoothingGroup.hpp>

#ifndef _WIN32
#	define C3D_Obj_API
#else
#	ifdef ObjImporter_EXPORTS
#		define C3D_Obj_API __declspec(dllexport)
#	else
#		define C3D_Obj_API __declspec(dllimport)
#	endif
#endif

namespace Castor3D
{
	//! OBJ file importer
	/*!
	Imports 3D data from OBJ files
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class ObjImporter : public Importer, public MemoryTraced<ObjImporter>
	{
	private:
		Point3rArray m_arrayVertex;
		Point2rArray  m_textureCoords;
		int m_iNbTexCoords;
		bool m_bReadingVertex;
		bool m_bReadingFaces;
		bool m_objectHasUV;
		MeshPtr m_pMesh;
		File * m_pMatFile;
		SubmeshPtrStrMap m_mapSubmeshes;
		SubmeshPtr m_pCurrentSubmesh;
		SubmeshPtrStrMap m_mapSmoothGroupSubmesh;
		IntStrMap m_mapSmoothGroups;
		int m_iGroup;
		float m_fAlpha;

		MaterialPtr m_pCurrentMaterial;

		File * m_pFile;

	public:
		/**
		 * Constructor
		 */
		ObjImporter();

	private:
		virtual bool _import();

		void _createSubmesh();
		void _readObjFile();
		void _selectSubmesh( String const & p_strName);
		void _applyMaterial( String const & p_strMaterialName);
		void _readMatFile( String const & p_strFileName);
		void _readMatLightComponent( String const & p_strLine);
		void _readMatTransparency( String const & p_strLine);
		void _readMatLightRefDifExp( String const & p_strLine);
		void _readSubmeshInfo( String const & p_strLine);
		void _readGroupInfo( String const & p_strLine);
		void _readVertexInfo( String const & p_strLine);
		void _readFaceInfo( String const & p_strLine);
	};
}

#endif
