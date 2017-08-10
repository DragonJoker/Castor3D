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
		:	public castor3d::Importer
		,	private castor::NonCopyable
	{
	private:
		DECLARE_MAP( castor3d::PassRPtr, castor::Point3f, FloatPass );
		DECLARE_VECTOR( castor3d::TextureLayoutSPtr, Texture );
		typedef std::shared_ptr< std::thread > ThreadSPtr;

	public:
		/**
		 * Constructor
		 */
		ObjImporter( castor3d::Engine & engine );

		static castor3d::ImporterUPtr create( castor3d::Engine & engine );

	private:
		/**
		 *\copydoc		castor3d::Importer::doImportScene
		 */
		bool doImportScene( castor3d::Scene & scene )override;
		/**
		 *\copydoc		castor3d::Importer::doImportMesh
		 */
		bool doImportMesh( castor3d::Mesh & mesh )override;

		void doReadObjFile( castor3d::Mesh & mesh );
		void doCreateSubmesh( castor3d::Mesh & mesh
			, castor::String const & mtlName
			, bool normals
			, std::vector< castor3d::FaceIndices > && faces
			, castor3d::InterleavedVertexArray && vertex );
		void doParseTexParams( castor::String & strValue
			, float * offset
			, float * scale
			, float * turb );
		void doParseTexParam( castor::String const & strParam
			, float * values );
		bool doIsValidValue( castor::String const & strParam
			, castor::String const & strSrc
			, uint32_t index );
		void doAddTexture( castor::String const & strValue
			, castor3d::Pass & pass
			, castor3d::TextureChannel channel );
		void doReadMaterials( castor3d::Mesh & mesh
			, castor::Path const & pathMatFile );

	private:
		castor::ImageCache & m_collImages;
		castor3d::MaterialPtrArray m_arrayLoadedMaterials;
		TextureArray m_arrayTextures;
		castor::TextFile * m_pFile;
		FloatPassMap m_mapOffsets;
		FloatPassMap m_mapScales;
		FloatPassMap m_mapTurbulences;
		ThreadSPtr m_pThread;
	};
}

#endif
