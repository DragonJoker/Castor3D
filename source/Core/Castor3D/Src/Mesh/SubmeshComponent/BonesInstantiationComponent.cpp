#include "BonesInstantiationComponent.hpp"

#include "Mesh/Submesh.hpp"
#include "Scene/Scene.hpp"
#include "Shader/ShaderStorageBuffer.hpp"

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

	void BonesInstantiationComponent::gather( renderer::VertexBufferCRefArray & buffers )
	{
	}

	bool BonesInstantiationComponent::doInitialise()
	{
		bool result = true;

		if ( m_instantiation.getMaxRefCount() > 1 )
		{
			if ( !m_instancedBonesBuffer )
			{
				m_instancedBonesBuffer = std::make_unique< ShaderStorageBuffer >( *getOwner()->getScene()->getEngine() );
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
		if ( m_instancedBonesBuffer )
		{
			m_instancedBonesBuffer->cleanup();
		}
	}

	void BonesInstantiationComponent::doFill()
	{
		if ( m_instancedBonesBuffer )
		{
			auto count = m_instantiation.getMaxRefCount();

			if ( count )
			{
				auto & bonesBuffer = *m_instancedBonesBuffer;
				auto stride = uint32_t( sizeof( float ) * 16u * 400u );
				uint32_t size = count * stride;

				if ( bonesBuffer.getSize() != size )
				{
					bonesBuffer.resize( size );
				}
			}
			else
			{
				m_instancedBonesBuffer.reset();
			}

			m_instancedBonesBuffer->initialise( renderer::MemoryPropertyFlag::eHostVisible );
		}
	}

	void BonesInstantiationComponent::doUpload()
	{
		if ( m_instancedBonesBuffer )
		{
			m_instancedBonesBuffer->upload();
		}
	}
}
