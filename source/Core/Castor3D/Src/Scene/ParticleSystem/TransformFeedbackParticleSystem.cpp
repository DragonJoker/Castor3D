#include "TransformFeedbackParticleSystem.hpp"

#include "ParticleSystem.hpp"
#include "Particle.hpp"

#include "Engine.hpp"

#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Miscellaneous/TransformFeedback.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Scene.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Graphics/PixelBuffer.hpp>

#include <random>

using namespace Castor;

namespace Castor3D
{
	TransformFeedbackParticleSystem::TransformFeedbackParticleSystem( ParticleSystem & p_parent )
		: ParticleSystemImpl{ ParticleSystemImpl::Type::eTransformFeedback, p_parent }
		, m_randomTexture{ *p_parent.GetScene()->GetEngine() }
		, m_ubo{ cuT( "ParticleSystem" ), *p_parent.GetScene()->GetEngine()->GetRenderSystem() }
	{
		m_deltaTime = m_ubo.CreateUniform< UniformType::eFloat >( cuT( "c3d_fDeltaTime" ) );
		m_time = m_ubo.CreateUniform< UniformType::eFloat >( cuT( "c3d_fTotalTime" ) );
		m_emitterPosition = m_ubo.CreateUniform< UniformType::eVec3f >( cuT( "c3d_v3EmitterPosition" ) );
	}

	TransformFeedbackParticleSystem::~TransformFeedbackParticleSystem()
	{
	}

	bool TransformFeedbackParticleSystem::Initialise()
	{
		bool l_return = m_updateProgram != nullptr;

		if ( l_return )
		{
			l_return = DoCreateRandomTexture();
		}

		if ( l_return )
		{
			l_return = m_updateProgram->Initialise();
		}

		if ( l_return )
		{
			l_return = DoCreateUpdatePipeline();
		}

		return l_return;
	}

	void TransformFeedbackParticleSystem::Cleanup()
	{
		m_deltaTime.reset();
		m_time.reset();
		m_launcherLifetime.reset();
		m_shellLifetime.reset();
		m_secondaryShellLifetime.reset();
		m_randomTexture.Cleanup();

		if ( m_updatePipeline )
		{
			m_updatePipeline->Cleanup();
			m_updatePipeline.reset();
		}

		for ( size_t i = 0u; i < 2; ++i )
		{
			if ( m_updateGeometryBuffers[i] )
			{
				m_updateGeometryBuffers[i]->Cleanup();
				m_updateGeometryBuffers[i].reset();
			}

			if ( m_transformFeedbacks[i] )
			{
				m_transformFeedbacks[i]->Cleanup();
				m_transformFeedbacks[i].reset();
			}

			if ( m_updateVertexBuffers[i] )
			{
				m_updateVertexBuffers[i]->Cleanup();
				m_updateVertexBuffers[i].reset();
			}
		}

		m_ubo.Cleanup();

		if ( m_updateProgram )
		{
			m_updateProgram->Cleanup();
			m_updateProgram.reset();
		}
	}

	uint32_t TransformFeedbackParticleSystem::Update( std::chrono::milliseconds const & p_time
		, std::chrono::milliseconds const & p_totalTime )
	{
		m_deltaTime->SetValue( float( p_time.count() ) );
		m_time->SetValue( float( p_totalTime.count() ) );
		m_emitterPosition->SetValue( m_parent.GetParent()->GetDerivedPosition() );
		m_ubo.Update();
		auto & l_gbuffers = *m_updateGeometryBuffers[m_vtx];
		auto & l_transform = *m_transformFeedbacks[m_tfb];

		m_updatePipeline->Apply();
		m_randomTexture.Bind();
		l_transform.Bind();

		l_gbuffers.Draw( std::max( 1u, m_transformFeedbacks[m_vtx]->GetWrittenPrimitives() ), 0 );

		l_transform.Unbind();
		m_randomTexture.Unbind();
		auto const l_written = std::max( 1u, l_transform.GetWrittenPrimitives() );
		auto const l_count = uint32_t( m_computed.stride() * std::max( 1u, l_written ) );
		m_parent.GetBillboards()->GetVertexBuffer().Copy( *m_updateVertexBuffers[m_tfb], l_count );

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

		m_vtx = m_tfb;
		m_tfb = 1 - m_tfb;
		return l_written;
	}

	void TransformFeedbackParticleSystem::AddParticleVariable( Castor::String const & p_name, ElementType p_type, Castor::String const & p_defaultValue )
	{
		m_computed.push_back( BufferElementDeclaration{ cuT( "out_" ) + p_name, 0u, p_type, m_computed.stride() } );
		m_inputs.push_back( BufferElementDeclaration{ p_name, 0u, p_type, m_inputs.stride() } );
	}

	void TransformFeedbackParticleSystem::SetUpdateProgram( ShaderProgramSPtr p_program )
	{
		m_updateProgram = p_program;
		m_updateProgram->CreateUniform( UniformType::eSampler, cuT( "c3d_mapRandom" ), ShaderType::eGeometry );

		m_updateProgram->SetTransformLayout( m_computed );
	}

	bool TransformFeedbackParticleSystem::DoCreateUpdatePipeline()
	{
		auto & l_engine = *m_parent.GetScene()->GetEngine();
		auto & l_renderSystem = *l_engine.GetRenderSystem();

		Particle l_particle{ m_computed, m_parent.GetDefaultValues() };
		bool l_return{ true };

		for ( uint32_t i = 0; i < 2 && l_return; ++i )
		{
			m_updateVertexBuffers[i] = std::make_shared< VertexBuffer >( l_engine, m_inputs );

			if ( l_return )
			{
				m_updateVertexBuffers[i]->Resize( uint32_t( m_parent.GetMaxParticlesCount() ) * m_computed.stride() );
				auto l_buffer = m_updateVertexBuffers[i]->data();

				for ( uint32_t i = 0u; i < m_parent.GetMaxParticlesCount(); ++i )
				{
					std::memcpy( l_buffer, l_particle.GetData(), m_computed.stride() );
					l_buffer += m_computed.stride();
				}

				l_return = m_updateVertexBuffers[i]->Initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw );
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
			DepthStencilState l_ds;
			l_ds.SetDepthTest( true );
			m_updatePipeline = l_renderSystem.CreateRenderPipeline( std::move( l_ds )
				, std::move( l_rs )
				, BlendState{}
				, MultisampleState{}
				, *m_updateProgram
				, PipelineFlags{} );
			m_updatePipeline->AddUniformBuffer( m_ubo );
		}

		return l_return;
	}

	bool TransformFeedbackParticleSystem::DoCreateRandomTexture()
	{
		auto & l_engine = *m_parent.GetScene()->GetEngine();
		auto & l_renderSystem = *l_engine.GetRenderSystem();

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

		auto l_sampler = m_parent.GetScene()->GetEngine()->GetSamplerCache().Add( cuT( "TFParticleSystem" ) );
		l_sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
		l_sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
		l_sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eRepeat );
		bool l_return = l_sampler->Initialise();

		if ( l_return )
		{
			m_randomTexture.SetTexture( l_texture );
			m_randomTexture.SetSampler( l_sampler );
			m_randomTexture.SetIndex( 0 );
			l_return = m_randomTexture.Initialise();
			l_texture->GenerateMipmaps();
		}

		return l_return;
	}
}
