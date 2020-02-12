/*
See LICENSE file in root folder
*/
#ifndef ___OBJ_IMPORTER_H___
#define ___OBJ_IMPORTER_H___

#include "ObjImporterPrerequisites.hpp"

namespace Obj
{
	/**
	\author		Sylvain DOREMUS
	\date		25/08/2010
	\brief		OBJ file importer
	*/
	class ObjImporter
		:	public castor3d::MeshImporter
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

		static castor3d::MeshImporterUPtr create( castor3d::Engine & engine );

	private:
		/**
		 *\copydoc		castor3d::MeshImporter::doImportMesh
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
		castor3d::TextureUnitSPtr doLoadTexture( castor::String value
			, castor3d::TextureConfiguration const & configuration );
		void doReadMaterials( castor3d::Mesh & mesh
			, castor::Path const & pathMatFile );

	private:
		castor3d::MaterialPtrArray m_loadedMaterials;
		castor::TextFile * m_file;
	};
}

#endif
