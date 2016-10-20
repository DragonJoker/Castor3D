#include "ParticleSystem.hpp"

#include "Engine.hpp"

#include "Material/Material.hpp"
#include "Mesh/Buffer/CpuTransformBuffer.hpp"
#include "Scene/Scene.hpp"

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

	ParticleSystem::ParticleSystem( String const & p_name, Scene & p_scene, SceneNodeSPtr p_parent, RenderSystem & p_renderSystem, size_t p_count )
		: MovableObject{ p_name, p_scene, MovableType::ParticleSystem, p_parent }
		, m_count{ p_count }
		, m_declaration{
		{
			TransformBufferElementDeclaration( cuT( "m_type" ), ElementType::Float ),
			TransformBufferElementDeclaration( cuT( "m_position" ), ElementType::Vec3 ),
			TransformBufferElementDeclaration( cuT( "m_velocity" ), ElementType::Vec3 ),
			TransformBufferElementDeclaration( cuT( "m_lifetime" ), ElementType::Float ),
		} }
	{
		for ( auto & l_transform : m_transformBuffers )
		{
			l_transform = std::make_unique< ParticleTransformBuffer >( *GetScene()->GetEngine(), m_declaration );
		}
	}

	ParticleSystem::~ParticleSystem()
	{
	}

	bool ParticleSystem::Initialise()
	{
		auto const & l_position = GetParent()->GetDerivedPosition();
		Particle l_particle
		{
			float( Particle::Type::Launcher ),
			l_position,
			Point3r{ 0.0f, 0.0001f, 0.0f },
			0.0f
		};

		bool l_return = true;

		for ( auto & l_transform : m_transformBuffers )
		{
			if ( l_return )
			{
				l_transform->Resize( uint32_t( m_count ) );
				( *l_transform )[0] = l_particle;
				l_return = l_transform->Create();
				l_return &= l_transform->Fill( l_transform->data(), m_count, BufferAccessType::Dynamic, BufferAccessNature::Draw );
				l_return &= l_transform->Initialise();
			}
		}

		m_timer.TimeMs();
		return l_return;
	}

	void ParticleSystem::Cleanup()
	{
		for ( auto & l_transform : m_transformBuffers )
		{
			l_transform->Cleanup();
			l_transform->Destroy();
		}
	}

	void ParticleSystem::Update()
	{
		auto l_time = m_timer.TimeMs();
	}

	void ParticleSystem::SetMaterial( MaterialSPtr p_material )
	{
		m_material = p_material;
	}

	void ParticleSystem::SetDimensions( Size const & p_dimensions )
	{
		m_dimensions = p_dimensions;
	}
}
