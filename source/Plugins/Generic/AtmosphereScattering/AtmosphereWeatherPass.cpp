#include "AtmosphereScattering/AtmosphereWeatherPass.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/Technique/RenderTechniqueVisitor.hpp>
#include <Castor3D/Shader/Program.hpp>

#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>

#include <ShaderWriter/Source.hpp>

#include <ashespp/Buffer/Buffer.hpp>

namespace atmosphere_scattering
{
	//*********************************************************************************************

	namespace volclouds
	{
		enum Bindings : uint32_t
		{
			eWeather,
			eOutput,
			eCount,
		};

		static castor3d::ShaderPtr getProgram( uint32_t dimension )
		{
			sdw::ComputeWriter writer;

			C3D_Weather( writer
				, uint32_t( Bindings::eWeather )
				, 0u );
			auto outputTexture = writer.declStorageImg< sdw::WImage2DRg32 >("outputTexture"
				, uint32_t( Bindings::eOutput )
				, 0u );

			auto random2D = writer.implementFunction< sdw::Float >( "random2D"
				, [&]( sdw::Vec2 const & st )
				{
					writer.returnStmt( fract( sin( dot( st.xy(), vec2( 12.9898_f, 78.233_f ) ) ) * 43758.5453123_f ) );
				}
				, sdw::InVec2{ writer, "coord" } );

			auto noiseInterpolation = writer.implementFunction< sdw::Float >( "noiseInterpolation"
				, [&]( sdw::Vec2 const & coord
					, sdw::Float const & size )
				{
					auto grid = writer.declLocale( "grid"
						, coord * size );

					auto randomInput = writer.declLocale( "randomInput"
						, floor( grid ) );
					auto weights = writer.declLocale( "weights"
						, fract( grid ) );

					auto p0 = writer.declLocale( "p0"
						, random2D( randomInput ) );
					auto p1 = writer.declLocale( "p1"
						, random2D( randomInput + vec2( 1.0_f, 0.0_f ) ) );
					auto p2 = writer.declLocale( "p2"
						, random2D( randomInput + vec2( 0.0_f, 1.0_f ) ) );
					auto p3 = writer.declLocale( "p3"
						, random2D( randomInput + vec2( 1.0_f, 1.0_f ) ) );

					weights = smoothStep( vec2( 0.0_f, 0.0_f )
						, vec2( 1.0_f, 1.0_f )
						, weights );

					writer.returnStmt( p0
						+ ( p1 - p0 ) * ( weights.x() )
						+ ( p2 - p0 ) * ( weights.y() ) * ( 1.0_f - weights.x() )
						+ ( p3 - p1 ) * ( weights.y() * weights.x() ) );
				}
				, sdw::InVec2{ writer, "coord" }
				, sdw::InFloat{ writer, "size" } );

			auto perlinNoise = writer.implementFunction< sdw::Float >( "perlinNoise"
				, [&]( sdw::Vec2 const & uv
					, sdw::Float const & sc
					, sdw::Float const & f
					, sdw::Float const & a
					, sdw::UInt const & o )
				{
					auto noiseValue = writer.declLocale( "noiseValue"
						, 0.0_f );
					auto localAmplitude = writer.declLocale( "localAmplitude"
						, a );
					auto localFrequency = writer.declLocale( "localFrequency"
						, f );

					FOR( writer, sdw::UInt, index, 0_u, index < o, ++index )
					{

						noiseValue += noiseInterpolation( uv, sc * localFrequency ) * localAmplitude;

						localAmplitude *= 0.25_f;
						localFrequency *= 3.0_f;
					}
					ROF;

					writer.returnStmt( noiseValue * noiseValue );
				}
				, sdw::InVec2{ writer, "uv" }
				, sdw::InFloat{ writer, "sc" }
				, sdw::InFloat{ writer, "f" }
				, sdw::InFloat{ writer, "a" }
				, sdw::InUInt{ writer, "o" } );

			writer.implementMainT< sdw::VoidT >( sdw::ComputeIn{ writer, 16u, 16u, 1u }
				, [&]( sdw::ComputeIn in )
				{
					auto pixel = writer.declLocale( "pixel"
						, ivec2( in.globalInvocationID.xy() ) );

					auto uv = writer.declLocale( "uv"
						, vec2( ( writer.cast< sdw::Float >( pixel.x() ) + 2.0f ) / float( dimension )
							, writer.cast< sdw::Float >( pixel.y() ) / float( dimension ) ) );
					auto suv = writer.declLocale( "suv"
						, uv + vec2( 5.5_f ) );

					auto cloudType = writer.declLocale( "cloudType"
						, clamp( perlinNoise( suv, c3d_weatherData.perlinScale() * 3.0_f
							, 0.3_f
							, 0.7_f
							, 10_u ), 0.0_f, 1.0_f ) );

					auto coverage = writer.declLocale( "coverage"
						, perlinNoise( uv
							, c3d_weatherData.perlinScale() * 0.95f
							, c3d_weatherData.perlinFrequency()
							, c3d_weatherData.perlinAmplitude()
							, c3d_weatherData.perlinOctaves() ) );
					auto weather = writer.declLocale( "weather"
						, vec2( clamp( coverage, 0.0_f, 1.0_f ), cloudType ) );

					outputTexture.store( pixel, weather );
				} );

			return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
		}
	}

	//************************************************************************************************

	AtmosphereWeatherPass::AtmosphereWeatherPass( crg::FramePassGroup & graph
		, crg::FramePassArray const & previousPasses
		, castor3d::RenderDevice const & device
		, WeatherUbo const & weatherUbo
		, crg::ImageViewId const & resultView
		, bool const & enabled )
		: m_computeShader{ VK_SHADER_STAGE_COMPUTE_BIT, "WeatherPass", volclouds::getProgram( getExtent( resultView ).width ) }
		, m_stages{ makeShaderState( device, m_computeShader ) }
	{
		auto renderSize = getExtent( resultView );
		auto & pass = graph.createPass( "WeatherPass"
			, [this, &device, &enabled, renderSize]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				auto result = std::make_unique< crg::ComputePass >( framePass
					, context
					, graph
					, crg::ru::Config{}
					, crg::cp::Config{}
						.groupCountX( renderSize.width / 8u )
						.groupCountY( renderSize.height / 8u )
						.enabled( &enabled )
						.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) ) );
				device.renderSystem.getEngine()->registerTimer( framePass.getFullName()
					, result->getTimer() );
				return result;
			} );
		pass.addDependencies( previousPasses );
		weatherUbo.createPassBinding( pass
			, volclouds::eWeather );
		pass.addOutputStorageView( resultView
			, volclouds::eOutput );
		m_lastPass = &pass;
	}

	void AtmosphereWeatherPass::accept( castor3d::PipelineVisitor & visitor )
	{
		visitor.visit( m_computeShader );
	}

	//************************************************************************************************
}
