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
		: public Castor3D::Importer
	{
	public:
		LwoImporter( Castor3D::Engine & p_engine );
		virtual ~LwoImporter();

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

		bool DoRead( stLWO_CHUNK & p_chunk );
		void DoProcess( Castor3D::Mesh & p_mesh, stLWO_CHUNK & p_chunk );
		void DoDiscard( stLWO_CHUNK & p_chunk );
		void DoDiscard( stLWO_SUBCHUNK & p_chunk );
		bool DoIsValidChunk( stLWO_CHUNK & p_chunk, stLWO_CHUNK & p_pParent );
		void DoToStr( char p_szId[5], UI4 p_uiId );
		bool DoRead( std::string & p_strString );
		bool DoRead( Castor::String const & p_strTabs, stLWO_SUBCHUNK & p_pSubchunk );
		UI2 DoReadVX( UI4 & p_index );
		bool DoIsChunk( eID_TAG p_eId );
		bool DoIsTagId( eID_TAG p_eId );
		void DoParsePTag( stLWO_CHUNK & p_chunk );
		void DoReadBlock( stLWO_SUBCHUNK & p_pSubchunk, Castor3D::PassSPtr p_pass );
		void DoSetChannel( Castor3D::TextureUnitSPtr p_pTexture, eTEX_CHANNEL p_channel );
		void DoReadShdr( stLWO_SUBCHUNK & p_pSubchunk );
		void DoReadGrad( stLWO_SUBCHUNK & p_pSubchunk );
		void DoReadProc( stLWO_SUBCHUNK & p_pSubchunk );
		void DoReadIMap( stLWO_SUBCHUNK & p_pSubchunk, eTEX_CHANNEL & p_channel );
		void DoReadTMap( stLWO_SUBCHUNK & p_pSubchunk );
		UI2 DoReadBlockHeader( stLWO_SUBCHUNK & p_pSubchunk, eTEX_CHANNEL & p_channel );
		void DoParseTags( stLWO_CHUNK & p_chunk );
		void DoParseSurf( stLWO_CHUNK & p_chunk );
		void DoParseClip( stLWO_CHUNK & p_chunk );
		void DoParsePnts( stLWO_CHUNK & p_chunk );
		void DoParseVMap( stLWO_CHUNK & p_chunk );
		void DoParsePols( stLWO_CHUNK & p_chunk );
		void DoParseLayr( stLWO_CHUNK & p_chunk );

	public:
		Castor::BinaryFile * m_file;
		std::vector< Castor::Point3f > m_arrayPoints;
		bool m_bHasUv;
		std::vector< Castor::Point2f > m_arrayUvs;
		std::vector< std::string > m_arrayTags;
		std::vector< SubmeshPtrStrPair > m_arraySubmeshByMatName;
		Castor3D::SubmeshSPtr m_pSubmesh;
		ImageVxMap m_mapImages;
		std::string m_strName;
		std::string m_strSource;
		std::map< std::string, Castor3D::TextureUnitSPtr > m_mapTextures;
		bool m_bIgnored;
	};
}

#endif
