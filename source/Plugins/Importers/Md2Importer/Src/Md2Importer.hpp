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
#ifndef ___C3D_Md2Importer___
#define ___C3D_Md2Importer___

#include <Mesh/Importer.hpp>

// These are the needed defines for the max values when loading .MD2 files
#define MD2_MAX_TRIANGLES		4096
#define MD2_MAX_VERTICES		2048
#define MD2_MAX_TEXCOORDS		2048
#define MD2_MAX_FRAMES			512
#define MD2_MAX_SKINS			32
#define MD2_MAX_FRAMESIZE		(MD2_MAX_VERTICES * 4 + 128)

namespace C3DMd2
{
	//! MD2 file importer
	/*!
	Imports data from MD2 (Quake2) files
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class Md2Importer
		: public Castor3D::Importer
	{
	private:
		struct Md2Header
		{
			int m_magic;
			int m_version;
			int m_skinWidth;
			int m_skinHeight;
			int m_frameSize;
			int m_numSkins;
			int m_numVertices;
			int m_numTexCoords;
			int m_numTriangles;
			int m_numGlCommands;
			int m_numFrames;
			int m_offsetSkins;
			int m_offsetTexCoords;
			int m_offsetTriangles;
			int m_offsetFrames;
			int m_offsetGlCommands;
			int om_ffsetEnd;
		};

		struct Md2AliasTriangle
		{
			uint8_t m_vertex[3];
			uint8_t m_lightNormalIndex;
		};

		struct Md2Vertex
		{
			Castor3D::real m_coords[3];
			Castor3D::real m_normal[3];
		};

		struct Md2Face
		{
			short m_vertexIndices[3];
			short m_textureIndices[3];
		};

		struct Md2TexCoord
		{
			short u, v;
		};

		struct Md2AliasFrame
		{
			Castor3D::real m_scale[3];
			Castor3D::real m_translate[3];
			char m_name[16];
			Md2AliasTriangle m_aliasVertices[1];
		};

		struct Md2Frame
		{
			std::string m_strName;
			Md2Vertex * m_vertices;
		};

		typedef char Md2Skin[64];

	public:
		Md2Importer( Castor3D::Engine & p_engine, Castor::String const & p_textureName = Castor::cuEmptyString );

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

		void DoReadMD2Data( Castor3D::Pass & p_pass );
		void DoConvertDataStructures( Castor3D::Mesh & p_mesh );
		void DoCleanUp();

	private:
		Md2Header m_header;
		Md2Skin * m_skins;
		Md2TexCoord * m_texCoords;
		Md2Face * m_triangles;
		Md2Frame * m_frames;
		Castor::String m_textureName;
		Castor::BinaryFile * m_pFile;
	};
}


#endif
