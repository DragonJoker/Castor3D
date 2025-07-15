#include "LightStreaksPostEffect/HiPass.hpp"

#include "LightStreaksPostEffect/LightStreaksPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>

#include <CastorUtils/Graphics/Image.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/RunnablePasses/ImageCopy.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <numeric>

namespace light_streaks
{
	namespace hipass
	{
		namespace c3ds = c3d::shader;

		template< typename T >
		inline constexpr T getSubresourceDimension( T const & extent
			, uint32_t mipLevel )noexcept
		{
			return std::max( T( 1 ), T( extent >> mipLevel ) );
		}

		static c3d::ShaderPtr getProgram( c3d::RenderDevice const & device )
		{
			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			auto c3d_mapColor = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapColor", 0u, 0u );

			writer.implementEntryPointT< c3ds::PosUv2FT, c3ds::Uv2FT >( [&]( sdw::VertexInT< c3ds::PosUv2FT > in
				, sdw::VertexOutT< c3ds::Uv2FT > out )
				{
					out.uv() = in.uv();
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< c3ds::Uv2FT, c3ds::Colour4FT >( [&]( sdw::FragmentInT< c3ds::Uv2FT > in
				, sdw::FragmentOutT< c3ds::Colour4FT > out )
				{
					out.colour() = vec4( c3d_mapColor.sample( in.uv(), 0.0_f ).xyz(), 1.0_f );
					auto maxComponent = writer.declLocale( "maxComponent"
						, max( out.colour().r(), out.colour().g() ) );
					maxComponent = max( maxComponent, out.colour().b() );

					sdwIF( writer, maxComponent > 1.0_f )
					{
						out.colour().xyz() /= maxComponent;
					}
					sdwELSE
					{
						out.colour().xyz() = vec3( 0.0_f, 0.0_f, 0.0_f );
					}
					sdwFI
				} );
			return writer.getBuilder().releaseShader();
		}
	}

	//*********************************************************************************************

	HiPass::HiPass( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, c3d::RenderDevice const & device
		, crg::ImageViewIdArray const & sceneView
		, crg::ImageViewIdArray const & resultViews
		, c3d::Extent2D size
		, bool const * enabled
		, uint32_t const * passIndex )
		: m_shader{ cuT( "LightStreaksHiPass" ), hipass::getProgram( device ) }
		, m_stages{ makeProgramStates( device, m_shader ) }
	{
		auto previous = &previousPass;
		auto & hiPass = graph.createPass( "HDR"
			, [this, &device, size, enabled, passIndex]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = crg::RenderQuadBuilder{}
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
					.texcoordConfig( crg::Texcoord{} )
					.enabled( enabled )
					.passIndex( passIndex )
					.renderSize( size )
					.build( framePass
						, context
						, graph
						, crg::ru::Config{ 2u } );
				device.renderSystem.getEngine()->registerTimer( c3d::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		crg::SamplerDesc linearSampler{ c3d::FilterMode::eLinear
			, c3d::FilterMode::eLinear
			, c3d::MipmapMode::eNearest
			, c3d::WrapMode::eClampToEdge
			, c3d::WrapMode::eClampToEdge
			, c3d::WrapMode::eClampToEdge };
		hiPass.addDependency( *previous );
		hiPass.addSampledView( sceneView
			, 0u
			, linearSampler );
		hiPass.addOutputColourView( resultViews[0] );

		for ( uint32_t i = 1u; i < resultViews.size(); ++i )
		{
			auto & pass = graph.createPass( "Copy" + c3d::string::toMbString( i )
				, [&device, size, enabled]( crg::FramePass const & framePass
					, crg::GraphContext & context
					, crg::RunnableGraph & graph )
				{
					auto result = c3d::makeRawUnique< crg::ImageCopy >( framePass
						, context
						, graph
						, c3d::Extent3D{ size.width, size.height, 1u }
						, crg::ru::Config{}
						, crg::RunnablePass::GetPassIndexCallback( [](){ return 0u; } )
						, crg::RunnablePass::IsEnabledCallback( [enabled](){ return ( enabled ? *enabled : true ); } ) );
					device.renderSystem.getEngine()->registerTimer( c3d::makeString( framePass.getFullName() )
						, result->getTimer() );
					return result;
				} );
			pass.addDependency( hiPass );
			pass.addTransferInputView( resultViews[0u] );
			pass.addTransferOutputView( resultViews[i] );
			m_lastPasses.push_back( &pass );
		}
	}

	void HiPass::accept( c3d::ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_shader );
	}
}
