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
#ifndef ___ExternalImporter___
#define ___ExternalImporter___

#include "../scene/Module_Scene.h"
#include "../geometry/Module_Geometry.h"


namespace Castor3D
{
#	define MAX_TEXTURES 100

	struct CS3D_API ImportedVertex3
	{
		float x, y, z;
	};

	struct CS3D_API ImportedVertex2 
	{
		float x, y;
	};

	struct CS3D_API ImportedFace
	{
		int m_vertIndex[3];
		int m_coordIndex[3];
	};

	struct CS3D_API ImportedMaterialInfo
	{
		Char  m_strName[255];
		Char  m_strFile[255];
		unsigned char  m_color[3];
		float m_fDiffuse[3];
		float m_fAmbient[3];
		float m_fSpecular[3];
		float m_fShininess;
		int m_texureId;
		float m_uTile;
		float m_vTile;
		float m_uOffset;
		float m_vOffset;

		ImportedMaterialInfo()
			:	m_texureId( -1),
				m_uTile( 0),
				m_vTile( 0),
				m_uOffset( 0),
				m_vOffset( 0),
				m_fShininess( 50)
		{
			m_fDiffuse[0] = 1;
			m_fDiffuse[1] = 1;
			m_fDiffuse[2] = 1;
			
			m_fSpecular[0] = 1;
			m_fSpecular[1] = 1;
			m_fSpecular[2] = 1;

			memset( m_fAmbient, 0, sizeof( m_fAmbient));
			memset( m_strName, 0, 255);
			memset( m_strFile, 0, 255);
			memset( m_color, 0, 3);
		}
	};

	struct CS3D_API Imported3DObject 
	{
		int  m_numOfVerts;
		int  m_numOfFaces;
		int  m_numTexVertex;
		int  m_materialID;
		bool m_hasTexture;
		Char m_strName[255];
		ImportedVertex3  * m_vertex;
		ImportedVertex3  * m_normals;
		ImportedVertex2  * m_texVerts;
		ImportedFace	 * m_faces;
	};

	struct CS3D_API Imported3DModel 
	{
		int m_numOfObjects;
		int m_numOfMaterials;
		std::vector<ImportedMaterialInfo> m_materials;
		std::vector<Imported3DObject> m_objects;
		int m_nodeIndex;
	};

	struct CS3D_API ImportedIndices
	{							
		unsigned short a, b, c, bVisible;
	};

	class CS3D_API ExternalImporter
	{
	protected:
		String m_fileName;
		String m_filePath;
		SceneNodePtrArray m_nodes;
		Scene * m_pScene;

	public:
		GeometryStrMap m_geometries;

	public:
		ExternalImporter()
			:	m_fileName( C3DEmptyString)
		{
		}

		bool Import( const String & p_fileName);

	protected:
		virtual bool _import()=0;
		void _convertToMesh( Mesh * p_mesh, Imported3DModel * p_model);

	public:
		inline SceneNode * GetNode() { return (m_nodes.empty() ? NULL : m_nodes[0]); }
	};
}

#endif