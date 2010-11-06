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
#ifndef ___C3D_Md3Importer___
#define ___C3D_Md3Importer___

#include "ExternalImporter.h"

namespace Castor3D
{
	//! MD3 file importer
	/*!
	Imports data from MD3 (Quake3) files
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class CS3D_API Md3Importer : public ExternalImporter
	{
	private:
		struct Md3Header
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

		struct Md3MeshInfo
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

		struct Md3Tag
		{
			char		m_strName[64];
			Point3r	m_position;
			real		m_rotation[3][3];
		};

		struct Md3Bone
		{
			real	m_mins[3];
			real	m_maxs[3];
			real	m_position[3];
			real	m_scale;
			char	m_creator[16];
		};


		struct Md3Triangle
		{
		   signed short	 m_vertex[3];
		   unsigned char m_normal[2];
		};

		struct Md3Face
		{
		   int m_vertexIndices[3];				
		};

		struct Md3TexCoord
		{
		   real m_textureCoord[2];
		};

		struct Md3Skin 
		{
			char m_strName[68];
		};

	public:
		Md3Header			m_header;

		Md3Skin			*	m_skins;
		Md3TexCoord		*	m_texCoords;
		Md3Face			*	m_triangles;
		Md3Triangle		*	m_vertices;
		Md3Bone			*	m_bones;

		Md3Tag			*	m_tags;
		int					m_numOfTags;
		MeshPtr			*	m_links;
		SubmeshPtrStrMap	m_mapSubmeshesByName;

		File			*	m_pFile;

	public:
		/**
		 * Constructor
		 */
		Md3Importer();

	private:
		virtual bool _import();
		void _readMD3Data( MeshPtr p_pMesh);
		void _convertDataStructures( MeshPtr p_pMesh, Md3MeshInfo p_meshHeader);
		bool _loadSkin( const String & p_strSkin);
		bool _loadShader( MeshPtr p_pMesh, const String & p_strShader);
		void _cleanUp();
	};
}

#endif