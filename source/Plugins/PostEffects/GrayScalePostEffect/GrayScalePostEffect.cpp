#include "GrayScalePostEffect/GrayScalePostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>
#include <Castor3D/Buffer/GpuBuffer.hpp>
#include <Castor3D/Cache/ShaderCache.hpp>
#include <Castor3D/Model/Vertex.hpp>
#include <Castor3D/Scene/ParticleSystem/ParticleDeclaration.hpp>
#include <Castor3D/Scene/ParticleSystem/ParticleElementDeclaration.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderPipeline.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Render/RenderWindow.hpp>
#include <Castor3D/Render/Viewport.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

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

namespace grayscale
{
	namespace postfx
	{
		namespace c3ds = c3d::shader;

		enum Idx : uint32_t
		{
			GrayCfgUboIdx,
			ColorTexIdx,
		};

		static c3d::ShaderPtr getProgram( c3d::Engine & engine )
		{
			sdw::TraditionalGraphicsWriter writer{ &engine.getShaderAllocator() };

			auto configUbo = writer.declUniformBuffer( "Configuration", GrayCfgUboIdx, 0u );
			auto c3d_factors = configUbo.declMember< sdw::Vec3 >( "c3d_factors" );
			configUbo.end();
			auto c3d_mapColor = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapColor", ColorTexIdx, 0u );

			writer.implementEntryPointT< c3ds::PosUv2FT, c3ds::Uv2FT >( []( sdw::VertexInT< c3ds::PosUv2FT > const & in
				, sdw::VertexOutT< c3ds::Uv2FT > out )
				{
					out.uv() = in.uv();
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< c3ds::Uv2FT, c3ds::Colour4FT >( [&writer, &c3d_mapColor, &c3d_factors]( sdw::FragmentInT< c3ds::Uv2FT > const & in
				, sdw::FragmentOutT< c3ds::Colour4FT > const & out )
				{
					auto colour = writer.declLocale( "colour"
						, c3d_mapColor.sample( in.uv() ).xyz() );
					out.colour() = vec4( vec3( dot( c3d_factors, colour ) ), 1.0_f );
				} );
			return writer.getBuilder().releaseShader();
		}
	}

	//*********************************************************************************************

	c3d::String PostEffect::Type = cuT( "grayscale" );
	c3d::MbString PostEffect::Name = "GrayScale PostEffect";

	PostEffect::PostEffect( c3d::RenderTarget & renderTarget
		, c3d::RenderSystem & renderSystem
		, c3d::Parameters const & params )
		: c3d::PostEffect{ PostEffect::Type
			, cuT( "GrayScale" )
			, c3d::makeString( PostEffect::Name )
			, renderTarget
			, renderSystem
			, params }
		, m_configUbo{ renderSystem.getRenderDevice().uboPool->getBuffer< c3d::Point3f >( c3d::MemoryPropertyFlags::eNone ) }
		, m_shader{ cuT( "GrayScale" ), postfx::getProgram( *renderTarget.getEngine() ) }
		, m_stages{ makeProgramStates( renderSystem.getRenderDevice(), m_shader ) }
	{
	}

	PostEffect::~PostEffect()
	{
		getRenderSystem()->getRenderDevice().uboPool->putBuffer( m_configUbo );
	}

	c3d::PostEffectUPtr PostEffect::create( c3d::RenderTarget & renderTarget
		, c3d::RenderSystem & renderSystem
		, c3d::Parameters const & params )
	{
		return c3d::makeUniqueDerived< c3d::PostEffect, PostEffect >( renderTarget
			, renderSystem
			, params );
	}

	void PostEffect::accept( c3d::ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_shader );
		visitor.visit( cuT( "Factors" )
			, m_factors );
	}

	void PostEffect::setParameters( c3d::Parameters parameters )
	{
	}

	bool PostEffect::doInitialise( c3d::RenderDevice const & device
		, c3d::Texture const & source
		, c3d::Texture & target )
	{
		auto extent = c3d::makeExtent2D( target.getExtent() );
		auto & pass = m_graph.createPass( "GrayScale"
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
				device.renderSystem.getEngine()->registerTimer( c3d::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		m_configUbo.createPassBinding( pass, postfx::GrayCfgUboIdx );
		pass.addInputSampled( *source.getSampledLastAttach(), postfx::ColorTexIdx );
		target.setLastAttach( pass.addOutputColourTarget( crg::ImageViewIdArray{ target.getTargetViewId(), source.getTargetViewId() } ) );
		return true;
	}

	void PostEffect::doCleanup( c3d::RenderDevice const & device )
	{
	}

	void PostEffect::doCpuUpdate( c3d::CpuUpdater & updater )
	{
		if ( m_factors.isDirty() )
		{
			m_configUbo.getData() = m_factors.value();
			m_factors.reset();
		}
	}

	bool PostEffect::doWriteInto( c3d::StringStream & file, c3d::String const & tabs )
	{
		file << ( tabs + cuT( "postfx \"" ) + Type + cuT( "\"\n" ) );
		return true;
	}

	//*********************************************************************************************
}
