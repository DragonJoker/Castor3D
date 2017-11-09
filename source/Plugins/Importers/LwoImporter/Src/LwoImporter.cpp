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

using namespace castor3d;
using namespace castor;

namespace Lwo
{
	LwoImporter::LwoImporter( castor3d::Engine & p_engine )
		: Importer( p_engine )
		, m_file( nullptr )
		, m_bIgnored( false )
		, m_bHasUv( false )
	{
	}

	LwoImporter::~LwoImporter()
	{
	}

	ImporterUPtr LwoImporter::create( Engine & p_engine )
	{
		return std::make_unique< LwoImporter >( p_engine );
	}

	bool LwoImporter::doImportScene( Scene & p_scene )
	{
		auto mesh = p_scene.getMeshCache().add( cuT( "Mesh_LWO" ) );
		bool result = doImportMesh( *mesh );

		if ( result )
		{
			SceneNodeSPtr node = p_scene.getSceneNodeCache().add( mesh->getName(), p_scene.getObjectRootNode() );
			GeometrySPtr geometry = p_scene.getGeometryCache().add( mesh->getName(), node, nullptr );
			geometry->setMesh( mesh );
			m_geometries.insert( { geometry->getName(), geometry } );
		}

		return result;
	}

	bool LwoImporter::doImportMesh( Mesh & p_mesh )
	{
		UIntArray faces;
		RealArray sizes;
		String nodeName = m_fileName.getFileName();
		String meshName = m_fileName.getFileName();
		String materialName = m_fileName.getFileName();
		m_file = new BinaryFile( m_fileName, File::OpenMode::eRead );
		stLWO_CHUNK currentChunk;
		bool result{ false };

		if ( m_file->isOk() )
		{
			Logger::logDebug( cuT( "**************************************************" ) );
			Logger::logDebug( cuT( "Importing mesh from file : [" ) + m_fileName + cuT( "]" ) );
			doRead( currentChunk );
			char name[5] = { 0, 0, 0, 0 , 0 };
			currentChunk.m_read += UI4( m_file->readArray( name, 4 ) );

			if ( currentChunk.m_id == eID_TAG_FORM && currentChunk.m_size == m_file->getLength() - 8 && std::string( name ) == "LWO2" )
			{
				doProcess( p_mesh, currentChunk );
				p_mesh.computeNormals();
			}

			Logger::logDebug( cuT( "**************************************************" ) );
			result = true;
		}

		return result;
	}

	bool LwoImporter::doRead( stLWO_CHUNK & p_chunk )
	{
		bool result = m_file->isOk();

		if ( result )
		{
			p_chunk.m_read = 0;
			m_file->read(	p_chunk.m_id );
			bigEndianToSystemEndian( p_chunk.m_id );
			result = doIsTagId( p_chunk.m_id ) && m_file->isOk();
			char id[5];
			StringStream toLog;
			doToStr( id, p_chunk.m_id );

			if ( result )
			{
				m_file->read(	p_chunk.m_size );
				bigEndianToSystemEndian( p_chunk.m_size );
				toLog << cuT( "Chunk : " ) << id << cuT( ", " ) << p_chunk.m_size;
				result = m_file->isOk();
			}
			else
			{
				toLog << cuT( "Invalid chunk : " ) << id;
				m_file->seek( -4, File::OffsetMode::eCurrent );
			}

			Logger::logDebug( toLog );
		}

		return result;
	}

	void LwoImporter::doProcess( Mesh & p_mesh, stLWO_CHUNK & p_chunk )
	{
		stLWO_CHUNK currentChunk;
		bool ok = m_file->isOk();

		while ( p_chunk.m_read < p_chunk.m_size && ok )
		{
			if ( doRead( currentChunk ) )
			{
				if ( !doIsValidChunk( currentChunk, p_chunk ) )
				{
					ok = false;
				}
				else
				{
					switch ( currentChunk.m_id )
					{
					case eID_TAG_LAYR:
						doParseLayr( currentChunk );
						break;

					default:
						if ( !m_bIgnored )
						{
							switch ( currentChunk.m_id )
							{
							case eID_TAG_LAYR:
							case eID_TAG_SURF:
								doParseSurf( currentChunk );
								break;

							case eID_TAG_CLIP:
								doParseClip( currentChunk );
								break;

							case eID_TAG_PNTS:
								m_pSubmesh = p_mesh.createSubmesh();
								doParsePnts( currentChunk );
								break;

							case eID_TAG_VMAP:
								doParseVMap( currentChunk );
								break;

							case eID_TAG_POLS:
								doParsePols( currentChunk );
								break;

							case eID_TAG_PTAG:
								doParsePTag( currentChunk );
								break;

							case eID_TAG_TAGS:
								doParseTags( currentChunk );
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
								doDiscard( currentChunk );
								break;
							}
						}
						else
						{
							doDiscard( currentChunk );
						}
					}
				}

				p_chunk.m_read += currentChunk.m_read + sizeof( eID_TAG ) + sizeof( UI4 );
				ok = m_file->isOk();
			}
			else
			{
				ok = false;
			}
		}

		if ( !ok )
		{
			doDiscard( p_chunk );
		}

		for ( auto it : m_arraySubmeshByMatName )
		{
			auto material = p_mesh.getScene()->getMaterialView().find( string::stringCast< xchar >( it.first ) );

			if ( material )
			{
				it.second->setDefaultMaterial( material );
			}
		}
	}

	void LwoImporter::doDiscard( stLWO_CHUNK & p_chunk )
	{
		if ( p_chunk.m_size > p_chunk.m_read )
		{
			UI4 size = p_chunk.m_size - p_chunk.m_read;
			std::size_t remaining = std::size_t( m_file->getLength() - m_file->tell() );

			try
			{
				if ( size <= remaining && size > 0 )
				{
					m_file->seek( size, castor::File::OffsetMode::eCurrent );
				}
				else
				{
					m_file->seek( 0, castor::File::OffsetMode::eEnd );
					throw std::range_error( "Bad chunk size" );
				}
			}
			catch ( std::exception & exc )
			{
				castor::Logger::logDebug( std::stringstream() << "Exception caught when discarding chunk: " << exc.what() );
			}
			catch ( ... )
			{
				castor::Logger::logDebug( cuT( "Exception caught when discarding chunk" ) );
			}

			p_chunk.m_read = p_chunk.m_size;
		}
	}

	void LwoImporter::doDiscard( stLWO_SUBCHUNK & p_chunk )
	{
		if ( p_chunk.m_size > p_chunk.m_read )
		{
			UI4 size = p_chunk.m_size - p_chunk.m_read;
			std::size_t remaining = std::size_t( m_file->getLength() - m_file->tell() );

			try
			{
				if ( size <= remaining && size > 0 )
				{
					m_file->seek( size, castor::File::OffsetMode::eCurrent );
				}
				else
				{
					m_file->seek( 0, castor::File::OffsetMode::eEnd );
					throw std::range_error( "	Bad subchunk size" );
				}
			}
			catch ( std::exception & exc )
			{
				castor::Logger::logDebug( std::stringstream() << "	Exception caught when discarding subchunk: " << exc.what() );
			}
			catch ( ... )
			{
				castor::Logger::logDebug( cuT( "	Exception caught when discarding subchunk" ) );
			}

			p_chunk.m_read = p_chunk.m_size;
		}
	}

	bool LwoImporter::doIsValidChunk( stLWO_CHUNK & p_chunk, stLWO_CHUNK & p_parent )
	{
		bool result = doIsTagId( p_chunk.m_id );

		if ( result )
		{
			result = p_chunk.m_size + p_parent.m_read <= p_parent.m_size;
		}

		return result;
	}

	void LwoImporter::doToStr( char p_szId[5], UI4 p_uiId )
	{
		p_szId[0] = ( ( uint8_t * )&p_uiId )[3];
		p_szId[1] = ( ( uint8_t * )&p_uiId )[2];
		p_szId[2] = ( ( uint8_t * )&p_uiId )[1];
		p_szId[3] = ( ( uint8_t * )&p_uiId )[0];
		p_szId[4] = 0;
	}

	bool LwoImporter::doRead( std::string & p_strString )
	{
		bool result = true;
		static char szTmp[1000];
		memset( szTmp, 0, 1000 );
		char cTmp = 1;
		std::size_t index = 0;

		while ( cTmp != 0 && m_file->isOk() )
		{
			m_file->read( cTmp );
			szTmp[index++] = cTmp;
		}

		if ( !m_file->isOk() )
		{
			m_file->seek( 1 - index, File::OffsetMode::eCurrent );
			result = false;
		}
		else
		{
			p_strString = szTmp;

			if ( result && ( p_strString.size() % 2 ) == 0 )
			{
				m_file->seek( 1, File::OffsetMode::eCurrent );
			}
		}

		return result;
	}

	bool LwoImporter::doRead( String const & p_tabs, stLWO_SUBCHUNK & p_subchunk )
	{
		bool result = m_file->isOk();

		if ( result )
		{
			p_subchunk.m_read = 0;
			m_file->read( p_subchunk.m_id );
			bigEndianToSystemEndian( p_subchunk.m_id );
			result = doIsTagId( p_subchunk.m_id ) && m_file->isOk();
			char id[5];
			StringStream toLog;
			doToStr( id, p_subchunk.m_id );

			if ( result )
			{
				m_file->read(	p_subchunk.m_size );
				bigEndianToSystemEndian( p_subchunk.m_size );
				toLog << p_tabs << cuT( "Subchunk : " ) << id << cuT( ", " ) << p_subchunk.m_size;
				result = m_file->isOk();
			}
			else
			{
				toLog << p_tabs << cuT( "Invalid subchunk : " ) << id;
				m_file->seek( -4, File::OffsetMode::eCurrent );
			}

			Logger::logDebug( toLog );
		}

		return result;
	}

	UI2 LwoImporter::doReadVX( UI4 & p_index )
	{
		UI2 result;
		p_index = 0;
		uint8_t byTest;
		m_file->read( byTest );

		if ( byTest == 0xFF )
		{
			m_file->read( p_index );
			bigEndianToSystemEndian( p_index );
			result = sizeof( UI4 );
		}
		else
		{
			m_file->seek( -1, File::OffsetMode::eCurrent );
			UI2 ui2Index = 0;
			m_file->read( ui2Index );
			bigEndianToSystemEndian( ui2Index );
			p_index = ui2Index;
			result = sizeof( UI2 );
		}

		return result;
	}

	bool LwoImporter::doIsChunk( eID_TAG p_eId )
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

	bool LwoImporter::doIsTagId( eID_TAG p_eId )
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

	void LwoImporter::doParsePTag( stLWO_CHUNK & p_chunk )
	{
		ePTAG_TYPE eType;
		bool ok = m_file->isOk();

		if ( ok )
		{
			p_chunk.m_read += UI4( m_file->read( eType ) );
			ok = m_file->isOk();
		}

		if ( ok )
		{
			char szType[5];
			bigEndianToSystemEndian( eType );
			doToStr( szType, eType );
			Logger::logDebug( std::stringstream() << "\tType: " << szType );

			if ( eType == ePTAG_TYPE_SURF )
			{
				while ( ok && p_chunk.m_read < p_chunk.m_size )
				{
					UI2 usTag;
					UI4 uiVx;
					p_chunk.m_read += doReadVX( uiVx );
					ok = m_file->isOk();

					if ( ok )
					{
						p_chunk.m_read += UI4( m_file->read( usTag ) );
						ok = m_file->isOk();
					}

					if ( ok )
					{
						bigEndianToSystemEndian( usTag );
						//Logger::logDebug( cuT( "\tPOLS Index : %d" ), uiVx );
						//Logger::logDebug( cuT( "\tTAGS Index : %d" ), usTag );

						if ( usTag < m_arrayTags.size() )
						{
							StringStream toLog( cuT( "\tTAG Name: " ) );
							Logger::logDebug( toLog << m_arrayTags[usTag].c_str() );
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
			doDiscard( p_chunk );
		}
	}

	UI2 LwoImporter::doReadBlockHeader( stLWO_SUBCHUNK & p_subchunk, eTEX_CHANNEL & p_channel )
	{
		std::string strOrdinal;
		stLWO_SUBCHUNK currentSubchunk;
		UI2 usReturn = 0;
		bool ok = doRead( cuT( "		" ), p_subchunk );

		if ( ok )
		{
			usReturn += sizeof( eID_TAG ) + sizeof( UI2 );
			ok	= doRead( strOrdinal );
		}

		if ( ok )
		{
			p_subchunk.m_read += UI2( strOrdinal.size() + 1 + ( 1 - strOrdinal.size() % 2 ) );
			Logger::logDebug( StringStream() << cuT( "			Header ordinal: 0x" ) << std::hex << int( strOrdinal[0] ) );
		}

		while ( ok && p_subchunk.m_read < p_subchunk.m_size )
		{
			if ( doRead( cuT( "			" ), currentSubchunk ) )
			{
				switch ( currentSubchunk.m_id )
				{
				default:
					doDiscard( currentSubchunk );
					break;

				case eID_TAG_BLOK_CHAN:
					currentSubchunk.m_read += UI2( m_file->read( p_channel ) );
					bigEndianToSystemEndian( p_channel );
					Logger::logDebug( StringStream() << cuT( "				Channel: " ) << p_channel );
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
			doDiscard( p_subchunk );
		}

		usReturn += p_subchunk.m_read;
		return usReturn;
	}

	void LwoImporter::doReadTMap( stLWO_SUBCHUNK & p_subchunk )
	{
		stLWO_SUBCHUNK currentSubchunk;
		bool ok = true;

		while ( ok && p_subchunk.m_read < p_subchunk.m_size )
		{
			if ( doRead( cuT( "			" ), currentSubchunk ) )
			{
				//switch( currentSubchunk.m_id )
				//{
				//default:
				doDiscard( currentSubchunk );
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
			doDiscard( p_subchunk );
		}
	}

	void LwoImporter::doReadIMap( stLWO_SUBCHUNK & p_subchunk, eTEX_CHANNEL & p_channel )
	{
		stLWO_SUBCHUNK currentSubchunk;
		bool ok = true;

		while ( ok && p_subchunk.m_read < p_subchunk.m_size )
		{
			if ( doRead( cuT( "			" ), currentSubchunk ) )
			{
				switch ( currentSubchunk.m_id )
				{
				default:
					doDiscard( currentSubchunk );
					break;

				case eID_TAG_BLOK_CHAN:
					currentSubchunk.m_read += UI2( m_file->read( p_channel ) );
					bigEndianToSystemEndian( p_channel );
					Logger::logDebug( StringStream() << cuT( "				Channel: " ) << p_channel );
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
			doDiscard( p_subchunk );
		}
	}

	void LwoImporter::doReadProc( stLWO_SUBCHUNK & p_subchunk )
	{
		stLWO_SUBCHUNK currentSubchunk;
		bool ok = true;

		while ( ok && p_subchunk.m_read < p_subchunk.m_size )
		{
			if ( doRead( cuT( "			" ), currentSubchunk ) )
			{
				//switch( currentSubchunk.m_id )
				//{
				//default:
				doDiscard( currentSubchunk );
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
			doDiscard( p_subchunk );
		}
	}

	void LwoImporter::doReadGrad( stLWO_SUBCHUNK & p_subchunk )
	{
		stLWO_SUBCHUNK currentSubchunk;
		bool ok = true;

		while ( ok && p_subchunk.m_read < p_subchunk.m_size )
		{
			if ( doRead( cuT( "			" ), currentSubchunk ) )
			{
				//switch( currentSubchunk.m_id )
				//{
				//default:
				doDiscard( currentSubchunk );
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
			doDiscard( p_subchunk );
		}
	}

	void LwoImporter::doReadShdr( stLWO_SUBCHUNK & p_subchunk )
	{
		stLWO_SUBCHUNK currentSubchunk;
		bool ok = true;

		while ( ok && p_subchunk.m_read < p_subchunk.m_size )
		{
			if ( doRead( cuT( "			" ), currentSubchunk ) )
			{
				//switch( currentSubchunk.m_id )
				//{
				//default:
				doDiscard( currentSubchunk );
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
			doDiscard( p_subchunk );
		}
	}

	void LwoImporter::doSetChannel( TextureUnitSPtr p_pTexture, eTEX_CHANNEL p_channel )
	{
		if ( p_pTexture )
		{
			switch ( p_channel )
			{
			case eTEX_CHANNEL_COLR:
				p_pTexture->setChannel( TextureChannel::eDiffuse );
				break;

			case eTEX_CHANNEL_DIFF:
				p_pTexture->setChannel( TextureChannel::eDiffuse );
				break;

			case eTEX_CHANNEL_SPEC:
				p_pTexture->setChannel( TextureChannel::eSpecular );
				break;

			case eTEX_CHANNEL_GLOS:
				p_pTexture->setChannel( TextureChannel::eGloss );
				break;

			case eTEX_CHANNEL_TRAN:
				p_pTexture->setChannel( TextureChannel::eOpacity );
				break;

			case eTEX_CHANNEL_BUMP:
				p_pTexture->setChannel( TextureChannel::eNormal );
				break;
			}
		}
	}

	void LwoImporter::doReadBlock( stLWO_SUBCHUNK & p_subchunk, PassSPtr p_pass )
	{
		stLWO_SUBCHUNK currentSubchunk;
		stLWO_SUBCHUNK blockHeader;
		UI4 uiVx;
		eTEX_CHANNEL eChannel = eTEX_CHANNEL_COLR;
		bool ok = true;
		TextureUnitSPtr unit;
		ImageVxMap::iterator it;
		p_subchunk.m_read += doReadBlockHeader( blockHeader, eChannel );

		while ( ok && p_subchunk.m_read < p_subchunk.m_size )
		{
			if ( doRead( cuT( "		" ), currentSubchunk ) )
			{
				switch ( currentSubchunk.m_id )
				{
				case eID_TAG_BLOK_TMAP:
					doReadTMap( currentSubchunk );
					break;

				case eID_TAG_BLOK_IMAP:
					doReadIMap( currentSubchunk, eChannel );
					break;

				case eID_TAG_BLOK_PROC:
					doReadProc( currentSubchunk );
					break;

				case eID_TAG_BLOK_GRAD:
					doReadGrad( currentSubchunk );
					break;

				case eID_TAG_BLOK_SHDR:
					doReadShdr( currentSubchunk );
					break;

				case eID_TAG_BLOK_IMAG:
					currentSubchunk.m_read += doReadVX( uiVx );
					it = m_mapImages.find( uiVx );

					if ( it != m_mapImages.end() )
					{
						StringStream toLog( cuT( "			Texture found: " ) );
						Logger::logDebug( toLog << it->second->getPath().c_str() );
						unit = std::make_shared< TextureUnit >( *getEngine() );
						auto texture = getEngine()->getRenderSystem()->createTexture( TextureType::eTwoDimensions, AccessType::eNone, AccessType::eRead );
						texture->setSource( it->second->getPixels() );
						unit->setTexture( texture );
						doSetChannel( unit, eChannel );
						p_pass->addTextureUnit( unit );
					}
					else
					{
						unit.reset();
					}

					break;

				case eID_TAG_BLOK_CHAN:
					currentSubchunk.m_read += UI2( m_file->read( eChannel ) );
					bigEndianToSystemEndian( eChannel );
					Logger::logDebug( StringStream() << cuT( "			Channel: " ) << eChannel );
					doSetChannel( unit, eChannel );
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
					doDiscard( currentSubchunk );
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
			doDiscard( p_subchunk );
		}
	}

	void LwoImporter::doParseTags( stLWO_CHUNK & p_chunk )
	{
		std::string strTag;
		bool ok = m_file->isOk();

		while ( ok && p_chunk.m_read < p_chunk.m_size )
		{
			ok = doRead( strTag );

			if ( ok )
			{
				p_chunk.m_read += UI4( strTag.size() + 1 + ( 1 - strTag.size() % 2 ) );
				m_arrayTags.push_back( strTag );
				StringStream toLog( cuT( "\tName : " ) );
				Logger::logDebug( toLog << strTag.c_str() );
			}
		}

		if ( !ok )
		{
			doDiscard( p_chunk );
		}
	}

	void LwoImporter::doParseSurf( stLWO_CHUNK & p_chunk )
	{
		stLWO_SUBCHUNK currentSubchunk;
		stLWO_SUBCHUNK blockHeader;
		RgbColour clrBase;
		MaterialSPtr pMaterial;
		LegacyPassSPtr pass;
		UI4 uiVx;
		float fDiff = 1;
		float fSpec = 0;
		float fGlos = 0;
		UI2 usSide = 1;
		bool ok = doRead( m_strName );

		if ( ok )
		{
			p_chunk.m_read += UI4( m_strName.size() + 1 + ( 1 - m_strName.size() % 2 ) );
			ok = doRead( m_strSource );
		}

		if ( ok )
		{
			p_chunk.m_read += UI4( m_strSource.size() + 1 + ( 1 - m_strSource.size() % 2 ) );
			Logger::logDebug( cuT( "	Name : " ) + string::stringCast< xchar >( m_strName ) );
			Logger::logDebug( cuT( "	Source : " ) + string::stringCast< xchar >( m_strSource ) );
			pMaterial = getEngine()->getMaterialCache().add( string::stringCast< xchar >( m_strName ), MaterialType::eLegacy );
			REQUIRE( pMaterial->getType() == MaterialType::eLegacy );
			pass = pMaterial->getTypedPass< MaterialType::eLegacy >( 0u );
		}

		while ( ok && p_chunk.m_read < p_chunk.m_size )
		{
			if ( doRead( cuT( "	" ), currentSubchunk ) )
			{
				switch ( currentSubchunk.m_id )
				{
				case eID_TAG_SURF_COLR:
					currentSubchunk.m_read += UI2( m_file->readArray( clrBase.ptr(), 3 ) );
					bigEndianToSystemEndian( clrBase.ptr()[0] );
					bigEndianToSystemEndian( clrBase.ptr()[1] );
					bigEndianToSystemEndian( clrBase.ptr()[2] );
					currentSubchunk.m_read += doReadVX( uiVx );
					Logger::logDebug( StringStream() << cuT( "		Colour: " ) << clrBase.red().value() << cuT( ", " ) << clrBase.green().value() << cuT( ", " ) << clrBase.blue().value() << cuT( " - VX : 0x" ) << std::hex << uiVx );
					break;

				case eID_TAG_SURF_DIFF:
					currentSubchunk.m_read += UI2( m_file->read( fDiff ) );
					bigEndianToSystemEndian( fDiff );
					currentSubchunk.m_read += doReadVX( uiVx );
					Logger::logDebug( StringStream() << cuT( "		Diff. base level: " ) << fDiff << cuT( " - VX : 0x" ) << std::hex << uiVx );
					break;

				case eID_TAG_SURF_SPEC:
					currentSubchunk.m_read += UI2( m_file->read( fSpec ) );
					bigEndianToSystemEndian( fSpec );
					currentSubchunk.m_read += doReadVX( uiVx );
					Logger::logDebug( StringStream() << cuT( "		Spec. base level: " ) << fSpec << cuT( " - VX : 0x" ) << std::hex << uiVx );
					break;

				case eID_TAG_SURF_GLOS:
					currentSubchunk.m_read += UI2( m_file->read( fGlos ) );
					bigEndianToSystemEndian( fGlos );
					currentSubchunk.m_read += doReadVX( uiVx );
					Logger::logDebug( StringStream() << cuT( "		Glossiness: " ) << fGlos << cuT( " - VX : 0x" ) << std::hex << uiVx );
					break;

				case eID_TAG_SURF_SIDE:
					currentSubchunk.m_read += UI2( m_file->read( usSide ) );
					bigEndianToSystemEndian( usSide );
					Logger::logDebug( StringStream() << cuT( "		Sidedness: " ) << usSide );
					break;

				case eID_TAG_SURF_BLOK:
					doReadBlock( currentSubchunk, pass );
					break;

				case eID_TAG_SURF_LUMI:
				case eID_TAG_SURF_REFL:
				case eID_TAG_SURF_TRAN:
				case eID_TAG_SURF_TRNL:
				case eID_TAG_SURF_BUMP:
				default:
					doDiscard( currentSubchunk );
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
			pass->setDiffuse( clrBase * fDiff );
			pass->setSpecular( clrBase * fSpec );
			pass->setShininess( float( 2 ^ int( ( 10 * fGlos ) + 2 ) ) );
			pass->setTwoSided( usSide == 3 );
		}
		else
		{
			doDiscard( p_chunk );
		}
	}

	void LwoImporter::doParseClip( stLWO_CHUNK & p_chunk )
	{
		UI4	uiIndex;
		stLWO_SUBCHUNK currentSubchunk;
		bool ok = m_file->read( uiIndex ) == sizeof( UI4 );
		std::string strName;
		Path pathImage;
		ImageSPtr pImage;

		if ( ok )
		{
			bigEndianToSystemEndian( uiIndex );
			p_chunk.m_read += sizeof( UI4 );
			Logger::logDebug( StringStream() << cuT( "	Index: 0x" ) << std::hex << uiIndex );
		}

		while ( ok && p_chunk.m_read < p_chunk.m_size )
		{
			if ( doRead( cuT( "	" ), currentSubchunk ) )
			{
				switch ( currentSubchunk.m_id )
				{
				case eID_TAG_CLIP_STIL:
				{
					doRead( strName );
					currentSubchunk.m_read += UI2( strName.size() + 1 + ( 1 - strName.size() % 2 ) );
					Logger::logDebug( cuT( "		Image : " ) + string::stringCast< xchar >( strName ) );
					pathImage = Path{ string::stringCast< xchar >( strName ) };

					if ( !File::fileExists( pathImage ) )
					{
						pathImage = m_file->getFilePath() / pathImage;
					}

					if ( !File::fileExists( pathImage ) )
					{
						pathImage = m_file->getFilePath() / cuT( "Texture" ) / pathImage;
					}

					ImageSPtr pImage = getEngine()->getImageCache().add( string::stringCast< xchar >( strName ), pathImage );
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
					doDiscard( currentSubchunk );
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
			doDiscard( p_chunk );
		}
	}

	void LwoImporter::doParsePnts( stLWO_CHUNK & p_chunk )
	{
		if ( p_chunk.m_size > 0 && p_chunk.m_size % 3 == 0 )
		{
			F4 fCoords[3];
			UI4 uiCount = p_chunk.m_size / ( sizeof( F4 ) * 3 );
			Logger::logDebug( StringStream() << cuT( "\tCount: " ) << uiCount );
			m_arrayPoints.resize( uiCount );
			m_arrayUvs.resize( uiCount );

			for ( UI4 i = 0 ; i < uiCount ; i++ )
			{
				p_chunk.m_read += UI4( m_file->readArray( fCoords, 3 ) );
				bigEndianToSystemEndian( fCoords[0] );
				bigEndianToSystemEndian( fCoords[1] );
				bigEndianToSystemEndian( fCoords[2] );
				m_pSubmesh->addPoint( fCoords );
			}
		}
	}

	void LwoImporter::doParseVMap( stLWO_CHUNK & p_chunk )
	{
		eVMAP_TYPE eType = eVMAP_TYPE( 0 );
		UI2 usDimension;
		std::string strName;
		bool ok = m_file->isOk();

		if ( ok )
		{
			p_chunk.m_read += UI4( m_file->read( eType ) );
			ok = m_file->isOk();
		}

		if ( ok )
		{
			p_chunk.m_read += UI4( m_file->read( usDimension ) );
			ok = m_file->isOk();
		}

		if ( ok )
		{
			ok = doRead( strName );
		}

		if ( ok )
		{
			p_chunk.m_read += UI4( strName.size() + 1 + ( 1 - strName.size() % 2 ) );
			bigEndianToSystemEndian( eType );
			bigEndianToSystemEndian( usDimension );
		}

		if ( ok )
		{
			char szType[5];
			doToStr( szType, eType );
			Logger::logDebug( StringStream() << cuT( "\tType: " ) << szType );
			Logger::logDebug( StringStream() << cuT( "\tDimensions: " ) << usDimension );
			Logger::logDebug( StringStream() << cuT( "\tName: " ) << strName.c_str() );

			if ( eType == eVMAP_TYPE_TXUV )
			{
				F4	fUv[2];
				UI4 i = 0;

				while ( ok && p_chunk.m_read < p_chunk.m_size )
				{
					UI4 uiVx;
					p_chunk.m_read += doReadVX( uiVx );
					ok = m_file->isOk();

					if ( ok )
					{
						p_chunk.m_read += UI4( m_file->readArray( fUv, 2 ) );
						ok = m_file->isOk();
					}

					if ( ok )
					{
						bigEndianToSystemEndian( fUv[0] );
						bigEndianToSystemEndian( fUv[1] );
						Vertex::setTexCoord( m_pSubmesh->getPoint( uiVx ), fUv[0], fUv[1] );
						i++;
					}

					//Logger::logDebug( cuT( "\tVertex : %d - %f, %f" ), uiVx, fUv[0], fUv[1] );
				}

				m_bHasUv = true;
				Logger::logDebug( StringStream() << cuT( "\tUV count: " ) << i );
			}
			else
			{
				ok = false;
			}
		}

		if ( !ok )
		{
			doDiscard( p_chunk );
		}
	}

	void LwoImporter::doParsePols( stLWO_CHUNK & p_chunk )
	{
		std::vector< Lwo::Face > arrayFaces;
		std::set< UI4 > setPntsIds;
		ePOLS_TYPE eType;
		bool ok = m_file->isOk();
		UI4 uiCount = 0;

		if ( ok )
		{
			p_chunk.m_read += UI4( m_file->read( eType ) );
			ok = m_file->isOk();
		}

		if ( ok )
		{
			char szType[5];
			bigEndianToSystemEndian( eType );
			doToStr( szType, eType );
			StringStream toLog( cuT( "\tType : " ) );
			Logger::logDebug( toLog << szType );

			if ( eType == ePOLS_TYPE_FACE )
			{
				while ( ok && p_chunk.m_read < p_chunk.m_size )
				{
					UI2 usCountFlags = 0;
					p_chunk.m_read += UI4( m_file->read( usCountFlags ) );
					UI2 usCount = usCountFlags & 0xFFC0;
					bigEndianToSystemEndian( usCount );
					Lwo::Face face( usCount );
					ok = m_file->isOk();

					for ( UI2 i = 0 ; i < usCount && ok ; i++ )
					{
						p_chunk.m_read += doReadVX( face[i] );
						setPntsIds.insert( face[i] );
						ok = m_file->isOk();
					}

					if ( ok )
					{
						arrayFaces.push_back( face );
						uiCount++;
					}
				}

				Logger::logDebug( StringStream() << cuT( "\tFaces count: " ) << uiCount );
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
					m_pSubmesh->addPoint( m_arrayPoints[*it] );
					mapPntsIds.insert( std::make_pair( *it, uiCount ) );
				}

				for ( auto & face : arrayFaces )
				{
					m_pSubmesh->addFace( mapPntsIds[face[1]], mapPntsIds[face[0]], mapPntsIds[face[2]] );
				}
			}
		}
		else
		{
			doDiscard( p_chunk );
		}
	}

	void LwoImporter::doParseLayr( stLWO_CHUNK & p_chunk )
	{
		bool ok = m_file->isOk();
		UI2 usNumber;
		UI2 usFlags;
		Point3f ptPivot;
		std::string strName;
		UI2 usParent;

		if ( ok )
		{
			p_chunk.m_read += UI4( m_file->read( usNumber ) );
			ok = m_file->isOk();
		}

		if ( ok && p_chunk.m_read < p_chunk.m_size )
		{
			p_chunk.m_read += UI4( m_file->read( usFlags ) );
			ok = m_file->isOk();
		}

		if ( ok && p_chunk.m_read < p_chunk.m_size )
		{
			p_chunk.m_read += UI4( m_file->readArray( ptPivot.ptr(), 3 ) );
			ok = m_file->isOk();
		}

		if ( ok && p_chunk.m_read < p_chunk.m_size )
		{
			doRead( strName );
			ok = m_file->isOk();
		}

		if ( ok )
		{
			p_chunk.m_read += UI4( strName.size() + 1 + ( 1 - strName.size() % 2 ) );

			if ( p_chunk.m_read < p_chunk.m_size )
			{
				p_chunk.m_read += UI4( m_file->read( usParent ) );
				ok = m_file->isOk();
			}
		}

		if ( ok )
		{
			bigEndianToSystemEndian( usNumber );
			bigEndianToSystemEndian( usFlags );
			bigEndianToSystemEndian( ptPivot[0] );
			bigEndianToSystemEndian( ptPivot[1] );
			bigEndianToSystemEndian( ptPivot[2] );
			bigEndianToSystemEndian( usParent );
			StringStream toLog( cuT( "\tName   : " ) );
			Logger::logDebug( toLog << strName.c_str() );
			Logger::logDebug( StringStream() << cuT( "\tNumber: " ) << usNumber );
			Logger::logDebug( StringStream() << cuT( "\tFlags:  0x" ) << std::hex << usFlags );
			Logger::logDebug( StringStream() << cuT( "\tPivot:  " ) << ptPivot );
			Logger::logDebug( StringStream() << cuT( "\tParent: " ) << usParent );
			String strNameTmp = string::stringCast< xchar >( strName );
			string::toLowerCase( strNameTmp );

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
			doDiscard( p_chunk );
		}
	}
}
