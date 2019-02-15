#include "Castor3D/Scene/BillboardList.hpp"

#include "Castor3D/Engine.hpp"

#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <Ashes/Buffer/VertexBuffer.hpp>
#include <Ashes/Pipeline/VertexLayout.hpp>

#include <CastorUtils/Design/ArrayView.hpp>

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
			result = file.writeText( m_tabs + cuT( "\tdimensions " )
				+ string::toString( obj.getDimensions()[0], std::locale{ "C" } ) + cuT( " " )
				+ string::toString( obj.getDimensions()[1], std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
			MovableObject::TextWriter::checkError( result, "BillboardList dimensions" );
		}

		if ( result && obj.getCount() )
		{
			result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "\tpositions\n" ) ) > 0
					   && file.writeText( m_tabs + cuT( "\t{\n" ) ) > 0;
			MovableObject::TextWriter::checkError( result, "BillboardList positions" );

			for ( auto const & point : obj )
			{
				result = file.writeText( m_tabs + cuT( "\t\tpos " )
					+ string::toString( point[0], std::locale{ "C" } ) + cuT( " " )
					+ string::toString( point[1], std::locale{ "C" } ) + cuT( " " )
					+ string::toString( point[2], std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
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
		, ashes::VertexLayoutPtr && vertexLayout
		, ashes::VertexBufferBasePtr && vertexBuffer )
		: m_vertexLayout{ std::move( vertexLayout ) }
		, m_vertexBuffer{ vertexBuffer ? std::move( vertexBuffer ) : nullptr }
		, m_scene{ scene }
		, m_node{ node }
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
			Quad vertices
			{
				Vertex{ castor::Point3f{ -0.5f, -0.5f, 1.0f }, castor::Point2f{ 0.0f, 0.0f } },
				Vertex{ castor::Point3f{ -0.5f, +0.5f, 1.0f }, castor::Point2f{ 0.0f, 1.0f } },
				Vertex{ castor::Point3f{ +0.5f, -0.5f, 1.0f }, castor::Point2f{ 1.0f, 0.0f } },
				Vertex{ castor::Point3f{ +0.5f, +0.5f, 1.0f }, castor::Point2f{ 1.0f, 1.0f } },
			};
			auto & device = getCurrentDevice( m_scene );
			m_quadBuffer = ashes::makeVertexBuffer< Quad >( device
				, 1u
				, ashes::BufferTarget::eTransferDst
				, ashes::MemoryPropertyFlag::eHostVisible );

			if ( auto buffer = m_quadBuffer->lock( 0u, 1u, ashes::MemoryMapFlag::eWrite ) )
			{
				*buffer = vertices;
				m_quadBuffer->flush( 0u, 1u );
				m_quadBuffer->unlock();
			}

			m_quadLayout = ashes::makeLayout< Vertex >( 0u, ashes::VertexInputRate::eVertex );
			m_quadLayout->createAttribute( 0u, ashes::Format::eR32G32B32_SFLOAT, offsetof( Vertex, position ) );
			m_quadLayout->createAttribute( 1u, ashes::Format::eR32G32_SFLOAT, offsetof( Vertex, uv ) );

			ashes::BufferCRefArray buffers;
			std::vector< uint64_t > offsets;
			ashes::VertexLayoutCRefArray layouts;
			doGatherBuffers( buffers, offsets, layouts );

			m_geometryBuffers.vbo = buffers;
			m_geometryBuffers.vboOffsets = offsets;
			m_geometryBuffers.layouts = layouts;
			m_geometryBuffers.vtxCount = 4u;
			m_initialised = true;
		}

		return m_initialised;
	}

	void BillboardBase::cleanup()
	{
		if ( m_initialised )
		{
			m_initialised = false;
			m_geometryBuffers.vbo.clear();
			m_geometryBuffers.vboOffsets.clear();
			m_geometryBuffers.layouts.clear();
			m_geometryBuffers.ibo = nullptr;
			m_geometryBuffers.iboOffset = 0u;
			m_geometryBuffers.idxCount = 0u;
			m_geometryBuffers.vtxCount = 0u;
			m_quadLayout.reset();
			m_quadBuffer.reset();
			m_vertexLayout.reset();
			m_vertexBuffer.reset();
		}
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
			uint32_t stride = m_vertexLayout->getStride();

			if ( auto gpuBuffer = m_vertexBuffer->getBuffer().lock( 0
				, m_count * stride
				, ashes::MemoryMapFlag::eRead | ashes::MemoryMapFlag::eWrite ) )
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

				m_vertexBuffer->getBuffer().flush( 0u, m_count * stride );
				m_vertexBuffer->getBuffer().unlock();
			}
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

	void BillboardBase::setMaterial( MaterialSPtr value )
	{
		MaterialSPtr oldMaterial = getMaterial();

		if ( oldMaterial != value )
		{
			m_material = value;

			if ( oldMaterial )
			{
				onMaterialChanged( *this, oldMaterial, value );
			}
		}
	}

	void BillboardBase::setCount( uint32_t value )
	{
		if ( m_count != value )
		{
			m_count = value;
			getParentScene().setChanged();
		}
	}

	void BillboardBase::doGatherBuffers( ashes::BufferCRefArray & buffers
		, std::vector< uint64_t > & offsets
		, ashes::VertexLayoutCRefArray & layouts )
	{
		buffers.emplace_back( m_quadBuffer->getBuffer() );
		offsets.emplace_back( 0u );
		layouts.emplace_back( *m_quadLayout );
		buffers.emplace_back( m_vertexBuffer->getBuffer() );
		offsets.emplace_back( 0u );
		layouts.emplace_back( *m_vertexLayout );
	}

	//*************************************************************************************************

	ashes::VertexLayoutPtr doCreateLayout( ashes::Device const & device )
	{
		ashes::VertexLayoutPtr result = ashes::makeLayout< castor::Point3f >( 1u, ashes::VertexInputRate::eInstance );
		result->createAttribute( 2u, ashes::Format::eR32G32B32_SFLOAT, 0u );
		return result;
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
			, nullptr }
	{
	}

	BillboardList::~BillboardList()
	{
	}

	bool BillboardList::initialise()
	{
		if ( !m_vertexLayout
			|| !m_vertexBuffer
			|| m_arrayPositions.size() != m_vertexBuffer->getSize() )
		{
			if ( !m_vertexLayout )
			{
				m_vertexLayout = doCreateLayout( getCurrentDevice( m_scene ) );
			}

			uint32_t stride = m_vertexLayout->getStride();
			auto & device = getCurrentDevice( m_scene );
			m_vertexBuffer = ashes::makeVertexBuffer< castor::Point3f >( device
				, uint32_t( m_arrayPositions.size() )
				, ashes::BufferTarget::eTransferDst
				, ashes::MemoryPropertyFlag::eHostVisible );

			if ( auto * buffer = m_vertexBuffer->getBuffer().lock( 0u
				, uint32_t( stride * m_arrayPositions.size() )
				, ashes::MemoryMapFlag::eWrite | ashes::MemoryMapFlag::eInvalidateRange ) )
			{
				for ( auto & pos : m_arrayPositions )
				{
					std::memcpy( buffer, pos.constPtr(), stride );
					buffer += stride;
				}

				m_vertexBuffer->getBuffer().flush( 0u, uint32_t( stride * m_arrayPositions.size() ) );
				m_vertexBuffer->getBuffer().unlock();
			}
		}

		return BillboardBase::initialise( uint32_t( m_arrayPositions.size() ) );
	}

	void BillboardList::removePoint( uint32_t index )
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
