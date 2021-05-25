#include "FxaaPostEffect/FxaaPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Cache/SamplerCache.hpp>
#include <Castor3D/Cache/ShaderCache.hpp>
#include <Castor3D/Material/Texture/Sampler.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderPassTimer.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTarget.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>

#include <RenderGraph/RenderQuad.hpp>

#include <numeric>

using namespace castor;

namespace fxaa
{
	namespace
	{
		static String const PosPos = cuT( "vtx_posPos" );

		enum Idx : uint32_t
		{
			FxaaCfgUboIdx,
			ColorTexIdx,
		};

		std::unique_ptr< ast::Shader > getVertexProgram()
		{
			using namespace sdw;
			VertexWriter writer;

			// Shader inputs
			UBO_FXAA( writer, 0u, 0u );
			auto position = writer.declInput< Vec2 >( "position", 0u );
			auto uv = writer.declInput< Vec2 >( "uv", 1u );

			// Shader outputs
			auto vtx_texture = writer.declOutput< Vec2 >( "vtx_texture", 0u );
			auto vtx_posPos = writer.declOutput< Vec4 >( PosPos, 1u );
			auto out = writer.getOut();

			castor3d::shader::Utils utils{ writer };
			utils.declareInvertVec2Y();

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					vtx_texture = uv;
					out.vtx.position = vec4( position.xy(), 0.0_f, 1.0_f );
					vtx_posPos.xy() = position.xy();
					vtx_posPos.zw() = position.xy() - ( c3d_pixelSize * ( 0.5_f + c3d_subpixShift ) );
				} );
			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}

		std::unique_ptr< ast::Shader > getFragmentProgram()
		{
			using namespace sdw;
			FragmentWriter writer;

			// Shader inputs
			UBO_FXAA( writer, FxaaCfgUboIdx, 0u );
			auto c3d_mapColor = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapColor", ColorTexIdx, 0u );
			auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );
			auto vtx_posPos = writer.declInput< Vec4 >( PosPos, 1u );

			// Shader outputs
			auto pxl_fragColor = writer.declOutput< Vec4 >( "pxl_fragColor", 0 );

#define FXAA_REDUCE_MIN	Float{ 1.0 / 128.0 }

			writer.implementFunction< sdw::Void >( "main"
				, [&]()
				{
					auto rgbNW = writer.declLocale( "rgbNW"
						, c3d_mapColor.lod( vtx_texture, 0.0_f, ivec2( -1_i, -1_i ) ).rgb() );
					auto rgbNE = writer.declLocale( "rgbNE"
						, c3d_mapColor.lod( vtx_texture, 0.0_f, ivec2( 1_i, -1_i ) ).rgb() );
					auto rgbSW = writer.declLocale( "rgbSW"
						, c3d_mapColor.lod( vtx_texture, 0.0_f, ivec2( -1_i, 1_i ) ).rgb() );
					auto rgbSE = writer.declLocale( "rgbSE"
						, c3d_mapColor.lod( vtx_texture, 0.0_f, ivec2( 1_i, 1_i ) ).rgb() );
					auto rgbM = writer.declLocale( "rgbM"
						, c3d_mapColor.sample( vtx_texture, 0.0_f ).rgb() );

					auto luma = writer.declLocale( "luma"
						, vec3( 0.299_f, 0.587_f, 0.114_f ) );
					auto lumaNW = writer.declLocale( "lumaNW"
						, dot( rgbNW, luma ) );
					auto lumaNE = writer.declLocale( "lumaNE"
						, dot( rgbNE, luma ) );
					auto lumaSW = writer.declLocale( "lumaSW"
						, dot( rgbSW, luma ) );
					auto lumaSE = writer.declLocale( "lumaSE"
						, dot( rgbSE, luma ) );
					auto lumaM = writer.declLocale( "lumaM"
						, dot( rgbM, luma ) );

					auto lumaMin = writer.declLocale( "lumaMin"
						, min( lumaM, min( min( lumaNW, lumaNE ), min( lumaSW, lumaSE ) ) ) );
					auto lumaMax = writer.declLocale( "lumaMax"
						, max( lumaM, max( max( lumaNW, lumaNE ), max( lumaSW, lumaSE ) ) ) );

					auto dir = writer.declLocale( "dir"
						, vec2( -( ( lumaNW + lumaNE ) - ( lumaSW + lumaSE ) )
							, ( ( lumaNW + lumaSW ) - ( lumaNE + lumaSE ) ) ) );

					auto dirReduce = writer.declLocale( "dirReduce"
						, max( ( lumaNW + lumaNE + lumaSW + lumaSE ) * ( 0.25_f * c3d_reduceMul ), FXAA_REDUCE_MIN ) );
					auto rcpDirMin = writer.declLocale( "rcpDirMin"
						, 1.0_f / ( min( abs( dir.x() ), abs( dir.y() ) ) + dirReduce ) );
					dir = min( vec2( c3d_spanMax, c3d_spanMax )
						, max( vec2( -c3d_spanMax, -c3d_spanMax )
							, dir * rcpDirMin ) ) * c3d_pixelSize;

					auto texcoord0 = writer.declLocale( "texcoord0"
						, vtx_texture + dir * ( 1.0_f / 3.0_f - 0.5_f ) );
					auto texcoord1 = writer.declLocale( "texcoord1"
						, vtx_texture + dir * ( 2.0_f / 3.0_f - 0.5_f ) );

					auto rgbA = writer.declLocale( "rgbA"
						, ( c3d_mapColor.sample( texcoord0, 0.0_f ).rgb()
								+ c3d_mapColor.sample( texcoord1, 0.0_f ).rgb() )
							* ( 1.0_f / 2.0_f ) );

					texcoord0 = vtx_texture + dir * ( 0.0_f / 3.0_f - 0.5_f );
					texcoord1 = vtx_texture + dir * ( 3.0_f / 3.0_f - 0.5_f );

					auto rgbB = writer.declLocale( "rgbB"
						, ( rgbA * 1.0_f / 2.0_f )
							+ ( c3d_mapColor.sample( texcoord0, 0.0_f ).rgb()
									+ c3d_mapColor.sample( texcoord1, 0.0_f ).rgb() )
								* ( 1.0_f / 4.0_f ) );
					auto lumaB = writer.declLocale( "lumaB"
						, dot( rgbB, luma ) );

					pxl_fragColor = vec4( writer.ternary( lumaB < lumaMin || lumaB > lumaMax, rgbA, rgbB )
						, 1.0_f );
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//*********************************************************************************************

	String PostEffect::Type = cuT( "fxaa" );
	String PostEffect::Name = cuT( "FXAA PostEffect" );

	PostEffect::PostEffect( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & parameters )
		: castor3d::PostEffect{ PostEffect::Type
			, PostEffect::Name
			, renderTarget
			, renderSystem
			, parameters
			, 1u }
		, m_vertexShader{ VK_SHADER_STAGE_VERTEX_BIT, "Fxaa", getVertexProgram() }
		, m_pixelShader{ VK_SHADER_STAGE_FRAGMENT_BIT, "Fxaa", getFragmentProgram() }
		, m_stages{ makeShaderState( *renderSystem.getMainRenderDevice(), m_vertexShader )
			, makeShaderState( *renderSystem.getMainRenderDevice(), m_pixelShader ) }
		, m_fxaaUbo{ *renderSystem.getMainRenderDevice(), m_renderTarget.getSize() }
	{
		String param;

		if ( parameters.get( cuT( "SubpixShift" ), param ) )
		{
			m_subpixShift = string::toFloat( param );
		}

		if ( parameters.get( cuT( "MaxSpan" ), param ) )
		{
			m_spanMax = string::toFloat( param );
		}

		if ( parameters.get( cuT( "ReduceMul" ), param ) )
		{
			m_reduceMul = string::toFloat( param );
		}
	}

	PostEffect::~PostEffect()
	{
	}

	castor3d::PostEffectSPtr PostEffect::create( castor3d::RenderTarget & renderTarget
		, castor3d::RenderSystem & renderSystem
		, castor3d::Parameters const & params )
	{
		return std::make_shared< PostEffect >( renderTarget
			, renderSystem
			, params );
	}

	void PostEffect::accept( castor3d::PipelineVisitorBase & visitor )
	{
		visitor.visit( m_vertexShader );
		visitor.visit( m_pixelShader );
		visitor.visit( cuT( "Fxaa" )
			, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "Fxaa" )
			, cuT( "SubPixShift" )
			, m_subpixShift );
		visitor.visit( cuT( "Fxaa" )
			, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "Fxaa" )
			, cuT( "SpanMax" )
			, m_spanMax );
		visitor.visit( cuT( "Fxaa" )
			, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "Fxaa" )
			, cuT( "ReduceMul" )
			, m_reduceMul );
	}

	void PostEffect::update( castor3d::CpuUpdater & updater )
	{
		if ( m_subpixShift.isDirty()
			|| m_spanMax.isDirty()
			|| m_reduceMul.isDirty() )
		{
			m_fxaaUbo.cpuUpdate( m_subpixShift
				, m_spanMax
				, m_reduceMul );
			m_subpixShift.reset();
			m_spanMax.reset();
			m_reduceMul.reset();
		}
	}

	crg::ImageViewId const * PostEffect::doInitialise( castor3d::RenderDevice const & device
		, castor3d::RenderPassTimer const & timer
		, crg::FramePass const & previousPass )
	{
		m_resultImg = m_renderTarget.getGraph().createImage( crg::ImageData{ "FxaaResult"
			, 0u
			, VK_IMAGE_TYPE_2D
			, m_target->data->info.format
			, castor3d::makeExtent3D( m_renderTarget.getSize() )
			, ( VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				| VK_IMAGE_USAGE_SAMPLED_BIT
				| VK_IMAGE_USAGE_TRANSFER_SRC_BIT ) } );
		m_resultView = m_renderTarget.getGraph().createView( crg::ImageViewData{ "FxaaResult"
			, m_resultImg
			, 0u
			, VK_IMAGE_VIEW_TYPE_2D
			, m_resultImg.data->info.format
			, { VK_IMAGE_ASPECT_COLOR_BIT, 0u, 1u, 0u, 1u } } );
		m_pass = &m_renderTarget.getGraph().createPass( "FxaaPass"
			, [this]( crg::FramePass const & pass
				, crg::GraphContext const & context
				, crg::RunnableGraph & graph )
			{
				return crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( castor3d::makeExtent2D( m_renderTarget.getSize() ) )
					.texcoordConfig( {} )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.build( pass, context, graph );
			} );
		m_pass->addDependency( previousPass );
		m_fxaaUbo.createPassBinding( *m_pass
			, FxaaCfgUboIdx );
		m_pass->addSampledView( *m_target
			, ColorTexIdx
			, VK_IMAGE_LAYOUT_UNDEFINED );
		m_pass->addOutputColourView( m_resultView );
		return &m_resultView;
	}

	void PostEffect::doCleanup( castor3d::RenderDevice const & device )
	{
	}

	bool PostEffect::doWriteInto( StringStream & file, String const & tabs )
	{
		file << ( tabs + cuT( "postfx \"" ) + Type + cuT( "\"" ) );
		return true;
	}
}
