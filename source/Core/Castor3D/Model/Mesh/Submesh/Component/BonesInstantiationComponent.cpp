#include "Castor3D/Model/Mesh/Submesh/Component/BonesInstantiationComponent.hpp"

#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
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
		: SubmeshComponent{ submesh, Name }
		, m_instantiation{ instantiation }
		, m_bones{ bones }
	{
	}

	BonesInstantiationComponent::~BonesInstantiationComponent()
	{
		cleanup();
	}

	void BonesInstantiationComponent::gather( MaterialSPtr material
		, ashes::BufferCRefArray & buffers
		, std::vector< uint64_t > & offsets
		, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts )
	{
	}

	bool BonesInstantiationComponent::doInitialise()
	{
		bool result = true;
		auto count = m_instantiation.getMaxRefCount();

		if ( count > m_instantiation.getThreshold() )
		{
			if ( !m_instancedBonesBuffer )
			{
				auto stride = uint32_t( sizeof( float ) * 16u * 400u );
				m_instancedBonesBuffer = std::make_unique< ShaderBuffer >( *getOwner()->getOwner()->getScene()->getEngine()
					, count * stride
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

	void BonesInstantiationComponent::doFill()
	{
		if ( m_instancedBonesBuffer )
		{
			auto count = m_instantiation.getMaxRefCount();
			auto stride = uint32_t( sizeof( float ) * 16u * 400u );

			if ( count > m_instantiation.getThreshold()
				&& ( !m_instancedBonesBuffer || m_instancedBonesBuffer->getSize() < count * stride ) )
			{
				m_instancedBonesBuffer = std::make_unique< ShaderBuffer >( *getOwner()->getOwner()->getScene()->getEngine()
					, count * stride
					, cuT( "InstancedBonesBuffer" ) );
			}
			else if ( count <= m_instantiation.getThreshold() )
			{
				m_instancedBonesBuffer.reset();
			}
		}
	}

	void BonesInstantiationComponent::doUpload()
	{
		if ( m_instancedBonesBuffer )
		{
			m_instancedBonesBuffer->update();
		}
	}
}
