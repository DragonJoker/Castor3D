#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>

CU_ImplementSmartPtr( castor3d, MorphComponent )

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

		static std::pair< MorphFlags, uint32_t > computeMorphFlags( SubmeshAnimationBuffer const & buffer )
		{
			MorphFlags flags{};
			uint32_t count{};

			if ( !buffer.positions.empty() )
			{
				flags |= castor3d::MorphFlag::ePositions;
				++count;
			}

			if ( !buffer.normals.empty() )
			{
				flags |= castor3d::MorphFlag::eNormals;
				++count;
			}

			if ( !buffer.tangents.empty() )
			{
				flags |= castor3d::MorphFlag::eTangents;
				++count;
			}

			if ( !buffer.bitangents.empty() )
			{
				flags |= castor3d::MorphFlag::eBitangents;
				++count;
			}

			if ( !buffer.texcoords0.empty() )
			{
				flags |= castor3d::MorphFlag::eTexcoords0;
				++count;
			}

			if ( !buffer.texcoords1.empty() )
			{
				flags |= castor3d::MorphFlag::eTexcoords1;
				++count;
			}

			if ( !buffer.texcoords2.empty() )
			{
				flags |= castor3d::MorphFlag::eTexcoords2;
				++count;
			}

			if ( !buffer.texcoords3.empty() )
			{
				flags |= castor3d::MorphFlag::eTexcoords3;
				++count;
			}

			if ( !buffer.colours.empty() )
			{
				flags |= castor3d::MorphFlag::eColours;
				++count;
			}

			return { flags, count };
		}
	}

	castor::String const MorphComponent::TypeName = cuT( "morph" );

	MorphComponent::MorphComponent( Submesh & submesh )
		: SubmeshComponent{ submesh, TypeName }
	{
	}

	SubmeshComponentUPtr MorphComponent::clone( Submesh & submesh )const
	{
		auto result = castor::makeUnique< MorphComponent >( submesh );
		result->m_flags = m_flags;
		result->m_targetDataCount = m_targetDataCount;
		result->m_targets = m_targets;
		return castor::ptrRefCast< SubmeshComponent >( result );
	}

	void MorphComponent::addMorphTarget( SubmeshAnimationBuffer data )
	{
		if ( m_flags == MorphFlags{} )
		{
			auto [flags, count] = smshcompmorph::computeMorphFlags( data );
			m_flags = flags;
			m_targetDataCount = count;
		}

		smshcompmorph::computeBoundingBox( data );
		m_targets.emplace_back( std::move( data ) );
	}

	bool MorphComponent::doInitialise( RenderDevice const & device )
	{
		auto vertexCount = getOwner()->getPointsCount();
		auto size = m_targetDataCount * vertexCount * MaxMorphTargets;

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

	void MorphComponent::doUpload( UploadData & uploader )
	{
		if ( !m_buffer )
		{
			return;
		}

		uint32_t index{};
		auto vertexCount = getOwner()->getPointsCount();
		auto bufferIt = m_buffer.getData().begin();
		auto stride = m_targetDataCount * MaxMorphTargets;

		while ( index < vertexCount )
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

				if ( auto binIt = target.bitangents.begin();
					binIt != target.bitangents.end() )
				{
					binIt += index;
					*bufIt = castor::Point4f{ *binIt };
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
