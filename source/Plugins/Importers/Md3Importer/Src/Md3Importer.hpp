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
		:	public castor3d::Importer
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
				: castor::AlignedFrom< castor::Point3r >
		{
			char m_strName[64];
			castor::Point3r m_position;
			castor3d::real m_rotation[3][3];
		};

		struct Md3Bone
		{
			castor3d::real m_mins[3];
			castor3d::real m_maxs[3];
			castor3d::real m_position[3];
			castor3d::real m_scale;
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
			castor3d::real m_textureCoord[2];
		};

		struct Md3Skin
		{
			char m_strName[68];
		};

	public:
		/**
		 * Constructor
		 */
		Md3Importer( castor3d::Engine & p_engine );

		static castor3d::ImporterUPtr create( castor3d::Engine & p_engine );

	private:
		/**
		 *\copydoc		castor3d::Importer::doImportScene
		 */
		bool doImportScene( castor3d::Scene & p_scene )override;
		/**
		 *\copydoc		castor3d::Importer::doImportMesh
		 */
		bool doImportMesh( castor3d::Mesh & p_mesh )override;

		void doReadMD3Data( castor3d::Mesh & p_mesh, castor3d::LegacyPass & p_pass );
		void doConvertDataStructures( castor3d::Mesh & p_mesh, Md3MeshInfo p_meshHeader );
		bool doLoadSkin( castor3d::Mesh & p_mesh, castor::Path const & p_strSkin );
		bool doLoadShader( castor3d::Mesh & p_mesh, castor::Path const & p_strShader );
		void doCleanUp();

	public:
		Md3Header m_header;

		Md3Skin * m_skins;
		Md3TexCoord * m_texCoords;
		Md3Face * m_triangles;
		Md3Triangle * m_vertices;
		Md3Bone * m_bones;

		Md3Tag * m_tags;
		int m_numOfTags;
		castor3d::MeshSPtr * m_links;
		castor3d::SubmeshPtrStrMap m_mapSubmeshesByName;

		castor::BinaryFile * m_pFile;
	};
}

#endif
