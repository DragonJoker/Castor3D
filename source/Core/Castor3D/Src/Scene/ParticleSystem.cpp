#include "ParticleSystem.hpp"

#include "Engine.hpp"

#include "Material/Material.hpp"
#include "Material/Pass.hpp"
#include "Mesh/Buffer/BufferElementGroup.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Miscellaneous/TransformFeedback.hpp"
#include "Render/Pipeline.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Scene.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/ShaderObject.hpp"
#include "Shader/ShaderProgram.hpp"
#include "State/RasteriserState.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

#include <Graphics/PixelBuffer.hpp>

#include <random>

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
			l_return = MovableObject::TextWriter{ m_tabs }( *p_obj.GetBillboards(), p_file );
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

	Particle::Particle( BufferDeclaration const & p_description, StrStrMap const & p_defaultValues )
		: m_description{ p_description }
	{
		m_data.resize( p_description.stride() );
		uint32_t l_index{ 0u };

		for ( auto l_element : m_description )
		{
			auto l_it = p_defaultValues.find( l_element.m_name );

			if ( l_it != p_defaultValues.end() && !l_it->second.empty() )
			{
				ParseValue( l_it->second, l_element.m_dataType, *this, l_index );
			}

			++l_index;
		}
	}

	//*************************************************************************************************

	ParticleSystem::ParticleSystem( String const & p_name, Scene & p_scene, SceneNodeSPtr p_parent, size_t p_count )
		: Named{ p_name }
		, OwnedBy< Scene >{ p_scene }
		, m_parentNode{ p_parent }
		, m_particlesCount{ p_count }
		, m_randomTexture{ *p_scene.GetEngine() }
	{
	}

	ParticleSystem::~ParticleSystem()
	{
	}

	bool ParticleSystem::Initialise()
	{
		auto & l_engine = *GetScene()->GetEngine();
		m_particlesBillboard = std::make_unique< BillboardListBase >( GetName(), *GetScene(), m_parentNode, std::make_shared< VertexBuffer >( l_engine, m_inputs ) );
		m_particlesBillboard->SetDimensions( m_dimensions );
		m_particlesBillboard->SetMaterial( m_material.lock() );
		bool l_return = m_particlesBillboard->Initialise();

		if ( l_return )
		{
			auto & l_vbo = m_particlesBillboard->GetVertexBuffer();
			l_vbo.Resize( uint32_t( m_particlesCount ) * m_inputs.stride() );
			l_return = l_vbo.Upload( BufferAccessType::eDynamic, BufferAccessNature::eDraw );
		}

		if ( l_return )
		{
			l_return = m_updateProgram->Initialise();
		}

		if ( l_return )
		{
			l_return = DoCreateUpdatePipeline();
		}

		m_timer.TimeMs();
		return l_return;
	}

	void ParticleSystem::Cleanup()
	{
		m_particlesBillboard->Cleanup();
		m_particlesBillboard.reset();
		m_deltaTime.reset();
		m_time.reset();
		m_launcherLifetime.reset();
		m_shellLifetime.reset();
		m_secondaryShellLifetime.reset();
		m_randomTexture.Cleanup();

		m_updatePipeline->Cleanup();
		m_updatePipeline.reset();

		for ( size_t i = 0u; i < 2; ++i )
		{
			m_updateGeometryBuffers[i]->Cleanup();
			m_updateGeometryBuffers[i].reset();
			m_transformFeedbacks[i]->Cleanup();
			m_transformFeedbacks[i].reset();
			m_updateVertexBuffers[i]->Destroy();
		}

		m_updateProgram->Cleanup();
		m_updateProgram.reset();
	}

	void ParticleSystem::Update()
	{
		auto l_time = float( m_timer.TimeMs() );

		if ( m_firstUpdate )
		{
			l_time = 0.0f;
		}

		m_totalTime += l_time;
		m_deltaTime->SetValue( l_time );
		m_time->SetValue( m_totalTime );
		auto & l_gbuffers = *m_updateGeometryBuffers[m_vtx];
		auto & l_transform = *m_transformFeedbacks[m_tfb];

		m_updatePipeline->Apply();
		m_updateProgram->BindUbos();
		m_randomTexture.Bind();
		l_transform.Bind();

		l_gbuffers.Draw( std::max( 1u, m_transformFeedbacks[m_vtx]->GetWrittenPrimitives() ), 0 );

		l_transform.Unbind();
		m_randomTexture.Unbind();
		auto const l_written = std::max( 1u, l_transform.GetWrittenPrimitives() );
		auto const l_count = uint32_t( m_computed.stride() * std::max( 1u, l_written ) );
		m_particlesBillboard->GetVertexBuffer().Copy( *m_updateVertexBuffers[m_tfb], l_count );

#if 0 && !defined( NDEBUG )

		{
			struct MyParticle
			{
				float type;
				Point3f position;
				Point3f velocity;
				float age;
			};

			m_updateVertexBuffers[m_tfb]->Bind();
			auto l_buffer = reinterpret_cast< MyParticle * >( m_updateVertexBuffers[m_tfb]->Lock( 0, l_count, AccessType::eRead ) );

			if ( l_buffer )
			{
				MyParticle l_particles[10000];
				std::memcpy( l_particles, l_buffer, l_count );
				m_updateVertexBuffers[m_tfb]->Unlock();
			}

			m_updateVertexBuffers[m_tfb]->Unbind();
		}

#endif

		m_particlesBillboard->SetCount( l_written );
		m_vtx = m_tfb;
		m_tfb = 1 - m_tfb;
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

	MaterialSPtr ParticleSystem::GetMaterial()const
	{
		return m_material.lock();
	}

	Size const & ParticleSystem::GetDimensions()const
	{
		return m_dimensions;
	}

	void ParticleSystem::Detach()
	{
		m_parentNode = nullptr;

		if ( m_particlesBillboard )
		{
			m_particlesBillboard->Detach();
		}
	}

	void ParticleSystem::AttachTo( SceneNodeSPtr p_node )
	{
		m_parentNode = p_node;

		if ( m_particlesBillboard )
		{
			m_particlesBillboard->AttachTo( p_node );
		}
	}

	SceneNodeSPtr ParticleSystem::GetParent()const
	{
		return m_parentNode;
	}

	void ParticleSystem::AddParticleVariable( Castor::String const & p_name, ElementType p_type, Castor::String const & p_defaultValue )
	{
		m_computed.push_back( BufferElementDeclaration{ cuT( "out_" ) + p_name, 0u, p_type, m_computed.stride() } );
		m_inputs.push_back( BufferElementDeclaration{ p_name, 0u, p_type, m_inputs.stride() } );
		m_defaultValues[cuT ("out_") + p_name] = p_defaultValue;
	}

	void ParticleSystem::SetUpdateProgram( ShaderProgramSPtr p_program )
	{
		m_updateProgram = p_program;
		m_updateProgram->CreateFrameVariable( FrameVariableType::eSampler, cuT( "c3d_mapRandom" ), ShaderType::eGeometry );

		auto & l_ubo = m_updateProgram->CreateFrameVariableBuffer( cuT( "ParticleSystem" ), MASK_SHADER_TYPE_VERTEX | MASK_SHADER_TYPE_GEOMETRY );
		m_deltaTime = l_ubo.CreateVariable< OneFloatFrameVariable >( cuT( "c3d_fDeltaTime" ) );
		m_time = l_ubo.CreateVariable< OneFloatFrameVariable >( cuT( "c3d_fTotalTime" ) );
		m_ubo = m_updateProgram->FindFrameVariableBuffer( cuT( "ParticleSystem" ) );

		m_updateProgram->SetTransformLayout( m_computed );
	}

	uint32_t ParticleSystem::GetParticlesCount()const
	{
		return m_transformFeedbacks[m_vtx]->GetWrittenPrimitives();
	}

	bool ParticleSystem::DoCreateUpdatePipeline()
	{
		auto & l_engine = *GetScene()->GetEngine();
		auto & l_renderSystem = *l_engine.GetRenderSystem();

		Particle l_particle{ m_computed, m_defaultValues };
		bool l_return{ true };

		for ( uint32_t i = 0; i < 2 && l_return; ++i )
		{
			m_updateVertexBuffers[i] = std::make_shared< VertexBuffer >( l_engine, m_inputs ),
			l_return = m_updateVertexBuffers[i]->Create();

			if ( l_return )
			{
				m_updateVertexBuffers[i]->Resize( uint32_t( m_particlesCount ) * m_computed.stride() );
				std::memcpy( m_updateVertexBuffers[i]->data(), l_particle.GetData(), m_computed.stride() );
				l_return = m_updateVertexBuffers[i]->Upload( BufferAccessType::eDynamic, BufferAccessNature::eDraw );
			}
		}

		for ( uint32_t i = 0; i < 2 && l_return; ++i )
		{
			m_transformFeedbacks[i] = l_renderSystem.CreateTransformFeedback( m_computed, Topology::ePoints, *m_updateProgram );
			l_return = m_transformFeedbacks[i]->Initialise( { *m_updateVertexBuffers[i] } );

			if ( l_return )
			{
				m_updateGeometryBuffers[i] = l_renderSystem.CreateGeometryBuffers( Topology::ePoints, *m_updateProgram );
				l_return = m_updateGeometryBuffers[i]->Initialise( { *m_updateVertexBuffers[i] }, nullptr );
			}
		}

		if ( l_return )
		{
			RasteriserState l_rs;
			l_rs.SetDiscardPrimitives( true );
			m_updatePipeline = l_renderSystem.CreatePipeline( DepthStencilState{}, std::move( l_rs ), BlendState{}, MultisampleState{}, *m_updateProgram, PipelineFlags{} );

			auto l_texture = l_renderSystem.CreateTexture( TextureType::eOneDimension, AccessType::eNone, AccessType::eRead, PixelFormat::eRGB32F, Size{ 1024, 1 } );
			l_texture->GetImage().InitialiseSource();
			auto & l_buffer = *std::static_pointer_cast< PxBuffer< PixelFormat::eRGB32F > >( l_texture->GetImage().GetBuffer() );
			auto l_pixels = reinterpret_cast< float * >( l_buffer.ptr() );

			static std::random_device l_device;
			std::uniform_real_distribution< float > l_distribution{ -1.0f, 1.0f };

			for ( uint32_t i{ 0u }; i < l_buffer.count(); ++i )
			{
				*l_pixels++ = l_distribution( l_device );
				*l_pixels++ = l_distribution( l_device );
				*l_pixels++ = l_distribution( l_device );
			}

			auto l_sampler = GetScene()->GetEngine()->GetSamplerCache().Add( cuT( "ParticleSystem" ) );
			l_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
			l_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
			l_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eRepeat );
			l_sampler->Initialise();

			m_randomTexture.SetTexture( l_texture );
			m_randomTexture.SetSampler( l_sampler );
			m_randomTexture.SetIndex( 0 );
			m_randomTexture.Initialise();
			l_texture->GenerateMipmaps();
		}

		return l_return;
	}
}
