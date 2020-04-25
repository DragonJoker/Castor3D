#include "Castor3D/Scene/ParticleSystem/ParticleSystem.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/BillboardUboPools.hpp"
#include "Castor3D/Material/Material.hpp"
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

	ParticleSystem::TextWriter::TextWriter( String const & tabs )
		: MovableObject::TextWriter{ tabs }
	{
	}

	bool ParticleSystem::TextWriter::operator()( ParticleSystem const & obj, castor::TextFile & file )
	{
		bool result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "particle_system \"" ) + obj.getName() + cuT( "\"\n" ) ) > 0
						&& file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
		MovableObject::TextWriter::checkError( result, "ParticleSystem name" );

		if ( result )
		{
			result = MovableObject::TextWriter{ m_tabs + cuT( "\t" ) }( obj, file );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\tparticles_count " ) + string::toString( obj.getMaxParticlesCount(), std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
			MovableObject::TextWriter::checkError( result, "ParticleSystem particles count" );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\tdimensions " )
				+ string::toString( obj.getDimensions()[0], std::locale{ "C" } ) + cuT( " " )
				+ string::toString( obj.getDimensions()[1], std::locale{ "C" } ) + cuT( "\n" ) ) > 0;
			MovableObject::TextWriter::checkError( result, "ParticleSystem dimensions" );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\tmaterial \"" ) + obj.getMaterial()->getName() + cuT( "\"\n" ) ) > 0;
			MovableObject::TextWriter::checkError( result, "ParticleSystem material" );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "\tparticle\n" ) ) > 0
					   && file.writeText( m_tabs + cuT( "\t{\n" ) ) > 0;
			MovableObject::TextWriter::checkError( result, "ParticleSystem particle" );

			if ( result && !obj.getParticleType().empty() )
			{
				result = file.writeText( m_tabs + cuT( "\t\ttype \"" ) + obj.getParticleType() + cuT( "\"\n" ) ) > 0;
				MovableObject::TextWriter::checkError( result, "ParticleSystem particle" );
			}

			auto values = obj.getDefaultValues();

			for ( auto & var : obj.getParticleVariables() )
			{
				if ( result )
				{
					result = file.writeText( m_tabs + cuT( "\t\tvariable \"" ) + var.m_name + cuT( "\" " ) + castor3d::getName( var.m_dataType ) + cuT( " " ) + values[cuT( "out_" ) + var.m_name] + cuT( "\n" ) ) > 0;
					MovableObject::TextWriter::checkError( result, "ParticleSystem particle variable" );
				}
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "\t}\n" ) ) > 0;
				MovableObject::TextWriter::checkError( result, "ParticleSystem particle" );
			}
		}

		if ( result && obj.m_csImpl->hasUpdateProgram() )
		{
			result = ShaderProgram::TextWriter( m_tabs + cuT( "\t" ), cuT( "cs_shader_program" ) )( obj.m_csImpl->getUpdateProgram(), file );
		}

		if ( result )
		{
			result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
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

	bool ParticleSystem::initialise()
	{
		auto & device = getCurrentRenderDevice( *this );
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
		m_particlesBillboard->setMaterial( m_material.lock() );
		bool result = m_particlesBillboard->initialise( m_particlesCount );
		getScene()->getBillboardPools().registerElement( *m_particlesBillboard );

		if ( result )
		{
			result = m_csImpl->initialise();
		}

		if ( result )
		{
			log::info << cuT( "Using Compute Shader Particle System" ) << std::endl;
			m_impl = m_csImpl.get();
		}
		else
		{
			result = m_cpuImpl->initialise();

			if ( result )
			{
				log::info << cuT( "Using CPU Particle System" ) << std::endl;
				m_impl = m_cpuImpl.get();
			}
		}

		m_timer.getElapsed();
		return result;
	}

	void ParticleSystem::cleanup()
	{
		getScene()->getBillboardPools().unregisterElement( *m_particlesBillboard );
		m_particlesBillboard->cleanup();
		m_particlesBillboard.reset();
		m_csImpl->cleanup();
		m_cpuImpl->cleanup();
		m_impl = nullptr;
	}

	void ParticleSystem::update( RenderPassTimer & timer
		, uint32_t index )
	{
		CU_Require( m_impl );
		auto time = std::chrono::duration_cast< Milliseconds >( m_timer.getElapsed() );

		if ( m_firstUpdate )
		{
			time = 0_ms;
		}

		m_totalTime += time;
		m_activeParticlesCount = m_impl->update( timer, time, m_totalTime, index );

		if ( getBillboards()->getCount() != m_activeParticlesCount )
		{
			getBillboards()->setCount( m_activeParticlesCount );
			getBillboards()->update();
		}

		m_firstUpdate = false;
	}

	void ParticleSystem::setMaterial( MaterialSPtr material )
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

	MaterialSPtr ParticleSystem::getMaterial()const
	{
		return m_material.lock();
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
		m_inputs.push_back( ParticleElementDeclaration{ name, 0u, type, m_inputs.stride(), 1u } );
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
