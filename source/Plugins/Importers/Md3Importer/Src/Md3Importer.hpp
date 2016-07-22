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
#ifndef ___C3D_Md3Importer___
#define ___C3D_Md3Importer___

#include <Mesh/Importer.hpp>

namespace C3DMd3
{
	//! MD3 file importer
	/*!
	Imports data from MD3 (Quake3) files
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class Md3Importer
		:	public Castor3D::Importer
	{
	private:
		struct Md3Header
		{
			char m_fileID[4];
			int m_version;
			char m_strFile[68];
			int m_numFrames;
			int m_numTags;
			int m_numMeshes;
			int m_numMaxSkins;
			int m_headerSize;
			int m_tagStart;
			int m_tagEnd;
			int m_fileSize;
		};

		struct Md3MeshInfo
		{
			char m_meshID[4];
			char m_strName[68];
			int m_numMeshFrames;
			int m_numSkins;
			int m_numVertices;
			int m_numTriangles;
			int m_triStart;
			int m_headerSize;
			int m_uvStart;
			int m_vertexStart;
			int m_meshSize;
		};

		struct Md3Tag
				: Castor::AlignedFrom< Castor::Point3r >
		{
			char m_strName[64];
			Castor::Point3r m_position;
			Castor3D::real m_rotation[3][3];
		};

		struct Md3Bone
		{
			Castor3D::real m_mins[3];
			Castor3D::real m_maxs[3];
			Castor3D::real m_position[3];
			Castor3D::real m_scale;
			char m_creator[16];
		};


		struct Md3Triangle
		{
			signed short  m_vertex[3];
			uint8_t m_normal[2];
		};

		struct Md3Face
		{
			int m_vertexIndices[3];
		};

		struct Md3TexCoord
		{
			Castor3D::real m_textureCoord[2];
		};

		struct Md3Skin
		{
			char m_strName[68];
		};

	public:
		/**
		 * Constructor
		 */
		Md3Importer( Castor3D::Engine & p_engine );

		static Castor3D::ImporterUPtr Create( Castor3D::Engine & p_engine );

	private:
		virtual Castor3D::SceneSPtr DoImportScene();
		virtual Castor3D::MeshSPtr DoImportMesh( Castor3D::Scene & p_scene );
		void DoReadMD3Data( Castor3D::Scene & p_scene, Castor3D::MeshSPtr p_pMesh, Castor3D::PassSPtr p_pPass );
		void DoConvertDataStructures( Castor3D::MeshSPtr p_pMesh, Md3MeshInfo p_meshHeader );
		bool DoLoadSkin( Castor3D::Scene & p_scene, Castor::Path const & p_strSkin );
		bool DoLoadShader( Castor3D::Scene & p_scene, Castor3D::MeshSPtr p_pMesh, Castor::Path const & p_strShader );
		void DoCleanUp();

	public:
		Md3Header m_header;

		Md3Skin * m_skins;
		Md3TexCoord * m_texCoords;
		Md3Face * m_triangles;
		Md3Triangle * m_vertices;
		Md3Bone * m_bones;

		Md3Tag * m_tags;
		int m_numOfTags;
		Castor3D::MeshSPtr * m_links;
		Castor3D::SubmeshPtrStrMap m_mapSubmeshesByName;

		Castor::BinaryFile * m_pFile;
	};
}

#endif
