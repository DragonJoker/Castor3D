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

using namespace Castor;

namespace Castor3D
{
	ParticleSystem::TextWriter::TextWriter( String const & p_tabs )
		: MovableObject::TextWriter{ p_tabs }
	{
	}

	bool ParticleSystem::TextWriter::operator()( ParticleSystem const & p_obj, Castor::TextFile & p_file )
	{
		bool result = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "particle_system \"" ) + p_obj.GetName() + cuT( "\"\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
		MovableObject::TextWriter::CheckError( result, "ParticleSystem name" );

		if ( result )
		{
			result = MovableObject::TextWriter{ m_tabs + cuT( "\t" ) }( p_obj, p_file );
		}

		if ( result )
		{
			result = p_file.Print( 256, cuT( "%s\tparticles_count %d\n" ), m_tabs.c_str(), uint32_t( p_obj.GetMaxParticlesCount() ) ) > 0;
			MovableObject::TextWriter::CheckError( result, "ParticleSystem particles count" );
		}

		if ( result )
		{
			result = p_file.Print( 256, cuT( "%s\tdimensions %d %d\n" ), m_tabs.c_str(), p_obj.GetDimensions().width(), p_obj.GetDimensions().height() ) > 0;
			MovableObject::TextWriter::CheckError( result, "ParticleSystem dimensions" );
		}

		if ( result )
		{
			result = p_file.WriteText( m_tabs + cuT( "\tmaterial \"" ) + p_obj.GetMaterial()->GetName() + cuT( "\"\n" ) ) > 0;
			MovableObject::TextWriter::CheckError( result, "ParticleSystem material" );
		}

		if ( result )
		{
			result = p_file.WriteText( m_tabs + cuT( "\tparticle\n" ) ) > 0
					   && p_file.WriteText( m_tabs + cuT( "\t{\n" ) ) > 0;
			MovableObject::TextWriter::CheckError( result, "ParticleSystem particle" );

			if ( result && !p_obj.GetParticleType().empty() )
			{
				result = p_file.WriteText( m_tabs + cuT( "\t\ttype \"" ) + p_obj.GetParticleType() + cuT( "\"\n" ) ) > 0;
				MovableObject::TextWriter::CheckError( result, "ParticleSystem particle" );
			}

			auto values = p_obj.GetDefaultValues();

			for ( auto & var : p_obj.GetParticleVariables() )
			{
				if ( result )
				{
					result = p_file.WriteText( m_tabs + cuT( "\t\tvariable \"" ) + var.m_name + cuT( "\" " ) + Castor3D::GetName( var.m_dataType ) + cuT( " " ) + values[cuT( "out_" ) + var.m_name] + cuT( "\n" ) ) > 0;
					MovableObject::TextWriter::CheckError( result, "ParticleSystem particle variable" );
				}
			}

			if ( result )
			{
				result = p_file.WriteText( m_tabs + cuT( "\t}\n" ) ) > 0;
				MovableObject::TextWriter::CheckError( result, "ParticleSystem particle" );
			}
		}

		if ( result && p_obj.m_tfImpl->HasUpdateProgram() )
		{
			result = ShaderProgram::TextWriter( m_tabs + cuT( "\t" ), cuT( "tf_shader_program" ) )( p_obj.m_tfImpl->GetUpdateProgram(), p_file );
		}

		if ( result && p_obj.m_csImpl->HasUpdateProgram() )
		{
			result = ShaderProgram::TextWriter( m_tabs + cuT( "\t" ), cuT( "cs_shader_program" ) )( p_obj.m_csImpl->GetUpdateProgram(), p_file );
		}

		if ( result )
		{
			result = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
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

	bool ParticleSystem::Initialise()
	{
		auto & engine = *GetScene()->GetEngine();
		m_particlesBillboard = std::make_unique< BillboardBase >(
			*GetScene(),
			GetScene()->GetObjectRootNode(),
			std::make_shared< VertexBuffer >( engine, m_billboardInputs ) );
		m_particlesBillboard->SetBillboardType( BillboardType::eSpherical );
		m_particlesBillboard->SetDimensions( m_dimensions );
		m_particlesBillboard->SetMaterial( m_material.lock() );
		m_particlesBillboard->SetCenterOffset( m_centerOffset );
		bool result = m_particlesBillboard->Initialise( uint32_t( m_particlesCount ) );

		if ( result )
		{
			result = m_csImpl->Initialise();
		}

		if ( result )
		{
			Logger::LogInfo( cuT( "Using Compute Shader Particle System" ) );
			m_impl = m_csImpl.get();
		}
		else
		{
			result = m_tfImpl->Initialise();

			if ( result )
			{
				Logger::LogInfo( cuT( "Using Transform Feedback Particle System" ) );
				m_impl = m_tfImpl.get();
			}
			else
			{
				result = m_cpuImpl->Initialise();

				if ( result )
				{
					Logger::LogInfo( cuT( "Using CPU Particle System" ) );
					m_impl = m_cpuImpl.get();
				}
			}
		}

		m_timer.Time();
		return result;
	}

	void ParticleSystem::Cleanup()
	{
		m_particlesBillboard->Cleanup();
		m_particlesBillboard.reset();
		m_csImpl->Cleanup();
		m_tfImpl->Cleanup();
		m_cpuImpl->Cleanup();
		m_impl = nullptr;
	}

	void ParticleSystem::Update()
	{
		REQUIRE( m_impl );
		auto time = std::chrono::duration_cast< Castor::Milliseconds >( m_timer.Time() );

		if ( m_firstUpdate )
		{
			time = 0_ms;
		}

		m_totalTime += time;
		m_activeParticlesCount = m_impl->Update( time, m_totalTime );
		GetBillboards()->SetCount( m_activeParticlesCount );
		m_firstUpdate = false;
	}

	void ParticleSystem::SetMaterial( MaterialSPtr p_material )
	{
		m_material = p_material;

		if ( m_particlesBillboard )
		{
			m_particlesBillboard->SetMaterial( p_material );
		}
	}

	void ParticleSystem::SetDimensions( Size const & p_dimensions )
	{
		m_dimensions = p_dimensions;

		if ( m_particlesBillboard )
		{
			m_particlesBillboard->SetDimensions( p_dimensions );
		}
	}

	void ParticleSystem::SetParticleType( Castor::String const & p_value )
	{
		auto & factory = GetScene()->GetEngine()->GetParticleFactory();
		m_particleType = p_value;

		if ( factory.IsRegistered( p_value ) )
		{
			m_cpuImpl = factory.Create( p_value, *this );
		}
		else
		{
			CASTOR_EXCEPTION( cuT( "Particle type [" ) + p_value + cuT( "] is not registered, make sure you've got the matching plug-in installed." ) );
		}
	}

	MaterialSPtr ParticleSystem::GetMaterial()const
	{
		return m_material.lock();
	}

	Size const & ParticleSystem::GetDimensions()const
	{
		return m_dimensions;
	}

	void ParticleSystem::AddParticleVariable( Castor::String const & p_name, ElementType p_type, Castor::String const & p_defaultValue )
	{
		m_csImpl->AddParticleVariable( p_name, p_type, p_defaultValue );
		m_tfImpl->AddParticleVariable( p_name, p_type, p_defaultValue );
		m_cpuImpl->AddParticleVariable( p_name, p_type, p_defaultValue );

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

	void ParticleSystem::SetTFUpdateProgram( ShaderProgramSPtr p_program )
	{
		m_tfImpl->SetUpdateProgram( p_program );
	}

	void ParticleSystem::SetCSUpdateProgram( ShaderProgramSPtr p_program )
	{
		m_csImpl->SetUpdateProgram( p_program );
	}
}
