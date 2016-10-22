#include "ParticleSystem.hpp"

#include "Engine.hpp"

#include "Material/Material.hpp"
#include "Mesh/Buffer/VertexBuffer.hpp"
#include "Miscellaneous/TransformFeedback.hpp"
#include "Render/Pipeline.hpp"
#include "Scene/Scene.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/ShaderProgram.hpp"
#include "State/RasteriserState.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

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
		, m_randomTexture{ *p_scene.GetEngine() }
		, m_computed{
		{
			BufferElementDeclaration( cuT( "geo_type" ), 0u, ElementType::Float ),
			BufferElementDeclaration( cuT( "geo_position" ), uint32_t( ElementUsage::Position ), ElementType::Vec3 ),
			BufferElementDeclaration( cuT( "geo_velocity" ), 0u, ElementType::Vec3 ),
			BufferElementDeclaration( cuT( "geo_lifetime" ), 0u, ElementType::Float ),
		} }
	{
		BufferDeclaration const l_declaration{
		{
			BufferElementDeclaration( cuT( "type" ), 0u, ElementType::Float ),
			BufferElementDeclaration( cuT( "position" ), uint32_t( ElementUsage::Position ), ElementType::Vec3 ),
			BufferElementDeclaration( cuT( "velocity" ), 0u, ElementType::Vec3 ),
			BufferElementDeclaration( cuT( "lifetime" ), 0u, ElementType::Float ),
		} };

		for ( auto & l_transform : m_transformBuffers )
		{
			l_transform = std::make_unique< VertexBuffer >( *GetScene()->GetEngine(), l_declaration );
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

		bool l_return = DoCreateUpdateProgram();

		for ( auto & l_buffer : m_transformBuffers )
		{
			if ( l_return )
			{
				l_buffer->Resize( uint32_t( m_count ) );
				std::memcpy( l_buffer->data(), &l_particle, sizeof( Particle ) );
				l_return = l_buffer->Create();
				l_return &= l_buffer->Upload( BufferAccessType::Dynamic, BufferAccessNature::Draw );
			}
		}

		if ( l_return )
		{
			uint32_t i{ 0u };

			for ( auto & l_transform : m_transformFeedbacks )
			{
				l_transform = GetScene()->GetEngine()->GetRenderSystem()->CreateTransformFeedback( m_computed, Topology::Points, *m_updateProgram );
				l_transform->Initialise( VertexBufferArray{ std::ref( *m_transformBuffers[i++] ) } );
			}
		}

		if ( l_return )
		{
			RasteriserState l_rs;
			l_rs.SetDiscardPrimitives( true );
			m_updatePipeline = GetScene()->GetEngine()->GetRenderSystem()->CreatePipeline( DepthStencilState{}, std::move( l_rs ), BlendState{}, MultisampleState{}, *m_updateProgram, PipelineFlags{} );

			auto l_texture = GetScene()->GetEngine()->GetRenderSystem()->CreateTexture( TextureType::OneDimension, AccessType::Write, AccessType::Read, PixelFormat::RGB32F, Size{ 1024, 1 } );
			l_texture->GetImage().InitialiseSource();
			auto & l_buffer = *std::static_pointer_cast< PxBuffer< PixelFormat::RGB32F > >( l_texture->GetImage().GetBuffer() );
			auto l_pixels = reinterpret_cast< float * >( l_buffer.ptr() );

			static std::random_device l_device;
			std::uniform_real< float > l_distribution;

			for ( auto i{ 0u }; i < l_buffer.count() * 3; ++i )
			{
				*l_pixels++ = l_distribution( l_device );
			}

			auto l_sampler = GetScene()->GetEngine()->GetSamplerCache().Add( cuT( "ParticleSystem" ) );
			l_sampler->SetInterpolationMode( InterpolationFilter::Min, InterpolationMode::Linear );
			l_sampler->SetInterpolationMode( InterpolationFilter::Mag, InterpolationMode::Linear );
			l_sampler->SetWrappingMode( TextureUVW::U, WrapMode::Repeat );
			l_sampler->Initialise();

			m_randomTexture.SetTexture( l_texture );
			m_randomTexture.SetSampler( l_sampler );
			m_randomTexture.SetIndex( 0 );
			m_randomTexture.Initialise();
		}

		m_timer.TimeMs();
		return l_return;
	}

	void ParticleSystem::Cleanup()
	{
		m_deltaTime.reset();
		m_time.reset();
		m_launcherLifetime.reset();
		m_shellLifetime.reset();
		m_secondaryShellLifetime.reset();
		m_randomTexture.Cleanup();
		m_updatePipeline->Cleanup();
		m_updatePipeline.reset();

		for ( auto & l_transform : m_transformFeedbacks )
		{
			l_transform->Cleanup();
			l_transform.reset();
		}

		for ( auto & l_buffer : m_transformBuffers )
		{
			l_buffer->Destroy();
		}
	}

	void ParticleSystem::Render()
	{
		DoUpdate();
		DoRender();
		m_currentRenderBuffer = m_currentUpdateBuffer;
		m_currentUpdateBuffer = 1 - m_currentUpdateBuffer;
	}

	void ParticleSystem::SetMaterial( MaterialSPtr p_material )
	{
		m_material = p_material;
	}

	void ParticleSystem::SetDimensions( Size const & p_dimensions )
	{
		m_dimensions = p_dimensions;
	}

	bool ParticleSystem::DoCreateUpdateProgram()
	{
		auto & l_engine = *GetScene()->GetEngine();
		auto & l_renderSystem = *l_engine.GetRenderSystem();
		String l_vtx;
		{
			using namespace GLSL;
			auto l_writer = l_renderSystem.CreateGlslWriter();

			// Shader inputs
			auto type = l_writer.GetInput< Float >( cuT( "type" ) );
			auto position = l_writer.GetInput< Vec3 >( cuT( "position" ) );
			auto velocity = l_writer.GetInput< Vec3 >( cuT( "velocity" ) );
			auto lifetime = l_writer.GetInput< Float >( cuT( "lifetime" ) );

			// Shader outputs
			auto vtx_type = l_writer.GetOutput< Float >( cuT( "vtx_type" ) );
			auto vtx_position = l_writer.GetOutput< Vec3 >( cuT( "vtx_position" ) );
			auto vtx_velocity = l_writer.GetOutput< Vec3 >( cuT( "vtx_velocity" ) );
			auto vtx_lifetime = l_writer.GetOutput< Float >( cuT( "vtx_lifetime" ) );

			auto l_main = [&]()
			{
				vtx_type = type;
				vtx_position = position;
				vtx_velocity = velocity;
				vtx_lifetime = lifetime;
			};
			l_writer.ImplementFunction< Void >( cuT( "main" ), l_main );
			l_vtx = l_writer.Finalise();
		}

		String l_geo;
		{
			constexpr float PARTICLE_TYPE_LAUNCHER = 0.0f;
			constexpr float PARTICLE_TYPE_SHELL = 1.0f;
			constexpr float PARTICLE_TYPE_SECONDARY_SHELL = 2.0f;

			using namespace GLSL;
			auto l_writer = l_renderSystem.CreateGlslWriter();

			// Shader layouts
			l_writer.InputGeometryLayout( cuT( "points" ) );
			l_writer.OutputGeometryLayout( cuT( "points" ), 30 );

			// Shader inputs
			auto vtx_type = l_writer.GetInputArray< Float >( cuT( "vtx_type" ) );
			auto vtx_position = l_writer.GetInputArray< Vec3 >( cuT( "vtx_position" ) );
			auto vtx_velocity = l_writer.GetInputArray< Vec3 >( cuT( "vtx_velocity" ) );
			auto vtx_lifetime = l_writer.GetInputArray< Float >( cuT( "vtx_lifetime" ) );

			Ubo ParticleSystem{ l_writer, cuT( "ParticleSystem" ) };
			auto c3d_fDeltaTime = ParticleSystem.GetUniform< Float >( cuT( "c3d_fDeltaTime" ) );
			auto c3d_fTime = ParticleSystem.GetUniform< Float >( cuT( "c3d_fTime" ) );
			auto c3d_fLancherLifetime = ParticleSystem.GetUniform< Float >( cuT( "c3d_fLancherLifetime" ) );
			auto c3d_fShellLifetime = ParticleSystem.GetUniform< Float >( cuT( "c3d_fShellLifetime" ) );
			auto c3d_fSecondaryShellLifetime = ParticleSystem.GetUniform< Float >( cuT( "c3d_fSecondaryShellLifetime" ) );
			ParticleSystem.End();

			auto c3d_mapRandom = l_writer.GetUniform< Sampler1D >( cuT( "c3d_mapRandom" ) );

			// Shader outputs
			auto geo_type = l_writer.GetOutput< Float >( cuT( "geo_type" ) );
			auto geo_position = l_writer.GetOutput< Vec3 >( cuT( "geo_position" ) );
			auto geo_velocity = l_writer.GetOutput< Vec3 >( cuT( "geo_velocity" ) );
			auto geo_lifetime = l_writer.GetOutput< Float >( cuT( "geo_lifetime" ) );

			auto l_randomDir = [&]( Float const & p_uv )
			{
				auto l_dir = l_writer.GetLocale( cuT( "l_dir" ), texture( c3d_mapRandom, p_uv ).xyz() );
				l_dir -= vec3( Float( 0.5 ), 0.5, 0.5 );
				l_writer.Return( l_dir );
			};
			l_writer.ImplementFunction< Vec3 >( cuT( "GetRandomDir" )
												, l_randomDir
												, InParam< Float >{ &l_writer, cuT( "p_uv" ) } );

			auto l_main = [&]()
			{
				auto l_age = l_writer.GetLocale( cuT( "l_age" ), vtx_lifetime[0] + c3d_fDeltaTime );

				IF( l_writer, vtx_type[0] == PARTICLE_TYPE_LAUNCHER )
				{
					IF( l_writer, l_age >= c3d_fLancherLifetime )
					{
						geo_type = PARTICLE_TYPE_SHELL;
						geo_position = vtx_position[0];
						auto l_dir = l_writer.GetLocale( cuT( "l_dir" ), WriteFunctionCall< Vec3 >( &l_writer, cuT( "GetRandomDir" ), c3d_fTime / 1000.0f ) );
						l_dir.y() = max( l_dir.y(), 0.5 );
						geo_velocity = normalize( l_dir ) / 20.0;
						geo_lifetime = Float( 0.0 );
						l_writer.EmitVertex();
						l_writer.EndPrimitive();
						l_age = Float( 0.0 );
					}
					FI;

					geo_type = PARTICLE_TYPE_LAUNCHER;
					geo_position = vtx_position[0];
					geo_velocity = vtx_velocity[0];
					geo_lifetime = l_age;
					l_writer.EmitVertex();
					l_writer.EndPrimitive();
				}
				ELSE
				{
					auto l_deltaTime = l_writer.GetLocale( cuT( "l_deltaTime" ), c3d_fDeltaTime / 1000.0f );
					auto l_t1 = l_writer.GetLocale( cuT( "l_t1" ), vtx_lifetime[0] / 1000.0 );
					auto l_t2 = l_writer.GetLocale( cuT( "l_t2" ), l_age / 1000.0 );
					auto l_deltaP = l_writer.GetLocale( cuT( "l_deltaP" ), vec3( l_deltaTime ) * vtx_velocity[0] );
					auto l_deltaV = l_writer.GetLocale( cuT( "l_deltaV" ), vec3( l_deltaTime ) * vec3( Float( 0.0 ), -9.81, 0.0 ) );

					IF( l_writer, vtx_type[0] == PARTICLE_TYPE_SHELL )
					{
						IF( l_writer, l_age < c3d_fShellLifetime )
						{
							geo_type = PARTICLE_TYPE_SHELL;
							geo_position = vtx_position[0] + l_deltaP;
							geo_velocity = vtx_velocity[0] + l_deltaV;
							geo_lifetime = l_age;
							l_writer.EmitVertex();
							l_writer.EndPrimitive();
						}
						ELSE
						{
							FOR( l_writer, Int, i, 0, cuT( "i < 10" ), cuT( "i++" ) )
							{
								geo_type = PARTICLE_TYPE_SECONDARY_SHELL;
								geo_position = vtx_position[0];
								auto l_dir = l_writer.GetLocale( cuT( "l_dir" ), WriteFunctionCall< Vec3 >( &l_writer, cuT( "GetRandomDir" ), ( c3d_fTime + i ) / 1000.0f ) );
								geo_velocity = normalize( l_dir ) / 20.0;
								geo_lifetime = Float( 0.0 );
								l_writer.EmitVertex();
								l_writer.EndPrimitive();
							}
							ROF;
						}
						FI;
					}
					ELSE
					{
						IF( l_writer, l_age < c3d_fSecondaryShellLifetime )
						{
							geo_type = PARTICLE_TYPE_SECONDARY_SHELL;
							geo_position = vtx_position[0] + l_deltaP;
							geo_velocity = vtx_velocity[0] + l_deltaV;
							geo_lifetime = l_age;
							l_writer.EmitVertex();
							l_writer.EndPrimitive();
						}
						FI;
					}
					FI;
				}
				FI;
			};
			l_writer.ImplementFunction< Void >( cuT( "main" ), l_main );
			l_geo = l_writer.Finalise();
		}

		auto l_model = l_renderSystem.GetGpuInformations().GetMaxShaderModel();
		m_updateProgram = l_engine.GetShaderProgramCache().GetNewProgram();
		m_updateProgram->CreateObject( ShaderType::Geometry );
		m_updateProgram->SetSource( ShaderType::Vertex, l_model, l_vtx );
		m_updateProgram->SetSource( ShaderType::Geometry, l_model, l_geo );

		m_updateProgram->CreateFrameVariable( FrameVariableType::Sampler, cuT( "c3d_mapRandom" ), ShaderType::Geometry );

		auto & l_ubo = m_updateProgram->CreateFrameVariableBuffer( cuT( "ParticleSystem" ), MASK_SHADER_TYPE_GEOMETRY );
		m_deltaTime = l_ubo.CreateVariable< OneFloatFrameVariable >( cuT( "c3d_fDeltaTime" ) );
		m_time = l_ubo.CreateVariable< OneFloatFrameVariable >( cuT( "c3d_fTime" ) );
		m_launcherLifetime = l_ubo.CreateVariable< OneFloatFrameVariable >( cuT( "c3d_fLancherLifetime" ) );
		m_shellLifetime = l_ubo.CreateVariable< OneFloatFrameVariable >( cuT( "c3d_fShellLifetime" ) );
		m_secondaryShellLifetime = l_ubo.CreateVariable< OneFloatFrameVariable >( cuT( "c3d_fSecondaryShellLifetime" ) );
		m_launcherLifetime->SetValue( 100.0f );
		m_shellLifetime->SetValue( 10000.0f );
		m_secondaryShellLifetime->SetValue( 2500.0f );

		m_updateProgram->SetTransformLayout( m_computed );
		return m_updateProgram->Initialise();
	}

	void ParticleSystem::DoUpdate()
	{
		auto l_time = m_timer.TimeMs();
		m_deltaTime->SetValue( float( l_time ) );
		m_time->SetValue( m_time->GetValue() + float( l_time ) );

		m_updatePipeline->Apply();
		m_randomTexture.Bind();
		auto & l_buffer = *m_transformBuffers[m_currentRenderBuffer];
		l_buffer.Bind();
		m_transformFeedbacks[m_currentUpdateBuffer]->Bind();

		//m_transformFeedbacks[m_currentRenderBuffer]->Draw();

		m_transformFeedbacks[m_currentUpdateBuffer]->Unbind();
		l_buffer.Unbind();
		m_randomTexture.Unbind();
	}

	void ParticleSystem::DoRender()
	{
	}
}
