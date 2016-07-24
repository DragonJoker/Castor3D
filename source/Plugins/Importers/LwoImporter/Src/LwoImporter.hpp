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
