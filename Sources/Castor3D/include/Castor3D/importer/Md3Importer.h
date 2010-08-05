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
#ifndef ___C3D_Md3Importer___
#define ___C3D_Md3Importer___

#include "ExternalImporter.h"

namespace Castor3D
{
	struct CS3D_API Md3Header
	{ 
		char	m_fileID[4];
		int		m_version;
		char	m_strFile[68];
		int		m_numFrames;
		int		m_numTags;
		int		m_numMeshes;
		int		m_numMaxSkins;
		int		m_headerSize;
		int		m_tagStart;
		int		m_tagEnd;
		int		m_fileSize;
	};

	struct CS3D_API Md3MeshInfo
	{
		char	m_meshID[4];
		char	m_strName[68];
		int		m_numMeshFrames;
		int		m_numSkins;
		int     m_numVertices;
		int		m_numTriangles;
		int		m_triStart;
		int		m_headerSize;
		int     m_uvStart;
		int		m_vertexStart;
		int		m_meshSize;
	};

	struct CS3D_API Md3Tag
	{
		char		m_strName[64];
		ImportedVertex3	m_position;
		float		m_rotation[3][3];
	};

	struct CS3D_API Md3Bone
	{
		float	m_mins[3];
		float	m_maxs[3];
		float	m_position[3];
		float	m_scale;
		char	m_creator[16];
	};


	struct CS3D_API Md3Triangle
	{
	   signed short	 m_vertex[3];
	   unsigned char m_normal[2];
	};

	struct CS3D_API Md3Face
	{
	   int m_vertexIndices[3];				
	};

	struct CS3D_API Md3TexCoord
	{
	   float m_textureCoord[2];
	};

	struct CS3D_API Md3Skin 
	{
		char m_strName[68];
	};

	class CS3D_API Md3Importer : public ExternalImporter
	{
	private:
		Md3Header			m_header;

		Md3Skin			*	m_skins;
		Md3TexCoord		*	m_texCoords;
		Md3Face			*	m_triangles;
		Md3Triangle		*	m_vertices;
		Md3Bone			*	m_bones;

		Md3Tag			*	m_tags;
		int					m_numOfTags;
		Imported3DModel	**	m_links;

		String				m_textureName;

		File			*	m_pFile;

	public:
		Md3Importer( const String & p_textureName = C3DEmptyString);								

	private:
		virtual bool _import();
		bool _loadSkin( Imported3DModel * p_model, const String & p_strSkin);
		bool _loadShader( Imported3DModel * p_model, const String & p_strShader);
		void _readMD3Data( Imported3DModel * p_model);
		void _convertDataStructures( Imported3DModel * p_model, Md3MeshInfo p_meshHeader);
		void _cleanUp();
	};
}

#endif