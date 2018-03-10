#include "BonesInstantiationComponent.hpp"

#include "Mesh/Submesh.hpp"
#include "Scene/Scene.hpp"
#include "Shader/ShaderBuffer.hpp"

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

	void BonesInstantiationComponent::gather( renderer::BufferCRefArray & buffers
		, std::vector< uint64_t > & offsets
		, renderer::VertexLayoutCRefArray & layouts )
	{
	}

	bool BonesInstantiationComponent::doInitialise()
	{
		return true;
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

			if ( count > 1
				&& ( !m_instancedBonesBuffer || m_instancedBonesBuffer->getSize() < count * stride ) )
			{
				m_instancedBonesBuffer = std::make_unique< ShaderBuffer >( *getOwner()->getScene()->getEngine()
					, count * stride );
			}
			else
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
