#include "BillboardList.hpp"
#include "PointFrameVariable.hpp"
#include "Buffer.hpp"
#include "RenderSystem.hpp"
#include "Scene.hpp"
#include "SceneNode.hpp"
#include "ShaderProgram.hpp"
#include "ShaderObject.hpp"
#include "Material.hpp"
#include "MaterialManager.hpp"
#include "Pass.hpp"
#include "PassRenderer.hpp"
#include "Pipeline.hpp"
#include "ShaderManager.hpp"

using namespace Castor;

namespace Castor3D
{
	bool BillboardList::TextLoader::operator()( BillboardList const & p_obj, Castor::TextFile & p_file )
	{
		bool l_bReturn = p_file.WriteText( cuT( "\tbillboard \"" ) + p_obj.GetName() + cuT( "\"\n\t{\n" ) ) > 0;

		if ( l_bReturn )
		{
			l_bReturn = MovableObject::TextLoader()( p_obj, p_file );
		}

		if ( l_bReturn )
		{
			l_bReturn = p_file.WriteText( cuT( "\t\tmaterial \"" ) + p_obj.GetMaterial()->GetName() + cuT( "\"\n" ) ) > 0;
		}

		if ( l_bReturn )
		{
			l_bReturn = p_file.Print( 256, cuT( "\t\tdimensions %d %d" ), p_obj.GetDimensions().width(), p_obj.GetDimensions().height() ) > 0;
		}

		if ( l_bReturn && p_obj.GetCount() )
		{
			l_bReturn = p_file.WriteText( cuT( "\t\tpositions\n\t\t{\n" ) ) > 0;

			for ( Point3rArray::const_iterator l_it = p_obj.Begin(); l_it != p_obj.End(); ++l_it )
			{
				l_bReturn = p_file.Print( 256, cuT( "\t\t\tpos %f %f %f" ), l_it->at( 0 ), l_it->at( 1 ), l_it->at( 2 ) ) > 0;
			}

			l_bReturn = p_file.WriteText( cuT( "\t\t}\n" ) ) > 0;
		}

		if ( l_bReturn )
		{
			l_bReturn = p_file.WriteText( cuT( "\t}\n" ) ) > 0;
		}

		return l_bReturn;
	}

	//*************************************************************************************************

	BillboardList::BinaryParser::BinaryParser( Path const & p_path )
		:	MovableObject::BinaryParser( p_path )
	{
	}

	bool BillboardList::BinaryParser::Fill( BillboardList const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_BILLBOARD );

		if ( l_bReturn )
		{
			l_bReturn = MovableObject::BinaryParser( m_path ).Fill( p_obj, l_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetMaterial()->GetName(), eCHUNK_TYPE_BILLBOARD_MATERIAL, l_chunk );
		}

		if ( l_bReturn )
		{
			l_bReturn = DoFillChunk( p_obj.GetDimensions(), eCHUNK_TYPE_BILLBOARD_DIMENSIONS, l_chunk );
		}

		if ( l_bReturn && p_obj.GetCount() )
		{
			for ( Point3rArray::const_iterator l_it = p_obj.Begin(); l_it != p_obj.End(); ++l_it )
			{
				l_bReturn = DoFillChunk( *l_it, eCHUNK_TYPE_BILLBOARD_POSITION, l_chunk );
			}
		}

		return l_bReturn;
	}

	bool BillboardList::BinaryParser::Parse( BillboardList & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_bReturn = true;
		String l_name;
		Size l_size;
		Point3r l_position;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_bReturn = p_chunk.GetSubChunk( l_chunk );

			if ( l_bReturn )
			{
				switch ( l_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_BILLBOARD_MATERIAL:
					l_bReturn = DoParseChunk( l_name, l_chunk );

					if ( l_bReturn )
					{
						p_obj.SetMaterial( p_obj.GetScene()->GetEngine()->GetMaterialManager().find( l_name ) );
					}

					break;

				case eCHUNK_TYPE_BILLBOARD_DIMENSIONS:
					l_bReturn = DoParseChunk( l_size, l_chunk );
					p_obj.SetDimensions( l_size );
					break;

				case eCHUNK_TYPE_BILLBOARD_POSITION:
					l_bReturn = DoParseChunk( l_position, l_chunk );
					p_obj.AddPoint( l_position );
					break;

				default:
					l_bReturn = MovableObject::BinaryParser( m_path ).Parse( p_obj, l_chunk );
					break;
				}
			}

			if ( !l_bReturn )
			{
				p_chunk.EndParse();
			}
		}

		return l_bReturn;
	}

	//*************************************************************************************************

	BillboardList::BillboardList( SceneSPtr p_pScene, RenderSystem * p_pRenderSystem )
		:	MovableObject( p_pScene, eMOVABLE_TYPE_BILLBOARD )
		,	m_pRenderSystem( p_pRenderSystem )
		,	m_bNeedUpdate( false )
	{
		BufferElementDeclaration l_vertexDeclarationElements[] = { BufferElementDeclaration( 0, eELEMENT_USAGE_POSITION, eELEMENT_TYPE_3FLOATS ) };
		m_pDeclaration = std::make_shared< BufferDeclaration >( l_vertexDeclarationElements );
	}

	BillboardList::~BillboardList()
	{
	}

	bool BillboardList::Initialise()
	{
		VertexBufferUPtr l_pVtxBuffer;
		l_pVtxBuffer = std::make_unique< VertexBuffer >( m_pRenderSystem, &( *m_pDeclaration )[0], m_pDeclaration->Size() );
		uint32_t l_uiStride = m_pDeclaration->GetStride();
		l_pVtxBuffer->Resize( uint32_t( m_arrayPositions.size() * l_uiStride ) );
		uint8_t * l_pBuffer = l_pVtxBuffer->data();

		for ( Point3rArrayConstIt l_it = m_arrayPositions.begin(); l_it != m_arrayPositions.end(); ++l_it )
		{
			std::memcpy( l_pBuffer, l_it->const_ptr(), l_uiStride );
			l_pBuffer += l_uiStride;
		}

		m_pGeometryBuffers = m_pRenderSystem->CreateGeometryBuffers( std::move( l_pVtxBuffer ), nullptr, nullptr );
		MaterialSPtr l_pMaterial = m_wpMaterial.lock();
		bool l_bReturn = false;

		if ( l_pMaterial && l_pMaterial->GetPassCount() )
		{
			l_pMaterial->Cleanup();
			ShaderProgramBaseSPtr l_pProgram = DoGetProgram( l_pMaterial->GetPass( 0 )->GetTextureFlags() );
			m_wpProgram = l_pProgram;
			l_bReturn = DoInitialise();

			if ( l_bReturn )
			{
				l_pMaterial->Initialise();
				m_pDimensionsUniform->SetValue( Point2i( m_dimensions.width(), m_dimensions.height() ) );
				m_pGeometryBuffers->GetVertexBuffer().Create();
				m_pGeometryBuffers->GetVertexBuffer().Initialise( eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW, l_pProgram );
				m_pGeometryBuffers->Initialise();
				m_bNeedUpdate = false;
			}
		}

		return l_bReturn;
	}

	void BillboardList::Cleanup()
	{
		m_pDimensionsUniform.reset();
		ShaderProgramBaseSPtr l_pProgram = m_wpProgram.lock();
		l_pProgram->Cleanup();
		m_pGeometryBuffers->GetVertexBuffer().Cleanup();
		m_pGeometryBuffers->Cleanup();
		m_pGeometryBuffers.reset();
	}

	void BillboardList::RemovePoint( uint32_t p_uiIndex )
	{
		if ( p_uiIndex < m_arrayPositions.size() )
		{
			m_arrayPositions.erase( m_arrayPositions.begin() + p_uiIndex );
			m_bNeedUpdate = true;
		}
	}

	void BillboardList::AddPoint( Castor::Point3r const & p_ptPosition )
	{
		m_arrayPositions.push_back( p_ptPosition );
		m_bNeedUpdate = true;
	}

	void BillboardList::AddPoints( Castor::Point3rArray const & p_ptPositions )
	{
		m_arrayPositions.insert( m_arrayPositions.end(), p_ptPositions.begin(), p_ptPositions.end() );
		m_bNeedUpdate = true;
	}

	void BillboardList::Render()
	{
		if ( !m_bNeedUpdate )
		{
			Pipeline * l_pPipeline = m_pRenderSystem->GetPipeline();
			ShaderProgramBaseSPtr l_pProgram = m_wpProgram.lock();
			MaterialSPtr l_pMaterial = m_wpMaterial.lock();
			VertexBuffer & l_vtxBuffer = m_pGeometryBuffers->GetVertexBuffer();
			uint32_t l_uiSize = l_vtxBuffer.GetSize() / l_vtxBuffer.GetDeclaration().GetStride();

			if ( l_pProgram && l_pMaterial )
			{
				uint8_t l_index = 0;
				uint8_t l_count = l_pMaterial->GetPassCount();

				for ( PassPtrArrayConstIt l_it = l_pMaterial->Begin(); l_it != l_pMaterial->End(); ++l_it )
				{
					PassSPtr l_pass = *l_it;
					l_pass->BindToProgram( l_pProgram );
					auto l_matrixBuffer = l_pass->GetMatrixBuffer();

					if ( l_matrixBuffer )
					{
						l_pPipeline->ApplyMatrices( *l_matrixBuffer );
					}

					l_pass->Render( l_index++, l_count );
					m_pGeometryBuffers->Draw( eTOPOLOGY_POINTS, l_pProgram, l_uiSize, 0 );
					l_pass->EndRender();
				}
			}
		}
	}

	void BillboardList::SetMaterial( MaterialSPtr p_pMaterial )
	{
		m_wpMaterial = p_pMaterial;
	}

	void BillboardList::SetDimensions( Size const & p_dimensions )
	{
		m_dimensions = p_dimensions;

		if ( m_pDimensionsUniform )
		{
			m_pDimensionsUniform->SetValue( Point2i( m_dimensions.width(), m_dimensions.height() ) );
		}
	}
}
