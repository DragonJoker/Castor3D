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
#ifndef ___C3D_ObjImporter___
#define ___C3D_ObjImporter___

#include "ExternalImporter.h"
#include "../geometry/basic/SmoothingGroup.h"

namespace Castor3D
{
	//! OBJ file importer
	/*!
	Imports 3D data from OBJ files
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class C3D_API ObjImporter : public ExternalImporter, public MemoryTraced<ObjImporter>
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
		void _selectSubmesh( const String & p_strName);
		void _applyMaterial( const String & p_strMaterialName);
		void _readMatFile( const String & p_strFileName);
		void _readMatLightComponent( const String & p_strLine);
		void _readMatTransparency( const String & p_strLine);
		void _readMatLightRefDifExp( const String & p_strLine);
		void _readSubmeshInfo( const String & p_strLine);
		void _readGroupInfo( const String & p_strLine);
		void _readVertexInfo( const String & p_strLine);
		void _readFaceInfo( const String & p_strLine);
	};
}

#endif