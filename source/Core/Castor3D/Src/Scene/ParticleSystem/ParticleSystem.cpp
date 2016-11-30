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
		bool l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "particle_system \"" ) + p_obj.GetName() + cuT( "\"\n" ) ) > 0
						&& p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
		MovableObject::TextWriter::CheckError( l_return, "ParticleSystem name" );

		if ( l_return )
		{
			l_return = MovableObject::TextWriter{ m_tabs }( p_obj, p_file );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "\tmaterial \"" ) + p_obj.GetMaterial()->GetName() + cuT( "\"\n" ) ) > 0;
			MovableObject::TextWriter::CheckError( l_return, "ParticleSystem material" );
		}

		if ( l_return )
		{
			l_return = p_file.Print( 256, cuT( "%s\tdimensions %d %d\n" ), m_tabs.c_str(), p_obj.GetDimensions().width(), p_obj.GetDimensions().height() ) > 0;
			MovableObject::TextWriter::CheckError( l_return, "ParticleSystem dimensions" );
		}

		if ( l_return )
		{
			l_return = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
		}

		return l_return;
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
		auto & l_engine = *GetScene()->GetEngine();
		m_particlesBillboard = std::make_unique< BillboardBase >(
			*GetScene(),
			GetScene()->GetObjectRootNode(),
			std::make_shared< VertexBuffer >( l_engine, m_billboardInputs ) );
		m_particlesBillboard->SetBillboardType( BillboardType::eSpherical );
		m_particlesBillboard->SetDimensions( m_dimensions );
		m_particlesBillboard->SetMaterial( m_material.lock() );
		m_particlesBillboard->SetCenterOffset( m_centerOffset );
		bool l_return = m_particlesBillboard->Initialise( uint32_t( m_particlesCount ) );

		if ( l_return )
		{
			l_return = m_csImpl->Initialise();
		}

		if ( l_return )
		{
			m_impl = m_csImpl.get();
		}
		else
		{
			l_return = m_tfImpl->Initialise();

			if ( l_return )
			{
				m_impl = m_tfImpl.get();
			}
			else
			{
				l_return = m_cpuImpl->Initialise();

				if ( l_return )
				{
					m_impl = m_cpuImpl.get();
				}
			}
		}

		m_timer.TimeMs();
		return l_return;
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
		auto l_time = float( m_timer.TimeMs() );

		if ( m_firstUpdate )
		{
			l_time = 0.0f;
		}

		m_totalTime += l_time;
		m_activeParticlesCount = m_impl->Update( l_time, m_totalTime );
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
		auto & l_factory = GetScene()->GetEngine()->GetParticleFactory();

		if ( l_factory.IsRegistered( p_value ) )
		{
			m_cpuImpl = l_factory.Create( p_value, *this );
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
