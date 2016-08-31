#include "LwoImporter.hpp"
#include "LwoChunk.hpp"
#include "LwoSubChunk.hpp"

#include <Graphics/Colour.hpp>
#include <Graphics/Image.hpp>

#include <Engine.hpp>
#include <Material/Material.hpp>
#include <Mesh/Mesh.hpp>
#include <Scene/Geometry.hpp>
#include <Scene/Scene.hpp>
#include <Scene/SceneNode.hpp>

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
		auto l_mesh = p_scene.GetMeshCache().Add( cuT( "Mesh_LWO" ) );
		bool l_return = DoImportMesh( *l_mesh );

		if ( l_return )
		{
			SceneNodeSPtr l_node = p_scene.GetSceneNodeCache().Add( l_mesh->GetName(), p_scene.GetObjectRootNode() );
			GeometrySPtr l_geometry = p_scene.GetGeometryCache().Add( l_mesh->GetName(), l_node, nullptr );
			l_geometry->SetMesh( l_mesh );
			m_geometries.insert( { l_geometry->GetName(), l_geometry } );
		}

		return l_return;
	}

	bool LwoImporter::DoImportMesh( Mesh & p_mesh )
	{
		UIntArray l_faces;
		RealArray l_sizes;
		String l_nodeName = m_fileName.GetFileName();
		String l_meshName = m_fileName.GetFileName();
		String l_materialName = m_fileName.GetFileName();
		m_file = new BinaryFile( m_fileName, File::eOPEN_MODE_READ );
		stLWO_CHUNK l_currentChunk;
		bool l_return{ false };

		if ( m_file->IsOk() )
		{
			Logger::LogDebug( cuT( "**************************************************" ) );
			Logger::LogDebug( cuT( "Importing mesh from file : [" ) + m_fileName + cuT( "]" ) );
			DoRead( l_currentChunk );
			char l_name[5] = { 0, 0, 0, 0 , 0 };
			l_currentChunk.m_read += UI4( m_file->ReadArray( l_name, 4 ) );

			if ( l_currentChunk.m_id == eID_TAG_FORM && l_currentChunk.m_size == m_file->GetLength() - 8 && std::string( l_name ) == "LWO2" )
			{
				DoProcess( p_mesh, l_currentChunk );
				p_mesh.ComputeNormals();
			}

			Logger::LogDebug( cuT( "**************************************************" ) );
			l_return = true;
		}

		return l_return;
	}

	bool LwoImporter::DoRead( stLWO_CHUNK & p_chunk )
	{
		bool l_return = m_file->IsOk();

		if ( l_return )
		{
			p_chunk.m_read = 0;
			m_file->Read(	p_chunk.m_id );
			BigEndianToSystemEndian( p_chunk.m_id );
			l_return = DoIsTagId( p_chunk.m_id ) && m_file->IsOk();
			char l_id[5];
			StringStream l_toLog;
			DoToStr( l_id, p_chunk.m_id );

			if ( l_return )
			{
				m_file->Read(	p_chunk.m_size );
				BigEndianToSystemEndian( p_chunk.m_size );
				l_toLog << cuT( "Chunk : " ) << l_id << cuT( ", " ) << p_chunk.m_size;
				l_return = m_file->IsOk();
			}
			else
			{
				l_toLog << cuT( "Invalid chunk : " ) << l_id;
				m_file->Seek( -4, File::eOFFSET_MODE_CURRENT );
			}

			Logger::LogDebug( l_toLog );
		}

		return l_return;
	}

	void LwoImporter::DoProcess( Mesh & p_mesh, stLWO_CHUNK & p_chunk )
	{
		stLWO_CHUNK l_currentChunk;
		bool l_continue = m_file->IsOk();

		while ( p_chunk.m_read < p_chunk.m_size && l_continue )
		{
			if ( DoRead( l_currentChunk ) )
			{
				if ( !DoIsValidChunk( l_currentChunk, p_chunk ) )
				{
					l_continue = false;
				}
				else
				{
					switch ( l_currentChunk.m_id )
					{
					case eID_TAG_LAYR:
						DoParseLayr( l_currentChunk );
						break;

					default:
						if ( !m_bIgnored )
						{
							switch ( l_currentChunk.m_id )
							{
							case eID_TAG_LAYR:
							case eID_TAG_SURF:
								DoParseSurf( l_currentChunk );
								break;

							case eID_TAG_CLIP:
								DoParseClip( l_currentChunk );
								break;

							case eID_TAG_PNTS:
								m_pSubmesh = p_mesh.CreateSubmesh();
								DoParsePnts( l_currentChunk );
								break;

							case eID_TAG_VMAP:
								DoParseVMap( l_currentChunk );
								break;

							case eID_TAG_POLS:
								DoParsePols( l_currentChunk );
								break;

							case eID_TAG_PTAG:
								DoParsePTag( l_currentChunk );
								break;

							case eID_TAG_TAGS:
								DoParseTags( l_currentChunk );
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
								DoDiscard( l_currentChunk );
								break;
							}
						}
						else
						{
							DoDiscard( l_currentChunk );
						}
					}
				}

				p_chunk.m_read += l_currentChunk.m_read + sizeof( eID_TAG ) + sizeof( UI4 );
				l_continue = m_file->IsOk();
			}
			else
			{
				l_continue = false;
			}
		}

		if ( !l_continue )
		{
			DoDiscard( p_chunk );
		}

		for ( auto l_it : m_arraySubmeshByMatName )
		{
			auto l_material = p_mesh.GetScene()->GetMaterialView().Find( string::string_cast< xchar >( l_it.first ) );

			if ( l_material )
			{
				l_it.second->SetDefaultMaterial( l_material );
			}
		}
	}

	void LwoImporter::DoDiscard( stLWO_CHUNK & p_chunk )
	{
		if ( p_chunk.m_size > p_chunk.m_read )
		{
			UI4 l_size = p_chunk.m_size - p_chunk.m_read;
			std::size_t l_remaining = std::size_t( m_file->GetLength() - m_file->Tell() );

			try
			{
				if ( l_size <= l_remaining && l_size > 0 )
				{
					m_file->Seek( l_size, Castor::File::eOFFSET_MODE_CURRENT );
				}
				else
				{
					m_file->Seek( 0, Castor::File::eOFFSET_MODE_END );
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
			UI4 l_size = p_chunk.m_size - p_chunk.m_read;
			std::size_t l_remaining = std::size_t( m_file->GetLength() - m_file->Tell() );

			try
			{
				if ( l_size <= l_remaining && l_size > 0 )
				{
					m_file->Seek( l_size, Castor::File::eOFFSET_MODE_CURRENT );
				}
				else
				{
					m_file->Seek( 0, Castor::File::eOFFSET_MODE_END );
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
		bool l_return = DoIsTagId( p_chunk.m_id );

		if ( l_return )
		{
			l_return = p_chunk.m_size + p_parent.m_read <= p_parent.m_size;
		}

		return l_return;
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
		bool l_return = true;
		static char l_szTmp[1000];
		memset( l_szTmp, 0, 1000 );
		char l_cTmp = 1;
		std::size_t l_index = 0;

		while ( l_cTmp != 0 && m_file->IsOk() )
		{
			m_file->Read( l_cTmp );
			l_szTmp[l_index++] = l_cTmp;
		}

		if ( !m_file->IsOk() )
		{
			m_file->Seek( 1 - l_index, File::eOFFSET_MODE_CURRENT );
			l_return = false;
		}
		else
		{
			p_strString = l_szTmp;

			if ( l_return && p_strString.size() % 2 == 0 )
			{
				m_file->Seek( 1, File::eOFFSET_MODE_CURRENT );
			}
		}

		return l_return;
	}

	bool LwoImporter::DoRead( String const & p_tabs, stLWO_SUBCHUNK & p_subchunk )
	{
		bool l_return = m_file->IsOk();

		if ( l_return )
		{
			p_subchunk.m_read = 0;
			m_file->Read( p_subchunk.m_id );
			BigEndianToSystemEndian( p_subchunk.m_id );
			l_return = DoIsTagId( p_subchunk.m_id ) && m_file->IsOk();
			char l_id[5];
			StringStream l_toLog;
			DoToStr( l_id, p_subchunk.m_id );

			if ( l_return )
			{
				m_file->Read(	p_subchunk.m_size );
				BigEndianToSystemEndian( p_subchunk.m_size );
				l_toLog << p_tabs << cuT( "Subchunk : " ) << l_id << cuT( ", " ) << p_subchunk.m_size;
				l_return = m_file->IsOk();
			}
			else
			{
				l_toLog << p_tabs << cuT( "Invalid subchunk : " ) << l_id;
				m_file->Seek( -4, File::eOFFSET_MODE_CURRENT );
			}

			Logger::LogDebug( l_toLog );
		}

		return l_return;
	}

	UI2 LwoImporter::DoReadVX( UI4 & p_index )
	{
		UI2 l_return;
		p_index = 0;
		uint8_t l_byTest;
		m_file->Read( l_byTest );

		if ( l_byTest == 0xFF )
		{
			m_file->Read( p_index );
			BigEndianToSystemEndian( p_index );
			l_return = sizeof( UI4 );
		}
		else
		{
			m_file->Seek( -1, File::eOFFSET_MODE_CURRENT );
			UI2 l_ui2Index = 0;
			m_file->Read( l_ui2Index );
			BigEndianToSystemEndian( l_ui2Index );
			p_index = l_ui2Index;
			l_return = sizeof( UI2 );
		}

		return l_return;
	}

	bool LwoImporter::DoIsChunk( eID_TAG p_eId )
	{
		bool l_return = false;

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
			l_return = true;
			break;

		default:
			l_return = false;
			break;
		}

		return l_return;
	}

	bool LwoImporter::DoIsTagId( eID_TAG p_eId )
	{
		bool l_return = false;

		switch ( p_eId )
		{
		default:
			l_return = false;
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
			l_return = true;
			break;
		}

		return l_return;
	}

	void LwoImporter::DoParsePTag( stLWO_CHUNK & p_chunk )
	{
		ePTAG_TYPE l_eType;
		bool l_continue = m_file->IsOk();

		if ( l_continue )
		{
			p_chunk.m_read += UI4( m_file->Read( l_eType ) );
			l_continue = m_file->IsOk();
		}

		if ( l_continue )
		{
			char l_szType[5];
			BigEndianToSystemEndian( l_eType );
			DoToStr( l_szType, l_eType );
			Logger::LogDebug( std::stringstream() << "\tType: " << l_szType );

			if ( l_eType == ePTAG_TYPE_SURF )
			{
				while ( l_continue && p_chunk.m_read < p_chunk.m_size )
				{
					UI2 l_usTag;
					UI4 l_uiVx;
					p_chunk.m_read += DoReadVX( l_uiVx );
					l_continue = m_file->IsOk();

					if ( l_continue )
					{
						p_chunk.m_read += UI4( m_file->Read( l_usTag ) );
						l_continue = m_file->IsOk();
					}

					if ( l_continue )
					{
						BigEndianToSystemEndian( l_usTag );
						//Logger::LogDebug( cuT( "\tPOLS Index : %d" ), l_uiVx );
						//Logger::LogDebug( cuT( "\tTAGS Index : %d" ), l_usTag );

						if ( l_usTag < m_arrayTags.size() )
						{
							StringStream l_toLog( cuT( "\tTAG Name: " ) );
							Logger::LogDebug( l_toLog << m_arrayTags[l_usTag].c_str() );
							m_arraySubmeshByMatName.push_back( std::make_pair( m_arrayTags[l_usTag], m_pSubmesh ) );
						}
					}
				}
			}
			else
			{
				l_continue = false;
			}
		}

		if ( !l_continue )
		{
			DoDiscard( p_chunk );
		}
	}

	UI2 LwoImporter::DoReadBlockHeader( stLWO_SUBCHUNK & p_subchunk, eTEX_CHANNEL & p_channel )
	{
		std::string l_strOrdinal;
		stLWO_SUBCHUNK l_currentSubchunk;
		UI2 l_usReturn = 0;
		bool l_continue = DoRead( cuT( "		" ), p_subchunk );

		if ( l_continue )
		{
			l_usReturn += sizeof( eID_TAG ) + sizeof( UI2 );
			l_continue	= DoRead( l_strOrdinal );
		}

		if ( l_continue )
		{
			p_subchunk.m_read += UI2( l_strOrdinal.size() + 1 + ( 1 - l_strOrdinal.size() % 2 ) );
			Logger::LogDebug( StringStream() << cuT( "			Header ordinal: 0x" ) << std::hex << int( l_strOrdinal[0] ) );
		}

		while ( l_continue && p_subchunk.m_read < p_subchunk.m_size )
		{
			if ( DoRead( cuT( "			" ), l_currentSubchunk ) )
			{
				switch ( l_currentSubchunk.m_id )
				{
				default:
					DoDiscard( l_currentSubchunk );
					break;

				case eID_TAG_BLOK_CHAN:
					l_currentSubchunk.m_read += UI2( m_file->Read( p_channel ) );
					BigEndianToSystemEndian( p_channel );
					Logger::LogDebug( StringStream() << cuT( "				Channel: " ) << p_channel );
					break;
				}

				p_subchunk.m_read += l_currentSubchunk.m_read + sizeof( eID_TAG ) + sizeof( UI2 );
			}
			else
			{
				l_continue = false;
			}
		}

		if ( !l_continue )
		{
			DoDiscard( p_subchunk );
		}

		l_usReturn += p_subchunk.m_read;
		return l_usReturn;
	}

	void LwoImporter::DoReadTMap( stLWO_SUBCHUNK & p_subchunk )
	{
		stLWO_SUBCHUNK l_currentSubchunk;
		bool l_continue = true;

		while ( l_continue && p_subchunk.m_read < p_subchunk.m_size )
		{
			if ( DoRead( cuT( "			" ), l_currentSubchunk ) )
			{
				//switch( l_currentSubchunk.m_id )
				//{
				//default:
				DoDiscard( l_currentSubchunk );
				//	break;
				//}
				p_subchunk.m_read += l_currentSubchunk.m_read + sizeof( eID_TAG ) + sizeof( UI2 );
			}
			else
			{
				l_continue = false;
			}
		}

		if ( !l_continue )
		{
			DoDiscard( p_subchunk );
		}
	}

	void LwoImporter::DoReadIMap( stLWO_SUBCHUNK & p_subchunk, eTEX_CHANNEL & p_channel )
	{
		stLWO_SUBCHUNK l_currentSubchunk;
		bool l_continue = true;

		while ( l_continue && p_subchunk.m_read < p_subchunk.m_size )
		{
			if ( DoRead( cuT( "			" ), l_currentSubchunk ) )
			{
				switch ( l_currentSubchunk.m_id )
				{
				default:
					DoDiscard( l_currentSubchunk );
					break;

				case eID_TAG_BLOK_CHAN:
					l_currentSubchunk.m_read += UI2( m_file->Read( p_channel ) );
					BigEndianToSystemEndian( p_channel );
					Logger::LogDebug( StringStream() << cuT( "				Channel: " ) << p_channel );
					break;
				}

				p_subchunk.m_read += l_currentSubchunk.m_read + sizeof( eID_TAG ) + sizeof( UI2 );
			}
			else
			{
				l_continue = false;
			}
		}

		if ( !l_continue )
		{
			DoDiscard( p_subchunk );
		}
	}

	void LwoImporter::DoReadProc( stLWO_SUBCHUNK & p_subchunk )
	{
		stLWO_SUBCHUNK l_currentSubchunk;
		bool l_continue = true;

		while ( l_continue && p_subchunk.m_read < p_subchunk.m_size )
		{
			if ( DoRead( cuT( "			" ), l_currentSubchunk ) )
			{
				//switch( l_currentSubchunk.m_id )
				//{
				//default:
				DoDiscard( l_currentSubchunk );
				//	break;
				//}
				p_subchunk.m_read += l_currentSubchunk.m_read + sizeof( eID_TAG ) + sizeof( UI2 );
			}
			else
			{
				l_continue = false;
			}
		}

		if ( !l_continue )
		{
			DoDiscard( p_subchunk );
		}
	}

	void LwoImporter::DoReadGrad( stLWO_SUBCHUNK & p_subchunk )
	{
		stLWO_SUBCHUNK l_currentSubchunk;
		bool l_continue = true;

		while ( l_continue && p_subchunk.m_read < p_subchunk.m_size )
		{
			if ( DoRead( cuT( "			" ), l_currentSubchunk ) )
			{
				//switch( l_currentSubchunk.m_id )
				//{
				//default:
				DoDiscard( l_currentSubchunk );
				//	break;
				//}
				p_subchunk.m_read += l_currentSubchunk.m_read + sizeof( eID_TAG ) + sizeof( UI2 );
			}
			else
			{
				l_continue = false;
			}
		}

		if ( !l_continue )
		{
			DoDiscard( p_subchunk );
		}
	}

	void LwoImporter::DoReadShdr( stLWO_SUBCHUNK & p_subchunk )
	{
		stLWO_SUBCHUNK l_currentSubchunk;
		bool l_continue = true;

		while ( l_continue && p_subchunk.m_read < p_subchunk.m_size )
		{
			if ( DoRead( cuT( "			" ), l_currentSubchunk ) )
			{
				//switch( l_currentSubchunk.m_id )
				//{
				//default:
				DoDiscard( l_currentSubchunk );
				//	break;
				//}
				p_subchunk.m_read += l_currentSubchunk.m_read + sizeof( eID_TAG ) + sizeof( UI2 );
			}
			else
			{
				l_continue = false;
			}
		}

		if ( !l_continue )
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
				p_pTexture->SetChannel( TextureChannel::Diffuse );
				break;

			case eTEX_CHANNEL_DIFF:
				p_pTexture->SetChannel( TextureChannel::Diffuse );
				break;

			case eTEX_CHANNEL_SPEC:
				p_pTexture->SetChannel( TextureChannel::Specular );
				break;

			case eTEX_CHANNEL_GLOS:
				p_pTexture->SetChannel( TextureChannel::Gloss );
				break;

			case eTEX_CHANNEL_TRAN:
				p_pTexture->SetChannel( TextureChannel::Opacity );
				break;

			case eTEX_CHANNEL_BUMP:
				p_pTexture->SetChannel( TextureChannel::Normal );
				break;
			}
		}
	}

	void LwoImporter::DoReadBlock( stLWO_SUBCHUNK & p_subchunk, PassSPtr p_pass )
	{
		stLWO_SUBCHUNK l_currentSubchunk;
		stLWO_SUBCHUNK l_blockHeader;
		UI4 l_uiVx;
		eTEX_CHANNEL l_eChannel = eTEX_CHANNEL_COLR;
		bool l_continue = true;
		TextureUnitSPtr l_unit;
		ImageVxMap::iterator l_it;
		p_subchunk.m_read += DoReadBlockHeader( l_blockHeader, l_eChannel );

		while ( l_continue && p_subchunk.m_read < p_subchunk.m_size )
		{
			if ( DoRead( cuT( "		" ), l_currentSubchunk ) )
			{
				switch ( l_currentSubchunk.m_id )
				{
				case eID_TAG_BLOK_TMAP:
					DoReadTMap( l_currentSubchunk );
					break;

				case eID_TAG_BLOK_IMAP:
					DoReadIMap( l_currentSubchunk, l_eChannel );
					break;

				case eID_TAG_BLOK_PROC:
					DoReadProc( l_currentSubchunk );
					break;

				case eID_TAG_BLOK_GRAD:
					DoReadGrad( l_currentSubchunk );
					break;

				case eID_TAG_BLOK_SHDR:
					DoReadShdr( l_currentSubchunk );
					break;

				case eID_TAG_BLOK_IMAG:
					l_currentSubchunk.m_read += DoReadVX( l_uiVx );
					l_it = m_mapImages.find( l_uiVx );

					if ( l_it != m_mapImages.end() )
					{
						StringStream l_toLog( cuT( "			Texture found: " ) );
						Logger::LogDebug( l_toLog << l_it->second->GetPath().c_str() );
						l_unit = std::make_shared< TextureUnit >( *p_pass->GetEngine() );
						auto l_texture = GetEngine()->GetRenderSystem()->CreateTexture( TextureType::TwoDimensions, AccessType::None, AccessType::Read );
						l_texture->GetImage().SetSource( l_it->second->GetPixels() );
						l_unit->SetTexture( l_texture );
						DoSetChannel( l_unit, l_eChannel );
						p_pass->AddTextureUnit( l_unit );
					}
					else
					{
						l_unit.reset();
					}

					break;

				case eID_TAG_BLOK_CHAN:
					l_currentSubchunk.m_read += UI2( m_file->Read( l_eChannel ) );
					BigEndianToSystemEndian( l_eChannel );
					Logger::LogDebug( StringStream() << cuT( "			Channel: " ) << l_eChannel );
					DoSetChannel( l_unit, l_eChannel );
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
					DoDiscard( l_currentSubchunk );
					break;
				}

				p_subchunk.m_read += l_currentSubchunk.m_read + sizeof( eID_TAG ) + sizeof( UI2 );
			}
			else
			{
				l_continue = false;
			}
		}

		if ( !l_continue )
		{
			DoDiscard( p_subchunk );
		}
	}

	void LwoImporter::DoParseTags( stLWO_CHUNK & p_chunk )
	{
		std::string l_strTag;
		bool l_continue = m_file->IsOk();

		while ( l_continue && p_chunk.m_read < p_chunk.m_size )
		{
			l_continue = DoRead( l_strTag );

			if ( l_continue )
			{
				p_chunk.m_read += UI4( l_strTag.size() + 1 + ( 1 - l_strTag.size() % 2 ) );
				m_arrayTags.push_back( l_strTag );
				StringStream l_toLog( cuT( "\tName : " ) );
				Logger::LogDebug( l_toLog << l_strTag.c_str() );
			}
		}

		if ( !l_continue )
		{
			DoDiscard( p_chunk );
		}
	}

	void LwoImporter::DoParseSurf( stLWO_CHUNK & p_chunk )
	{
		stLWO_SUBCHUNK l_currentSubchunk;
		stLWO_SUBCHUNK l_blockHeader;
		Colour l_clrBase;
		MaterialSPtr l_pMaterial;
		PassSPtr l_pass;
		UI4 l_uiVx;
		float l_fDiff = 1;
		float l_fSpec = 0;
		float l_fGlos = 0;
		UI2 l_usSide = 1;
		bool l_continue = DoRead( m_strName );

		if ( l_continue )
		{
			p_chunk.m_read += UI4( m_strName.size() + 1 + ( 1 - m_strName.size() % 2 ) );
			l_continue = DoRead( m_strSource );
		}

		if ( l_continue )
		{
			p_chunk.m_read += UI4( m_strSource.size() + 1 + ( 1 - m_strSource.size() % 2 ) );
			Logger::LogDebug( cuT( "	Name : " ) + string::string_cast< xchar >( m_strName ) );
			Logger::LogDebug( cuT( "	Source : " ) + string::string_cast< xchar >( m_strSource ) );
			l_pMaterial = GetEngine()->GetMaterialCache().Add( string::string_cast< xchar >( m_strName ) );
			l_pass = l_pMaterial->GetPass( 0 );
		}

		while ( l_continue && p_chunk.m_read < p_chunk.m_size )
		{
			if ( DoRead( cuT( "	" ), l_currentSubchunk ) )
			{
				switch ( l_currentSubchunk.m_id )
				{
				case eID_TAG_SURF_COLR:
					l_currentSubchunk.m_read += UI2( m_file->ReadArray( l_clrBase.ptr(), 3 ) );
					BigEndianToSystemEndian( l_clrBase.ptr()[0] );
					BigEndianToSystemEndian( l_clrBase.ptr()[1] );
					BigEndianToSystemEndian( l_clrBase.ptr()[2] );
					l_clrBase.alpha() = 1.0f;
					l_currentSubchunk.m_read += DoReadVX( l_uiVx );
					Logger::LogDebug( StringStream() << cuT( "		Colour: " ) << l_clrBase.red().value() << cuT( ", " ) << l_clrBase.green().value() << cuT( ", " ) << l_clrBase.blue().value() << cuT( " - VX : 0x" ) << std::hex << l_uiVx );
					break;

				case eID_TAG_SURF_DIFF:
					l_currentSubchunk.m_read += UI2( m_file->Read( l_fDiff ) );
					BigEndianToSystemEndian( l_fDiff );
					l_currentSubchunk.m_read += DoReadVX( l_uiVx );
					Logger::LogDebug( StringStream() << cuT( "		Diff. base level: " ) << l_fDiff << cuT( " - VX : 0x" ) << std::hex << l_uiVx );
					break;

				case eID_TAG_SURF_SPEC:
					l_currentSubchunk.m_read += UI2( m_file->Read( l_fSpec ) );
					BigEndianToSystemEndian( l_fSpec );
					l_currentSubchunk.m_read += DoReadVX( l_uiVx );
					Logger::LogDebug( StringStream() << cuT( "		Spec. base level: " ) << l_fSpec << cuT( " - VX : 0x" ) << std::hex << l_uiVx );
					break;

				case eID_TAG_SURF_GLOS:
					l_currentSubchunk.m_read += UI2( m_file->Read( l_fGlos ) );
					BigEndianToSystemEndian( l_fGlos );
					l_currentSubchunk.m_read += DoReadVX( l_uiVx );
					Logger::LogDebug( StringStream() << cuT( "		Glossiness: " ) << l_fGlos << cuT( " - VX : 0x" ) << std::hex << l_uiVx );
					break;

				case eID_TAG_SURF_SIDE:
					l_currentSubchunk.m_read += UI2( m_file->Read( l_usSide ) );
					BigEndianToSystemEndian( l_usSide );
					Logger::LogDebug( StringStream() << cuT( "		Sidedness: " ) << l_usSide );
					break;

				case eID_TAG_SURF_BLOK:
					DoReadBlock( l_currentSubchunk, l_pass );
					break;

				case eID_TAG_SURF_LUMI:
				case eID_TAG_SURF_REFL:
				case eID_TAG_SURF_TRAN:
				case eID_TAG_SURF_TRNL:
				case eID_TAG_SURF_BUMP:
				default:
					DoDiscard( l_currentSubchunk );
					break;
				}

				p_chunk.m_read += l_currentSubchunk.m_read + sizeof( eID_TAG ) + sizeof( UI2 );
			}
			else
			{
				l_continue = false;
			}
		}

		if ( l_continue )
		{
			l_pass->SetDiffuse( l_clrBase * l_fDiff );
			l_pass->SetSpecular( l_clrBase * l_fSpec );
			l_pass->SetShininess( float( 2 ^ int( ( 10 * l_fGlos ) + 2 ) ) );
			l_pass->SetTwoSided( l_usSide == 3 );
		}
		else
		{
			DoDiscard( p_chunk );
		}
	}

	void LwoImporter::DoParseClip( stLWO_CHUNK & p_chunk )
	{
		UI4	l_uiIndex;
		stLWO_SUBCHUNK l_currentSubchunk;
		bool l_continue = m_file->Read( l_uiIndex ) == sizeof( UI4 );
		std::string l_strName;
		Path l_pathImage;
		ImageSPtr l_pImage;

		if ( l_continue )
		{
			BigEndianToSystemEndian( l_uiIndex );
			p_chunk.m_read += sizeof( UI4 );
			Logger::LogDebug( StringStream() << cuT( "	Index: 0x" ) << std::hex << l_uiIndex );
		}

		while ( l_continue && p_chunk.m_read < p_chunk.m_size )
		{
			if ( DoRead( cuT( "	" ), l_currentSubchunk ) )
			{
				switch ( l_currentSubchunk.m_id )
				{
				case eID_TAG_CLIP_STIL:
				{
					DoRead( l_strName );
					l_currentSubchunk.m_read += UI2( l_strName.size() + 1 + ( 1 - l_strName.size() % 2 ) );
					Logger::LogDebug( cuT( "		Image : " ) + string::string_cast< xchar >( l_strName ) );
					l_pathImage = Path{ string::string_cast< xchar >( l_strName ) };

					if ( !File::FileExists( l_pathImage ) )
					{
						l_pathImage = m_file->GetFilePath() / l_pathImage;
					}

					if ( !File::FileExists( l_pathImage ) )
					{
						l_pathImage = m_file->GetFilePath() / cuT( "Texture" ) / l_pathImage;
					}

					ImageSPtr l_pImage = GetEngine()->GetImageCache().Add( string::string_cast< xchar >( l_strName ), l_pathImage );
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
					DoDiscard( l_currentSubchunk );
					break;
				}

				p_chunk.m_read += l_currentSubchunk.m_read + sizeof( eID_TAG ) + sizeof( UI2 );
			}
			else
			{
				l_continue = false;
			}
		}

		if ( !l_continue )
		{
			DoDiscard( p_chunk );
		}
	}

	void LwoImporter::DoParsePnts( stLWO_CHUNK & p_chunk )
	{
		if ( p_chunk.m_size > 0 && p_chunk.m_size % 3 == 0 )
		{
			F4 l_fCoords[3];
			UI4 l_uiCount = p_chunk.m_size / ( sizeof( F4 ) * 3 );
			Logger::LogDebug( StringStream() << cuT( "\tCount: " ) << l_uiCount );
			m_arrayPoints.resize( l_uiCount );
			m_arrayUvs.resize( l_uiCount );

			for ( UI4 i = 0 ; i < l_uiCount ; i++ )
			{
				p_chunk.m_read += UI4( m_file->ReadArray( l_fCoords, 3 ) );
				BigEndianToSystemEndian( l_fCoords[0] );
				BigEndianToSystemEndian( l_fCoords[1] );
				BigEndianToSystemEndian( l_fCoords[2] );
				m_pSubmesh->AddPoint( l_fCoords );
			}
		}
	}

	void LwoImporter::DoParseVMap( stLWO_CHUNK & p_chunk )
	{
		eVMAP_TYPE l_eType = eVMAP_TYPE( 0 );
		UI2 l_usDimension;
		std::string l_strName;
		bool l_continue = m_file->IsOk();

		if ( l_continue )
		{
			p_chunk.m_read += UI4( m_file->Read( l_eType ) );
			l_continue = m_file->IsOk();
		}

		if ( l_continue )
		{
			p_chunk.m_read += UI4( m_file->Read( l_usDimension ) );
			l_continue = m_file->IsOk();
		}

		if ( l_continue )
		{
			l_continue = DoRead( l_strName );
		}

		if ( l_continue )
		{
			p_chunk.m_read += UI4( l_strName.size() + 1 + ( 1 - l_strName.size() % 2 ) );
			BigEndianToSystemEndian( l_eType );
			BigEndianToSystemEndian( l_usDimension );
		}

		if ( l_continue )
		{
			char l_szType[5];
			DoToStr( l_szType, l_eType );
			Logger::LogDebug( StringStream() << cuT( "\tType: " ) << l_szType );
			Logger::LogDebug( StringStream() << cuT( "\tDimensions: " ) << l_usDimension );
			Logger::LogDebug( StringStream() << cuT( "\tName: " ) << l_strName.c_str() );

			if ( l_eType == eVMAP_TYPE_TXUV )
			{
				F4	l_fUv[2];
				UI4 i = 0;

				while ( l_continue && p_chunk.m_read < p_chunk.m_size )
				{
					UI4 l_uiVx;
					p_chunk.m_read += DoReadVX( l_uiVx );
					l_continue = m_file->IsOk();

					if ( l_continue )
					{
						p_chunk.m_read += UI4( m_file->ReadArray( l_fUv, 2 ) );
						l_continue = m_file->IsOk();
					}

					if ( l_continue )
					{
						BigEndianToSystemEndian( l_fUv[0] );
						BigEndianToSystemEndian( l_fUv[1] );
						Vertex::SetTexCoord( m_pSubmesh->GetPoint( l_uiVx ), l_fUv[0], l_fUv[1] );
						i++;
					}

					//Logger::LogDebug( cuT( "\tVertex : %d - %f, %f" ), l_uiVx, l_fUv[0], l_fUv[1] );
				}

				m_bHasUv = true;
				Logger::LogDebug( StringStream() << cuT( "\tUV count: " ) << i );
			}
			else
			{
				l_continue = false;
			}
		}

		if ( !l_continue )
		{
			DoDiscard( p_chunk );
		}
	}

	void LwoImporter::DoParsePols( stLWO_CHUNK & p_chunk )
	{
		std::vector< Lwo::Face > l_arrayFaces;
		std::set< UI4 > l_setPntsIds;
		ePOLS_TYPE l_eType;
		bool l_continue = m_file->IsOk();
		UI4 l_uiCount = 0;

		if ( l_continue )
		{
			p_chunk.m_read += UI4( m_file->Read( l_eType ) );
			l_continue = m_file->IsOk();
		}

		if ( l_continue )
		{
			char l_szType[5];
			BigEndianToSystemEndian( l_eType );
			DoToStr( l_szType, l_eType );
			StringStream l_toLog( cuT( "\tType : " ) );
			Logger::LogDebug( l_toLog << l_szType );

			if ( l_eType == ePOLS_TYPE_FACE )
			{
				while ( l_continue && p_chunk.m_read < p_chunk.m_size )
				{
					UI2 l_usCountFlags = 0;
					p_chunk.m_read += UI4( m_file->Read( l_usCountFlags ) );
					UI2 l_usCount = l_usCountFlags & 0xFFC0;
					BigEndianToSystemEndian( l_usCount );
					Lwo::Face l_face( l_usCount );
					l_continue = m_file->IsOk();

					for ( UI2 i = 0 ; i < l_usCount && l_continue ; i++ )
					{
						p_chunk.m_read += DoReadVX( l_face[i] );
						l_setPntsIds.insert( l_face[i] );
						l_continue = m_file->IsOk();
					}

					if ( l_continue )
					{
						l_arrayFaces.push_back( l_face );
						l_uiCount++;
					}
				}

				Logger::LogDebug( StringStream() << cuT( "\tFaces count: " ) << l_uiCount );
			}
			else
			{
				l_continue = false;
			}
		}

		if ( l_continue )
		{
			if ( m_pSubmesh )
			{
				l_uiCount = 0;
				std::map< UI4, UI4 > l_mapPntsIds;
				FaceSPtr l_pFace;

				for ( std::set< UI4 >::iterator l_it = l_setPntsIds.begin() ; l_it != l_setPntsIds.end() ; ++l_it, ++l_uiCount )
				{
					m_pSubmesh->AddPoint( m_arrayPoints[*l_it] );
					l_mapPntsIds.insert( std::make_pair( *l_it, l_uiCount ) );
				}

				for ( std::vector< Lwo::Face >::iterator l_it = l_arrayFaces.begin() ; l_it != l_arrayFaces.end() ; ++l_it )
				{
					m_pSubmesh->AddFace( l_mapPntsIds[l_it->at( 1 )], l_mapPntsIds[l_it->at( 0 )], l_mapPntsIds[l_it->at( 2 )] );
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
		bool l_continue = m_file->IsOk();
		UI2 l_usNumber;
		UI2 l_usFlags;
		Point3f l_ptPivot;
		std::string l_strName;
		UI2 l_usParent;

		if ( l_continue )
		{
			p_chunk.m_read += UI4( m_file->Read( l_usNumber ) );
			l_continue = m_file->IsOk();
		}

		if ( l_continue && p_chunk.m_read < p_chunk.m_size )
		{
			p_chunk.m_read += UI4( m_file->Read( l_usFlags ) );
			l_continue = m_file->IsOk();
		}

		if ( l_continue && p_chunk.m_read < p_chunk.m_size )
		{
			p_chunk.m_read += UI4( m_file->ReadArray( l_ptPivot.ptr(), 3 ) );
			l_continue = m_file->IsOk();
		}

		if ( l_continue && p_chunk.m_read < p_chunk.m_size )
		{
			DoRead( l_strName );
			l_continue = m_file->IsOk();
		}

		if ( l_continue )
		{
			p_chunk.m_read += UI4( l_strName.size() + 1 + ( 1 - l_strName.size() % 2 ) );

			if ( p_chunk.m_read < p_chunk.m_size )
			{
				p_chunk.m_read += UI4( m_file->Read( l_usParent ) );
				l_continue = m_file->IsOk();
			}
		}

		if ( l_continue )
		{
			BigEndianToSystemEndian( l_usNumber );
			BigEndianToSystemEndian( l_usFlags );
			BigEndianToSystemEndian( l_ptPivot[0] );
			BigEndianToSystemEndian( l_ptPivot[1] );
			BigEndianToSystemEndian( l_ptPivot[2] );
			BigEndianToSystemEndian( l_usParent );
			StringStream l_toLog( cuT( "\tName   : " ) );
			Logger::LogDebug( l_toLog << l_strName.c_str() );
			Logger::LogDebug( StringStream() << cuT( "\tNumber: " ) << l_usNumber );
			Logger::LogDebug( StringStream() << cuT( "\tFlags:  0x" ) << std::hex << l_usFlags );
			Logger::LogDebug( StringStream() << cuT( "\tPivot:  " ) << l_ptPivot );
			Logger::LogDebug( StringStream() << cuT( "\tParent: " ) << l_usParent );
			String l_strNameTmp = string::string_cast< xchar >( l_strName );
			string::to_lower_case( l_strNameTmp );

			if ( l_usFlags & 0x01 || l_strNameTmp.find( cuT( "bip" ) ) == 0 || l_strNameTmp.find( cuT( "fcfx" ) ) == 0 || l_strNameTmp.find( cuT( "gundummy" ) ) == 0 || l_strNameTmp.find( cuT( "flap_bone" ) ) == 0 )
			{
				m_bIgnored = true;
			}
			else
			{
				m_bIgnored = false;
			}
		}

		if ( !l_continue )
		{
			DoDiscard( p_chunk );
		}
	}
}
