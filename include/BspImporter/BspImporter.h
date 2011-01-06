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
#ifndef ___C3D_BspImporter___
#define ___C3D_BspImporter___

#include <Castor3D/importer/Importer.h>

#	ifdef BspImporter_EXPORTS
#		define C3D_Bsp_API __declspec(dllexport)
#	else
#		define C3D_Bsp_API __declspec(dllimport)
#	endif
/*
namespace Castor3D
{
	#define FACE_POLYGON	1

	struct C3D_API BSPHeader
	{
		char m_strID[4];
		int m_version;
	}; 

	struct C3D_API BSPLump
	{
		int m_offset;
		int m_length;
	};

	struct C3D_API BSPVertex
	{
		Point3r m_position;
		Point2r m_textureCoord;
		Point2r m_lightmapCoord;
		Point3r m_normal;
		unsigned char m_color[4];
	};

	struct C3D_API BSPFace
	{
		int m_textureID;
		int m_effect;
		int m_type;
		int m_startVertIndex;
		int m_numOfVerts;
		int m_startIndex;
		int m_numOfIndices;
		int m_lightmapID;
		int m_mapCorner[2];
		int m_mapSize[2];
		Point3r m_mapPos;
		Point3r m_mapVecs[2];
		Point3r m_normal;
		int m_size[2];
	};

	struct C3D_API BSPTexture
	{
		char m_strName[64];
		int m_flags;
		int m_contents;
	};

	enum eLumps
	{
		kEntities = 0,
		kTextures,
		kPlanes,
		kNodes,
		kLeafs,
		kLeafFaces,
		kLeafBrushes,
		kModels,
		kBrushes,
		kBrushSides,
		kVertices,
		kIndices,
		kShaders,
		kFaces,
		kLightmaps,
		kLightVolumes,
		kVisData,
		kMaxLumps
	};

	class BSPBitset
	{
	private:
		unsigned int *m_bits;
		int m_size;

	public:
		BSPBitset()
			:	m_bits( 0),
				m_size( 0)
		{}
		~BSPBitset() 
		{
			if (m_bits) 
			{
				delete m_bits;
				m_bits = NULL;
			}
		}

		void Resize( int count) 
		{ 
			m_size = count/32 + 1;
			if(m_bits) 
			{
				delete m_bits;
				m_bits = 0;
			}

			m_bits = new unsigned int[m_size];
			ClearAll();
		}

		void Set(int i) 
		{
			m_bits[i >> 5] |= (1 << (i & 31));
		}

		int On(int i) 
		{
			return m_bits[i >> 5] & (1 << (i & 31 ));
		}

		void Clear(int i) 
		{
			m_bits[i >> 5] &= ~(1 << (i & 31));
		}

		void ClearAll() 
		{
			memset(m_bits, 0, sizeof(unsigned int) * m_size);
		}
	};


	class C3D_Bsp_API BspImporter : public Importer, public MemoryTraced<BSPImporter>
	{
	private:
		int  m_numOfVerts;
		int  m_numOfFaces;
		int  m_numOfIndices;
		int  m_numOfTextures;

		int			*	m_indices;
		BSPVertex	*	m_verts;
		BSPFace		*	m_faces;

		File		*	m_pFile;

		unsigned int m_textures[MAX_TEXTURES];	

		BSPBitset m_facesDrawn;

	public:
		BspImporter( SceneManager * p_pManager);
		~BspImporter();

		virtual bool _import();

	private:
		void _renderLevel( const Point3r & p_pos);
		void _destroy();
		void _findTextureExtension( const String & p_strFileName);
		void _renderFace( int p_faceIndex);
		void _convertToMesh( MeshPtr p_mesh, BSPTexture * p_textures);
	};
}
*/
#endif