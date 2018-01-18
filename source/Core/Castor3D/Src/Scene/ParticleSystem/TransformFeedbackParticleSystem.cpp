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

using namespace castor;

namespace castor3d
{
	TransformFeedbackParticleSystem::TransformFeedbackParticleSystem( ParticleSystem & p_parent )
		: ParticleSystemImpl{ ParticleSystemImpl::Type::eTransformFeedback, p_parent }
		, m_randomTexture{ *p_parent.getScene()->getEngine() }
		, m_ubo{ cuT( "ParticleSystem" )
			, *p_parent.getScene()->getEngine()->getRenderSystem()
			, 1u }
	{
		m_deltaTime = m_ubo.createUniform< UniformType::eFloat >( cuT( "c3d_fDeltaTime" ) );
		m_time = m_ubo.createUniform< UniformType::eFloat >( cuT( "c3d_fTotalTime" ) );
		m_emitterPosition = m_ubo.createUniform< UniformType::eVec3f >( cuT( "c3d_v3EmitterPosition" ) );
	}

	TransformFeedbackParticleSystem::~TransformFeedbackParticleSystem()
	{
	}

	bool TransformFeedbackParticleSystem::initialise()
	{
		bool result = m_updateProgram != nullptr;

		if ( result )
		{
			result = doCreateRandomTexture();
		}

		if ( result )
		{
			result = m_updateProgram->initialise();
		}

		if ( result )
		{
			result = doCreateUpdatePipeline();
		}

		return result;
	}

	void TransformFeedbackParticleSystem::cleanup()
	{
		m_deltaTime.reset();
		m_time.reset();
		m_launcherLifetime.reset();
		m_shellLifetime.reset();
		m_secondaryShellLifetime.reset();
		m_randomTexture.cleanup();

		if ( m_updatePipeline )
		{
			m_updatePipeline->cleanup();
			m_updatePipeline.reset();
		}

		for ( size_t i = 0u; i < 2; ++i )
		{
			if ( m_updateGeometryBuffers[i] )
			{
				m_updateGeometryBuffers[i]->cleanup();
				m_updateGeometryBuffers[i].reset();
			}

			if ( m_transformFeedbacks[i] )
			{
				m_transformFeedbacks[i]->cleanup();
				m_transformFeedbacks[i].reset();
			}

			if ( m_updateVertexBuffers[i] )
			{
				m_updateVertexBuffers[i]->cleanup();
				m_updateVertexBuffers[i].reset();
			}
		}

		m_ubo.cleanup();

		if ( m_updateProgram )
		{
			m_updateProgram->cleanup();
			m_updateProgram.reset();
		}
	}

	uint32_t TransformFeedbackParticleSystem::update( Milliseconds const & p_time
		, Milliseconds const & p_totalTime )
	{
		m_deltaTime->setValue( float( p_time.count() ) );
		m_time->setValue( float( p_totalTime.count() ) );
		m_emitterPosition->setValue( m_parent.getParent()->getDerivedPosition() );
		m_ubo.update();
		auto & gbuffers = *m_updateGeometryBuffers[m_vtx];
		auto & transform = *m_transformFeedbacks[m_tfb];

		m_updatePipeline->apply();
		m_randomTexture.bind();
		transform.bind();

		gbuffers.draw( std::max( 1u, m_transformFeedbacks[m_vtx]->getWrittenPrimitives() ), 0u );

		transform.unbind();
		m_randomTexture.unbind();
		auto const written = std::max( 1u, transform.getWrittenPrimitives() );
		auto const count = uint32_t( m_computed.stride() * std::max( 1u, written ) );
		m_parent.getBillboards()->getVertexBuffer().copy( *m_updateVertexBuffers[m_tfb], count );

#if 0 && !defined( NDEBUG )

		{
			struct MyParticle
			{
				float type;
				Point3f position;
				Point3f velocity;
				float age;
			};

			m_updateVertexBuffers[m_tfb]->bind();
			auto buffer = reinterpret_cast< MyParticle * >( m_updateVertexBuffers[m_tfb]->lock( 0, count, AccessType::eRead ) );

			if ( buffer )
			{
				MyParticle particles[10000];
				std::memcpy( particles, buffer, count );
				m_updateVertexBuffers[m_tfb]->unlock();
			}

			m_updateVertexBuffers[m_tfb]->unbind();
		}

#endif

		m_vtx = m_tfb;
		m_tfb = 1 - m_tfb;
		return written;
	}

	void TransformFeedbackParticleSystem::addParticleVariable( castor::String const & p_name, renderer::AttributeFormat p_type, castor::String const & p_defaultValue )
	{
		m_computed.push_back( BufferElementDeclaration{ cuT( "out_" ) + p_name, 0u, p_type, m_computed.stride() } );
		m_inputs.push_back( BufferElementDeclaration{ p_name, 0u, p_type, m_inputs.stride() } );
	}

	void TransformFeedbackParticleSystem::setUpdateProgram( ShaderProgramSPtr p_program )
	{
		m_updateProgram = p_program;
		m_updateProgram->createUniform< UniformType::eSampler >( cuT( "c3d_mapRandom" ), ShaderType::eGeometry )->setValue( MinTextureIndex );

		m_updateProgram->setTransformLayout( m_computed );
	}

	bool TransformFeedbackParticleSystem::doCreateUpdatePipeline()
	{
		auto & engine = *m_parent.getScene()->getEngine();
		auto & renderSystem = *engine.getRenderSystem();

		Particle particle{ m_computed, m_parent.getDefaultValues() };
		bool result{ true };

		for ( uint32_t i = 0; i < 2 && result; ++i )
		{
			m_updateVertexBuffers[i] = std::make_shared< VertexBuffer >( engine, m_inputs );

			if ( result )
			{
				m_updateVertexBuffers[i]->resize( uint32_t( m_parent.getMaxParticlesCount() ) * m_computed.stride() );
				auto buffer = m_updateVertexBuffers[i]->getData();

				for ( uint32_t i = 0u; i < m_parent.getMaxParticlesCount(); ++i )
				{
					std::memcpy( buffer, particle.getData(), m_computed.stride() );
					buffer += m_computed.stride();
				}

				result = m_updateVertexBuffers[i]->initialise( renderer::MemoryPropertyFlag::eHostVisible );
			}
		}

		for ( uint32_t i = 0; i < 2 && result; ++i )
		{
			m_transformFeedbacks[i] = renderSystem.createTransformFeedback( m_computed, Topology::ePoints, *m_updateProgram );
			result = m_transformFeedbacks[i]->initialise( { *m_updateVertexBuffers[i] } );

			if ( result )
			{
				m_updateGeometryBuffers[i] = renderSystem.createGeometryBuffers( Topology::ePoints, *m_updateProgram );
				result = m_updateGeometryBuffers[i]->initialise( { *m_updateVertexBuffers[i] }, nullptr );
			}
		}

		if ( result )
		{
			RasteriserState rs;
			rs.setDiscardPrimitives( true );
			DepthStencilState ds;
			ds.setDepthTest( true );
			m_updatePipeline = renderSystem.createRenderPipeline( std::move( ds )
				, std::move( rs )
				, BlendState{}
				, MultisampleState{}
				, *m_updateProgram
				, PipelineFlags{} );
			m_updatePipeline->addUniformBuffer( m_ubo );
		}

		return result;
	}

	bool TransformFeedbackParticleSystem::doCreateRandomTexture()
	{
		auto & engine = *m_parent.getScene()->getEngine();
		auto & renderSystem = *engine.getRenderSystem();

		auto texture = renderSystem.createTexture( TextureType::eOneDimension, AccessType::eNone, AccessType::eRead, PixelFormat::eRGB32F, Size{ 1024, 1 } );
		texture->getImage().initialiseSource();
		auto & buffer = *std::static_pointer_cast< PxBuffer< PixelFormat::eRGB32F > >( texture->getImage().getBuffer() );
		auto pixels = reinterpret_cast< float * >( buffer.ptr() );

		static std::random_device device;
		std::uniform_real_distribution< float > distribution{ -1.0f, 1.0f };

		for ( uint32_t i{ 0u }; i < buffer.count(); ++i )
		{
			*pixels++ = distribution( device );
			*pixels++ = distribution( device );
			*pixels++ = distribution( device );
		}

		auto sampler = m_parent.getScene()->getEngine()->getSamplerCache().add( cuT( "TFParticleSystem" ) );
		sampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
		sampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
		sampler->setWrappingMode( TextureUVW::eU, WrapMode::eRepeat );
		bool result = sampler->initialise();

		if ( result )
		{
			m_randomTexture.setTexture( texture );
			m_randomTexture.setSampler( sampler );
			m_randomTexture.setIndex( MinTextureIndex );
			result = m_randomTexture.initialise();
			texture->generateMipmaps();
		}

		return result;
	}
}
