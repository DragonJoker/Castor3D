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
#ifndef ___C3D_ObjImporter___
#define ___C3D_ObjImporter___

#include "ExternalImporter.h"

namespace Castor3D
{
	//! OBJ file importer
	/*!
	Imports 3D data from OBJ files
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class CS3D_API ObjImporter : public ExternalImporter
	{
	private:
		std::vector <Vector3f *> m_arrayVertex;
		std::vector <Point2D<float>>  m_textureCoords;
		int m_iNbTexCoords;
		bool m_bReadingVertex;
		bool m_bReadingFaces;
		bool m_objectHasUV;
		Mesh * m_pMesh;
		FileIO * m_pMatFile;
		std::map <String, Submesh *> m_mapSubmeshes;
		Submesh * m_pCurrentSubmesh;
		std::map <String, Submesh *> m_mapSmoothGroupSubmesh;
		std::map <String, SmoothingGroup *> m_mapSmoothGroups;
		SmoothingGroup * m_pSmoothingGroup;
		float m_fAlpha;

		Material * m_pCurrentMaterial;

		FileIO * m_pFile;

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
		void _readMatFileIO( const String & p_strFileName);
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