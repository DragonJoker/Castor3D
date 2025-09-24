#include "AtmosphereScattering/CloudsWeatherPass.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Render/RenderTechniqueVisitor.hpp>
#include <Castor3D/Shader/Program.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>

#include <RenderGraph/RunnableGraph.hpp>
#include <RenderGraph/RunnablePasses/ComputePass.hpp>
#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <ashespp/Buffer/Buffer.hpp>

namespace atmosphere_scattering
{
	//*********************************************************************************************

	namespace weather
	{
		enum class Bindings : uint32_t
		{
			eWeather,
			eOutput,
			eCount,
		};

		static constexpr bool useCompute = true;

		template< bool ComputeT >
		struct ShaderWriter;

		template<>
		struct ShaderWriter< false >
		{
			using Type = sdw::TraditionalGraphicsWriter;

			template< typename FuncT >
			static void implementMain( Type & writer, FuncT func )
			{
				writer.implementEntryPointT< c3ds::Position2FT, sdw::VoidT >( []( sdw::VertexInT< c3ds::Position2FT > const & in
						, sdw::VertexOut out )
					{
						out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
					} );

				writer.implementEntryPointT< sdw::VoidT, c3ds::Colour4FT >( [&writer, &func]( sdw::FragmentIn const & in
					, sdw::FragmentOutT< c3ds::Colour4FT > const & out )
					{
						auto fragCoord = writer.declLocale( "fragCoord"
							, ivec2( in.fragCoord.xy() ) );
						out.colour() = func( fragCoord );
					} );
			}
		};

		template<>
		struct ShaderWriter< true >
		{
			using Type = sdw::ComputeWriter;

			template< typename FuncT >
			static void implementMain( Type & writer, FuncT func )
			{
				auto outputTexture = writer.declStorageImg< sdw::WImage2DRg32 >( "outputTexture"
					, uint32_t( Bindings::eOutput )
					, 0u );

				writer.implementMainT< sdw::VoidT >( sdw::ComputeIn{ writer, 8u, 8u, 1u }
					, [&writer, &func, &outputTexture]( sdw::ComputeIn const & in )
					{
						auto fragCoord = writer.declLocale( "fragCoord"
							, ivec2( in.globalInvocationID.xy() ) );
						outputTexture.store( fragCoord, func( fragCoord ) );
					} );
			}
		};

		static c3d::ShaderPtr getProgram( c3d::Engine & engine
			, uint32_t dimension )
		{
			ShaderWriter< useCompute >::Type writer{ &engine.getShaderAllocator() };

			C3D_Weather( writer
				, uint32_t( Bindings::eWeather )
				, 0u );

			auto random2D = writer.implementFunction< sdw::Float >( "random2D"
				, [&writer]( sdw::Vec2 const & st )
				{
					auto vRes = dvec2( 12.9898_d, 78.233_d );
					auto dRes = writer.cast< sdw::Float >( dot( dvec2( st ), vRes ) );
					auto sRes = writer.cast< sdw::Double >( sin( dRes ) );
					writer.returnStmt( writer.cast< sdw::Float >( fract( sRes * 43758.5453123_d ) ) );
				}
				, sdw::InVec2{ writer, "st" } );

			auto noiseInterpolation = writer.implementFunction< sdw::Float >( "noiseInterpolation"
				, [&writer, &random2D]( sdw::Vec2 const & coord
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

					weights = smoothStep( vec2( 0.0_f ), vec2( 1.0_f ), weights );

					writer.returnStmt( p0
						+ ( p1 - p0 ) * ( weights.x() )
						+ ( p2 - p0 ) * ( weights.y() ) * ( 1.0_f - weights.x() )
						+ ( p3 - p1 ) * ( weights.y() * weights.x() ) );
				}
				, sdw::InVec2{ writer, "coord" }
				, sdw::InFloat{ writer, "size" } );

			auto perlinNoise = writer.implementFunction< sdw::Float >( "perlinNoise"
				, [&writer, &noiseInterpolation]( sdw::Vec2 const & coord
					, sdw::Float const & scale
					, sdw::Float const & frequency
					, sdw::Float const & amplitude
					, sdw::UInt const & octaves )
				{
					auto noiseValue = writer.declLocale( "noiseValue"
						, 0.0_f );
					auto localAmplitude = writer.declLocale( "localAmplitude"
						, amplitude );
					auto localFrequency = writer.declLocale( "localFrequency"
						, frequency );

					sdwFOR( writer, sdw::UInt, index, 0_u, index < octaves, ++index )
					{
						noiseValue += noiseInterpolation( coord, scale * localFrequency ) * localAmplitude;

						localAmplitude *= 0.25_f;
						localFrequency *= 3.0_f;
					}
					sdwROF

					writer.returnStmt( noiseValue * noiseValue );
				}
				, sdw::InVec2{ writer, "coord" }
				, sdw::InFloat{ writer, "scale" }
				, sdw::InFloat{ writer, "frequency" }
				, sdw::InFloat{ writer, "amplitude" }
				, sdw::InUInt{ writer, "octaves" } );

			ShaderWriter< useCompute >::implementMain( writer
				, [&writer, &perlinNoise, &c3d_weatherData, &dimension]( sdw::IVec2 const & pixel )
				{
					auto uv = writer.declLocale( "uv"
						, vec2( ( writer.cast< sdw::Float >( pixel.x() ) ) / float( dimension )
							, writer.cast< sdw::Float >( pixel.y() ) / float( dimension ) ) );
					auto suv = writer.declLocale( "suv"
						, uv + vec2( 5.5_f ) );

					auto cloudType = writer.declLocale( "cloudType"
						, clamp( perlinNoise( suv
							, c3d_weatherData.perlinScale() * 3.0_f
							, 0.3_f
							, 0.7_f
							, 10_u ), 0.0_f, 1.0_f ) );

					auto coverage = writer.declLocale( "coverage"
						, perlinNoise( uv
							, c3d_weatherData.perlinScale() * 0.95f
							, c3d_weatherData.perlinFrequency()
							, c3d_weatherData.perlinAmplitude()
							, c3d_weatherData.perlinOctaves() ) );
					return vec2( clamp( coverage, 0.0_f, 1.0_f ), cloudType );
				} );

			return writer.getBuilder().releaseShader();
		}
	}

	//************************************************************************************************

	CloudsWeatherPass::CloudsWeatherPass( crg::FramePassGroup & graph
		, c3d::RenderDevice const & device
		, WeatherUbo const & weatherUbo
		, c3d::Texture & result
		, bool const & enabled )
		: m_shader{ cuT( "Clouds/WeatherPass" ), weather::getProgram( *device.renderSystem.getEngine(), result.getExtent().width ) }
		, m_stages{ makeProgramStates( device, m_shader ) }
	{
		auto renderSize = result.getExtent();
		auto & pass = graph.createPass( "Clouds/WeatherPass"
			, [this, &device, &enabled, renderSize]( crg::FramePass const & framePass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph )
			{
				crg::RunnablePassPtr result;

				if constexpr ( weather::useCompute )
				{
					result = c3d::makeRawUnique< crg::ComputePass >( framePass
						, context
						, graph
						, crg::ru::Config{}
						, crg::cp::Config{}
							.groupCountX( renderSize.width / 8u )
							.groupCountY( renderSize.height / 8u )
							.enabled( &enabled )
							.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) ) );
				}
				else
				{
					result = crg::RenderQuadBuilder{}
						.renderSize( { renderSize.width, renderSize.height } )
						.enabled( &enabled )
						.program( ashes::makeVkArray< VkPipelineShaderStageCreateInfo >( m_stages ) )
						.build( framePass, context, graph );
				}

				device.renderSystem.getEngine()->registerTimer( c3d::makeString( framePass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		weatherUbo.createPassBinding( pass, weather::Bindings::eWeather );

		if constexpr ( weather::useCompute )
		{
			result.setLastAttach( pass.addOutputStorageImageT( result.getTargetViewId(), weather::Bindings::eOutput ) );
		}
		else
		{
			result.setLastAttach( pass.addOutputColourTarget( result.getTargetViewId() ) );
		}
	}

	void CloudsWeatherPass::accept( c3d::ConfigurationVisitorBase & visitor )const
	{
		visitor.visit( m_shader );
	}

	//************************************************************************************************
}
