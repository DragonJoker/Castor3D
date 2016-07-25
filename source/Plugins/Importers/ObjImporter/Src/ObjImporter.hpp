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
#ifndef ___OBJ_IMPORTER_H___
#define ___OBJ_IMPORTER_H___

#include "ObjImporterPrerequisites.hpp"

namespace Obj
{
	/*!
	\author		Sylvain DOREMUS
	\date		25/08/2010
	\brief		OBJ file importer
	*/
	class ObjImporter
		:	public Castor3D::Importer
		,	private Castor::NonCopyable
	{
	private:
		DECLARE_MAP( Castor3D::PassRPtr, Castor::Point3f, FloatPass );
		DECLARE_VECTOR( Castor3D::TextureLayoutSPtr, Texture );
		typedef std::shared_ptr< std::thread > ThreadSPtr;

	public:
		/**
		 * Constructor
		 */
		ObjImporter( Castor3D::Engine & p_engine );

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

		void DoReadObjFile( Castor3D::Mesh & p_mesh );
		void DoCreateSubmesh( Castor3D::Mesh & p_mesh, Castor::String const & p_mtlName, std::vector< Castor3D::FaceIndices > && p_faces, Castor3D::InterleavedVertexArray && p_vertex );
		void DoParseTexParams( Castor::String & p_strValue, float * p_offset, float * p_scale, float * p_turb );
		void DoParseTexParam( Castor::String const & p_strParam, float * p_values );
		bool DoIsValidValue( Castor::String const & p_strParam, Castor::String const & p_strSrc, uint32_t p_index );
		void DoAddTexture( Castor::String const & p_strValue, Castor3D::Pass & p_pass, Castor3D::TextureChannel p_channel );
		void DoReadMaterials( Castor3D::Mesh & p_mesh, Castor::Path const & p_pathMatFile );

	private:
		Castor::ImageCache & m_collImages;
		Castor3D::MaterialPtrArray m_arrayLoadedMaterials;
		TextureArray m_arrayTextures;
		Castor::TextFile * m_pFile;
		FloatPassMap m_mapOffsets;
		FloatPassMap m_mapScales;
		FloatPassMap m_mapTurbulences;
		ThreadSPtr m_pThread;
	};
}

#endif
