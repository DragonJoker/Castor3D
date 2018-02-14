#include "ParticleSystem.hpp"

#include "Engine.hpp"
#include "Material/Material.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Scene.hpp"
#include "Scene/ParticleSystem/ComputeParticleSystem.hpp"
#include "Scene/ParticleSystem/CpuParticleSystem.hpp"

#include <Core/Device.hpp>
#include <Pipeline/VertexLayout.hpp>

using namespace castor;

namespace castor3d
{
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
			result = file.print( 256, cuT( "%s\tparticles_count %d\n" ), m_tabs.c_str(), uint32_t( obj.getMaxParticlesCount() ) ) > 0;
			MovableObject::TextWriter::checkError( result, "ParticleSystem particles count" );
		}

		if ( result )
		{
			result = file.print( 256, cuT( "%s\tdimensions %f %f\n" ), m_tabs.c_str(), obj.getDimensions()[0], obj.getDimensions()[1] ) > 0;
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
					result = file.writeText( m_tabs + cuT( "\t\tvariable \"" ) + var.m_name + cuT( "\" " ) + renderer::getName( var.m_dataType ) + cuT( " " ) + values[cuT( "out_" ) + var.m_name] + cuT( "\n" ) ) > 0;
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
			//result = ShaderProgram::TextWriter( m_tabs + cuT( "\t" ), cuT( "cs_shader_program" ) )( obj.m_csImpl->getUpdateProgram(), file );
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
		, SceneNodeSPtr parent
		, size_t count )
		: MovableObject{ name, scene, MovableType::eParticleEmitter, parent }
		, m_particlesCount{ count }
		, m_csImpl{ std::make_unique< ComputeParticleSystem >( *this ) }
	{
	}

	ParticleSystem::~ParticleSystem()
	{
	}

	bool ParticleSystem::initialise()
	{
		auto & device = *getScene()->getEngine()->getRenderSystem()->getCurrentDevice();
		auto vertexLayout = device.createVertexLayout( 0u, m_inputs.stride() );
		uint32_t index{ 0u };

		for ( auto & attribute : m_inputs )
		{
			vertexLayout->createAttribute( index++, attribute.m_dataType, attribute.m_offset );
		}

		m_particlesBillboard = std::make_unique< BillboardBase >( *getScene()
			, getScene()->getObjectRootNode()
			, std::move( vertexLayout ) );
		m_particlesBillboard->setBillboardType( BillboardType::eSpherical );
		m_particlesBillboard->setDimensions( m_dimensions );
		m_particlesBillboard->setMaterial( m_material.lock() );
		m_particlesBillboard->setCenterOffset( m_centerOffset );
		bool result = m_particlesBillboard->initialise( uint32_t( m_particlesCount ) );

		if ( result )
		{
			result = m_csImpl->initialise();
		}

		if ( result )
		{
			Logger::logInfo( cuT( "Using Compute Shader Particle System" ) );
			m_impl = m_csImpl.get();
		}
		else
		{
			result = m_cpuImpl->initialise();

			if ( result )
			{
				Logger::logInfo( cuT( "Using CPU Particle System" ) );
				m_impl = m_cpuImpl.get();
			}
		}

		m_timer.getElapsed();
		return result;
	}

	void ParticleSystem::cleanup()
	{
		m_particlesBillboard->cleanup();
		m_particlesBillboard.reset();
		m_csImpl->cleanup();
		m_cpuImpl->cleanup();
		m_impl = nullptr;
	}

	void ParticleSystem::update()
	{
		REQUIRE( m_impl );
		auto time = std::chrono::duration_cast< Milliseconds >( m_timer.getElapsed() );

		if ( m_firstUpdate )
		{
			time = 0_ms;
		}

		m_totalTime += time;
		m_activeParticlesCount = m_impl->update( time, m_totalTime );
		getBillboards()->setCount( m_activeParticlesCount );
		m_firstUpdate = false;
	}

	void ParticleSystem::setMaterial( MaterialSPtr p_material )
	{
		m_material = p_material;

		if ( m_particlesBillboard )
		{
			m_particlesBillboard->setMaterial( p_material );
		}
	}

	void ParticleSystem::setDimensions( Point2f const & p_dimensions )
	{
		m_dimensions = p_dimensions;

		if ( m_particlesBillboard )
		{
			m_particlesBillboard->setDimensions( p_dimensions );
		}
	}

	void ParticleSystem::setParticleType( castor::String const & p_value )
	{
		auto & factory = getScene()->getEngine()->getParticleFactory();
		m_particleType = p_value;

		if ( factory.isTypeRegistered( p_value ) )
		{
			m_cpuImpl = factory.create( p_value, *this );
		}
		else
		{
			CASTOR_EXCEPTION( cuT( "Particle type [" ) + p_value + cuT( "] is not registered, make sure you've got the matching plug-in installed." ) );
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

	void ParticleSystem::addParticleVariable( castor::String const & p_name, renderer::AttributeFormat p_type, castor::String const & p_defaultValue )
	{
		m_csImpl->addParticleVariable( p_name, p_type, p_defaultValue );
		m_cpuImpl->addParticleVariable( p_name, p_type, p_defaultValue );

		if ( p_name == cuT( "center" )
			 || p_name == cuT( "position" ) )
		{
			m_billboardInputs.push_back( ParticleElementDeclaration{ cuT( "center" ), 0u, p_type, m_billboardInputs.stride(), 1u } );
			m_centerOffset = m_billboardInputs.stride();
		}
		else
		{
			m_billboardInputs.push_back( ParticleElementDeclaration{ p_name, 0u, p_type, m_billboardInputs.stride(), 1u } );
		}

		m_inputs.push_back( ParticleElementDeclaration{ p_name, 0u, p_type, m_billboardInputs.stride(), 1u } );
		m_defaultValues[cuT ("out_") + p_name] = p_defaultValue;
	}

	void ParticleSystem::setCSUpdateProgram( renderer::ShaderProgram const & program )
	{
		m_csImpl->setUpdateProgram( program );
	}
}
