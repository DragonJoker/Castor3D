#include "BillboardList.hpp"

#include "Engine.hpp"

#include "Event/Frame/FrameListener.hpp"
#include "Material/Material.hpp"
#include "Mesh/Buffer/Buffer.hpp"
#include "Scene/Scene.hpp"
#include "Shader/ShaderProgram.hpp"

#include <Design/ArrayView.hpp>

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	BillboardList::TextWriter::TextWriter( String const & tabs )
		: MovableObject::TextWriter{ tabs }
	{
	}

	bool BillboardList::TextWriter::operator()( BillboardList const & obj, castor::TextFile & file )
	{
		bool result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "billboard \"" ) + obj.getName() + cuT( "\"\n" ) ) > 0
						&& file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
		MovableObject::TextWriter::checkError( result, "BillboardList name" );

		if ( result )
		{
			result = MovableObject::TextWriter{ m_tabs + cuT( "\t" ) }( obj, file );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\tmaterial \"" ) + obj.getMaterial()->getName() + cuT( "\"\n" ) ) > 0;
			MovableObject::TextWriter::checkError( result, "BillboardList material" );
		}

		if ( result )
		{
			result = file.print( 256, cuT( "%s\tdimensions %d %d\n" ), m_tabs.c_str(), obj.getDimensions()[0], obj.getDimensions()[1] ) > 0;
			MovableObject::TextWriter::checkError( result, "BillboardList dimensions" );
		}

		if ( result && obj.getCount() )
		{
			result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "\tpositions\n" ) ) > 0
					   && file.writeText( m_tabs + cuT( "\t{\n" ) ) > 0;
			MovableObject::TextWriter::checkError( result, "BillboardList positions" );

			for ( auto const & point : obj )
			{
				result &= file.print( 256, cuT( "%s\t\tpos %f %f %f" ), m_tabs.c_str(), point[0], point[1], point[2] ) > 0;
				MovableObject::TextWriter::checkError( result, "BillboardList position" );
			}

			result &= file.writeText( m_tabs + cuT( "\t}\n" ) ) > 0;
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	//*************************************************************************************************

	BillboardBase::BillboardBase( Scene & scene
		, SceneNodeSPtr node
		, VertexBufferSPtr vertexBuffer )
		: m_vertexBuffer{ vertexBuffer }
		, m_scene{ scene }
		, m_node{ node }
		, m_quad( std::make_unique< VertexBuffer >( *scene.getEngine(), BufferDeclaration
		{
			{
				BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec3 ),
				BufferElementDeclaration( ShaderProgram::Texture, uint32_t( ElementUsage::eTexCoords ), ElementType::eVec2 ),
			}
		} ) )
	{
	}

	BillboardBase::~BillboardBase()
	{
	}

	bool BillboardBase::initialise( uint32_t count )
	{
		if ( !m_initialised )
		{
			m_count = count;
			uint32_t stride = m_vertexBuffer->getDeclaration().stride();

			if ( m_vertexBuffer->getSize() < uint32_t( count * stride ) )
			{
				m_vertexBuffer->resize( uint32_t( count * stride ) );
			}

			m_initialised = m_vertexBuffer->initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw );

			if ( m_initialised )
			{
				std::array< std::array< float, 5 >, 4 > vertices
				{
					{
						std::array< float, 5 >{ { -0.5f, 0.5f, 1.0f, 0.0f, 1.0f } },
						std::array< float, 5 >{ { -0.5f, -0.5f, 1.0f, 0.0f, 0.0f } },
						std::array< float, 5 >{ { 0.5f, -0.5f, 1.0f, 1.0f, 0.0f } },
						std::array< float, 5 >{ { 0.5f, 0.5f, 1.0f, 1.0f, 1.0f } },
					}
				};

				stride = m_quad->getDeclaration().stride();
				m_quad->resize( 4 * stride );
				auto buffer = m_quad->getData();

				for ( auto & vertex : vertices )
				{
					std::memcpy( buffer, vertex.data(), stride );
					buffer += stride;
				}

				m_initialised = m_quad->initialise( BufferAccessType::eStatic, BufferAccessNature::eDraw );
			}
		}

		return m_initialised;
	}

	void BillboardBase::cleanup()
	{
		if ( m_initialised )
		{
			m_initialised = false;
			m_quad->cleanup();
			m_vertexBuffer->cleanup();
		}
	}

	void BillboardBase::draw( GeometryBuffers const & geometryBuffers )
	{
		if ( m_needUpdate )
		{
			update();
			m_needUpdate = false;
		}

		geometryBuffers.drawInstanced( 4, 0u, m_count );
	}

	void BillboardBase::sortByDistance( Point3r const & cameraPosition )
	{
		m_needUpdate = m_cameraPosition != cameraPosition;
		m_cameraPosition = cameraPosition;
	}

	void BillboardBase::update()
	{
		if ( m_count )
		{
			m_vertexBuffer->bind();
			uint32_t stride = m_vertexBuffer->getDeclaration().stride();
			auto gpuBuffer = m_vertexBuffer->lock( 0, m_count * stride, AccessType::eRead | AccessType::eWrite );

			if ( gpuBuffer )
			{
				struct Element
				{
					uint8_t * m_buffer;
					Coords3r m_position;
					uint32_t m_stride;

					Element( uint8_t * buffer
						, uint32_t offset
						, uint32_t stride )
						: m_buffer{ buffer }
						, m_position{ reinterpret_cast< real * >( buffer + offset ) }
						, m_stride{ stride }
					{
					}

					Element( Element const & rhs )
						: m_buffer{ rhs.m_buffer }
						, m_position{ rhs.m_position }
						, m_stride{ rhs.m_stride }
					{
					}

					Element( Element && rhs )
						: m_buffer{ rhs.m_buffer }
						, m_position{ std::move( rhs.m_position ) }
						, m_stride{ rhs.m_stride }
					{
						rhs.m_buffer = nullptr;
					}

					Element & operator=( Element const & rhs )
					{
						std::memcpy( m_buffer, rhs.m_buffer, m_stride );
						return *this;
					}

					Element & operator=( Element && rhs )
					{
						if ( &rhs != this )
						{
							m_buffer = rhs.m_buffer;
							m_position = std::move( rhs.m_position );
							m_stride = std::move( rhs.m_stride );
							rhs.m_buffer = nullptr;
						}
						return *this;
					}
				};

				ByteArray copy{ gpuBuffer, gpuBuffer + ( stride * m_count ) };
				std::vector< Element > elements;
				auto buffer = copy.data();
				elements.reserve( m_count );

				for ( uint32_t i = 0u; i < m_count; ++i )
				{
					elements.emplace_back( buffer, m_centerOffset, stride );
					buffer += stride;
				}

				try
				{
					std::sort( elements.begin()
						, elements.end()
						, [this]( Element const & p_a
						, Element const & p_b )
						{
							return point::lengthSquared( p_a.m_position - m_cameraPosition )
								> point::lengthSquared( p_b.m_position - m_cameraPosition );
						} );

					for ( auto & element : elements )
					{
						std::memcpy( gpuBuffer, element.m_buffer, stride );
						gpuBuffer += stride;
					}
				}
				catch ( Exception const & p_exc )
				{
					Logger::logError( std::stringstream() << "Submesh::SortFaces - Error: " << p_exc.what());
				}

				m_vertexBuffer->unlock();
			}

			m_vertexBuffer->unbind();
		}
	}

	ProgramFlags BillboardBase::getProgramFlags()const
	{
		ProgramFlags result = uint32_t( ProgramFlag::eBillboards );

		if ( m_billboardType == BillboardType::eSpherical )
		{
			addFlag( result, ProgramFlag::eSpherical );
		}

		if ( m_billboardSize == BillboardSize::eFixed )
		{
			addFlag( result, ProgramFlag::eFixedSize );
		}

		return result;
	}

	void BillboardBase::gatherBuffers( VertexBufferArray & buffers )
	{
		buffers.emplace_back( *m_quad );
		buffers.emplace_back( *m_vertexBuffer );
	}

	//*************************************************************************************************

	BillboardList::BillboardList( String const & name
		, Scene & scene
		, SceneNodeSPtr parent )
		: MovableObject( name
			, scene
			, MovableType::eBillboard
			, parent )
		, BillboardBase{ scene
			, parent
			, std::make_shared< VertexBuffer >( *scene.getEngine()
				, BufferDeclaration
				{
					{ BufferElementDeclaration( cuT( "center" ), uint32_t( 0u ), ElementType::eVec3, 0u, 1u ) }
				} ) }
	{
	}

	BillboardList::~BillboardList()
	{
	}

	bool BillboardList::initialise()
	{
		uint32_t stride = m_vertexBuffer->getDeclaration().stride();
		m_vertexBuffer->resize( uint32_t( m_arrayPositions.size() * stride ) );
		uint8_t * buffer = m_vertexBuffer->getData();

		for ( auto & pos : m_arrayPositions )
		{
			std::memcpy( buffer, pos.constPtr(), stride );
			buffer += stride;
		}

		return BillboardBase::initialise( uint32_t( m_arrayPositions.size() ) );
	}

	void BillboardList::RemovePoint( uint32_t index )
	{
		if ( index < m_arrayPositions.size() )
		{
			m_arrayPositions.erase( m_arrayPositions.begin() + index );
			m_needUpdate = true;
		}
	}

	void BillboardList::addPoint( castor::Point3r const & position )
	{
		m_arrayPositions.push_back( position );
		m_needUpdate = true;
	}

	void BillboardList::addPoints( castor::Point3rArray const & positions )
	{
		m_arrayPositions.insert( m_arrayPositions.end(), positions.begin(), positions.end() );
		m_needUpdate = true;
	}

	void BillboardList::attachTo( SceneNodeSPtr node )
	{
		MovableObject::attachTo( node );
		setNode( node );
	}

	//*************************************************************************************************
}
