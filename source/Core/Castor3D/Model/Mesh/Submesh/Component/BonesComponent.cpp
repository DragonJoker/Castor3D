#include "Castor3D/Model/Mesh/Submesh/Component/BonesComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Skeleton/BonedVertex.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/ShaderBuffer.hpp"

using namespace castor;

namespace castor3d
{
	String const BonesComponent::Name = cuT( "bones" );

	BonesComponent::BonesComponent( Submesh & submesh )
		: SubmeshComponent{ submesh, Name, BindingPoint }
	{
	}

	void BonesComponent::addBoneDatas( VertexBoneData const * const begin
		, VertexBoneData const * const end )
	{
		m_bones.insert( m_bones.end(), begin, end );
	}

	SkeletonSPtr BonesComponent::getSkeleton()const
	{
		return getOwner()->getParent().getSkeleton();
	}

	void BonesComponent::gather( ShaderFlags const & shaderFlags
		, ProgramFlags const & programFlags
		, MaterialRPtr material
		, ashes::BufferCRefArray & buffers
		, std::vector< uint64_t > & offsets
		, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
		, uint32_t instanceMult
		, TextureFlagsArray const & mask
		, uint32_t & currentLocation )
	{
	}

	SubmeshComponentSPtr BonesComponent::clone( Submesh & submesh )const
	{
		auto result = std::make_shared< BonesComponent >( submesh );
		result->m_bones = m_bones;
		return std::static_pointer_cast< SubmeshComponent >( result );
	}

	void BonesComponent::addBoneDatas( std::vector< VertexBoneData > const & boneData )
	{
		addBoneDatas( boneData.data(), boneData.data() + boneData.size() );
	}

	bool BonesComponent::doInitialise( RenderDevice const & device )
	{
		if ( !m_bonesBuffer || m_bonesBuffer.getCount() != m_bones.size() )
		{
			m_bonesBuffer = device.bufferPool->getBuffer< VertexBoneData >( VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
				, uint32_t( m_bones.size() )
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT );
		}

		if ( !m_transformsBuffer )
		{
			auto & scene = *getOwner()->getOwner()->getScene();
			auto & engine = *device.renderSystem.getEngine();
			auto stride = uint32_t( sizeof( float ) * 16u * 400u );
			auto size = stride * scene.getDirectionalShadowCascades();
			m_transformsBuffer = castor::makeUnique< ShaderBuffer >( engine
				, device
				, size
				, cuT( "SkinningTransformsBuffer" ) );
			engine.registerBuffer( *m_transformsBuffer );
		}

		return bool( m_bonesBuffer );
	}

	void BonesComponent::doCleanup( RenderDevice const & device )
	{
		if ( m_transformsBuffer )
		{
			auto & scene = *getOwner()->getOwner()->getScene();
			auto & engine = *scene.getEngine();
			engine.unregisterBuffer( *m_transformsBuffer );
			m_transformsBuffer.reset();
		}

		if ( m_bonesBuffer )
		{
			device.bufferPool->putBuffer( m_bonesBuffer );
			m_bonesBuffer = {};
		}
	}

	void BonesComponent::doUpload()
	{
		auto count = uint32_t( m_bones.size() );

		if ( count && m_bonesBuffer )
		{
			if ( auto * buffer = m_bonesBuffer.lock() )
			{
				std::copy( m_bones.begin(), m_bones.end(), buffer );
				m_bonesBuffer.flush();
				m_bonesBuffer.unlock();
			}

			//m_bones.clear();
			//m_bonesData.clear();
		}
	}
}
