#include "Castor3D/Model/Mesh/Submesh/Component/BonesInstantiationComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/ShaderBuffer.hpp"

using namespace castor;

//*************************************************************************************************

namespace castor3d
{
	String const BonesInstantiationComponent::Name = cuT( "instantiated_bones" );

	BonesInstantiationComponent::BonesInstantiationComponent( Submesh & submesh
		, InstantiationComponent const & instantiation
		, BonesComponent const & bones )
		: SubmeshComponent{ submesh, Name, uint32_t( std::hash< std::string >{}( Name ) ) }
		, m_instantiation{ instantiation }
		, m_bones{ bones }
	{
	}

	void BonesInstantiationComponent::gather( ShaderFlags const & flags
		, MaterialRPtr material
		, ashes::BufferCRefArray & buffers
		, std::vector< uint64_t > & offsets
		, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
		, uint32_t instanceMult
		, TextureFlagsArray const & mask
		, uint32_t & currentLocation )
	{
	}

	SubmeshComponentSPtr BonesInstantiationComponent::clone( Submesh & submesh )const
	{
		auto result = std::make_shared< BonesInstantiationComponent >( submesh
			, *submesh.getComponent< InstantiationComponent >()
			, *submesh.getComponent< BonesComponent >() );
		return std::static_pointer_cast< SubmeshComponent >( result );
	}

	bool BonesInstantiationComponent::doInitialise( RenderDevice const & device )
	{
		bool result = true;

		if ( m_instantiation.isInstanced()
			&& getOwner()->getOwner()->getScene()->getEngine()->getRenderSystem()->getGpuInformations().hasShaderStorageBuffers() )
		{
			if ( !m_instancedBonesBuffer )
			{
				auto count = m_instantiation.getMaxRefCount();
				auto stride = uint32_t( sizeof( float ) * 16u * 400u );
				auto size = count * stride * getOwner()->getOwner()->getScene()->getDirectionalShadowCascades();
				m_instancedBonesBuffer = castor::makeUnique< ShaderBuffer >( *getOwner()->getOwner()->getScene()->getEngine()
					, device
					, size
					, cuT( "InstancedBonesBuffer" ) );
			}
		}
		else
		{
			m_instancedBonesBuffer.reset();
		}

		return result;
	}

	void BonesInstantiationComponent::doCleanup()
	{
		m_instancedBonesBuffer.reset();
	}

	void BonesInstantiationComponent::doUpload()
	{
		if ( m_instancedBonesBuffer )
		{
			m_instancedBonesBuffer->update();
		}
	}
}
