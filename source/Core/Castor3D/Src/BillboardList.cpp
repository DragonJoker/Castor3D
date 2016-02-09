#include "BillboardList.hpp"

#include "Buffer.hpp"
#include "Engine.hpp"
#include "FrameVariableBuffer.hpp"
#include "Material.hpp"
#include "MaterialManager.hpp"
#include "Pass.hpp"
#include "Pipeline.hpp"
#include "PointFrameVariable.hpp"
#include "RenderSystem.hpp"
#include "Scene.hpp"
#include "SceneNode.hpp"
#include "ShaderManager.hpp"
#include "ShaderObject.hpp"
#include "ShaderProgram.hpp"

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

			for ( auto const & l_point : p_obj )
			{
				l_return = p_file.Print( 256, cuT( "\t\t\tpos %f %f %f" ), l_point[0], l_point[1], l_point[2] ) > 0;
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
			for ( auto const & l_point : p_obj )
			{
				l_return = DoFillChunk( l_point, eCHUNK_TYPE_BILLBOARD_POSITION, l_chunk );
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
						p_obj.SetMaterial( p_obj.GetScene()->GetEngine()->GetMaterialManager().Find( l_name ) );
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

	BillboardList::BillboardList( String const & p_name, Scene & p_scene, SceneNodeSPtr p_parent, RenderSystem & p_renderSystem )
		: MovableObject( p_name, p_scene, eMOVABLE_TYPE_BILLBOARD, p_parent )
		, m_bNeedUpdate( false )
	{
		BufferElementDeclaration l_vertexDeclarationElements[] = { BufferElementDeclaration( cuT( "" ), eELEMENT_TYPE_3FLOATS ) };
		m_declaration = std::make_shared< BufferDeclaration >( l_vertexDeclarationElements );
	}

	BillboardList::~BillboardList()
	{
	}

	bool BillboardList::Initialise()
	{
		m_vertexBuffer = std::make_unique< VertexBuffer >( *GetScene()->GetEngine(), *m_declaration );
		uint32_t l_stride = m_declaration->GetStride();
		m_vertexBuffer->Resize( uint32_t( m_arrayPositions.size() * l_stride ) );
		uint8_t * l_pBuffer = m_vertexBuffer->data();

		for ( auto & l_pos : m_arrayPositions )
		{
			l_pos[2] = -l_pos[2];
			std::memcpy( l_pBuffer, l_pos.const_ptr(), l_stride );
			l_pBuffer += l_stride;
		}

		return true;
	}

	bool BillboardList::InitialiseShader( RenderTechniqueBase & p_technique )
	{
		MaterialSPtr l_material = m_wpMaterial.lock();
		bool l_return = false;

		if ( l_material && l_material->GetPassCount() )
		{
			auto & l_manager = GetScene()->GetEngine()->GetShaderManager();

			for ( auto l_pass : *l_material )
			{
				ShaderProgramSPtr l_program = l_manager.GetBillboardProgram( l_pass->GetTextureFlags(), ePROGRAM_FLAG_BILLBOARDS );

				if ( !l_program )
				{
					l_program = GetScene()->GetEngine()->GetRenderSystem()->CreateBillboardsProgram( p_technique, l_pass->GetTextureFlags() );
					l_manager.AddBillboardProgram( l_program, l_pass->GetTextureFlags(), ePROGRAM_FLAG_BILLBOARDS );
				}

				if ( m_wpProgram.expired() || m_wpProgram.lock() != l_program )
				{
					l_pass->Cleanup();
					m_wpProgram = l_program;

					if ( l_program )
					{
						auto l_config = l_program->FindFrameVariableBuffer( cuT( "Billboard" ) );

						if ( l_config )
						{
							l_config->GetVariable( cuT( "c3d_v2iDimensions" ), m_pDimensionsUniform );

							if ( !m_pDimensionsUniform )
							{
								Logger::LogError( cuT( "Couldn't find Config UBO in billboard shader program" ) );
							}
							else
							{
								l_return = l_program->Initialise();
							}
						}
						else
						{
							Logger::LogError( cuT( "Couldn't find Config UBO in billboard shader program" ) );
						}
					}

					if ( l_return )
					{
						l_pass->Initialise();
						m_vertexBuffer->Create();
						m_vertexBuffer->Initialise( eBUFFER_ACCESS_TYPE_DYNAMIC, eBUFFER_ACCESS_NATURE_DRAW );
						m_pDimensionsUniform->SetValue( Point2i( m_dimensions.width(), m_dimensions.height() ) );
						DoPrepareGeometryBuffers( *l_pass );
						m_bNeedUpdate = false;
					}
				}
				else
				{
					l_return = true;
				}
			}
		}

		return l_return;
	}

	GeometryBuffers & BillboardList::DoPrepareGeometryBuffers( Pass const & p_pass )
	{
		ShaderProgramSPtr l_program = p_pass.GetShader();

		if ( !l_program || l_program->GetStatus() == ePROGRAM_STATUS_LINKED )
		{
			CASTOR_EXCEPTION( "Can't retrieve a program input layout from a non compiled shader." );
		}

		GeometryBuffersSPtr l_buffers;
		auto const & l_layout = l_program->GetLayout();
		auto l_it = std::find_if( std::begin( m_geometryBuffers ), std::end( m_geometryBuffers ), [&l_layout]( GeometryBuffersSPtr p_buffers )
		{
			return p_buffers->GetLayout() == l_layout;
		} );

		if ( l_it == m_geometryBuffers.end() )
		{
			l_buffers = GetScene()->GetEngine()->GetRenderSystem()->CreateGeometryBuffers( eTOPOLOGY_TRIANGLES, *l_program, m_vertexBuffer.get(), nullptr, nullptr, nullptr );
			m_geometryBuffers.push_back( l_buffers );
		}
		else
		{
			l_buffers = *l_it;
		}

		return *l_buffers;
	}

	void BillboardList::Cleanup()
	{
		m_pDimensionsUniform.reset();
		ShaderProgramSPtr l_program = m_wpProgram.lock();
		l_program->Cleanup();
		m_geometryBuffers.clear();
		m_vertexBuffer->Cleanup();
		m_vertexBuffer->Destroy();
		m_vertexBuffer.reset();
	}

	void BillboardList::RemovePoint( uint32_t p_index )
	{
		if ( p_index < m_arrayPositions.size() )
		{
			m_arrayPositions.erase( m_arrayPositions.begin() + p_index );
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
			Pipeline & l_pipeline = GetScene()->GetEngine()->GetRenderSystem()->GetPipeline();
			ShaderProgramSPtr l_program = m_wpProgram.lock();
			MaterialSPtr l_material = m_wpMaterial.lock();
			uint32_t l_uiSize = m_vertexBuffer->GetSize() / m_vertexBuffer->GetDeclaration().GetStride();

			if ( l_program && l_material )
			{
				for ( auto && l_pass : *l_material )
				{
					auto & l_buffers = DoPrepareGeometryBuffers( *l_pass );
					l_pass->BindToAutomaticProgram( l_program );
					auto l_matrixBuffer = l_pass->GetMatrixBuffer();

					if ( l_matrixBuffer )
					{
						l_pipeline.ApplyMatrices( *l_matrixBuffer, 0xFFFFFFFFFFFFFFFF );
					}

					l_pass->Render();
					l_buffers.Draw( *l_program, l_uiSize, 0 );
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
