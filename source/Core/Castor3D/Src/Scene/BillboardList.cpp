#include "BillboardList.hpp"

#include "Engine.hpp"
#include "MaterialCache.hpp"
#include "ShaderCache.hpp"

#include "Scene.hpp"
#include "SceneNode.hpp"

#include "Event/Frame/FunctorEvent.hpp"
#include "Material/Material.hpp"
#include "Material/Pass.hpp"
#include "Mesh/Buffer/Buffer.hpp"
#include "Render/Pipeline.hpp"
#include "Render/RenderSystem.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/PointFrameVariable.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace Castor;

namespace Castor3D
{
	BillboardList::TextWriter::TextWriter( String const & p_tabs )
		: MovableObject::TextWriter{ p_tabs }
	{
	}

	bool BillboardList::TextWriter::operator()( BillboardList const & p_obj, Castor::TextFile & p_file )
	{
		bool l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "billboard \"" ) + p_obj.GetName() + cuT( "\"\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
		MovableObject::TextWriter::CheckError( l_return, "BillboardList name" );

		if ( l_return )
		{
			l_return = MovableObject::TextWriter{ m_tabs }( p_obj, p_file );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\tmaterial \"" ) + p_obj.GetMaterial()->GetName() + cuT( "\"\n" ) ) > 0;
			MovableObject::TextWriter::CheckError( l_return, "BillboardList material" );
		}

		if ( l_return )
		{
			l_return = p_file.Print( 256, cuT( "%s\tdimensions %d %d\n" ), m_tabs.c_str(), p_obj.GetDimensions().width(), p_obj.GetDimensions().height() ) > 0;
			MovableObject::TextWriter::CheckError( l_return, "BillboardList dimensions" );
		}

		if ( l_return && p_obj.GetCount() )
		{
			l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "\tpositions\n" ) ) > 0
					   && p_file.WriteText( m_tabs + cuT( "\t{\n" ) ) > 0;
			MovableObject::TextWriter::CheckError( l_return, "BillboardList positions" );

			for ( auto const & l_point : p_obj )
			{
				l_return &= p_file.Print( 256, cuT( "%s\t\tpos %f %f %f" ), m_tabs.c_str(), l_point[0], l_point[1], l_point[2] ) > 0;
				MovableObject::TextWriter::CheckError( l_return, "BillboardList position" );
			}

			l_return &= p_file.WriteText( m_tabs + cuT( "\t}\n" ) ) > 0;
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return l_return;
	}

	//*************************************************************************************************

	BillboardListBase::BillboardListBase( String const & p_name
										  , Scene & p_scene
										  , SceneNodeSPtr p_parent
										  , VertexBufferSPtr p_vertexBuffer )
		: MovableObject( p_name, p_scene, MovableType::Billboard, p_parent )
		, m_vertexBuffer{ p_vertexBuffer }
	{
	}

	BillboardListBase::~BillboardListBase()
	{
	}

	bool BillboardListBase::Initialise()
	{
		if ( !m_initialised )
		{
			m_initialised = m_vertexBuffer->Create();
		}

		return m_initialised;
	}

	void BillboardListBase::Cleanup()
	{
		if ( m_initialised )
		{
			m_initialised = false;

			for ( auto l_buffers : m_geometryBuffers )
			{
				l_buffers->Cleanup();
			}

			m_geometryBuffers.clear();
			m_vertexBuffer->Destroy();
			m_vertexBuffer.reset();
		}
	}

	void BillboardListBase::SortByDistance( Castor::Point3r const & p_cameraPosition )
	{
	}

	void BillboardListBase::Draw( GeometryBuffers const & p_geometryBuffers )
	{
		DoUpdate();
		p_geometryBuffers.Draw( m_count, 0 );
	}

	GeometryBuffersSPtr BillboardListBase::GetGeometryBuffers( ShaderProgram const & p_program )
	{
		GeometryBuffersSPtr l_buffers;
		auto l_it = std::find_if( std::begin( m_geometryBuffers ), std::end( m_geometryBuffers ), [&p_program]( GeometryBuffersSPtr p_buffers )
		{
			return &p_buffers->GetProgram() == &p_program;
		} );

		if ( l_it == m_geometryBuffers.end() )
		{
			l_buffers = GetScene()->GetEngine()->GetRenderSystem()->CreateGeometryBuffers( Topology::Points, p_program );

			GetScene()->GetEngine()->PostEvent( MakeFunctorEvent( EventType::PreRender, [this, l_buffers]()
			{
				l_buffers->Initialise( { *m_vertexBuffer }, nullptr );
			} ) );
			m_geometryBuffers.push_back( l_buffers );
		}
		else
		{
			l_buffers = *l_it;
		}

		return l_buffers;
	}

	//*************************************************************************************************

	BillboardList::BillboardList( String const & p_name, Scene & p_scene, SceneNodeSPtr p_parent )
		: BillboardListBase( p_name
							 , p_scene
							 , p_parent
							 , std::make_shared< VertexBuffer >( *p_scene.GetEngine(), BufferDeclaration{ {
									 BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::Position ), ElementType::Vec3 )
								 } } ) )
		, m_needUpdate{ false }
	{
	}

	BillboardList::~BillboardList()
	{
	}

	bool BillboardList::Initialise()
	{
		if ( !m_initialised )
		{
			uint32_t l_stride = m_vertexBuffer->GetDeclaration().GetStride();
			m_vertexBuffer->Resize( uint32_t( m_arrayPositions.size() * l_stride ) );
			uint8_t * l_buffer = m_vertexBuffer->data();

			for ( auto & l_pos : m_arrayPositions )
			{
				std::memcpy( l_buffer, l_pos.const_ptr(), l_stride );
				l_buffer += l_stride;
			}

			m_vertexBuffer->Create();
			m_vertexBuffer->Upload( BufferAccessType::Dynamic, BufferAccessNature::Draw );
			m_initialised = true;
		}

		return m_initialised;
	}

	void BillboardList::Cleanup()
	{
		if ( m_initialised )
		{
			m_initialised = false;

			for ( auto l_buffers : m_geometryBuffers )
			{
				l_buffers->Cleanup();
			}

			m_geometryBuffers.clear();
			m_vertexBuffer->Destroy();
			m_vertexBuffer.reset();
		}
	}

	void BillboardList::RemovePoint( uint32_t p_index )
	{
		if ( p_index < m_arrayPositions.size() )
		{
			m_arrayPositions.erase( m_arrayPositions.begin() + p_index );
			m_needUpdate = true;
		}
	}

	void BillboardList::AddPoint( Castor::Point3r const & p_position )
	{
		m_arrayPositions.push_back( p_position );
		m_needUpdate = true;
	}

	void BillboardList::AddPoints( Castor::Point3rArray const & p_ptPositions )
	{
		m_arrayPositions.insert( m_arrayPositions.end(), p_ptPositions.begin(), p_ptPositions.end() );
		m_needUpdate = true;
	}

	void BillboardList::SortByDistance( Point3r const & p_cameraPosition )
	{
		try
		{
			if ( m_cameraPosition != p_cameraPosition )
			{
				m_cameraPosition = p_cameraPosition;

				std::sort( std::begin( m_arrayPositions ), std::end( m_arrayPositions ), [&p_cameraPosition]( Point3r const & p_a, Point3r const & p_b )
				{
					return point::distance_squared( p_a - p_cameraPosition ) > point::distance_squared( p_b - p_cameraPosition );
				} );

				m_needUpdate = true;
			}
		}
		catch ( Exception const & p_exc )
		{
			Logger::LogError( std::stringstream() << "Submesh::SortFaces - Error: " << p_exc.what() );
		}
	}

	void BillboardList::DoUpdate()
	{
		if ( m_needUpdate )
		{
			uint32_t l_stride = m_vertexBuffer->GetDeclaration().GetStride();
			m_vertexBuffer->Resize( uint32_t( m_arrayPositions.size() * l_stride ) );
			uint8_t * l_buffer = m_vertexBuffer->data();

			for ( auto & l_pos : m_arrayPositions )
			{
				std::memcpy( l_buffer, l_pos.const_ptr(), l_stride );
				l_buffer += l_stride;
			}

			m_vertexBuffer->Upload( BufferAccessType::Dynamic, BufferAccessNature::Draw );
			m_needUpdate = false;
			m_count = uint32_t( m_arrayPositions.size() );
		}
	}
}
