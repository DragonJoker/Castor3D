#include "LinearMotionBlurPostEffect/LinearMotionBlurPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>
#include <Castor3D/Buffer/GpuBuffer.hpp>
#include <Castor3D/Cache/ShaderCache.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Model/Vertex.hpp>
#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Render/RenderNodesPass.hpp>
#include <Castor3D/Render/RenderPipeline.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderWindow.hpp>
#include <Castor3D/Render/Viewport.hpp>
#include <Castor3D/Render/RenderTechnique.hpp>
#include <Castor3D/Render/RenderTechniquePass.hpp>
#include <Castor3D/Scene/ParticleSystem/ParticleDeclaration.hpp>
#include <Castor3D/Scene/ParticleSystem/ParticleElementDeclaration.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>

#include <CastorUtils/Design/ResourceCache.hpp>

#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/GraphicsPipelineCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <numeric>

namespace motion_blur
{
	namespace postfx
	{
		namespace c3d = castor3d::shader;

		enum Idx : uint32_t
		{
			BlurCfgUboIdx,
			VelocityTexIdx,
			ColorTexIdx,
		};

		static castor3d::ShaderPtr getProgram( castor3d::RenderDevice const & device )
		{
			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			auto configuration = writer.declUniformBuffer( "Configuration", BlurCfgUboIdx, 0u );
			auto c3d_samplesCount = configuration.declMember< sdw::UInt >( "c3d_samplesCount" );
			auto c3d_vectorDivider = configuration.declMember< sdw::Float >( "c3d_vectorDivider" );
			auto c3d_blurScale = configuration.declMember< sdw::Float >( "c3d_blurScale" );
			configuration.end();
			auto c3d_mapVelocity = writer.declCombinedImg< FImg2DRg32 >( "c3d_mapVelocity", VelocityTexIdx, 0u );
			auto c3d_mapColor = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapColor", ColorTexIdx, 0u );

			writer.implementEntryPointT< c3d::PosUv2FT, c3d::Uv2FT >( [&]( sdw::VertexInT< c3d::PosUv2FT > in
				, sdw::VertexOutT< c3d::Uv2FT > out )
				{
					out.uv() = in.uv();
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< c3d::Uv2FT, c3d::Colour4FT >( [&]( sdw::FragmentInT< c3d::Uv2FT > in
				, sdw::FragmentOutT< c3d::Colour4FT > out )
				{
					auto blurVector = writer.declLocale( "blurVector"
						, ( c3d_mapVelocity.sample( in.uv() ) / c3d_vectorDivider ) * c3d_blurScale );
					blurVector.y() = -blurVector.y();
					out.colour() = c3d_mapColor.sample( in.uv() );

					FOR( writer, sdw::UInt, i, 0u, i < c3d_samplesCount, ++i )
					{
						auto offset = writer.declLocale( "offset"
							, blurVector * ( writer.cast< sdw::Float >( i ) / writer.cast< sdw::Float >( c3d_samplesCount - 1_u ) - 0.5f ) );
						out.colour() += c3d_mapColor.sample( in.uv() + offset );
					}
					ROF;

					out.colour() /= writer.cast< sdw::Float >( c3d_samplesCount );
				} );
			return writer.getBuilder().releaseShader();
		}
	}

	//*********************************************************************************************

	castor::String PostEffect::Type = cuT( "linear_motion_blur" );
	castor::String PostEffect::Name = cuT( "LinearMotionBlur PostEffect" );

	PostEffect::PostEffect( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & parameters )
		: castor3d::PostEffect{ PostEffect::Type
			, "LinearMotionBlur"
			, PostEffect::Name
			, renderTarget
			, renderSystem
			, parameters
			, 1u }
		, m_ubo{ renderSystem.getRenderDevice().uboPool->getBuffer< Configuration >( 0u ) }
		, m_shader{ "LinearMotionBlur", postfx::getProgram( renderSystem.getRenderDevice() ) }
		, m_stages{ makeProgramStates( renderSystem.getRenderDevice(), m_shader ) }
	{
		setParameters( parameters );
	}

	PostEffect::~PostEffect()
	{
		getRenderSystem()->getRenderDevice().uboPool->putBuffer( m_ubo );
	}

	castor3d::PostEffectUPtr PostEffect::create( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & params )
	{
		return castor::makeUniqueDerived< castor3d::PostEffect, PostEffect >( renderTarget, renderSystem, params );
	}

	void PostEffect::accept( castor3d::ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_shader );
	}

	void PostEffect::setParameters( castor3d::Parameters parameters )
	{
		parameters.get( cuT( "vectorDivider" ), m_configuration.vectorDivider );
		parameters.get( cuT( "samplesCount" ), m_configuration.samplesCount );
		parameters.get( cuT( "fpsScale" ), m_fpsScale );
	}

	bool PostEffect::doInitialise( castor3d::RenderDevice const & device
		, castor3d::Texture const & source
		, castor3d::Texture const & target
		, crg::FramePass const & previousPass )
	{
		auto extent = castor3d::makeExtent2D( target.getExtent() );
		m_pass = &m_graph.createPass( "LinearMotionBlur"
			, [this, &device, extent]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( extent )
					.texcoordConfig( {} )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.enabled( &isEnabled() )
					.passIndex( &m_passIndex )
					.build( framePass
						, context
						, graph
						, crg::ru::Config{ 2u } );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		m_pass->addDependency( previousPass );
		m_ubo.createPassBinding( *m_pass
			, "BlurCfg"
			, postfx::BlurCfgUboIdx );
		m_pass->addSampledView( crg::ImageViewIdArray{ source.sampledViewId, target.sampledViewId }
			, postfx::ColorTexIdx );
		m_pass->addSampledView( m_renderTarget.getVelocity().sampledViewId
			, postfx::VelocityTexIdx );
		m_pass->addOutputColourView( crg::ImageViewIdArray{ target.targetViewId, source.targetViewId } );
		m_saved = Clock::now();
		return true;
	}

	void PostEffect::doCleanup( castor3d::RenderDevice const & device )
	{
	}

	void PostEffect::doCpuUpdate( castor3d::CpuUpdater & updater )
	{
		if ( m_fpsScale )
		{
			auto current = Clock::now();
			auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( current - m_saved );
			auto fps = 1000.0f / float( duration.count() );
			auto & configuration = m_ubo.getData();
			configuration.samplesCount = m_configuration.samplesCount;
			configuration.vectorDivider = m_configuration.vectorDivider;
			configuration.blurScale = ( getRenderSystem()->getEngine()->getRenderLoop().getWantedFps() != castor3d::RenderLoop::UnlimitedFPS )
				? fps / float( getRenderSystem()->getEngine()->getRenderLoop().getWantedFps() )
				: 1.0f;
			m_saved = current;
		}
	}

	bool PostEffect::doWriteInto( castor::StringStream & file, castor::String const & tabs )
	{
		static Configuration const ref;
		file << ( cuT( "\n" ) + tabs + Type + cuT( "\n" ) );
		file << ( tabs + cuT( "{\n" ) );

		if ( m_configuration.vectorDivider != ref.vectorDivider )
		{
			file << ( tabs + cuT( "\tvectorDivider " ) + castor::string::toString( m_configuration.vectorDivider ) + cuT( "\n" ) );
		}

		if ( m_configuration.samplesCount != ref.samplesCount )
		{
			file << ( tabs + cuT( "\tsamples " ) + castor::string::toString( m_configuration.samplesCount ) + cuT( "\n" ) );
		}

		if ( !m_fpsScale )
		{
			file << ( tabs + cuT( "\tfpsScale false\n" ) );
		}

		file << ( tabs + cuT( "}\n" ) );
		return true;
	}

	//*********************************************************************************************
}
