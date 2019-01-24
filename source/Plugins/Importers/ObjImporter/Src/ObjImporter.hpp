/*
See LICENSE file in root folder
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
		CU_DeclareMap( castor3d::PassRPtr, castor::Point3f, FloatPass );
		CU_DeclareVector( castor3d::TextureLayoutSPtr, Texture );
		typedef std::shared_ptr< std::thread > ThreadSPtr;

	public:
		/**
		 * Constructor
		 */
		explicit ObjImporter( castor3d::Engine & engine );

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
