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
		bool result = m_updateProgram != nullptr;

		if ( result )
		{
			result = DoCreateRandomTexture();
		}

		if ( result )
		{
			result = m_updateProgram->Initialise();
		}

		if ( result )
		{
			result = DoCreateUpdatePipeline();
		}

		return result;
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

	uint32_t TransformFeedbackParticleSystem::Update( Castor::Milliseconds const & p_time
		, Castor::Milliseconds const & p_totalTime )
	{
		m_deltaTime->SetValue( float( p_time.count() ) );
		m_time->SetValue( float( p_totalTime.count() ) );
		m_emitterPosition->SetValue( m_parent.GetParent()->GetDerivedPosition() );
		m_ubo.Update();
		auto & gbuffers = *m_updateGeometryBuffers[m_vtx];
		auto & transform = *m_transformFeedbacks[m_tfb];

		m_updatePipeline->Apply();
		m_randomTexture.Bind();
		transform.Bind();

		gbuffers.Draw( std::max( 1u, m_transformFeedbacks[m_vtx]->GetWrittenPrimitives() ), 0 );

		transform.Unbind();
		m_randomTexture.Unbind();
		auto const written = std::max( 1u, transform.GetWrittenPrimitives() );
		auto const count = uint32_t( m_computed.stride() * std::max( 1u, written ) );
		m_parent.GetBillboards()->GetVertexBuffer().Copy( *m_updateVertexBuffers[m_tfb], count );

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
			auto buffer = reinterpret_cast< MyParticle * >( m_updateVertexBuffers[m_tfb]->Lock( 0, count, AccessType::eRead ) );

			if ( buffer )
			{
				MyParticle particles[10000];
				std::memcpy( particles, buffer, count );
				m_updateVertexBuffers[m_tfb]->Unlock();
			}

			m_updateVertexBuffers[m_tfb]->Unbind();
		}

#endif

		m_vtx = m_tfb;
		m_tfb = 1 - m_tfb;
		return written;
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
		auto & engine = *m_parent.GetScene()->GetEngine();
		auto & renderSystem = *engine.GetRenderSystem();

		Particle particle{ m_computed, m_parent.GetDefaultValues() };
		bool result{ true };

		for ( uint32_t i = 0; i < 2 && result; ++i )
		{
			m_updateVertexBuffers[i] = std::make_shared< VertexBuffer >( engine, m_inputs );

			if ( result )
			{
				m_updateVertexBuffers[i]->Resize( uint32_t( m_parent.GetMaxParticlesCount() ) * m_computed.stride() );
				auto buffer = m_updateVertexBuffers[i]->GetData();

				for ( uint32_t i = 0u; i < m_parent.GetMaxParticlesCount(); ++i )
				{
					std::memcpy( buffer, particle.GetData(), m_computed.stride() );
					buffer += m_computed.stride();
				}

				result = m_updateVertexBuffers[i]->Initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw );
			}
		}

		for ( uint32_t i = 0; i < 2 && result; ++i )
		{
			m_transformFeedbacks[i] = renderSystem.CreateTransformFeedback( m_computed, Topology::ePoints, *m_updateProgram );
			result = m_transformFeedbacks[i]->Initialise( { *m_updateVertexBuffers[i] } );

			if ( result )
			{
				m_updateGeometryBuffers[i] = renderSystem.CreateGeometryBuffers( Topology::ePoints, *m_updateProgram );
				result = m_updateGeometryBuffers[i]->Initialise( { *m_updateVertexBuffers[i] }, nullptr );
			}
		}

		if ( result )
		{
			RasteriserState rs;
			rs.SetDiscardPrimitives( true );
			DepthStencilState ds;
			ds.SetDepthTest( true );
			m_updatePipeline = renderSystem.CreateRenderPipeline( std::move( ds )
				, std::move( rs )
				, BlendState{}
				, MultisampleState{}
				, *m_updateProgram
				, PipelineFlags{} );
			m_updatePipeline->AddUniformBuffer( m_ubo );
		}

		return result;
	}

	bool TransformFeedbackParticleSystem::DoCreateRandomTexture()
	{
		auto & engine = *m_parent.GetScene()->GetEngine();
		auto & renderSystem = *engine.GetRenderSystem();

		auto texture = renderSystem.CreateTexture( TextureType::eOneDimension, AccessType::eNone, AccessType::eRead, PixelFormat::eRGB32F, Size{ 1024, 1 } );
		texture->GetImage().InitialiseSource();
		auto & buffer = *std::static_pointer_cast< PxBuffer< PixelFormat::eRGB32F > >( texture->GetImage().GetBuffer() );
		auto pixels = reinterpret_cast< float * >( buffer.ptr() );

		static std::random_device device;
		std::uniform_real_distribution< float > distribution{ -1.0f, 1.0f };

		for ( uint32_t i{ 0u }; i < buffer.count(); ++i )
		{
			*pixels++ = distribution( device );
			*pixels++ = distribution( device );
			*pixels++ = distribution( device );
		}

		auto sampler = m_parent.GetScene()->GetEngine()->GetSamplerCache().Add( cuT( "TFParticleSystem" ) );
		sampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
		sampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
		sampler->SetWrappingMode( TextureUVW::eU, WrapMode::eRepeat );
		bool result = sampler->Initialise();

		if ( result )
		{
			m_randomTexture.SetTexture( texture );
			m_randomTexture.SetSampler( sampler );
			m_randomTexture.SetIndex( 0 );
			result = m_randomTexture.Initialise();
			texture->GenerateMipmaps();
		}

		return result;
	}
}
