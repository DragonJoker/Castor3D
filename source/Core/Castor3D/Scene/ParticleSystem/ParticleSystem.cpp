#include "Castor3D/Scene/ParticleSystem/ParticleSystem.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Cache/BillboardCache.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/ParticleSystem/ComputeParticleSystem.hpp"
#include "Castor3D/Scene/ParticleSystem/CpuParticleSystem.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <CastorUtils/Design/Factory.hpp>

#include <ashespp/Core/Device.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		size_t getVkCount( ParticleFormat format )
		{
			switch ( format )
			{
			case ParticleFormat::eMat2f:
				return 2u;
			case ParticleFormat::eMat3f:
				return 3u;
			case ParticleFormat::eMat4f:
				return 4u;
			default:
				return 1u;
			}
		}

		ParticleFormat getComponent( ParticleFormat format )
		{
			switch ( format )
			{
			case ParticleFormat::eVec2i:
			case ParticleFormat::eVec3i:
			case ParticleFormat::eVec4i:
				return ParticleFormat::eInt;
			case ParticleFormat::eVec2ui:
			case ParticleFormat::eVec3ui:
			case ParticleFormat::eVec4ui:
				return ParticleFormat::eUInt;
			case ParticleFormat::eVec2f:
			case ParticleFormat::eVec3f:
			case ParticleFormat::eVec4f:
				return ParticleFormat::eFloat;
			case ParticleFormat::eMat2f:
				return ParticleFormat::eVec2f;
			case ParticleFormat::eMat3f:
				return ParticleFormat::eVec3f;
			case ParticleFormat::eMat4f:
				return ParticleFormat::eVec4f;
			default:
				return format;
			}
		}

		VkFormat getVkFormat( ParticleFormat format )
		{
			switch ( format )
			{
			case ParticleFormat::eInt:
				return VK_FORMAT_R32_SINT;
			case ParticleFormat::eVec2i:
				return VK_FORMAT_R32G32_SINT;
			case ParticleFormat::eVec3i:
				return VK_FORMAT_R32G32B32_SINT;
			case ParticleFormat::eVec4i:
				return VK_FORMAT_R32G32B32A32_SINT;
			case ParticleFormat::eUInt:
				return VK_FORMAT_R32_UINT;
			case ParticleFormat::eVec2ui:
				return VK_FORMAT_R32G32_UINT;
			case ParticleFormat::eVec3ui:
				return VK_FORMAT_R32G32B32_UINT;
			case ParticleFormat::eVec4ui:
				return VK_FORMAT_R32G32B32A32_UINT;
			case ParticleFormat::eFloat:
				return VK_FORMAT_R32_SFLOAT;
			case ParticleFormat::eVec2f:
			case ParticleFormat::eMat2f:
				return VK_FORMAT_R32G32_SFLOAT;
			case ParticleFormat::eVec3f:
			case ParticleFormat::eMat3f:
				return VK_FORMAT_R32G32B32_SFLOAT;
			case ParticleFormat::eVec4f:
			case ParticleFormat::eMat4f:
				return VK_FORMAT_R32G32B32A32_SFLOAT;
			default:
				assert( false );
				return VK_FORMAT_R32G32B32A32_SFLOAT;
			}
		}

		VkFormat getVkComponentFormat( ParticleFormat format )
		{
			return getVkFormat( getComponent( format ) );
		}

		VkDeviceSize getComponentSize( ParticleFormat format )
		{
			return getSize( getComponent( format ) );
		}
	}

	//*************************************************************************************************

	ParticleSystem::ParticleSystem( String const & name
		, Scene & scene
		, SceneNode & node
		, uint32_t count )
		: MovableObject{ name, scene, MovableType::eParticleEmitter, node }
		, m_particlesCount{ count }
		, m_csImpl{ std::make_unique< ComputeParticleSystem >( *this ) }
	{
	}

	ParticleSystem::~ParticleSystem()
	{
	}

	bool ParticleSystem::initialise( RenderDevice const & device )
	{
		ashes::VkVertexInputBindingDescriptionArray bindings
		{
			{ 1u, m_inputs.stride(), VK_VERTEX_INPUT_RATE_INSTANCE }
		};
		uint32_t index{ 2u };
		uint32_t stride{ 0u };
		ashes::VkVertexInputAttributeDescriptionArray attributes;

		for ( auto & attribute : m_inputs )
		{
			auto fmt = getVkFormat( attribute.m_dataType );
			auto compSize = uint32_t( getComponentSize( attribute.m_dataType ) );
			uint32_t offset = 0u;

			for ( size_t i = 0u; i < getVkCount( attribute.m_dataType ); ++i )
			{
				attributes.push_back( { index++, 1u, fmt, attribute.m_offset + offset } );
				offset += compSize;
			}

			stride += uint32_t( getSize( attribute.m_dataType ) );
		}

		m_particlesBillboard = std::make_unique< BillboardBase >( *getScene()
			, getScene()->getObjectRootNode().get()
			, std::make_unique< ashes::PipelineVertexInputStateCreateInfo >( 0u
				, bindings
				, attributes )
			, stride
			, makeVertexBufferBase( device
				, VkDeviceSize( stride ) * m_particlesCount
				, VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, "ParticleSystemBillboards" ) );
		m_particlesBillboard->setBillboardType( BillboardType::eSpherical );
		m_particlesBillboard->setDimensions( m_dimensions );
		m_particlesBillboard->setMaterial( m_material );
		bool result = m_particlesBillboard->initialise( device, m_particlesCount );
		getScene()->getBillboardListCache().registerElement( *m_particlesBillboard );

		if ( result )
		{
			result = m_csImpl->initialise( device );
		}

		if ( result )
		{
			log::info << cuT( "Using Compute Shader Particle System" ) << std::endl;
			m_impl = m_csImpl.get();
		}
		else
		{
			result = m_cpuImpl->initialise( device );

			if ( result )
			{
				log::info << cuT( "Using CPU Particle System" ) << std::endl;
				m_impl = m_cpuImpl.get();
			}
		}

		m_timer.getElapsed();
		return result;
	}

	void ParticleSystem::cleanup( RenderDevice const & device )
	{
		getScene()->getBillboardListCache().unregisterElement( *m_particlesBillboard );
		m_particlesBillboard->cleanup( device );
		m_particlesBillboard.reset();
		m_csImpl->cleanup( device );
		m_cpuImpl->cleanup( device );
		m_impl = nullptr;
	}

	void ParticleSystem::update( CpuUpdater & updater )
	{
		CU_Require( m_impl );
		auto time = std::chrono::duration_cast< Milliseconds >( m_timer.getElapsed() );

		if ( m_firstUpdate )
		{
			time = 0_ms;
		}

		time = updater.tslf > 0_ms
			? updater.tslf
			: time;
		m_time = time;
		m_totalTime += time;
		updater.time = m_time;
		updater.total = m_totalTime;
		m_impl->update( updater );
	}

	void ParticleSystem::update( GpuUpdater & updater )
	{
		CU_Require( m_impl );
		updater.time = m_time;
		updater.total = m_totalTime;
		m_activeParticlesCount = m_impl->update( updater );

		if ( getBillboards()->getCount() != m_activeParticlesCount )
		{
			getBillboards()->setCount( m_activeParticlesCount );
			getBillboards()->update( updater );
		}

		m_firstUpdate = false;
	}

	void ParticleSystem::setMaterial( MaterialRPtr material )
	{
		m_material = material;

		if ( m_particlesBillboard )
		{
			m_particlesBillboard->setMaterial( material );
		}
	}

	void ParticleSystem::setDimensions( Point2f const & dimensions )
	{
		m_dimensions = dimensions;

		if ( m_particlesBillboard )
		{
			m_particlesBillboard->setDimensions( dimensions );
		}
	}

	void ParticleSystem::setParticleType( castor::String const & value )
	{
		auto & factory = getScene()->getEngine()->getParticleFactory();
		m_particleType = value;

		if ( factory.isTypeRegistered( value ) )
		{
			m_cpuImpl = factory.create( value, *this );
		}
		else
		{
			CU_Exception( cuT( "Particle type [" ) + value + cuT( "] is not registered, make sure you've got the matching plug-in installed." ) );
		}
	}

	MaterialRPtr ParticleSystem::getMaterial()const
	{
		return m_material;
	}

	Point2f const & ParticleSystem::getDimensions()const
	{
		return m_dimensions;
	}

	void ParticleSystem::addParticleVariable( String const & name
		, ParticleFormat type
		, String const & defaultValue )
	{
		m_csImpl->addParticleVariable( name, type, defaultValue );
		m_cpuImpl->addParticleVariable( name, type, defaultValue );
		m_inputs.push_back( ParticleElementDeclaration{ name, ElementUsage::eUnknown, type, m_inputs.stride(), 1u } );
		m_defaultValues[cuT ("out_") + name] = defaultValue;
	}

	void ParticleSystem::setCSUpdateProgram( ShaderProgramSPtr program )
	{
		m_csImpl->setUpdateProgram( program );
	}

	void ParticleSystem::setCSGroupSizes( castor::Point3i sizes )
	{
		m_csImpl->setGroupSizes( sizes );
	}
}
