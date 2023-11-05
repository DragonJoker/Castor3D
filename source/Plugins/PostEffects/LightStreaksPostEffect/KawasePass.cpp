#include "LightStreaksPostEffect/KawasePass.hpp"

#include "LightStreaksPostEffect/LightStreaksPostEffect.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>

#include <CastorUtils/Graphics/Image.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <numeric>

namespace light_streaks
{
	namespace kawase
	{
		namespace c3d = castor3d::shader;

		enum Idx
		{
			KawaseUboIdx,
			DifImgIdx,
		};

		static castor3d::ShaderPtr getProgram( castor3d::RenderDevice const & device )
		{
			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			C3D_Kawase( writer, KawaseUboIdx, 0u );
			auto c3d_mapHiPass = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapHiPass", DifImgIdx, 0u );

			writer.implementEntryPointT< c3d::PosUv2FT, c3d::Uv2FT >( [&]( sdw::VertexInT< c3d::PosUv2FT > in
				, sdw::VertexOutT< c3d::Uv2FT > out )
				{
					out.uv() = in.uv();
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPointT< c3d::Uv2FT, c3d::Colour4FT >( [&]( sdw::FragmentInT< c3d::Uv2FT > in
				, sdw::FragmentOutT< c3d::Colour4FT > out )
				{
					auto colour = writer.declLocale( "colour"
						, vec3( 0.0_f ) );
					auto b = writer.declLocale( "b"
						, pow( writer.cast< sdw::Float >( c3d_kawaseData.samples ), writer.cast< sdw::Float >( c3d_kawaseData.pass ) ) );
					auto texcoords = writer.declLocale( "texcoords"
						, in.uv() );

					FOR( writer, sdw::Int, s, 0, s < c3d_kawaseData.samples, ++s )
					{
						// Weight = a^(b*s)
						auto weight = writer.declLocale( "weight"
							, pow( c3d_kawaseData.attenuation, b * writer.cast< sdw::Float >( s ) ) );
						// Streak direction is a 2D vector in image space
						auto sampleCoord = writer.declLocale( "sampleCoord"
							, texcoords + ( c3d_kawaseData.direction * b * vec2( s, s ) * c3d_kawaseData.pixelSize ) );
						// Scale and accumulate
						colour += c3d_mapHiPass.sample( sampleCoord ).rgb() * clamp( weight, 0.0_f, 1.0_f );
					}
					ROF;

					out.colour() = vec4( clamp( colour, vec3( 0.0_f ), vec3( 1.0_f ) ), 1.0_f );
				} );
			return writer.getBuilder().releaseShader();
		}

		static std::vector< KawasePass::Subpass > doCreateSubpasses( crg::FramePassGroup & graph
			, crg::FramePassArray const & previousPasses
			, castor3d::RenderDevice const & device
			, crg::ImageViewIdArray const & srcImages
			, crg::ImageViewIdArray const & dstImages
			, VkExtent2D dimensions
			, ashes::PipelineShaderStageCreateInfoArray const & stages
			, KawaseUbo const & kawaseUbo
			, bool const * enabled
			, crg::FramePassArray & lastPasses )
		{
			std::vector< KawasePass::Subpass > result;
			assert( srcImages.size() == dstImages.size() + 1u
				&& dstImages.size() == PostEffect::Count );
			uint32_t index = 0u;

			for ( auto i = 0u; i < PostEffect::Count; ++i )
			{
				auto * source = &srcImages[i + 1u];
				auto * destination = &dstImages[i];
				auto previousPass = previousPasses[i];
				result.emplace_back( graph
					, *previousPass
					, device
					, *source
					, *destination
					, dimensions
					, stages
					, kawaseUbo
					, index
					, enabled );
				previousPass = &result.back().pass;
				++index;

				for ( auto j = 1u; j < 3u; ++j )
				{
					std::swap( source, destination );
					result.emplace_back( graph
						, *previousPass
						, device
						, *source
						, *destination
						, dimensions
						, stages
						, kawaseUbo
						, index
						, enabled );
					previousPass = &result.back().pass;
					++index;
				}

				lastPasses.push_back( previousPass );
			}

			return result;
		}
	}

	//*********************************************************************************************

	KawasePass::Subpass::Subpass( crg::FramePassGroup & graph
		, crg::FramePass const & previousPass
		, castor3d::RenderDevice const & device
		, crg::ImageViewId const & srcView
		, crg::ImageViewId const & dstView
		, VkExtent2D dimensions
		, ashes::PipelineShaderStageCreateInfoArray const & stages
		, KawaseUbo const & kawaseUbo
		, uint32_t index
		, bool const * enabled )
		: pass{ graph.createPass( "Kawase" + std::to_string( index )
			, [&device, &stages, dimensions, enabled]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = crg::RenderQuadBuilder{}
					.renderPosition( {} )
					.renderSize( dimensions )
					.texcoordConfig( crg::Texcoord{} )
					.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( stages ) )
					.enabled( enabled )
					.build( framePass, context, graph );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} ) }
	{
		crg::SamplerDesc linearSampler{ VK_FILTER_LINEAR
			, VK_FILTER_LINEAR
			, VK_SAMPLER_MIPMAP_MODE_NEAREST
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE };
		pass.addDependency( previousPass );
		kawaseUbo.createPassBinding( pass
			, kawase::KawaseUboIdx
			, index );
		pass.addSampledView( srcView
			, kawase::DifImgIdx
			, linearSampler );
		pass.addOutputColourView( dstView );
	}

	//*********************************************************************************************

	KawasePass::KawasePass( crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, castor3d::RenderDevice const & device
		, crg::ImageViewIdArray const & hiViews
		, crg::ImageViewIdArray const & kawaseViews
		, KawaseUbo & kawaseUbo
		, VkExtent2D dimensions
		, bool const * enabled )
		: m_device{ device }
		, m_kawaseUbo{ kawaseUbo }
		, m_shader{ "LightStreaksKawasePass", kawase::getProgram( device ) }
		, m_stages{ makeProgramStates( device, m_shader ) }
		, m_subpasses{ kawase::doCreateSubpasses( graph
			, previousPasses
			, m_device
			, hiViews
			, kawaseViews
			, dimensions
			, m_stages
			, m_kawaseUbo
			, enabled
			, m_lastPasses ) }
	{
	}

	void KawasePass::accept( castor3d::ConfigurationVisitorBase & visitor )
	{
		visitor.visit( m_shader );
	}
}
