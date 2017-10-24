/* See LICENSE file in root folder */
#ifndef ___LWO_IMPORTER_H___
#define ___LWO_IMPORTER_H___

#include "LwoPrerequisites.hpp"

namespace Lwo
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/09/2012
	\~english
	\brief		LWO file importer
	\remark		Imports data from LWO (LightWave Object) files
	\~french
	\brief		Importeur de fichiers LWO
	\remark		Importe les données à partir de fichiers LWO (LightWave Object)
	*/
	class LwoImporter
		: public castor3d::Importer
	{
	public:
		explicit LwoImporter( castor3d::Engine & p_engine );
		virtual ~LwoImporter();

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

		bool doRead( stLWO_CHUNK & p_chunk );
		void doProcess( castor3d::Mesh & p_mesh, stLWO_CHUNK & p_chunk );
		void doDiscard( stLWO_CHUNK & p_chunk );
		void doDiscard( stLWO_SUBCHUNK & p_chunk );
		bool doIsValidChunk( stLWO_CHUNK & p_chunk, stLWO_CHUNK & p_pParent );
		void doToStr( char p_szId[5], UI4 p_uiId );
		bool doRead( std::string & p_strString );
		bool doRead( castor::String const & p_strTabs, stLWO_SUBCHUNK & p_pSubchunk );
		UI2 doReadVX( UI4 & p_index );
		bool doIsChunk( eID_TAG p_eId );
		bool doIsTagId( eID_TAG p_eId );
		void doParsePTag( stLWO_CHUNK & p_chunk );
		void doReadBlock( stLWO_SUBCHUNK & p_pSubchunk, castor3d::PassSPtr p_pass );
		void doSetChannel( castor3d::TextureUnitSPtr p_pTexture, eTEX_CHANNEL p_channel );
		void doReadShdr( stLWO_SUBCHUNK & p_pSubchunk );
		void doReadGrad( stLWO_SUBCHUNK & p_pSubchunk );
		void doReadProc( stLWO_SUBCHUNK & p_pSubchunk );
		void doReadIMap( stLWO_SUBCHUNK & p_pSubchunk, eTEX_CHANNEL & p_channel );
		void doReadTMap( stLWO_SUBCHUNK & p_pSubchunk );
		UI2 doReadBlockHeader( stLWO_SUBCHUNK & p_pSubchunk, eTEX_CHANNEL & p_channel );
		void doParseTags( stLWO_CHUNK & p_chunk );
		void doParseSurf( stLWO_CHUNK & p_chunk );
		void doParseClip( stLWO_CHUNK & p_chunk );
		void doParsePnts( stLWO_CHUNK & p_chunk );
		void doParseVMap( stLWO_CHUNK & p_chunk );
		void doParsePols( stLWO_CHUNK & p_chunk );
		void doParseLayr( stLWO_CHUNK & p_chunk );

	public:
		castor::BinaryFile * m_file;
		std::vector< castor::Point3f > m_arrayPoints;
		bool m_bHasUv;
		std::vector< castor::Point2f > m_arrayUvs;
		std::vector< std::string > m_arrayTags;
		std::vector< SubmeshPtrStrPair > m_arraySubmeshByMatName;
		castor3d::SubmeshSPtr m_pSubmesh;
		ImageVxMap m_mapImages;
		std::string m_strName;
		std::string m_strSource;
		std::map< std::string, castor3d::TextureUnitSPtr > m_mapTextures;
		bool m_bIgnored;
	};
}

#endif
