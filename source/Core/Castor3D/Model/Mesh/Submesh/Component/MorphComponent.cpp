#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Miscellaneous/StagingData.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>

namespace castor3d
{
	namespace smshcompmorph
	{
		static void computeBoundingBox( SubmeshAnimationBuffer & buffer )
		{
			if ( buffer.positions.empty() )
			{
				return;
			}

			auto & points = buffer.positions;
			castor::Point3f min{ points[0] };
			castor::Point3f max{ points[0] };

			if ( points.size() > 1 )
			{
				for ( auto & vertex : castor::makeArrayView( &points[1], points.data() + points.size() ) )
				{
					castor::Point3f cur{ vertex };
					max[0] = std::max( cur[0], max[0] );
					max[1] = std::max( cur[1], max[1] );
					max[2] = std::max( cur[2], max[2] );
					min[0] = std::min( cur[0], min[0] );
					min[1] = std::min( cur[1], min[1] );
					min[2] = std::min( cur[2], min[2] );
				}
			}

			buffer.boundingBox.load( min, max );
		}
	}

	castor::String const MorphComponent::Name = cuT( "morph" );

	MorphComponent::MorphComponent( Submesh & submesh
		, MorphFlags flags )
		: SubmeshComponent{ submesh, Name, uint32_t( std::hash< castor::String >{}( Name ) ) }
		, m_flags{ flags }
	{
		if ( checkFlag( m_flags, MorphFlag::ePositions ) )
		{
			++m_targetDataCount;
		}

		if ( checkFlag( m_flags, MorphFlag::eNormals ) )
		{
			++m_targetDataCount;
		}

		if ( checkFlag( m_flags, MorphFlag::eTangents ) )
		{
			++m_targetDataCount;
		}

		if ( checkFlag( m_flags, MorphFlag::eTexcoords0 ) )
		{
			++m_targetDataCount;
		}

		if ( checkFlag( m_flags, MorphFlag::eTexcoords1 ) )
		{
			++m_targetDataCount;
		}

		if ( checkFlag( m_flags, MorphFlag::eTexcoords2 ) )
		{
			++m_targetDataCount;
		}

		if ( checkFlag( m_flags, MorphFlag::eTexcoords3 ) )
		{
			++m_targetDataCount;
		}

		if ( checkFlag( m_flags, MorphFlag::eColours ) )
		{
			++m_targetDataCount;
		}

		m_pointsCount = submesh.getPointsCount();
	}

	SubmeshComponentSPtr MorphComponent::clone( Submesh & submesh )const
	{
		auto result = submesh.createComponent< MorphComponent >( m_flags );
		result->m_pointsCount = m_pointsCount;
		result->m_targets = m_targets;
		return std::static_pointer_cast< SubmeshComponent >( result );
	}

	void MorphComponent::addMorphTarget( SubmeshAnimationBuffer data )
	{
		smshcompmorph::computeBoundingBox( data );
		m_targets.emplace_back( std::move( data ) );
	}

	bool MorphComponent::doInitialise( RenderDevice const & device )
	{
		auto size = m_targetDataCount * m_pointsCount * MaxMorphTargets;

		if ( !m_buffer
			|| size > m_buffer.getCount() )
		{
			m_buffer = device.bufferPool->getBuffer< castor::Point4f >( VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
				, size
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
		}

		return true;
	}

	void MorphComponent::doCleanup( RenderDevice const & device )
	{
		if ( m_buffer )
		{
			device.bufferPool->putBuffer( m_buffer );
			m_buffer = {};
		}
	}

	void MorphComponent::doUpload()
	{
		if ( !m_buffer )
		{
			return;
		}

		uint32_t index{};
		auto bufferIt = m_buffer.getData().begin();
		auto stride = m_targetDataCount * MaxMorphTargets;

		while ( index < m_pointsCount )
		{
			auto bufIt = bufferIt;

			for ( auto & target : m_targets )
			{
				if ( auto posIt = target.positions.begin();
					posIt != target.positions.end() )
				{
					posIt += index;
					*bufIt = castor::Point4f{ *posIt };
					++bufIt;
				}

				if ( auto nmlIt = target.normals.begin();
					nmlIt != target.normals.end() )
				{
					nmlIt += index;
					*bufIt = castor::Point4f{ *nmlIt };
					++bufIt;
				}

				if ( auto tanIt = target.tangents.begin();
					tanIt != target.tangents.end() )
				{
					tanIt += index;
					*bufIt = castor::Point4f{ *tanIt };
					++bufIt;
				}

				if ( auto tx0It = target.texcoords0.begin();
					tx0It != target.texcoords0.end() )
				{
					tx0It += index;
					*bufIt = castor::Point4f{ *tx0It };
					++bufIt;
				}

				if ( auto tx1It = target.texcoords1.begin();
					tx1It != target.texcoords1.end() )
				{
					tx1It += index;
					*bufIt = castor::Point4f{ *tx1It };
					++bufIt;
				}

				if ( auto tx2It = target.texcoords2.begin();
					tx2It != target.texcoords2.end() )
				{
					tx2It += index;
					*bufIt = castor::Point4f{ *tx2It };
					++bufIt;
				}

				if ( auto tx3It = target.texcoords3.begin();
					tx3It != target.texcoords3.end() )
				{
					tx3It += index;
					*bufIt = castor::Point4f{ *tx3It };
					++bufIt;
				}

				if ( auto colIt = target.colours.begin();
					colIt != target.colours.end() )
				{
					colIt += index;
					*bufIt = castor::Point4f{ *colIt };
					++bufIt;
				}
			}

			bufferIt += stride;
			++index;
		}

		m_buffer.markDirty( VK_ACCESS_SHADER_READ_BIT
			, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT );
	}
}
