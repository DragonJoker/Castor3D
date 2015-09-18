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
#include "Engine.hpp"
#include "Pipeline.hpp"
#include "ShaderManager.hpp"

using namespace Castor;

namespace Castor3D
{
	bool BillboardList::TextLoader::operator()( BillboardList const & p_obj, Castor::TextFile & p_file )
	{
		bool l_return = p_file.WriteText( cuT( "\tbillboard \"" ) + p_obj.GetName() + cuT( "\"\n\t{\n" ) ) > 0;

		if ( l_return )
		{
			l_return = MovableObject::TextLoader()( p_obj, p_file );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( cuT( "\t\tmaterial \"" ) + p_obj.GetMaterial()->GetName() + cuT( "\"\n" ) ) > 0;
		}

		if ( l_return )
		{
			l_return = p_file.Print( 256, cuT( "\t\tdimensions %d %d" ), p_obj.GetDimensions().width(), p_obj.GetDimensions().height() ) > 0;
		}

		if ( l_return && p_obj.GetCount() )
		{
			l_return = p_file.WriteText( cuT( "\t\tpositions\n\t\t{\n" ) ) > 0;

			for ( Point3rArray::const_iterator l_it = p_obj.Begin(); l_it != p_obj.End(); ++l_it )
			{
				l_return = p_file.Print( 256, cuT( "\t\t\tpos %f %f %f" ), l_it->at( 0 ), l_it->at( 1 ), l_it->at( 2 ) ) > 0;
			}

			l_return = p_file.WriteText( cuT( "\t\t}\n" ) ) > 0;
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( cuT( "\t}\n" ) ) > 0;
		}

		return l_return;
	}

	//*************************************************************************************************

	BillboardList::BinaryParser::BinaryParser( Path const & p_path )
		:	MovableObject::BinaryParser( p_path )
	{
	}

	bool BillboardList::BinaryParser::Fill( BillboardList const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_BILLBOARD );

		if ( l_return )
		{
			l_return = MovableObject::BinaryParser( m_path ).Fill( p_obj, l_chunk );
		}

		if ( l_return )
		{
			l_return = DoFillChunk( p_obj.GetMaterial()->GetName(), eCHUNK_TYPE_BILLBOARD_MATERIAL, l_chunk );
		}

		if ( l_return )
		{
			l_return = DoFillChunk( p_obj.GetDimensions(), eCHUNK_TYPE_BILLBOARD_DIMENSIONS, l_chunk );
		}

		if ( l_return && p_obj.GetCount() )
		{
			for ( Point3rArray::const_iterator l_it = p_obj.Begin(); l_it != p_obj.End(); ++l_it )
			{
				l_return = DoFillChunk( *l_it, eCHUNK_TYPE_BILLBOARD_POSITION, l_chunk );
			}
		}

		return l_return;
	}

	bool BillboardList::BinaryParser::Parse( BillboardList & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		String l_name;
		Size l_size;
		Point3r l_position;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_return = p_chunk.GetSubChunk( l_chunk );

			if ( l_return )
			{
				switch ( l_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_BILLBOARD_MATERIAL:
					l_return = DoParseChunk( l_name, l_chunk );

					if ( l_return )
					{
						p_obj.SetMaterial( p_obj.GetScene()->GetEngine()->GetMaterialManager().find( l_name ) );
					}

					break;

				case eCHUNK_TYPE_BILLBOARD_DIMENSIONS:
					l_return = DoParseChunk( l_size, l_chunk );
					p_obj.SetDimensions( l_size );
					break;

				case eCHUNK_TYPE_BILLBOARD_POSITION:
					l_return = DoParseChunk( l_position, l_chunk );
					p_obj.AddPoint( l_position );
					break;

				default:
					l_return = MovableObject::BinaryParser( m_path ).Parse( p_obj, l_chunk );
					break;
				}
			}

			if ( !l_return )
			{
				p_chunk.EndParse();
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	BillboardList::BillboardList( SceneSPtr p_scene, RenderSystem * p_pRenderSystem )
		:	MovableObject( p_scene, eMOVABLE_TYPE_BILLBOARD )
		,	m_renderSystem( p_pRenderSystem )
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
		l_pVtxBuffer = std::make_unique< VertexBuffer >( m_renderSystem, &( *m_pDeclaration )[0], m_pDeclaration->Size() );
		uint32_t l_uiStride = m_pDeclaration->GetStride();
		l_pVtxBuffer->Resize( uint32_t( m_arrayPositions.size() * l_uiStride ) );
		uint8_t * l_pBuffer = l_pVtxBuffer->data();

		for ( Point3rArrayConstIt l_it = m_arrayPositions.begin(); l_it != m_arrayPositions.end(); ++l_it )
		{
			std::memcpy( l_pBuffer, l_it->const_ptr(), l_uiStride );
			l_pBuffer += l_uiStride;
		}

		m_pGeometryBuffers = m_renderSystem->CreateGeometryBuffers( std::move( l_pVtxBuffer ), nullptr, nullptr );
		return true;
	}

	bool BillboardList::InitialiseShader( RenderTechniqueBase & p_technique )
	{
		MaterialSPtr l_pMaterial = m_wpMaterial.lock();
		bool l_return = false;

		if ( l_pMaterial && l_pMaterial->GetPassCount() )
		{
			l_pMaterial->Cleanup();
			ShaderProgramBaseSPtr l_pProgram = DoGetProgram( p_technique, l_pMaterial->GetPass( 0 )->GetTextureFlags() );

			if ( m_wpProgram.expired() || m_wpProgram.lock() != l_pProgram )
			{
				m_wpProgram = l_pProgram;
				l_return = DoInitialise();

				if ( l_return )
				{
					l_pMaterial->Initialise();
					m_pDimensionsUniform->SetValue( Point2i( m_dimensions.width(), m_dimensions.height() ) );
					m_pGeometryBuffers->Create();
					m_pGeometryBuffers->Initialise( l_pProgram, eBUFFER_ACCESS_TYPE_STATIC, eBUFFER_ACCESS_NATURE_DRAW );
					m_bNeedUpdate = false;
				}
			}
			else
			{
				l_return = true;
			}
		}

		return l_return;
	}

	void BillboardList::Cleanup()
	{
		m_pDimensionsUniform.reset();
		ShaderProgramBaseSPtr l_pProgram = m_wpProgram.lock();
		l_pProgram->Cleanup();
		m_pGeometryBuffers->Cleanup();
		m_pGeometryBuffers->Destroy();
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
			Pipeline & l_pipeline = m_renderSystem->GetPipeline();
			ShaderProgramBaseSPtr l_pProgram = m_wpProgram.lock();
			MaterialSPtr l_pMaterial = m_wpMaterial.lock();
			VertexBuffer & l_vtxBuffer = m_pGeometryBuffers->GetVertexBuffer();
			uint32_t l_uiSize = l_vtxBuffer.GetSize() / l_vtxBuffer.GetDeclaration().GetStride();

			if ( l_pProgram && l_pMaterial )
			{
				uint8_t l_index = 0;
				uint8_t l_count = l_pMaterial->GetPassCount();

				for ( auto && l_pass : *l_pMaterial )
				{
					l_pass->BindToAutomaticProgram( l_pProgram );
					auto l_matrixBuffer = l_pass->GetMatrixBuffer();

					if ( l_matrixBuffer )
					{
						l_pipeline.ApplyMatrices( *l_matrixBuffer, 0xFFFFFFFFFFFFFFFF );
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
