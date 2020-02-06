#include "LwoImporter.hpp"
#include "LwoChunk.hpp"
#include "LwoSubChunk.hpp"

#include <Colour.hpp>
#include <Material.hpp>
#include <Pass.hpp>
#include <TextureUnit.hpp>
#include <Mesh.hpp>
#include <Submesh.hpp>
#include <Vertex.hpp>
#include <Buffer.hpp>
#include <Geometry.hpp>
#include <Face.hpp>
#include <Scene.hpp>
#include <SceneNode.hpp>
#include <Version.hpp>
#include <Plugin.hpp>
#include <Engine.hpp>
#include <Texture.hpp>
#include <Vertex.hpp>
#include <InitialiseEvent.hpp>
#include <StaticTexture.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Lwo
{
	LwoImporter::LwoImporter( Castor3D::Engine * p_pEngine )
		: Importer( p_pEngine )
		, m_pFile( NULL )
		, m_bIgnored( false )
		, m_bHasUv( false )
	{
	}

	LwoImporter::~LwoImporter()
	{
	}

	SceneSPtr LwoImporter::DoImportScene()
	{
		MeshSPtr l_pMesh = DoImportMesh();
		SceneSPtr l_pScene;

		if ( l_pMesh )
		{
			l_pMesh->GenerateBuffers();
			l_pScene = m_pEngine->CreateScene( cuT( "Scene_LWO" ) );
			SceneNodeSPtr l_pNode = l_pScene->CreateSceneNode( l_pMesh->GetName(), l_pScene->GetObjectRootNode() );
			GeometrySPtr l_pGeometry = l_pScene->CreateGeometry( l_pMesh->GetName() );
			l_pGeometry->AttachTo( l_pNode.get() );
			l_pGeometry->SetMesh( l_pMesh );
		}

		return l_pScene;
	}

	MeshSPtr LwoImporter::DoImportMesh()
	{
		MeshSPtr l_pReturn;
		UIntArray l_faces;
		RealArray l_sizes;
		String l_nodeName = m_fileName.GetFileName();
		String l_meshName = m_fileName.GetFileName();
		String l_materialName = m_fileName.GetFileName();
		m_pFile = new BinaryFile( m_fileName, File::eOPEN_MODE_READ );
		stLWO_CHUNK l_currentChunk;

		if ( m_pFile->IsOk() )
		{
			Logger::LogDebug( cuT( "**************************************************" ) );
			Logger::LogDebug( cuT( "Importing mesh from file : [" ) + m_fileName + cuT( "]" ) );
			l_pReturn = m_pEngine->CreateMesh( eMESH_TYPE_CUSTOM, l_meshName, l_faces, l_sizes );
			DoRead( &l_currentChunk );
			char l_szName[5] = { 0, 0, 0, 0 , 0 };
			l_currentChunk.m_uiRead += UI4( m_pFile->ReadArray(	l_szName, 4	) );

			if ( l_currentChunk.m_eId == eID_TAG_FORM && l_currentChunk.m_uiSize == m_pFile->GetLength() - 8 && std::string( l_szName ) == "LWO2" )
			{
				DoProcess( &l_currentChunk, l_pReturn );
				l_pReturn->ComputeNormals();
			}

			Logger::LogDebug( cuT( "**************************************************" ) );
		}

		return l_pReturn;
	}

	bool LwoImporter::DoRead( stLWO_CHUNK * p_pChunk )
	{
		bool l_bReturn = m_pFile->IsOk();

		if ( l_bReturn )
		{
			p_pChunk->m_uiRead = 0;
			m_pFile->Read(		p_pChunk->m_eId		);
			SwitchEndianness( p_pChunk->m_eId		);
			l_bReturn = DoIsTagId( p_pChunk->m_eId ) && m_pFile->IsOk();
			char l_szId[5];
			String l_strLog;
			DoToStr( l_szId, p_pChunk->m_eId );

			if ( l_bReturn )
			{
				m_pFile->Read(		p_pChunk->m_uiSize	);
				SwitchEndianness( p_pChunk->m_uiSize 	);
				l_strLog << cuT( "Chunk : " ) << l_szId << cuT( ", " ) << p_pChunk->m_uiSize;
				l_bReturn = m_pFile->IsOk();
			}
			else
			{
				l_strLog << cuT( "Invalid chunk : " ) << l_szId;
				m_pFile->Seek( -4, File::eOFFSET_MODE_CURRENT );
			}

			Logger::LogDebug( l_strLog );
		}

		return l_bReturn;
	}

	void LwoImporter::DoProcess( stLWO_CHUNK * p_pChunk, MeshSPtr p_pMesh )
	{
		stLWO_CHUNK l_currentChunk;
		bool l_bContinue = m_pFile->IsOk();

		while ( p_pChunk->m_uiRead < p_pChunk->m_uiSize && l_bContinue )
		{
			if ( DoRead( &l_currentChunk ) )
			{
				if ( !DoIsValidChunk( &l_currentChunk, p_pChunk ) )
				{
					l_bContinue = false;
				}
				else
				{
					switch ( l_currentChunk.m_eId )
					{
					case eID_TAG_LAYR:
						DoParseLayr( &l_currentChunk );
						break;

					default:
						if ( !m_bIgnored )
						{
							switch ( l_currentChunk.m_eId )
							{
							case eID_TAG_LAYR:
							case eID_TAG_SURF:
								DoParseSurf( &l_currentChunk );
								break;

							case eID_TAG_CLIP:
								DoParseClip( &l_currentChunk );
								break;

							case eID_TAG_PNTS:
								m_pSubmesh = p_pMesh->CreateSubmesh();
								DoParsePnts( &l_currentChunk );
								break;

							case eID_TAG_VMAP:
								DoParseVMap( &l_currentChunk );
								break;

							case eID_TAG_POLS:
								DoParsePols( &l_currentChunk );
								break;

							case eID_TAG_PTAG:
								DoParsePTag( &l_currentChunk );
								break;

							case eID_TAG_TAGS:
								DoParseTags( &l_currentChunk );
								break;

							case eID_TAG_ENVL:
							case eID_TAG_FORM:
							case eID_TAG_VMAD:
							case eID_TAG_VMPA:
							case eID_TAG_BBOX:
							case eID_TAG_DESC:
							case eID_TAG_TEXT:
							case eID_TAG_ICON:
							default:
								DoDiscard( &l_currentChunk );
								break;
							}
						}
						else
						{
							DoDiscard( &l_currentChunk );
						}
					}
				}

				p_pChunk->m_uiRead += l_currentChunk.m_uiRead + sizeof( eID_TAG ) + sizeof( UI4 );
				l_bContinue = m_pFile->IsOk();
			}
			else
			{
				l_bContinue = false;
			}
		}

		if ( !l_bContinue )
		{
			DoDiscard( p_pChunk );
		}

		MaterialSPtr l_pMaterial;

		for ( std::vector< SubmeshPtrStrPair >::iterator l_it = m_arraySubmeshByMatName.begin() ; l_it != m_arraySubmeshByMatName.end() ; ++l_it )
		{
			l_pMaterial = m_pEngine->GetMaterialManager().find( str_utils::from_str( l_it->first ) );

			if ( l_pMaterial )
			{
				m_pEngine->PostEvent( std::make_shared< InitialiseEvent< Material > >( *l_pMaterial ) );
				l_it->second->SetDefaultMaterial( l_pMaterial );
			}
		}
	}

	void LwoImporter::DoDiscard( stLWO_CHUNK * p_pChunk )
	{
		if ( p_pChunk->m_uiSize > p_pChunk->m_uiRead )
		{
			UI4 l_uiSize = p_pChunk->m_uiSize - p_pChunk->m_uiRead;
			std::size_t l_uiLeft = std::size_t( m_pFile->GetLength() - m_pFile->Tell() );

			try
			{
				if ( l_uiSize <= l_uiLeft && l_uiSize > 0 )
				{
					m_pFile->Seek( l_uiSize, Castor::File::eOFFSET_MODE_CURRENT );
				}
				else
				{
					m_pFile->Seek( 0, Castor::File::eOFFSET_MODE_END );
					throw std::range_error( "Bad chunk size" );
				}
			}
			catch ( std::exception & exc )
			{
				Castor::Logger::LogDebug( cuT( "Exception caught when discarding chunk : " ), exc.what() );
			}
			catch ( ... )
			{
				Castor::Logger::LogDebug( cuT( "Exception caught when discarding chunk" ) );
			}

			p_pChunk->m_uiRead = p_pChunk->m_uiSize;
		}
	}

	void LwoImporter::DoDiscard( stLWO_SUBCHUNK * p_pChunk )
	{
		if ( p_pChunk->m_usSize > p_pChunk->m_usRead )
		{
			UI4 l_uiSize = p_pChunk->m_usSize - p_pChunk->m_usRead;
			std::size_t l_uiLeft = std::size_t( m_pFile->GetLength() - m_pFile->Tell() );

			try
			{
				if ( l_uiSize <= l_uiLeft && l_uiSize > 0 )
				{
					m_pFile->Seek( l_uiSize, Castor::File::eOFFSET_MODE_CURRENT );
				}
				else
				{
					m_pFile->Seek( 0, Castor::File::eOFFSET_MODE_END );
					throw std::range_error( "	Bad subchunk size" );
				}
			}
			catch ( std::exception & exc )
			{
				Castor::Logger::LogDebug( cuT( "	Exception caught when discarding subchunk : %s" ), exc.what() );
			}
			catch ( ... )
			{
				Castor::Logger::LogDebug( cuT( "	Exception caught when discarding subchunk" ) );
			}

			p_pChunk->m_usRead = p_pChunk->m_usSize;
		}
	}

	bool LwoImporter::DoIsValidChunk( stLWO_CHUNK * p_pChunk, stLWO_CHUNK * p_pParent )
	{
		bool l_bReturn = DoIsTagId( p_pChunk->m_eId );

		if ( l_bReturn )
		{
			l_bReturn = p_pChunk->m_uiSize + p_pParent->m_uiRead <= p_pParent->m_uiSize;
		}

		return l_bReturn;
	}

	void LwoImporter::DoToStr( char p_szId[5], UI4 p_uiId )
	{
		p_szId[0] = ( ( uint8_t * )&p_uiId )[3];
		p_szId[1] = ( ( uint8_t * )&p_uiId )[2];
		p_szId[2] = ( ( uint8_t * )&p_uiId )[1];
		p_szId[3] = ( ( uint8_t * )&p_uiId )[0];
		p_szId[4] = 0;
	}

	bool LwoImporter::DoRead( std::string & p_strString )
	{
		bool l_bReturn = true;
		static char l_szTmp[1000];
		memset( l_szTmp, 0, 1000 );
		char l_cTmp = 1;
		std::size_t l_iIndex = 0;

		while ( l_cTmp != 0 && m_pFile->IsOk() )
		{
			m_pFile->Read( l_cTmp );
			l_szTmp[l_iIndex++] = l_cTmp;
		}

		if ( !m_pFile->IsOk() )
		{
			m_pFile->Seek( 1 - l_iIndex, File::eOFFSET_MODE_CURRENT );
			l_bReturn = false;
		}
		else
		{
			p_strString = l_szTmp;

			if ( l_bReturn && p_strString.size() % 2 == 0 )
			{
				m_pFile->Seek( 1, File::eOFFSET_MODE_CURRENT );
			}
		}

		return l_bReturn;
	}

	bool LwoImporter::DoRead( String const & p_strTabs, stLWO_SUBCHUNK * p_pSubchunk )
	{
		bool l_bReturn = m_pFile->IsOk();

		if ( l_bReturn )
		{
			p_pSubchunk->m_usRead = 0;
			m_pFile->Read(		p_pSubchunk->m_eId		);
			SwitchEndianness( p_pSubchunk->m_eId		);
			l_bReturn = DoIsTagId( p_pSubchunk->m_eId ) && m_pFile->IsOk();
			char l_szId[5];
			String l_strLog;
			DoToStr( l_szId, p_pSubchunk->m_eId );

			if ( l_bReturn )
			{
				m_pFile->Read(		p_pSubchunk->m_usSize	);
				SwitchEndianness( p_pSubchunk->m_usSize 	);
				l_strLog << p_strTabs << cuT( "Subchunk : " ) << l_szId << cuT( ", " ) << p_pSubchunk->m_usSize;
				l_bReturn = m_pFile->IsOk();
			}
			else
			{
				l_strLog << p_strTabs << cuT( "Invalid subchunk : " ) << l_szId;
				m_pFile->Seek( -4, File::eOFFSET_MODE_CURRENT );
			}

			Logger::LogDebug( l_strLog );
		}

		return l_bReturn;
	}

	UI2 LwoImporter::DoReadVX( UI4 & p_uiIndex )
	{
		UI2 l_ui2Return;
		p_uiIndex = 0;
		uint8_t l_byTest;
		m_pFile->Read( l_byTest );

		if ( l_byTest == 0xFF )
		{
			m_pFile->Read( p_uiIndex );
			SwitchEndianness( p_uiIndex );
			l_ui2Return = sizeof( UI4 );
		}
		else
		{
			m_pFile->Seek( -1, File::eOFFSET_MODE_CURRENT );
			UI2 l_ui2Index = 0;
			m_pFile->Read( l_ui2Index	);
			SwitchEndianness( l_ui2Index );
			p_uiIndex = l_ui2Index;
			l_ui2Return = sizeof( UI2 );
		}

		return l_ui2Return;
	}

	bool LwoImporter::DoIsChunk( eID_TAG p_eId )
	{
		bool l_bReturn = false;

		switch ( p_eId )
		{
		default:
			l_bReturn = false;
			break;

		case eID_TAG_FORM	:
		case eID_TAG_LAYR	:
		case eID_TAG_PNTS	:
		case eID_TAG_VMAP	:
		case eID_TAG_POLS	:
		case eID_TAG_TAGS	:
		case eID_TAG_PTAG	:
		case eID_TAG_VMAD	:
		case eID_TAG_VMPA	:
		case eID_TAG_ENVL	:
		case eID_TAG_CLIP	:
		case eID_TAG_SURF	:
		case eID_TAG_BBOX	:
		case eID_TAG_DESC	:
		case eID_TAG_TEXT	:
		case eID_TAG_ICON	:
			l_bReturn = true;
			break;
		}

		return l_bReturn;
	}

	bool LwoImporter::DoIsTagId( eID_TAG p_eId )
	{
		bool l_bReturn = false;

		switch ( p_eId )
		{
		default:
			l_bReturn = false;
			break;

		case eID_TAG_FORM						:
		case eID_TAG_LAYR						:
		case eID_TAG_PNTS						:
		case eID_TAG_VMAP						:
		case eID_TAG_POLS						:
		case eID_TAG_TAGS						:
		case eID_TAG_PTAG						:
		case eID_TAG_VMAD						:
		case eID_TAG_VMPA						:
		case eID_TAG_ENVL						:

		//		Enveloppe Subchunk IDs
		case 	eID_TAG_ENVL_TYPE				:
		case 	eID_TAG_ENVL_PRE				:
		case 	eID_TAG_ENVL_POST				:
		case 	eID_TAG_ENVL_KEY				:
		case 	eID_TAG_ENVL_SPAN				:
		case 	eID_TAG_ENVL_CHAN				:
		case 	eID_TAG_ENVL_NAME				:
		case eID_TAG_CLIP						:

		//		Clip Subchunk IDs
		case 	eID_TAG_CLIP_STIL				:
		case 	eID_TAG_CLIP_ANIM				:
		case 	eID_TAG_CLIP_XREF				:
		case 	eID_TAG_CLIP_STCC				:
		case 	eID_TAG_CLIP_TIME				:
		case 	eID_TAG_CLIP_CLRS				:
		case 	eID_TAG_CLIP_CLRA				:
		case 	eID_TAG_CLIP_FILT				:
		case 	eID_TAG_CLIP_DITH				:
		case 	eID_TAG_CLIP_CONT				:
		case 	eID_TAG_CLIP_BRIT				:
		case 	eID_TAG_CLIP_SATR				:
		case 	eID_TAG_CLIP_HUE				:
		case 	eID_TAG_CLIP_GAMM				:
		case 	eID_TAG_CLIP_NEGA				:
		case 	eID_TAG_CLIP_IFLT				:
		case 	eID_TAG_CLIP_PFLT				:
		case eID_TAG_SURF						:

		//		Basic surface parameters Subchunk IDs
		case 	eID_TAG_SURF_COLR				:
		case 	eID_TAG_SURF_DIFF				:
		case 	eID_TAG_SURF_LUMI				:
		case 	eID_TAG_SURF_SPEC				:
		case 	eID_TAG_SURF_REFL				:
		case 	eID_TAG_SURF_TRAN				:
		case 	eID_TAG_SURF_TRNL				:
		case 	eID_TAG_SURF_GLOS				:
		case 	eID_TAG_SURF_SHRP				:
		case 	eID_TAG_SURF_BUMP				:
		case 	eID_TAG_SURF_SIDE				:
		case 	eID_TAG_SURF_SMAN				:
		case 	eID_TAG_SURF_RFOP				:
		case 	eID_TAG_SURF_RIMG				:
		case 	eID_TAG_SURF_RSAN				:
		case 	eID_TAG_SURF_RBLR				:
		case 	eID_TAG_SURF_RIND				:
		case 	eID_TAG_SURF_TROP				:
		case 	eID_TAG_SURF_TIMG				:
		case 	eID_TAG_SURF_TBLR				:
		case 	eID_TAG_SURF_CLRH				:
		case 	eID_TAG_SURF_CLRF				:
		case 	eID_TAG_SURF_ADTR				:
		case 	eID_TAG_SURF_GLOW				:
		case 	eID_TAG_SURF_LINE				:
		case 	eID_TAG_SURF_ALPH				:
		case 	eID_TAG_SURF_VCOL				:

		//		Surface block Subchunk IDs
		case 	eID_TAG_SURF_BLOK				:

		//			Block Header IDs
		case 		eID_TAG_BLOK_IMAP			:

		//				Image mapping
		case 			eID_TAG_BLOK_PROJ		:
		case 			eID_TAG_BLOK_AXIS		:
		case 			eID_TAG_BLOK_IMAG		:
		case 			eID_TAG_BLOK_WRAP		:
		case 			eID_TAG_BLOK_WRPW		:
		case 			eID_TAG_BLOK_WRPH		:
		case 			eID_TAG_BLOK_AAST		:
		case 			eID_TAG_BLOK_PIXB		:
		case 			eID_TAG_BLOK_STCK		:
		case 			eID_TAG_BLOK_TAMP		:
		case 		eID_TAG_BLOK_PROC			:

		//				Procedural textures
		case 			eID_TAG_BLOK_VALU		:
		case 			eID_TAG_BLOK_FUNC		:
		case 		eID_TAG_BLOK_GRAD			:

		//				Gradient textures
		case 			eID_TAG_BLOK_PNAM		:
		case 			eID_TAG_BLOK_INAM		:
		case 			eID_TAG_BLOK_GRST		:
		case 			eID_TAG_BLOK_GREN		:
		case 			eID_TAG_BLOK_GRPT		:
		case 			eID_TAG_BLOK_FKEY		:
		case 			eID_TAG_BLOK_IKEY		:
		case 		eID_TAG_BLOK_SHDR			:

		//			Block header subchunks IDs
		case 		eID_TAG_BLOK_ENAB			:
		case 		eID_TAG_BLOK_OPAC			:
		case 		eID_TAG_BLOK_TMAP			:

		//				Texture mapping
		case 			eID_TAG_BLOK_CNTR		:
		case 			eID_TAG_BLOK_SIZE		:
		case 			eID_TAG_BLOK_ROTA		:
		case 			eID_TAG_BLOK_OREF		:
		case 			eID_TAG_BLOK_FALL		:
		case 			eID_TAG_BLOK_CSYS		:
		case eID_TAG_BBOX						:
		case eID_TAG_DESC						:
		case eID_TAG_TEXT						:
		case eID_TAG_ICON						:
			l_bReturn = true;
			break;
		}

		return l_bReturn;
	}

	void LwoImporter::DoParsePTag( stLWO_CHUNK * p_pChunk )
	{
		ePTAG_TYPE l_eType;
		bool l_bContinue = m_pFile->IsOk();

		if ( l_bContinue )
		{
			p_pChunk->m_uiRead += UI4( m_pFile->Read( l_eType ) );
			l_bContinue = m_pFile->IsOk();
		}

		if ( l_bContinue )
		{
			char l_szType[5];
			SwitchEndianness( l_eType );
			DoToStr( l_szType, l_eType );
			String l_strLog( cuT( "\tType : " ) );
			Logger::LogDebug( l_strLog << l_szType );

			if ( l_eType == ePTAG_TYPE_SURF )
			{
				while ( l_bContinue && p_pChunk->m_uiRead < p_pChunk->m_uiSize )
				{
					UI2 l_usTag;
					UI4 l_uiVx;
					p_pChunk->m_uiRead += DoReadVX( l_uiVx );
					l_bContinue = m_pFile->IsOk();

					if ( l_bContinue )
					{
						p_pChunk->m_uiRead += UI4( m_pFile->Read( l_usTag ) );
						l_bContinue = m_pFile->IsOk();
					}

					if ( l_bContinue )
					{
						SwitchEndianness( l_usTag );
						//					Logger::LogDebug( cuT( "\tPOLS Index : %d" ), l_uiVx	);
						//					Logger::LogDebug( cuT( "\tTAGS Index : %d" ), l_usTag	);

						if ( l_usTag < m_arrayTags.size() )
						{
							String l_strLog( cuT( "\tTAG Name : " ) );
							Logger::LogDebug( l_strLog << m_arrayTags[l_usTag].c_str() );
							m_arraySubmeshByMatName.push_back( std::make_pair( m_arrayTags[l_usTag], m_pSubmesh ) );
						}
					}
				}
			}
			else
			{
				l_bContinue = false;
			}
		}

		if ( !l_bContinue )
		{
			DoDiscard( p_pChunk );
		}
	}

	UI2 LwoImporter::DoReadBlockHeader( stLWO_SUBCHUNK * p_pSubchunk, eTEX_CHANNEL & p_eChannel )
	{
		std::string	l_strOrdinal;
		stLWO_SUBCHUNK	l_currentSubchunk;
		UI2			l_usReturn 			= 0;
		bool l_bContinue = DoRead( cuT( "		" ), p_pSubchunk );

		if ( l_bContinue )
		{
			l_usReturn += sizeof( eID_TAG ) + sizeof( UI2 );
			l_bContinue	= DoRead( l_strOrdinal );
		}

		if ( l_bContinue )
		{
			p_pSubchunk->m_usRead += UI2( l_strOrdinal.size() + 1 + ( 1 - l_strOrdinal.size() % 2 ) );
			Logger::LogDebug( cuT( "			Header ordinal : 0x%x" ) + l_strOrdinal[0] );
		}

		while ( l_bContinue && p_pSubchunk->m_usRead < p_pSubchunk->m_usSize )
		{
			if ( DoRead( cuT( "			" ), &l_currentSubchunk ) )
			{
				switch ( l_currentSubchunk.m_eId )
				{
				default:
					DoDiscard( &l_currentSubchunk );
					break;

				case eID_TAG_BLOK_CHAN:
					l_currentSubchunk.m_usRead += UI2( m_pFile->Read( p_eChannel ) );
					SwitchEndianness( p_eChannel );
					Logger::LogDebug( cuT( "				Channel : %d" ), p_eChannel );
					break;
				}

				p_pSubchunk->m_usRead += l_currentSubchunk.m_usRead + sizeof( eID_TAG ) + sizeof( UI2 );
			}
			else
			{
				l_bContinue = false;
			}
		}

		if ( !l_bContinue )
		{
			DoDiscard( p_pSubchunk );
		}

		l_usReturn += p_pSubchunk->m_usRead;
		return l_usReturn;
	}

	void LwoImporter::DoReadTMap( stLWO_SUBCHUNK * p_pSubchunk )
	{
		stLWO_SUBCHUNK	l_currentSubchunk;
		bool 		l_bContinue			= true;

		while ( l_bContinue && p_pSubchunk->m_usRead < p_pSubchunk->m_usSize )
		{
			if ( DoRead( cuT( "			" ), &l_currentSubchunk ) )
			{
				// 			switch( l_currentSubchunk.m_eId )
				// 			{
				// 			default:
				DoDiscard( &l_currentSubchunk );
				// 				break;
				// 			}
				p_pSubchunk->m_usRead += l_currentSubchunk.m_usRead + sizeof( eID_TAG ) + sizeof( UI2 );
			}
			else
			{
				l_bContinue = false;
			}
		}

		if ( !l_bContinue )
		{
			DoDiscard( p_pSubchunk );
		}
	}

	void LwoImporter::DoReadIMap( stLWO_SUBCHUNK * p_pSubchunk, eTEX_CHANNEL & p_eChannel )
	{
		stLWO_SUBCHUNK	l_currentSubchunk;
		bool 		l_bContinue			= true;

		while ( l_bContinue && p_pSubchunk->m_usRead < p_pSubchunk->m_usSize )
		{
			if ( DoRead( cuT( "			" ), &l_currentSubchunk ) )
			{
				switch ( l_currentSubchunk.m_eId )
				{
				default:
					DoDiscard( &l_currentSubchunk );
					break;

				case eID_TAG_BLOK_CHAN:
					l_currentSubchunk.m_usRead += UI2( m_pFile->Read( p_eChannel ) );
					SwitchEndianness( p_eChannel );
					Logger::LogDebug( cuT( "				Channel : %d" ), p_eChannel );
					break;
				}

				p_pSubchunk->m_usRead += l_currentSubchunk.m_usRead + sizeof( eID_TAG ) + sizeof( UI2 );
			}
			else
			{
				l_bContinue = false;
			}
		}

		if ( !l_bContinue )
		{
			DoDiscard( p_pSubchunk );
		}
	}

	void LwoImporter::DoReadProc( stLWO_SUBCHUNK * p_pSubchunk )
	{
		stLWO_SUBCHUNK	l_currentSubchunk;
		bool 		l_bContinue			= true;

		while ( l_bContinue && p_pSubchunk->m_usRead < p_pSubchunk->m_usSize )
		{
			if ( DoRead( cuT( "			" ), &l_currentSubchunk ) )
			{
				// 			switch( l_currentSubchunk.m_eId )
				// 			{
				// 			default:
				DoDiscard( &l_currentSubchunk );
				// 				break;
				// 			}
				p_pSubchunk->m_usRead += l_currentSubchunk.m_usRead + sizeof( eID_TAG ) + sizeof( UI2 );
			}
			else
			{
				l_bContinue = false;
			}
		}

		if ( !l_bContinue )
		{
			DoDiscard( p_pSubchunk );
		}
	}

	void LwoImporter::DoReadGrad( stLWO_SUBCHUNK * p_pSubchunk )
	{
		stLWO_SUBCHUNK	l_currentSubchunk;
		bool 		l_bContinue			= true;

		while ( l_bContinue && p_pSubchunk->m_usRead < p_pSubchunk->m_usSize )
		{
			if ( DoRead( cuT( "			" ), &l_currentSubchunk ) )
			{
				// 			switch( l_currentSubchunk.m_eId )
				// 			{
				// 			default:
				DoDiscard( &l_currentSubchunk );
				// 				break;
				// 			}
				p_pSubchunk->m_usRead += l_currentSubchunk.m_usRead + sizeof( eID_TAG ) + sizeof( UI2 );
			}
			else
			{
				l_bContinue = false;
			}
		}

		if ( !l_bContinue )
		{
			DoDiscard( p_pSubchunk );
		}
	}

	void LwoImporter::DoReadShdr( stLWO_SUBCHUNK * p_pSubchunk )
	{
		stLWO_SUBCHUNK	l_currentSubchunk;
		bool 		l_bContinue			= true;

		while ( l_bContinue && p_pSubchunk->m_usRead < p_pSubchunk->m_usSize )
		{
			if ( DoRead( cuT( "			" ), &l_currentSubchunk ) )
			{
				// 			switch( l_currentSubchunk.m_eId )
				// 			{
				// 			default:
				DoDiscard( &l_currentSubchunk );
				// 				break;
				// 			}
				p_pSubchunk->m_usRead += l_currentSubchunk.m_usRead + sizeof( eID_TAG ) + sizeof( UI2 );
			}
			else
			{
				l_bContinue = false;
			}
		}

		if ( !l_bContinue )
		{
			DoDiscard( p_pSubchunk );
		}
	}

	void LwoImporter::DoSetChannel( TextureUnitSPtr p_pTexture, eTEX_CHANNEL p_eChannel )
	{
		if ( p_pTexture )
		{
			switch ( p_eChannel )
			{
			case eTEX_CHANNEL_COLR:
				p_pTexture->SetChannel( eTEXTURE_CHANNEL_DIFFUSE	);
				break;

			case eTEX_CHANNEL_DIFF:
				p_pTexture->SetChannel( eTEXTURE_CHANNEL_DIFFUSE	);
				break;

			case eTEX_CHANNEL_SPEC:
				p_pTexture->SetChannel( eTEXTURE_CHANNEL_SPECULAR	);
				break;

			case eTEX_CHANNEL_GLOS:
				p_pTexture->SetChannel( eTEXTURE_CHANNEL_GLOSS		);
				break;

			case eTEX_CHANNEL_TRAN:
				p_pTexture->SetChannel( eTEXTURE_CHANNEL_OPACITY	);
				break;

			case eTEX_CHANNEL_BUMP:
				p_pTexture->SetChannel( eTEXTURE_CHANNEL_NORMAL		);
				break;
			}
		}
	}

	void LwoImporter::DoReadBlock( stLWO_SUBCHUNK * p_pSubchunk, PassSPtr p_pPass )
	{
		stLWO_SUBCHUNK				l_currentSubchunk;
		stLWO_SUBCHUNK				l_blockHeader;
		UI4						l_uiVx;
		eTEX_CHANNEL			l_eChannel			= eTEX_CHANNEL_COLR;
		bool 					l_bContinue			= true;
		TextureUnitSPtr			l_pTexture;
		ImageVxMap::iterator	l_it;
		p_pSubchunk->m_usRead += DoReadBlockHeader( &l_blockHeader, l_eChannel );

		while ( l_bContinue && p_pSubchunk->m_usRead < p_pSubchunk->m_usSize )
		{
			if ( DoRead( cuT( "		" ), &l_currentSubchunk ) )
			{
				switch ( l_currentSubchunk.m_eId )
				{
				case eID_TAG_BLOK_TMAP:
					DoReadTMap( &l_currentSubchunk );
					break;

				case eID_TAG_BLOK_IMAP:
					DoReadIMap( &l_currentSubchunk, l_eChannel );
					break;

				case eID_TAG_BLOK_PROC:
					DoReadProc( &l_currentSubchunk );
					break;

				case eID_TAG_BLOK_GRAD:
					DoReadGrad( &l_currentSubchunk );
					break;

				case eID_TAG_BLOK_SHDR:
					DoReadShdr( &l_currentSubchunk );
					break;

				case eID_TAG_BLOK_IMAG:
					l_currentSubchunk.m_usRead += DoReadVX( l_uiVx );
					l_it = m_mapImages.find( l_uiVx );

					if ( l_it != m_mapImages.end() )
					{
						String l_strLog( cuT( "			Texture found : " ) );
						Logger::LogDebug( l_strLog << l_it->second->GetPath().c_str() );
						l_pTexture = p_pPass->AddTextureUnit();
						StaticTextureSPtr l_pStaTexture = m_pEngine->GetRenderSystem()->CreateStaticTexture();
						l_pStaTexture->SetDimension( eTEXTURE_DIMENSION_2D );
						l_pStaTexture->SetImage( l_it->second->GetPixels() );
						l_pTexture->SetTexture( l_pStaTexture );
						DoSetChannel( l_pTexture, l_eChannel );
					}
					else
					{
						l_pTexture.reset();
					}

					break;

				case eID_TAG_BLOK_CHAN:
					l_currentSubchunk.m_usRead += UI2( m_pFile->Read( l_eChannel ) );
					SwitchEndianness( l_eChannel );
					Logger::LogDebug( cuT( "			Channel : %d" ), l_eChannel );
					DoSetChannel( l_pTexture, l_eChannel );
					break;

				case eID_TAG_BLOK_PROJ:
				case eID_TAG_BLOK_AXIS:
				case eID_TAG_BLOK_WRAP:
				case eID_TAG_BLOK_WRPW:
				case eID_TAG_BLOK_WRPH:
				case eID_TAG_BLOK_AAST:
				case eID_TAG_BLOK_PIXB:
				case eID_TAG_BLOK_STCK:
				case eID_TAG_BLOK_TAMP:
				case eID_TAG_BLOK_VALU:
				case eID_TAG_BLOK_FUNC:
				case eID_TAG_BLOK_PNAM:
				case eID_TAG_BLOK_INAM:
				case eID_TAG_BLOK_GRST:
				case eID_TAG_BLOK_GREN:
				case eID_TAG_BLOK_GRPT:
				case eID_TAG_BLOK_FKEY:
				case eID_TAG_BLOK_IKEY:
				case eID_TAG_BLOK_ENAB:
				case eID_TAG_BLOK_OPAC:
				case eID_TAG_BLOK_CNTR:
				case eID_TAG_BLOK_SIZE:
				case eID_TAG_BLOK_ROTA:
				case eID_TAG_BLOK_OREF:
				case eID_TAG_BLOK_FALL:
				case eID_TAG_BLOK_CSYS:
				default:
					DoDiscard( &l_currentSubchunk );
					break;
				}

				p_pSubchunk->m_usRead += l_currentSubchunk.m_usRead + sizeof( eID_TAG ) + sizeof( UI2 );
			}
			else
			{
				l_bContinue = false;
			}
		}

		if ( !l_bContinue )
		{
			DoDiscard( p_pSubchunk );
		}
	}

	void LwoImporter::DoParseTags( stLWO_CHUNK * p_pChunk )
	{
		std::string	l_strTag;
		bool		l_bContinue	= m_pFile->IsOk();

		while ( l_bContinue && p_pChunk->m_uiRead < p_pChunk->m_uiSize )
		{
			l_bContinue = DoRead( l_strTag );

			if ( l_bContinue )
			{
				p_pChunk->m_uiRead += UI4( l_strTag.size() + 1 + ( 1 - l_strTag.size() % 2 ) );
				m_arrayTags.push_back( l_strTag );
				String l_strLog( cuT( "\tName : " ) );
				Logger::LogDebug( l_strLog << l_strTag.c_str() );
			}
		}

		if ( !l_bContinue )
		{
			DoDiscard( p_pChunk );
		}
	}

	void LwoImporter::DoParseSurf( stLWO_CHUNK * p_pChunk )
	{
		stLWO_SUBCHUNK 		l_currentSubchunk;
		stLWO_SUBCHUNK 		l_blockHeader;
		Colour 			l_clrBase;
		MaterialSPtr	l_pMaterial;
		PassSPtr		l_pPass;
		UI4 			l_uiVx;
		float 			l_fDiff		= 1;
		float			l_fSpec		= 0;
		float 			l_fGlos 	= 0;
		UI2 			l_usSide 	= 1;
		bool 			l_bContinue	= DoRead( m_strName );

		if ( l_bContinue )
		{
			p_pChunk->m_uiRead += UI4( m_strName.size() + 1 + ( 1 - m_strName.size() % 2 ) );
			l_bContinue = DoRead( m_strSource );
		}

		if ( l_bContinue )
		{
			p_pChunk->m_uiRead += UI4( m_strSource.size() + 1 + ( 1 - m_strSource.size() % 2 ) );
			Logger::LogDebug( cuT( "	Name : "	) + str_utils::from_str( m_strName 	) );
			Logger::LogDebug( cuT( "	Source : " 	) + str_utils::from_str( m_strSource 	) );
			l_pMaterial = std::make_shared< Material >( GetEngine(), str_utils::from_str( m_strName ) );
			l_pPass = l_pMaterial->GetPass( 0 );
		}

		while ( l_bContinue && p_pChunk->m_uiRead < p_pChunk->m_uiSize )
		{
			if ( DoRead( cuT( "	" ), &l_currentSubchunk ) )
			{
				switch ( l_currentSubchunk.m_eId )
				{
				case eID_TAG_SURF_COLR:
					l_currentSubchunk.m_usRead += UI2( m_pFile->ReadArray( l_clrBase.ptr(), 3 ) );
					SwitchEndianness( l_clrBase.ptr()[0] );
					SwitchEndianness( l_clrBase.ptr()[1] );
					SwitchEndianness( l_clrBase.ptr()[2] );
					l_clrBase.alpha() = 1.0f;
					l_currentSubchunk.m_usRead += DoReadVX( l_uiVx );
					Logger::LogDebug( cuT( "		Colour : %f, %f, %f - VX : 0x%x" ), l_clrBase.red().value(), l_clrBase.green().value(), l_clrBase.blue().value(), l_uiVx );
					break;

				case eID_TAG_SURF_DIFF:
					l_currentSubchunk.m_usRead += UI2( m_pFile->Read( l_fDiff ) );
					SwitchEndianness( l_fDiff );
					l_currentSubchunk.m_usRead += DoReadVX( l_uiVx );
					Logger::LogDebug( cuT( "		Diff. base level : %f - VX : 0x%x" ), l_fDiff, l_uiVx );
					break;

				case eID_TAG_SURF_SPEC:
					l_currentSubchunk.m_usRead += UI2( m_pFile->Read( l_fSpec ) );
					SwitchEndianness( l_fSpec );
					l_currentSubchunk.m_usRead += DoReadVX( l_uiVx );
					Logger::LogDebug( cuT( "		Spec. base level : %f - VX : 0x%x" ), l_fSpec, l_uiVx );
					break;

				case eID_TAG_SURF_GLOS:
					l_currentSubchunk.m_usRead += UI2( m_pFile->Read( l_fGlos ) );
					SwitchEndianness( l_fGlos );
					l_currentSubchunk.m_usRead += DoReadVX( l_uiVx );
					Logger::LogDebug( cuT( "		Glossiness : %f - VX : 0x%x" ), l_fGlos, l_uiVx );
					break;

				case eID_TAG_SURF_SIDE:
					l_currentSubchunk.m_usRead += UI2( m_pFile->Read( l_usSide ) );
					SwitchEndianness( l_usSide );
					Logger::LogDebug( cuT( "		Sidedness : %f" ), l_usSide );
					break;

				case eID_TAG_SURF_BLOK:
					DoReadBlock( &l_currentSubchunk, l_pPass );
					break;

				case eID_TAG_SURF_LUMI:
				case eID_TAG_SURF_REFL:
				case eID_TAG_SURF_TRAN:
				case eID_TAG_SURF_TRNL:
				case eID_TAG_SURF_BUMP:
				default:
					DoDiscard( &l_currentSubchunk );
					break;
				}

				p_pChunk->m_uiRead += l_currentSubchunk.m_usRead + sizeof( eID_TAG ) + sizeof( UI2 );
			}
			else
			{
				l_bContinue = false;
			}
		}

		if ( l_bContinue )
		{
			l_pPass->SetDiffuse(	l_clrBase * l_fDiff 					);
			l_pPass->SetSpecular(	l_clrBase * l_fSpec 					);
			l_pPass->SetShininess(	float( 2 ^ int( ( 10 * l_fGlos ) + 2 ) )	);
			l_pPass->SetTwoSided(	l_usSide == 3 							);
			GetEngine()->GetMaterialManager().insert( str_utils::from_str( m_strName ), l_pMaterial );
			GetEngine()->PostEvent( std::make_shared< InitialiseEvent< Material > >( *l_pMaterial ) );
		}
		else
		{
			DoDiscard( p_pChunk );
		}
	}

	void LwoImporter::DoParseClip( stLWO_CHUNK * p_pChunk )
	{
		UI4	l_uiIndex;
		stLWO_SUBCHUNK l_currentSubchunk;
		bool l_bContinue = m_pFile->Read( l_uiIndex ) == sizeof( UI4 );
		std::string l_strName;
		Path l_pathImage;
		ImageSPtr l_pImage;

		if ( l_bContinue )
		{
			SwitchEndianness( l_uiIndex );
			p_pChunk->m_uiRead += sizeof( UI4 );
			Logger::LogDebug( cuT( "	Index : 0x%x" ), l_uiIndex );
		}

		while ( l_bContinue && p_pChunk->m_uiRead < p_pChunk->m_uiSize )
		{
			if ( DoRead( cuT( "	" ), &l_currentSubchunk ) )
			{
				switch ( l_currentSubchunk.m_eId )
				{
				case eID_TAG_CLIP_STIL:
					DoRead( l_strName );
					l_currentSubchunk.m_usRead += UI2( l_strName.size() + 1 + ( 1 - l_strName.size() % 2 ) );
					Logger::LogDebug( cuT( "		Image : " ) + str_utils::from_str( l_strName ) );
					l_pathImage = str_utils::from_str( l_strName );
					l_pImage = GetEngine()->GetImageManager().find( l_pathImage );

					if ( !l_pImage )
					{
						if ( File::FileExists( l_pathImage ) )
						{
							l_pImage = std::make_shared< Image >( l_pathImage, l_pathImage );
						}
						else if ( File::FileExists( m_pFile->GetFilePath() / l_pathImage ) )
						{
							l_pImage = std::make_shared< Image >( l_pathImage, m_pFile->GetFilePath() / l_pathImage );
						}
						else if ( File::FileExists( m_pFile->GetFilePath() / cuT( "Texture" ) / l_pathImage ) )
						{
							l_pImage = std::make_shared< Image >( l_pathImage, m_pFile->GetFilePath() / cuT( "Texture" ) / l_pathImage );
						}

						if ( l_pImage )
						{
							GetEngine()->GetImageManager().insert( l_pathImage, l_pImage );
						}
					}

					m_mapImages.insert( std::make_pair( l_uiIndex, l_pImage ) );
					break;

				case eID_TAG_CLIP_ANIM:
				case eID_TAG_CLIP_XREF:
				case eID_TAG_CLIP_STCC:
				case eID_TAG_CLIP_TIME:
				case eID_TAG_CLIP_CLRS:
				case eID_TAG_CLIP_CLRA:
				case eID_TAG_CLIP_FILT:
				case eID_TAG_CLIP_DITH:
				case eID_TAG_CLIP_CONT:
				case eID_TAG_CLIP_BRIT:
				case eID_TAG_CLIP_SATR:
				case eID_TAG_CLIP_HUE:
				case eID_TAG_CLIP_GAMM:
				case eID_TAG_CLIP_NEGA:
				case eID_TAG_CLIP_IFLT:
				case eID_TAG_CLIP_PFLT:
				default:
					DoDiscard( &l_currentSubchunk );
					break;
				}

				p_pChunk->m_uiRead += l_currentSubchunk.m_usRead + sizeof( eID_TAG ) + sizeof( UI2 );
			}
			else
			{
				l_bContinue = false;
			}
		}

		if ( !l_bContinue )
		{
			DoDiscard( p_pChunk );
		}
	}

	void LwoImporter::DoParsePnts( stLWO_CHUNK * p_pChunk )
	{
		if ( p_pChunk->m_uiSize > 0 && p_pChunk->m_uiSize % 3 == 0 )
		{
			F4 l_fCoords[3];
			UI4 l_uiCount = p_pChunk->m_uiSize / ( sizeof( F4 ) * 3 );
			Logger::LogDebug( cuT( "\tCount : %d" ), l_uiCount );
			m_arrayPoints.resize( l_uiCount );
			m_arrayUvs.resize( l_uiCount );

			for ( UI4 i = 0 ; i < l_uiCount ; i++ )
			{
				p_pChunk->m_uiRead += UI4( m_pFile->ReadArray( l_fCoords, 3 ) );
				SwitchEndianness( l_fCoords[0] );
				SwitchEndianness( l_fCoords[1] );
				SwitchEndianness( l_fCoords[2] );
				m_pSubmesh->AddPoint( l_fCoords );
			}
		}
	}

	void LwoImporter::DoParseVMap( stLWO_CHUNK * p_pChunk )
	{
		eVMAP_TYPE 	l_eType = eVMAP_TYPE( 0 );
		UI2			l_usDimension;
		std::string	l_strName;
		bool l_bContinue = m_pFile->IsOk();

		if ( l_bContinue )
		{
			p_pChunk->m_uiRead += UI4( m_pFile->Read( l_eType ) );
			l_bContinue = m_pFile->IsOk();
		}

		if ( l_bContinue )
		{
			p_pChunk->m_uiRead += UI4( m_pFile->Read( l_usDimension ) );
			l_bContinue = m_pFile->IsOk();
		}

		if ( l_bContinue )
		{
			l_bContinue = DoRead( l_strName );
		}

		if ( l_bContinue )
		{
			p_pChunk->m_uiRead += UI4( l_strName.size() + 1 + ( 1 - l_strName.size() % 2 ) );
			SwitchEndianness( l_eType		);
			SwitchEndianness( l_usDimension	);
		}

		if ( l_bContinue )
		{
			char l_szType[5];
			DoToStr( l_szType, l_eType );
			String l_strLog1( cuT( "\tType : "	) );
			String l_strLog2( cuT( "\tName : "	) );
			Logger::LogDebug( l_strLog1 <<	l_szType			);
			Logger::LogDebug(	cuT( "\tDimensions : %d"		),	l_usDimension	);
			Logger::LogDebug( l_strLog2 <<	l_strName.c_str()	);

			if ( l_eType == eVMAP_TYPE_TXUV )
			{
				F4	l_fUv[2];
				UI4 i = 0;

				while ( l_bContinue && p_pChunk->m_uiRead < p_pChunk->m_uiSize )
				{
					UI4 l_uiVx;
					p_pChunk->m_uiRead += DoReadVX( l_uiVx );
					l_bContinue = m_pFile->IsOk();

					if ( l_bContinue )
					{
						p_pChunk->m_uiRead += UI4( m_pFile->ReadArray( l_fUv, 2 ) );
						l_bContinue = m_pFile->IsOk();
					}

					if ( l_bContinue )
					{
						SwitchEndianness( l_fUv[0] );
						SwitchEndianness( l_fUv[1] );
						Vertex::SetTexCoord( m_pSubmesh->GetPoint( l_uiVx ), l_fUv[0], l_fUv[1] );
						i++;
					}

					//				Logger::LogDebug( cuT( "\tVertex : %d - %f, %f"	), l_uiVx, l_fUv[0], l_fUv[1] );
				}

				m_bHasUv = true;
				Logger::LogDebug( cuT( "\tUV count : %d"	),	i );
			}
			else
			{
				l_bContinue = false;
			}
		}

		if ( !l_bContinue )
		{
			DoDiscard( p_pChunk );
		}
	}

	void LwoImporter::DoParsePols( stLWO_CHUNK * p_pChunk )
	{
		std::vector< Lwo::Face > 	l_arrayFaces;
		std::set< UI4 >				l_setPntsIds;
		ePOLS_TYPE 	l_eType;
		bool		l_bContinue		= m_pFile->IsOk();
		UI4			l_uiCount 		= 0;

		if ( l_bContinue )
		{
			p_pChunk->m_uiRead += UI4( m_pFile->Read( l_eType ) );
			l_bContinue = m_pFile->IsOk();
		}

		if ( l_bContinue )
		{
			char l_szType[5];
			SwitchEndianness( l_eType );
			DoToStr( l_szType, l_eType );
			String l_strLog( cuT( "\tType : " ) );
			Logger::LogDebug( l_strLog << l_szType );

			if ( l_eType == ePOLS_TYPE_FACE )
			{
				while ( l_bContinue && p_pChunk->m_uiRead < p_pChunk->m_uiSize )
				{
					UI2 l_usCountFlags = 0;
					p_pChunk->m_uiRead += UI4( m_pFile->Read( l_usCountFlags ) );
					UI2 l_usCount = l_usCountFlags & 0xFFC0;
					SwitchEndianness( l_usCount );
					Lwo::Face l_face( l_usCount );
					l_bContinue = m_pFile->IsOk();

					for ( UI2 i = 0 ; i < l_usCount && l_bContinue ; i++ )
					{
						p_pChunk->m_uiRead += DoReadVX( l_face[i] );
						l_setPntsIds.insert( l_face[i] );
						l_bContinue = m_pFile->IsOk();
					}

					if ( l_bContinue )
					{
						l_arrayFaces.push_back( l_face );
						l_uiCount++;
					}
				}

				Logger::LogDebug( cuT( "\tFaces count : %d"	),	l_uiCount		);
			}
			else
			{
				l_bContinue = false;
			}
		}

		if ( l_bContinue )
		{
			if ( m_pSubmesh )
			{
				l_uiCount = 0;
				std::map< UI4, UI4 >	l_mapPntsIds;
				FaceSPtr				l_pFace;

				for ( std::set< UI4 >::iterator l_it = l_setPntsIds.begin() ; l_it != l_setPntsIds.end() ; ++l_it, ++l_uiCount )
				{
					m_pSubmesh->AddPoint( m_arrayPoints[*l_it] );
					l_mapPntsIds.insert( std::make_pair( *l_it, l_uiCount ) );
				}

				for ( std::vector< Lwo::Face >::iterator l_it = l_arrayFaces.begin() ; l_it != l_arrayFaces.end() ; ++l_it )
				{
					l_pFace = m_pSubmesh->AddFace( l_mapPntsIds[l_it->at( 1 )], l_mapPntsIds[l_it->at( 0 )], l_mapPntsIds[l_it->at( 2 )] );
				}
			}
		}
		else
		{
			DoDiscard( p_pChunk );
		}
	}

	void LwoImporter::DoParseLayr( stLWO_CHUNK * p_pChunk )
	{
		bool		l_bContinue = m_pFile->IsOk();
		UI2			l_usNumber;
		UI2			l_usFlags;
		Point3f		l_ptPivot;
		std::string	l_strName;
		UI2			l_usParent;

		if ( l_bContinue )
		{
			p_pChunk->m_uiRead += UI4( m_pFile->Read( l_usNumber ) );
			l_bContinue = m_pFile->IsOk();
		}

		if ( l_bContinue && p_pChunk->m_uiRead < p_pChunk->m_uiSize )
		{
			p_pChunk->m_uiRead += UI4( m_pFile->Read( l_usFlags ) );
			l_bContinue = m_pFile->IsOk();
		}

		if ( l_bContinue && p_pChunk->m_uiRead < p_pChunk->m_uiSize )
		{
			p_pChunk->m_uiRead += UI4( m_pFile->ReadArray( l_ptPivot.ptr(), 3 ) );
			l_bContinue = m_pFile->IsOk();
		}

		if ( l_bContinue && p_pChunk->m_uiRead < p_pChunk->m_uiSize )
		{
			DoRead( l_strName );
			l_bContinue = m_pFile->IsOk();
		}

		if ( l_bContinue )
		{
			p_pChunk->m_uiRead += UI4( l_strName.size() + 1 + ( 1 - l_strName.size() % 2 ) );

			if ( p_pChunk->m_uiRead < p_pChunk->m_uiSize )
			{
				p_pChunk->m_uiRead += UI4( m_pFile->Read( l_usParent ) );
				l_bContinue = m_pFile->IsOk();
			}
		}

		if ( l_bContinue )
		{
			SwitchEndianness( l_usNumber	);
			SwitchEndianness( l_usFlags		);
			SwitchEndianness( l_ptPivot[0]	);
			SwitchEndianness( l_ptPivot[1]	);
			SwitchEndianness( l_ptPivot[2]	);
			SwitchEndianness( l_usParent	);
			String l_strLog( cuT( "\tName   : " )	);
			Logger::LogDebug( l_strLog						<<	l_strName.c_str()							);
			Logger::LogDebug( cuT( "\tNumber : %d"			),	l_usNumber									);
			Logger::LogDebug( cuT( "\tFlags  : 0x%x"		),	l_usFlags									);
			Logger::LogDebug( cuT( "\tPivot  : %f, %f, %f"	),	l_ptPivot[0], l_ptPivot[1], l_ptPivot[2]	);
			Logger::LogDebug( cuT( "\tParent : %d"			),	l_usParent									);
			String l_strNameTmp = str_utils::from_str( l_strName );
			str_utils::to_lower_case( l_strNameTmp );

			if ( l_usFlags & 0x01 || l_strNameTmp.find( cuT( "bip" ) ) == 0 || l_strNameTmp.find( cuT( "fcfx" ) ) == 0 || l_strNameTmp.find( cuT( "gundummy" ) ) == 0 || l_strNameTmp.find( cuT( "flap_bone" ) ) == 0 )
			{
				m_bIgnored = true;
			}
			else
			{
				m_bIgnored = false;
			}
		}

		if ( !l_bContinue )
		{
			DoDiscard( p_pChunk );
		}
	}
}
