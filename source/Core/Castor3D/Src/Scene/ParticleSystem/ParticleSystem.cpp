#include "ParticleSystem.hpp"

#include "ComputeParticleSystem.hpp"
#include "CpuParticleSystem.hpp"
#include "TransformFeedbackParticleSystem.hpp"

#include "Engine.hpp"

#include "Material/Material.hpp"
#include "Mesh/Buffer/BufferElementGroup.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Scene.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace castor;

namespace castor3d
{
	ParticleSystem::TextWriter::TextWriter( String const & p_tabs )
		: MovableObject::TextWriter{ p_tabs }
	{
	}

	bool ParticleSystem::TextWriter::operator()( ParticleSystem const & p_obj, castor::TextFile & p_file )
	{
		bool result = p_file.writeText( cuT( "\n" ) + m_tabs + cuT( "particle_system \"" ) + p_obj.getName() + cuT( "\"\n" ) ) > 0
						&& p_file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
		MovableObject::TextWriter::checkError( result, "ParticleSystem name" );

		if ( result )
		{
			result = MovableObject::TextWriter{ m_tabs + cuT( "\t" ) }( p_obj, p_file );
		}

		if ( result )
		{
			result = p_file.print( 256, cuT( "%s\tparticles_count %d\n" ), m_tabs.c_str(), uint32_t( p_obj.getMaxParticlesCount() ) ) > 0;
			MovableObject::TextWriter::checkError( result, "ParticleSystem particles count" );
		}

		if ( result )
		{
			result = p_file.print( 256, cuT( "%s\tdimensions %f %f\n" ), m_tabs.c_str(), p_obj.getDimensions()[0], p_obj.getDimensions()[1] ) > 0;
			MovableObject::TextWriter::checkError( result, "ParticleSystem dimensions" );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "\tmaterial \"" ) + p_obj.getMaterial()->getName() + cuT( "\"\n" ) ) > 0;
			MovableObject::TextWriter::checkError( result, "ParticleSystem material" );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "\tparticle\n" ) ) > 0
					   && p_file.writeText( m_tabs + cuT( "\t{\n" ) ) > 0;
			MovableObject::TextWriter::checkError( result, "ParticleSystem particle" );

			if ( result && !p_obj.getParticleType().empty() )
			{
				result = p_file.writeText( m_tabs + cuT( "\t\ttype \"" ) + p_obj.getParticleType() + cuT( "\"\n" ) ) > 0;
				MovableObject::TextWriter::checkError( result, "ParticleSystem particle" );
			}

			auto values = p_obj.getDefaultValues();

			for ( auto & var : p_obj.getParticleVariables() )
			{
				if ( result )
				{
					result = p_file.writeText( m_tabs + cuT( "\t\tvariable \"" ) + var.m_name + cuT( "\" " ) + castor3d::getName( var.m_dataType ) + cuT( " " ) + values[cuT( "out_" ) + var.m_name] + cuT( "\n" ) ) > 0;
					MovableObject::TextWriter::checkError( result, "ParticleSystem particle variable" );
				}
			}

			if ( result )
			{
				result = p_file.writeText( m_tabs + cuT( "\t}\n" ) ) > 0;
				MovableObject::TextWriter::checkError( result, "ParticleSystem particle" );
			}
		}

		if ( result && p_obj.m_tfImpl->hasUpdateProgram() )
		{
			result = ShaderProgram::TextWriter( m_tabs + cuT( "\t" ), cuT( "tf_shader_program" ) )( p_obj.m_tfImpl->getUpdateProgram(), p_file );
		}

		if ( result && p_obj.m_csImpl->hasUpdateProgram() )
		{
			result = ShaderProgram::TextWriter( m_tabs + cuT( "\t" ), cuT( "cs_shader_program" ) )( p_obj.m_csImpl->getUpdateProgram(), p_file );
		}

		if ( result )
		{
			result = p_file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return result;
	}

	//*************************************************************************************************

	ParticleSystem::ParticleSystem( String const & p_name, Scene & p_scene, SceneNodeSPtr p_parent, size_t p_count )
		: MovableObject{ p_name, p_scene, MovableType::eParticleEmitter, p_parent }
		, m_particlesCount{ p_count }
		, m_tfImpl{ std::make_unique< TransformFeedbackParticleSystem >( *this ) }
		, m_csImpl{ std::make_unique< ComputeParticleSystem >( *this ) }
	{
	}

	ParticleSystem::~ParticleSystem()
	{
	}

	bool ParticleSystem::initialise()
	{
		auto & engine = *getScene()->getEngine();
		m_particlesBillboard = std::make_unique< BillboardBase >(
			*getScene(),
			getScene()->getObjectRootNode(),
			std::make_shared< VertexBuffer >( engine, m_billboardInputs ) );
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
			result = m_tfImpl->initialise();

			if ( result )
			{
				Logger::logInfo( cuT( "Using Transform Feedback Particle System" ) );
				m_impl = m_tfImpl.get();
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
		}

		m_timer.getElapsed();
		return result;
	}

	void ParticleSystem::cleanup()
	{
		m_particlesBillboard->cleanup();
		m_particlesBillboard.reset();
		m_csImpl->cleanup();
		m_tfImpl->cleanup();
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

	void ParticleSystem::addParticleVariable( castor::String const & p_name, ElementType p_type, castor::String const & p_defaultValue )
	{
		m_csImpl->addParticleVariable( p_name, p_type, p_defaultValue );
		m_tfImpl->addParticleVariable( p_name, p_type, p_defaultValue );
		m_cpuImpl->addParticleVariable( p_name, p_type, p_defaultValue );

		if ( p_name == cuT( "center" )
			 || p_name == ShaderProgram::Position )
		{
			m_billboardInputs.push_back( BufferElementDeclaration{ cuT( "center" ), 0u, p_type, m_billboardInputs.stride(), 1u } );
			m_centerOffset = m_billboardInputs.stride();
		}
		else
		{
			m_billboardInputs.push_back( BufferElementDeclaration{ p_name, 0u, p_type, m_billboardInputs.stride(), 1u } );
		}

		m_inputs.push_back( BufferElementDeclaration{ p_name, 0u, p_type, m_billboardInputs.stride(), 1u } );
		m_defaultValues[cuT ("out_") + p_name] = p_defaultValue;
	}

	void ParticleSystem::setTFUpdateProgram( ShaderProgramSPtr p_program )
	{
		m_tfImpl->setUpdateProgram( p_program );
	}

	void ParticleSystem::setCSUpdateProgram( ShaderProgramSPtr p_program )
	{
		m_csImpl->setUpdateProgram( p_program );
	}
}
