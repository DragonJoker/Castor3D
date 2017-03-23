/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
		/**
		 *\copydoc		Castor3D::Importer::DoImportScene
		 */
		bool DoImportScene( Castor3D::Scene & p_scene )override;
		/**
		 *\copydoc		Castor3D::Importer::DoImportMesh
		 */
		bool DoImportMesh( Castor3D::Mesh & p_mesh )override;

		void DoReadMD3Data( Castor3D::Mesh & p_mesh, Castor3D::LegacyPass & p_pass );
		void DoConvertDataStructures( Castor3D::Mesh & p_mesh, Md3MeshInfo p_meshHeader );
		bool DoLoadSkin( Castor3D::Mesh & p_mesh, Castor::Path const & p_strSkin );
		bool DoLoadShader( Castor3D::Mesh & p_mesh, Castor::Path const & p_strShader );
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
