#include "LwoImporter.hpp"
#include "LwoChunk.hpp"
#include "LwoSubChunk.hpp"

#include <Graphics/Colour.hpp>
#include <Graphics/Image.hpp>

#include <Engine.hpp>
#include <Material/Material.hpp>
#include <Material/LegacyPass.hpp>
#include <Mesh/Mesh.hpp>
#include <Scene/Geometry.hpp>
#include <Scene/Scene.hpp>
#include <Scene/SceneNode.hpp>

#include <Event/Frame/FrameListener.hpp>
#include <Event/Frame/InitialiseEvent.hpp>
#include <Cache/CacheView.hpp>
#include <Material/Pass.hpp>
#include <Mesh/Face.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/Vertex.hpp>
#include <Mesh/Buffer/Buffer.hpp>
#include <Miscellaneous/Version.hpp>
#include <Plugin/Plugin.hpp>
#include <Render/RenderSystem.hpp>
#include <Texture/TextureLayout.hpp>
#include <Texture/TextureUnit.hpp>

using namespace Castor3D;
using namespace Castor;

namespace Lwo
{
	LwoImporter::LwoImporter( Castor3D::Engine & p_engine )
		: Importer( p_engine )
		, m_file( NULL )
		, m_bIgnored( false )
		, m_bHasUv( false )
	{
	}

	LwoImporter::~LwoImporter()
	{
	}

	ImporterUPtr LwoImporter::Create( Engine & p_engine )
	{
		return std::make_unique< LwoImporter >( p_engine );
	}

	bool LwoImporter::DoImportScene( Scene & p_scene )
	{
		auto mesh = p_scene.GetMeshCache().Add( cuT( "Mesh_LWO" ) );
		bool result = DoImportMesh( *mesh );

		if ( result )
		{
			SceneNodeSPtr node = p_scene.GetSceneNodeCache().Add( mesh->GetName(), p_scene.GetObjectRootNode() );
			GeometrySPtr geometry = p_scene.GetGeometryCache().Add( mesh->GetName(), node, nullptr );
			geometry->SetMesh( mesh );
			m_geometries.insert( { geometry->GetName(), geometry } );
		}

		return result;
	}

	bool LwoImporter::DoImportMesh( Mesh & p_mesh )
	{
		UIntArray faces;
		RealArray sizes;
		String nodeName = m_fileName.GetFileName();
		String meshName = m_fileName.GetFileName();
		String materialName = m_fileName.GetFileName();
		m_file = new BinaryFile( m_fileName, File::OpenMode::eRead );
		stLWO_CHUNK currentChunk;
		bool result{ false };

		if ( m_file->IsOk() )
		{
			Logger::LogDebug( cuT( "**************************************************" ) );
			Logger::LogDebug( cuT( "Importing mesh from file : [" ) + m_fileName + cuT( "]" ) );
			DoRead( currentChunk );
			char name[5] = { 0, 0, 0, 0 , 0 };
			currentChunk.m_read += UI4( m_file->ReadArray( name, 4 ) );

			if ( currentChunk.m_id == eID_TAG_FORM && currentChunk.m_size == m_file->GetLength() - 8 && std::string( name ) == "LWO2" )
			{
				DoProcess( p_mesh, currentChunk );
				p_mesh.ComputeNormals();
			}

			Logger::LogDebug( cuT( "**************************************************" ) );
			result = true;
		}

		return result;
	}

	bool LwoImporter::DoRead( stLWO_CHUNK & p_chunk )
	{
		bool result = m_file->IsOk();

		if ( result )
		{
			p_chunk.m_read = 0;
			m_file->Read(	p_chunk.m_id );
			BigEndianToSystemEndian( p_chunk.m_id );
			result = DoIsTagId( p_chunk.m_id ) && m_file->IsOk();
			char id[5];
			StringStream toLog;
			DoToStr( id, p_chunk.m_id );

			if ( result )
			{
				m_file->Read(	p_chunk.m_size );
				BigEndianToSystemEndian( p_chunk.m_size );
				toLog << cuT( "Chunk : " ) << id << cuT( ", " ) << p_chunk.m_size;
				result = m_file->IsOk();
			}
			else
			{
				toLog << cuT( "Invalid chunk : " ) << id;
				m_file->Seek( -4, File::OffsetMode::eCurrent );
			}

			Logger::LogDebug( toLog );
		}

		return result;
	}

	void LwoImporter::DoProcess( Mesh & p_mesh, stLWO_CHUNK & p_chunk )
	{
		stLWO_CHUNK currentChunk;
		bool ok = m_file->IsOk();

		while ( p_chunk.m_read < p_chunk.m_size && ok )
		{
			if ( DoRead( currentChunk ) )
			{
				if ( !DoIsValidChunk( currentChunk, p_chunk ) )
				{
					ok = false;
				}
				else
				{
					switch ( currentChunk.m_id )
					{
					case eID_TAG_LAYR:
						DoParseLayr( currentChunk );
						break;

					default:
						if ( !m_bIgnored )
						{
							switch ( currentChunk.m_id )
							{
							case eID_TAG_LAYR:
							case eID_TAG_SURF:
								DoParseSurf( currentChunk );
								break;

							case eID_TAG_CLIP:
								DoParseClip( currentChunk );
								break;

							case eID_TAG_PNTS:
								m_pSubmesh = p_mesh.CreateSubmesh();
								DoParsePnts( currentChunk );
								break;

							case eID_TAG_VMAP:
								DoParseVMap( currentChunk );
								break;

							case eID_TAG_POLS:
								DoParsePols( currentChunk );
								break;

							case eID_TAG_PTAG:
								DoParsePTag( currentChunk );
								break;

							case eID_TAG_TAGS:
								DoParseTags( currentChunk );
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
								DoDiscard( currentChunk );
								break;
							}
						}
						else
						{
							DoDiscard( currentChunk );
						}
					}
				}

				p_chunk.m_read += currentChunk.m_read + sizeof( eID_TAG ) + sizeof( UI4 );
				ok = m_file->IsOk();
			}
			else
			{
				ok = false;
			}
		}

		if ( !ok )
		{
			DoDiscard( p_chunk );
		}

		for ( auto it : m_arraySubmeshByMatName )
		{
			auto material = p_mesh.GetScene()->GetMaterialView().Find( string::string_cast< xchar >( it.first ) );

			if ( material )
			{
				it.second->SetDefaultMaterial( material );
			}
		}
	}

	void LwoImporter::DoDiscard( stLWO_CHUNK & p_chunk )
	{
		if ( p_chunk.m_size > p_chunk.m_read )
		{
			UI4 size = p_chunk.m_size - p_chunk.m_read;
			std::size_t remaining = std::size_t( m_file->GetLength() - m_file->Tell() );

			try
			{
				if ( size <= remaining && size > 0 )
				{
					m_file->Seek( size, Castor::File::OffsetMode::eCurrent );
				}
				else
				{
					m_file->Seek( 0, Castor::File::OffsetMode::eEnd );
					throw std::range_error( "Bad chunk size" );
				}
			}
			catch ( std::exception & exc )
			{
				Castor::Logger::LogDebug( std::stringstream() << "Exception caught when discarding chunk: " << exc.what() );
			}
			catch ( ... )
			{
				Castor::Logger::LogDebug( cuT( "Exception caught when discarding chunk" ) );
			}

			p_chunk.m_read = p_chunk.m_size;
		}
	}

	void LwoImporter::DoDiscard( stLWO_SUBCHUNK & p_chunk )
	{
		if ( p_chunk.m_size > p_chunk.m_read )
		{
			UI4 size = p_chunk.m_size - p_chunk.m_read;
			std::size_t remaining = std::size_t( m_file->GetLength() - m_file->Tell() );

			try
			{
				if ( size <= remaining && size > 0 )
				{
					m_file->Seek( size, Castor::File::OffsetMode::eCurrent );
				}
				else
				{
					m_file->Seek( 0, Castor::File::OffsetMode::eEnd );
					throw std::range_error( "	Bad subchunk size" );
				}
			}
			catch ( std::exception & exc )
			{
				Castor::Logger::LogDebug( std::stringstream() << "	Exception caught when discarding subchunk: " << exc.what() );
			}
			catch ( ... )
			{
				Castor::Logger::LogDebug( cuT( "	Exception caught when discarding subchunk" ) );
			}

			p_chunk.m_read = p_chunk.m_size;
		}
	}

	bool LwoImporter::DoIsValidChunk( stLWO_CHUNK & p_chunk, stLWO_CHUNK & p_parent )
	{
		bool result = DoIsTagId( p_chunk.m_id );

		if ( result )
		{
			result = p_chunk.m_size + p_parent.m_read <= p_parent.m_size;
		}

		return result;
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
		bool result = true;
		static char szTmp[1000];
		memset( szTmp, 0, 1000 );
		char cTmp = 1;
		std::size_t index = 0;

		while ( cTmp != 0 && m_file->IsOk() )
		{
			m_file->Read( cTmp );
			szTmp[index++] = cTmp;
		}

		if ( !m_file->IsOk() )
		{
			m_file->Seek( 1 - index, File::OffsetMode::eCurrent );
			result = false;
		}
		else
		{
			p_strString = szTmp;

			if ( result && p_strString.size() % 2 == 0 )
			{
				m_file->Seek( 1, File::OffsetMode::eCurrent );
			}
		}

		return result;
	}

	bool LwoImporter::DoRead( String const & p_tabs, stLWO_SUBCHUNK & p_subchunk )
	{
		bool result = m_file->IsOk();

		if ( result )
		{
			p_subchunk.m_read = 0;
			m_file->Read( p_subchunk.m_id );
			BigEndianToSystemEndian( p_subchunk.m_id );
			result = DoIsTagId( p_subchunk.m_id ) && m_file->IsOk();
			char id[5];
			StringStream toLog;
			DoToStr( id, p_subchunk.m_id );

			if ( result )
			{
				m_file->Read(	p_subchunk.m_size );
				BigEndianToSystemEndian( p_subchunk.m_size );
				toLog << p_tabs << cuT( "Subchunk : " ) << id << cuT( ", " ) << p_subchunk.m_size;
				result = m_file->IsOk();
			}
			else
			{
				toLog << p_tabs << cuT( "Invalid subchunk : " ) << id;
				m_file->Seek( -4, File::OffsetMode::eCurrent );
			}

			Logger::LogDebug( toLog );
		}

		return result;
	}

	UI2 LwoImporter::DoReadVX( UI4 & p_index )
	{
		UI2 result;
		p_index = 0;
		uint8_t byTest;
		m_file->Read( byTest );

		if ( byTest == 0xFF )
		{
			m_file->Read( p_index );
			BigEndianToSystemEndian( p_index );
			result = sizeof( UI4 );
		}
		else
		{
			m_file->Seek( -1, File::OffsetMode::eCurrent );
			UI2 ui2Index = 0;
			m_file->Read( ui2Index );
			BigEndianToSystemEndian( ui2Index );
			p_index = ui2Index;
			result = sizeof( UI2 );
		}

		return result;
	}

	bool LwoImporter::DoIsChunk( eID_TAG p_eId )
	{
		bool result = false;

		switch ( p_eId )
		{
		case eID_TAG_FORM:
		case eID_TAG_LAYR:
		case eID_TAG_PNTS:
		case eID_TAG_VMAP:
		case eID_TAG_POLS:
		case eID_TAG_TAGS:
		case eID_TAG_PTAG:
		case eID_TAG_VMAD:
		case eID_TAG_VMPA:
		case eID_TAG_ENVL:
		case eID_TAG_CLIP:
		case eID_TAG_SURF:
		case eID_TAG_BBOX:
		case eID_TAG_DESC:
		case eID_TAG_TEXT:
		case eID_TAG_ICON:
			result = true;
			break;

		default:
			result = false;
			break;
		}

		return result;
	}

	bool LwoImporter::DoIsTagId( eID_TAG p_eId )
	{
		bool result = false;

		switch ( p_eId )
		{
		default:
			result = false;
			break;

		case eID_TAG_FORM:
		case eID_TAG_LAYR:
		case eID_TAG_PNTS:
		case eID_TAG_VMAP:
		case eID_TAG_POLS:
		case eID_TAG_TAGS:
		case eID_TAG_PTAG:
		case eID_TAG_VMAD:
		case eID_TAG_VMPA:
		case eID_TAG_ENVL:

		//		Enveloppe Subchunk IDs
		case 	eID_TAG_ENVL_TYPE:
		case 	eID_TAG_ENVL_PRE:
		case 	eID_TAG_ENVL_POST:
		case 	eID_TAG_ENVL_KEY:
		case 	eID_TAG_ENVL_SPAN:
		case 	eID_TAG_ENVL_CHAN:
		case 	eID_TAG_ENVL_NAME:
		case eID_TAG_CLIP:

		//		Clip Subchunk IDs
		case 	eID_TAG_CLIP_STIL:
		case 	eID_TAG_CLIP_ANIM:
		case 	eID_TAG_CLIP_XREF:
		case 	eID_TAG_CLIP_STCC:
		case 	eID_TAG_CLIP_TIME:
		case 	eID_TAG_CLIP_CLRS:
		case 	eID_TAG_CLIP_CLRA:
		case 	eID_TAG_CLIP_FILT:
		case 	eID_TAG_CLIP_DITH:
		case 	eID_TAG_CLIP_CONT:
		case 	eID_TAG_CLIP_BRIT:
		case 	eID_TAG_CLIP_SATR:
		case 	eID_TAG_CLIP_HUE:
		case 	eID_TAG_CLIP_GAMM:
		case 	eID_TAG_CLIP_NEGA:
		case 	eID_TAG_CLIP_IFLT:
		case 	eID_TAG_CLIP_PFLT:
		case eID_TAG_SURF:

		//		Basic surface parameters Subchunk IDs
		case 	eID_TAG_SURF_COLR:
		case 	eID_TAG_SURF_DIFF:
		case 	eID_TAG_SURF_LUMI:
		case 	eID_TAG_SURF_SPEC:
		case 	eID_TAG_SURF_REFL:
		case 	eID_TAG_SURF_TRAN:
		case 	eID_TAG_SURF_TRNL:
		case 	eID_TAG_SURF_GLOS:
		case 	eID_TAG_SURF_SHRP:
		case 	eID_TAG_SURF_BUMP:
		case 	eID_TAG_SURF_SIDE:
		case 	eID_TAG_SURF_SMAN:
		case 	eID_TAG_SURF_RFOP:
		case 	eID_TAG_SURF_RIMG:
		case 	eID_TAG_SURF_RSAN:
		case 	eID_TAG_SURF_RBLR:
		case 	eID_TAG_SURF_RIND:
		case 	eID_TAG_SURF_TROP:
		case 	eID_TAG_SURF_TIMG:
		case 	eID_TAG_SURF_TBLR:
		case 	eID_TAG_SURF_CLRH:
		case 	eID_TAG_SURF_CLRF:
		case 	eID_TAG_SURF_ADTR:
		case 	eID_TAG_SURF_GLOW:
		case 	eID_TAG_SURF_LINE:
		case 	eID_TAG_SURF_ALPH:
		case 	eID_TAG_SURF_VCOL:

		//		Surface block Subchunk IDs
		case 	eID_TAG_SURF_BLOK:

		//			Block Header IDs
		case 		eID_TAG_BLOK_IMAP:

		//				Image mapping
		case 			eID_TAG_BLOK_PROJ:
		case 			eID_TAG_BLOK_AXIS:
		case 			eID_TAG_BLOK_IMAG:
		case 			eID_TAG_BLOK_WRAP:
		case 			eID_TAG_BLOK_WRPW:
		case 			eID_TAG_BLOK_WRPH:
		case 			eID_TAG_BLOK_AAST:
		case 			eID_TAG_BLOK_PIXB:
		case 			eID_TAG_BLOK_STCK:
		case 			eID_TAG_BLOK_TAMP:
		case 		eID_TAG_BLOK_PROC:

		//				Procedural textures
		case 			eID_TAG_BLOK_VALU:
		case 			eID_TAG_BLOK_FUNC:
		case 		eID_TAG_BLOK_GRAD:

		//				Gradient textures
		case 			eID_TAG_BLOK_PNAM:
		case 			eID_TAG_BLOK_INAM:
		case 			eID_TAG_BLOK_GRST:
		case 			eID_TAG_BLOK_GREN:
		case 			eID_TAG_BLOK_GRPT:
		case 			eID_TAG_BLOK_FKEY:
		case 			eID_TAG_BLOK_IKEY:
		case 		eID_TAG_BLOK_SHDR:

		//			Block header subchunks IDs
		case 		eID_TAG_BLOK_ENAB:
		case 		eID_TAG_BLOK_OPAC:
		case 		eID_TAG_BLOK_TMAP:

		//				Texture mapping
		case 			eID_TAG_BLOK_CNTR:
		case 			eID_TAG_BLOK_SIZE:
		case 			eID_TAG_BLOK_ROTA:
		case 			eID_TAG_BLOK_OREF:
		case 			eID_TAG_BLOK_FALL:
		case 			eID_TAG_BLOK_CSYS:
		case eID_TAG_BBOX:
		case eID_TAG_DESC:
		case eID_TAG_TEXT:
		case eID_TAG_ICON:
			result = true;
			break;
		}

		return result;
	}

	void LwoImporter::DoParsePTag( stLWO_CHUNK & p_chunk )
	{
		ePTAG_TYPE eType;
		bool ok = m_file->IsOk();

		if ( ok )
		{
			p_chunk.m_read += UI4( m_file->Read( eType ) );
			ok = m_file->IsOk();
		}

		if ( ok )
		{
			char szType[5];
			BigEndianToSystemEndian( eType );
			DoToStr( szType, eType );
			Logger::LogDebug( std::stringstream() << "\tType: " << szType );

			if ( eType == ePTAG_TYPE_SURF )
			{
				while ( ok && p_chunk.m_read < p_chunk.m_size )
				{
					UI2 usTag;
					UI4 uiVx;
					p_chunk.m_read += DoReadVX( uiVx );
					ok = m_file->IsOk();

					if ( ok )
					{
						p_chunk.m_read += UI4( m_file->Read( usTag ) );
						ok = m_file->IsOk();
					}

					if ( ok )
					{
						BigEndianToSystemEndian( usTag );
						//Logger::LogDebug( cuT( "\tPOLS Index : %d" ), uiVx );
						//Logger::LogDebug( cuT( "\tTAGS Index : %d" ), usTag );

						if ( usTag < m_arrayTags.size() )
						{
							StringStream toLog( cuT( "\tTAG Name: " ) );
							Logger::LogDebug( toLog << m_arrayTags[usTag].c_str() );
							m_arraySubmeshByMatName.push_back( std::make_pair( m_arrayTags[usTag], m_pSubmesh ) );
						}
					}
				}
			}
			else
			{
				ok = false;
			}
		}

		if ( !ok )
		{
			DoDiscard( p_chunk );
		}
	}

	UI2 LwoImporter::DoReadBlockHeader( stLWO_SUBCHUNK & p_subchunk, eTEX_CHANNEL & p_channel )
	{
		std::string strOrdinal;
		stLWO_SUBCHUNK currentSubchunk;
		UI2 usReturn = 0;
		bool ok = DoRead( cuT( "		" ), p_subchunk );

		if ( ok )
		{
			usReturn += sizeof( eID_TAG ) + sizeof( UI2 );
			ok	= DoRead( strOrdinal );
		}

		if ( ok )
		{
			p_subchunk.m_read += UI2( strOrdinal.size() + 1 + ( 1 - strOrdinal.size() % 2 ) );
			Logger::LogDebug( StringStream() << cuT( "			Header ordinal: 0x" ) << std::hex << int( strOrdinal[0] ) );
		}

		while ( ok && p_subchunk.m_read < p_subchunk.m_size )
		{
			if ( DoRead( cuT( "			" ), currentSubchunk ) )
			{
				switch ( currentSubchunk.m_id )
				{
				default:
					DoDiscard( currentSubchunk );
					break;

				case eID_TAG_BLOK_CHAN:
					currentSubchunk.m_read += UI2( m_file->Read( p_channel ) );
					BigEndianToSystemEndian( p_channel );
					Logger::LogDebug( StringStream() << cuT( "				Channel: " ) << p_channel );
					break;
				}

				p_subchunk.m_read += currentSubchunk.m_read + sizeof( eID_TAG ) + sizeof( UI2 );
			}
			else
			{
				ok = false;
			}
		}

		if ( !ok )
		{
			DoDiscard( p_subchunk );
		}

		usReturn += p_subchunk.m_read;
		return usReturn;
	}

	void LwoImporter::DoReadTMap( stLWO_SUBCHUNK & p_subchunk )
	{
		stLWO_SUBCHUNK currentSubchunk;
		bool ok = true;

		while ( ok && p_subchunk.m_read < p_subchunk.m_size )
		{
			if ( DoRead( cuT( "			" ), currentSubchunk ) )
			{
				//switch( currentSubchunk.m_id )
				//{
				//default:
				DoDiscard( currentSubchunk );
				//	break;
				//}
				p_subchunk.m_read += currentSubchunk.m_read + sizeof( eID_TAG ) + sizeof( UI2 );
			}
			else
			{
				ok = false;
			}
		}

		if ( !ok )
		{
			DoDiscard( p_subchunk );
		}
	}

	void LwoImporter::DoReadIMap( stLWO_SUBCHUNK & p_subchunk, eTEX_CHANNEL & p_channel )
	{
		stLWO_SUBCHUNK currentSubchunk;
		bool ok = true;

		while ( ok && p_subchunk.m_read < p_subchunk.m_size )
		{
			if ( DoRead( cuT( "			" ), currentSubchunk ) )
			{
				switch ( currentSubchunk.m_id )
				{
				default:
					DoDiscard( currentSubchunk );
					break;

				case eID_TAG_BLOK_CHAN:
					currentSubchunk.m_read += UI2( m_file->Read( p_channel ) );
					BigEndianToSystemEndian( p_channel );
					Logger::LogDebug( StringStream() << cuT( "				Channel: " ) << p_channel );
					break;
				}

				p_subchunk.m_read += currentSubchunk.m_read + sizeof( eID_TAG ) + sizeof( UI2 );
			}
			else
			{
				ok = false;
			}
		}

		if ( !ok )
		{
			DoDiscard( p_subchunk );
		}
	}

	void LwoImporter::DoReadProc( stLWO_SUBCHUNK & p_subchunk )
	{
		stLWO_SUBCHUNK currentSubchunk;
		bool ok = true;

		while ( ok && p_subchunk.m_read < p_subchunk.m_size )
		{
			if ( DoRead( cuT( "			" ), currentSubchunk ) )
			{
				//switch( currentSubchunk.m_id )
				//{
				//default:
				DoDiscard( currentSubchunk );
				//	break;
				//}
				p_subchunk.m_read += currentSubchunk.m_read + sizeof( eID_TAG ) + sizeof( UI2 );
			}
			else
			{
				ok = false;
			}
		}

		if ( !ok )
		{
			DoDiscard( p_subchunk );
		}
	}

	void LwoImporter::DoReadGrad( stLWO_SUBCHUNK & p_subchunk )
	{
		stLWO_SUBCHUNK currentSubchunk;
		bool ok = true;

		while ( ok && p_subchunk.m_read < p_subchunk.m_size )
		{
			if ( DoRead( cuT( "			" ), currentSubchunk ) )
			{
				//switch( currentSubchunk.m_id )
				//{
				//default:
				DoDiscard( currentSubchunk );
				//	break;
				//}
				p_subchunk.m_read += currentSubchunk.m_read + sizeof( eID_TAG ) + sizeof( UI2 );
			}
			else
			{
				ok = false;
			}
		}

		if ( !ok )
		{
			DoDiscard( p_subchunk );
		}
	}

	void LwoImporter::DoReadShdr( stLWO_SUBCHUNK & p_subchunk )
	{
		stLWO_SUBCHUNK currentSubchunk;
		bool ok = true;

		while ( ok && p_subchunk.m_read < p_subchunk.m_size )
		{
			if ( DoRead( cuT( "			" ), currentSubchunk ) )
			{
				//switch( currentSubchunk.m_id )
				//{
				//default:
				DoDiscard( currentSubchunk );
				//	break;
				//}
				p_subchunk.m_read += currentSubchunk.m_read + sizeof( eID_TAG ) + sizeof( UI2 );
			}
			else
			{
				ok = false;
			}
		}

		if ( !ok )
		{
			DoDiscard( p_subchunk );
		}
	}

	void LwoImporter::DoSetChannel( TextureUnitSPtr p_pTexture, eTEX_CHANNEL p_channel )
	{
		if ( p_pTexture )
		{
			switch ( p_channel )
			{
			case eTEX_CHANNEL_COLR:
				p_pTexture->SetChannel( TextureChannel::eDiffuse );
				break;

			case eTEX_CHANNEL_DIFF:
				p_pTexture->SetChannel( TextureChannel::eDiffuse );
				break;

			case eTEX_CHANNEL_SPEC:
				p_pTexture->SetChannel( TextureChannel::eSpecular );
				break;

			case eTEX_CHANNEL_GLOS:
				p_pTexture->SetChannel( TextureChannel::eGloss );
				break;

			case eTEX_CHANNEL_TRAN:
				p_pTexture->SetChannel( TextureChannel::eOpacity );
				break;

			case eTEX_CHANNEL_BUMP:
				p_pTexture->SetChannel( TextureChannel::eNormal );
				break;
			}
		}
	}

	void LwoImporter::DoReadBlock( stLWO_SUBCHUNK & p_subchunk, PassSPtr p_pass )
	{
		stLWO_SUBCHUNK currentSubchunk;
		stLWO_SUBCHUNK blockHeader;
		UI4 uiVx;
		eTEX_CHANNEL eChannel = eTEX_CHANNEL_COLR;
		bool ok = true;
		TextureUnitSPtr unit;
		ImageVxMap::iterator it;
		p_subchunk.m_read += DoReadBlockHeader( blockHeader, eChannel );

		while ( ok && p_subchunk.m_read < p_subchunk.m_size )
		{
			if ( DoRead( cuT( "		" ), currentSubchunk ) )
			{
				switch ( currentSubchunk.m_id )
				{
				case eID_TAG_BLOK_TMAP:
					DoReadTMap( currentSubchunk );
					break;

				case eID_TAG_BLOK_IMAP:
					DoReadIMap( currentSubchunk, eChannel );
					break;

				case eID_TAG_BLOK_PROC:
					DoReadProc( currentSubchunk );
					break;

				case eID_TAG_BLOK_GRAD:
					DoReadGrad( currentSubchunk );
					break;

				case eID_TAG_BLOK_SHDR:
					DoReadShdr( currentSubchunk );
					break;

				case eID_TAG_BLOK_IMAG:
					currentSubchunk.m_read += DoReadVX( uiVx );
					it = m_mapImages.find( uiVx );

					if ( it != m_mapImages.end() )
					{
						StringStream toLog( cuT( "			Texture found: " ) );
						Logger::LogDebug( toLog << it->second->GetPath().c_str() );
						unit = std::make_shared< TextureUnit >( *GetEngine() );
						auto texture = GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions, AccessType::eNone, AccessType::eRead );
						texture->SetSource( it->second->GetPixels() );
						unit->SetTexture( texture );
						DoSetChannel( unit, eChannel );
						p_pass->AddTextureUnit( unit );
					}
					else
					{
						unit.reset();
					}

					break;

				case eID_TAG_BLOK_CHAN:
					currentSubchunk.m_read += UI2( m_file->Read( eChannel ) );
					BigEndianToSystemEndian( eChannel );
					Logger::LogDebug( StringStream() << cuT( "			Channel: " ) << eChannel );
					DoSetChannel( unit, eChannel );
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
					DoDiscard( currentSubchunk );
					break;
				}

				p_subchunk.m_read += currentSubchunk.m_read + sizeof( eID_TAG ) + sizeof( UI2 );
			}
			else
			{
				ok = false;
			}
		}

		if ( !ok )
		{
			DoDiscard( p_subchunk );
		}
	}

	void LwoImporter::DoParseTags( stLWO_CHUNK & p_chunk )
	{
		std::string strTag;
		bool ok = m_file->IsOk();

		while ( ok && p_chunk.m_read < p_chunk.m_size )
		{
			ok = DoRead( strTag );

			if ( ok )
			{
				p_chunk.m_read += UI4( strTag.size() + 1 + ( 1 - strTag.size() % 2 ) );
				m_arrayTags.push_back( strTag );
				StringStream toLog( cuT( "\tName : " ) );
				Logger::LogDebug( toLog << strTag.c_str() );
			}
		}

		if ( !ok )
		{
			DoDiscard( p_chunk );
		}
	}

	void LwoImporter::DoParseSurf( stLWO_CHUNK & p_chunk )
	{
		stLWO_SUBCHUNK currentSubchunk;
		stLWO_SUBCHUNK blockHeader;
		Colour clrBase;
		MaterialSPtr pMaterial;
		LegacyPassSPtr pass;
		UI4 uiVx;
		float fDiff = 1;
		float fSpec = 0;
		float fGlos = 0;
		UI2 usSide = 1;
		bool ok = DoRead( m_strName );

		if ( ok )
		{
			p_chunk.m_read += UI4( m_strName.size() + 1 + ( 1 - m_strName.size() % 2 ) );
			ok = DoRead( m_strSource );
		}

		if ( ok )
		{
			p_chunk.m_read += UI4( m_strSource.size() + 1 + ( 1 - m_strSource.size() % 2 ) );
			Logger::LogDebug( cuT( "	Name : " ) + string::string_cast< xchar >( m_strName ) );
			Logger::LogDebug( cuT( "	Source : " ) + string::string_cast< xchar >( m_strSource ) );
			pMaterial = GetEngine()->GetMaterialCache().Add( string::string_cast< xchar >( m_strName ), MaterialType::eLegacy );
			REQUIRE( pMaterial->GetType() == MaterialType::eLegacy );
			pass = pMaterial->GetTypedPass< MaterialType::eLegacy >( 0u );
		}

		while ( ok && p_chunk.m_read < p_chunk.m_size )
		{
			if ( DoRead( cuT( "	" ), currentSubchunk ) )
			{
				switch ( currentSubchunk.m_id )
				{
				case eID_TAG_SURF_COLR:
					currentSubchunk.m_read += UI2( m_file->ReadArray( clrBase.ptr(), 3 ) );
					BigEndianToSystemEndian( clrBase.ptr()[0] );
					BigEndianToSystemEndian( clrBase.ptr()[1] );
					BigEndianToSystemEndian( clrBase.ptr()[2] );
					clrBase.alpha() = 1.0f;
					currentSubchunk.m_read += DoReadVX( uiVx );
					Logger::LogDebug( StringStream() << cuT( "		Colour: " ) << clrBase.red().value() << cuT( ", " ) << clrBase.green().value() << cuT( ", " ) << clrBase.blue().value() << cuT( " - VX : 0x" ) << std::hex << uiVx );
					break;

				case eID_TAG_SURF_DIFF:
					currentSubchunk.m_read += UI2( m_file->Read( fDiff ) );
					BigEndianToSystemEndian( fDiff );
					currentSubchunk.m_read += DoReadVX( uiVx );
					Logger::LogDebug( StringStream() << cuT( "		Diff. base level: " ) << fDiff << cuT( " - VX : 0x" ) << std::hex << uiVx );
					break;

				case eID_TAG_SURF_SPEC:
					currentSubchunk.m_read += UI2( m_file->Read( fSpec ) );
					BigEndianToSystemEndian( fSpec );
					currentSubchunk.m_read += DoReadVX( uiVx );
					Logger::LogDebug( StringStream() << cuT( "		Spec. base level: " ) << fSpec << cuT( " - VX : 0x" ) << std::hex << uiVx );
					break;

				case eID_TAG_SURF_GLOS:
					currentSubchunk.m_read += UI2( m_file->Read( fGlos ) );
					BigEndianToSystemEndian( fGlos );
					currentSubchunk.m_read += DoReadVX( uiVx );
					Logger::LogDebug( StringStream() << cuT( "		Glossiness: " ) << fGlos << cuT( " - VX : 0x" ) << std::hex << uiVx );
					break;

				case eID_TAG_SURF_SIDE:
					currentSubchunk.m_read += UI2( m_file->Read( usSide ) );
					BigEndianToSystemEndian( usSide );
					Logger::LogDebug( StringStream() << cuT( "		Sidedness: " ) << usSide );
					break;

				case eID_TAG_SURF_BLOK:
					DoReadBlock( currentSubchunk, pass );
					break;

				case eID_TAG_SURF_LUMI:
				case eID_TAG_SURF_REFL:
				case eID_TAG_SURF_TRAN:
				case eID_TAG_SURF_TRNL:
				case eID_TAG_SURF_BUMP:
				default:
					DoDiscard( currentSubchunk );
					break;
				}

				p_chunk.m_read += currentSubchunk.m_read + sizeof( eID_TAG ) + sizeof( UI2 );
			}
			else
			{
				ok = false;
			}
		}

		if ( ok )
		{
			pass->SetDiffuse( clrBase * fDiff );
			pass->SetSpecular( clrBase * fSpec );
			pass->SetShininess( float( 2 ^ int( ( 10 * fGlos ) + 2 ) ) );
			pass->SetTwoSided( usSide == 3 );
		}
		else
		{
			DoDiscard( p_chunk );
		}
	}

	void LwoImporter::DoParseClip( stLWO_CHUNK & p_chunk )
	{
		UI4	uiIndex;
		stLWO_SUBCHUNK currentSubchunk;
		bool ok = m_file->Read( uiIndex ) == sizeof( UI4 );
		std::string strName;
		Path pathImage;
		ImageSPtr pImage;

		if ( ok )
		{
			BigEndianToSystemEndian( uiIndex );
			p_chunk.m_read += sizeof( UI4 );
			Logger::LogDebug( StringStream() << cuT( "	Index: 0x" ) << std::hex << uiIndex );
		}

		while ( ok && p_chunk.m_read < p_chunk.m_size )
		{
			if ( DoRead( cuT( "	" ), currentSubchunk ) )
			{
				switch ( currentSubchunk.m_id )
				{
				case eID_TAG_CLIP_STIL:
				{
					DoRead( strName );
					currentSubchunk.m_read += UI2( strName.size() + 1 + ( 1 - strName.size() % 2 ) );
					Logger::LogDebug( cuT( "		Image : " ) + string::string_cast< xchar >( strName ) );
					pathImage = Path{ string::string_cast< xchar >( strName ) };

					if ( !File::FileExists( pathImage ) )
					{
						pathImage = m_file->GetFilePath() / pathImage;
					}

					if ( !File::FileExists( pathImage ) )
					{
						pathImage = m_file->GetFilePath() / cuT( "Texture" ) / pathImage;
					}

					ImageSPtr pImage = GetEngine()->GetImageCache().Add( string::string_cast< xchar >( strName ), pathImage );
					break;
				}

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
					DoDiscard( currentSubchunk );
					break;
				}

				p_chunk.m_read += currentSubchunk.m_read + sizeof( eID_TAG ) + sizeof( UI2 );
			}
			else
			{
				ok = false;
			}
		}

		if ( !ok )
		{
			DoDiscard( p_chunk );
		}
	}

	void LwoImporter::DoParsePnts( stLWO_CHUNK & p_chunk )
	{
		if ( p_chunk.m_size > 0 && p_chunk.m_size % 3 == 0 )
		{
			F4 fCoords[3];
			UI4 uiCount = p_chunk.m_size / ( sizeof( F4 ) * 3 );
			Logger::LogDebug( StringStream() << cuT( "\tCount: " ) << uiCount );
			m_arrayPoints.resize( uiCount );
			m_arrayUvs.resize( uiCount );

			for ( UI4 i = 0 ; i < uiCount ; i++ )
			{
				p_chunk.m_read += UI4( m_file->ReadArray( fCoords, 3 ) );
				BigEndianToSystemEndian( fCoords[0] );
				BigEndianToSystemEndian( fCoords[1] );
				BigEndianToSystemEndian( fCoords[2] );
				m_pSubmesh->AddPoint( fCoords );
			}
		}
	}

	void LwoImporter::DoParseVMap( stLWO_CHUNK & p_chunk )
	{
		eVMAP_TYPE eType = eVMAP_TYPE( 0 );
		UI2 usDimension;
		std::string strName;
		bool ok = m_file->IsOk();

		if ( ok )
		{
			p_chunk.m_read += UI4( m_file->Read( eType ) );
			ok = m_file->IsOk();
		}

		if ( ok )
		{
			p_chunk.m_read += UI4( m_file->Read( usDimension ) );
			ok = m_file->IsOk();
		}

		if ( ok )
		{
			ok = DoRead( strName );
		}

		if ( ok )
		{
			p_chunk.m_read += UI4( strName.size() + 1 + ( 1 - strName.size() % 2 ) );
			BigEndianToSystemEndian( eType );
			BigEndianToSystemEndian( usDimension );
		}

		if ( ok )
		{
			char szType[5];
			DoToStr( szType, eType );
			Logger::LogDebug( StringStream() << cuT( "\tType: " ) << szType );
			Logger::LogDebug( StringStream() << cuT( "\tDimensions: " ) << usDimension );
			Logger::LogDebug( StringStream() << cuT( "\tName: " ) << strName.c_str() );

			if ( eType == eVMAP_TYPE_TXUV )
			{
				F4	fUv[2];
				UI4 i = 0;

				while ( ok && p_chunk.m_read < p_chunk.m_size )
				{
					UI4 uiVx;
					p_chunk.m_read += DoReadVX( uiVx );
					ok = m_file->IsOk();

					if ( ok )
					{
						p_chunk.m_read += UI4( m_file->ReadArray( fUv, 2 ) );
						ok = m_file->IsOk();
					}

					if ( ok )
					{
						BigEndianToSystemEndian( fUv[0] );
						BigEndianToSystemEndian( fUv[1] );
						Vertex::SetTexCoord( m_pSubmesh->GetPoint( uiVx ), fUv[0], fUv[1] );
						i++;
					}

					//Logger::LogDebug( cuT( "\tVertex : %d - %f, %f" ), uiVx, fUv[0], fUv[1] );
				}

				m_bHasUv = true;
				Logger::LogDebug( StringStream() << cuT( "\tUV count: " ) << i );
			}
			else
			{
				ok = false;
			}
		}

		if ( !ok )
		{
			DoDiscard( p_chunk );
		}
	}

	void LwoImporter::DoParsePols( stLWO_CHUNK & p_chunk )
	{
		std::vector< Lwo::Face > arrayFaces;
		std::set< UI4 > setPntsIds;
		ePOLS_TYPE eType;
		bool ok = m_file->IsOk();
		UI4 uiCount = 0;

		if ( ok )
		{
			p_chunk.m_read += UI4( m_file->Read( eType ) );
			ok = m_file->IsOk();
		}

		if ( ok )
		{
			char szType[5];
			BigEndianToSystemEndian( eType );
			DoToStr( szType, eType );
			StringStream toLog( cuT( "\tType : " ) );
			Logger::LogDebug( toLog << szType );

			if ( eType == ePOLS_TYPE_FACE )
			{
				while ( ok && p_chunk.m_read < p_chunk.m_size )
				{
					UI2 usCountFlags = 0;
					p_chunk.m_read += UI4( m_file->Read( usCountFlags ) );
					UI2 usCount = usCountFlags & 0xFFC0;
					BigEndianToSystemEndian( usCount );
					Lwo::Face face( usCount );
					ok = m_file->IsOk();

					for ( UI2 i = 0 ; i < usCount && ok ; i++ )
					{
						p_chunk.m_read += DoReadVX( face[i] );
						setPntsIds.insert( face[i] );
						ok = m_file->IsOk();
					}

					if ( ok )
					{
						arrayFaces.push_back( face );
						uiCount++;
					}
				}

				Logger::LogDebug( StringStream() << cuT( "\tFaces count: " ) << uiCount );
			}
			else
			{
				ok = false;
			}
		}

		if ( ok )
		{
			if ( m_pSubmesh )
			{
				uiCount = 0;
				std::map< UI4, UI4 > mapPntsIds;
				FaceSPtr pFace;

				for ( std::set< UI4 >::iterator it = setPntsIds.begin() ; it != setPntsIds.end() ; ++it, ++uiCount )
				{
					m_pSubmesh->AddPoint( m_arrayPoints[*it] );
					mapPntsIds.insert( std::make_pair( *it, uiCount ) );
				}

				for ( std::vector< Lwo::Face >::iterator it = arrayFaces.begin() ; it != arrayFaces.end() ; ++it )
				{
					m_pSubmesh->AddFace( mapPntsIds[it->at( 1 )], mapPntsIds[it->at( 0 )], mapPntsIds[it->at( 2 )] );
				}
			}
		}
		else
		{
			DoDiscard( p_chunk );
		}
	}

	void LwoImporter::DoParseLayr( stLWO_CHUNK & p_chunk )
	{
		bool ok = m_file->IsOk();
		UI2 usNumber;
		UI2 usFlags;
		Point3f ptPivot;
		std::string strName;
		UI2 usParent;

		if ( ok )
		{
			p_chunk.m_read += UI4( m_file->Read( usNumber ) );
			ok = m_file->IsOk();
		}

		if ( ok && p_chunk.m_read < p_chunk.m_size )
		{
			p_chunk.m_read += UI4( m_file->Read( usFlags ) );
			ok = m_file->IsOk();
		}

		if ( ok && p_chunk.m_read < p_chunk.m_size )
		{
			p_chunk.m_read += UI4( m_file->ReadArray( ptPivot.ptr(), 3 ) );
			ok = m_file->IsOk();
		}

		if ( ok && p_chunk.m_read < p_chunk.m_size )
		{
			DoRead( strName );
			ok = m_file->IsOk();
		}

		if ( ok )
		{
			p_chunk.m_read += UI4( strName.size() + 1 + ( 1 - strName.size() % 2 ) );

			if ( p_chunk.m_read < p_chunk.m_size )
			{
				p_chunk.m_read += UI4( m_file->Read( usParent ) );
				ok = m_file->IsOk();
			}
		}

		if ( ok )
		{
			BigEndianToSystemEndian( usNumber );
			BigEndianToSystemEndian( usFlags );
			BigEndianToSystemEndian( ptPivot[0] );
			BigEndianToSystemEndian( ptPivot[1] );
			BigEndianToSystemEndian( ptPivot[2] );
			BigEndianToSystemEndian( usParent );
			StringStream toLog( cuT( "\tName   : " ) );
			Logger::LogDebug( toLog << strName.c_str() );
			Logger::LogDebug( StringStream() << cuT( "\tNumber: " ) << usNumber );
			Logger::LogDebug( StringStream() << cuT( "\tFlags:  0x" ) << std::hex << usFlags );
			Logger::LogDebug( StringStream() << cuT( "\tPivot:  " ) << ptPivot );
			Logger::LogDebug( StringStream() << cuT( "\tParent: " ) << usParent );
			String strNameTmp = string::string_cast< xchar >( strName );
			string::to_lower_case( strNameTmp );

			if ( usFlags & 0x01 || strNameTmp.find( cuT( "bip" ) ) == 0 || strNameTmp.find( cuT( "fcfx" ) ) == 0 || strNameTmp.find( cuT( "gundummy" ) ) == 0 || strNameTmp.find( cuT( "flap_bone" ) ) == 0 )
			{
				m_bIgnored = true;
			}
			else
			{
				m_bIgnored = false;
			}
		}

		if ( !ok )
		{
			DoDiscard( p_chunk );
		}
	}
}
