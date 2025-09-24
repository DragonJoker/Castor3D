#include "Castor3D/Render/Ssao/SsaoBlurPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Miscellaneous/ProgressBar.hpp"
#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslBaseIO.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/SsaoConfigUbo.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Image/Image.hpp>
#include <ashespp/Pipeline/GraphicsPipelineCreateInfo.hpp>
#include <ashespp/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

#include <RenderGraph/FrameGraph.hpp>

CU_ImplementSmartPtr( c3d, SsaoBlurPass )

namespace c3d
{
	namespace ssaoblr
	{
		enum class Idx : uint32_t
		{
			SsaoCfgUboIdx = 0u,
			CameraUboIdx,
			BlurCfgUboIdx,
			NmlImgIdx,
			InpImgIdx,
			BntImgIdx,
		};

		static ShaderPtr getProgram( RenderDevice const & device
			, bool useNormalsBuffer )
		{
			sdw::TraditionalGraphicsWriter writer{ &device.renderSystem.getEngine()->getShaderAllocator() };

			C3D_SsaoConfig( writer, Idx::SsaoCfgUboIdx, 0u );
			C3D_Camera( writer, Idx::CameraUboIdx, 0u );
			auto configuration = writer.declUniformBuffer( "BlurConfiguration", uint32_t( Idx::BlurCfgUboIdx ), 0u );
			/** (1, 0) or (0, 1)*/
			auto c3d_axis = configuration.declMember< sdw::IVec2 >( "c3d_axis" );
			auto c3d_dummy = configuration.declMember< sdw::IVec2 >( "c3d_dummy" );
			auto c3d_gaussian = configuration.declMember< sdw::Vec4 >( "c3d_gaussian", 2u );
			configuration.end();
			auto c3d_mapNormal = writer.declCombinedImg< Img2DRgba >( "c3d_mapNormal", uint32_t( Idx::NmlImgIdx ), 0u, useNormalsBuffer );
			auto c3d_mapInput = writer.declCombinedImg< Img2DRg >( "c3d_mapInput", uint32_t( Idx::InpImgIdx ), 0u );
			auto c3d_mapBentInput = writer.declCombinedImg< Img2DRgba >( "c3d_mapBentInput", uint32_t( Idx::BntImgIdx ), 0u );

			/** Same size as result buffer, do not offset by guard band when reading from it */
			auto c3d_readMultiplyFirst = writer.declConstant( "c3d_readMultiplyFirst", vec3( 2.0_f ) );
			auto c3d_readAddSecond = writer.declConstant( "c3d_readAddSecond", vec3( 1.0_f ) );

			// Shader outputs
			auto outColour = writer.declOutput< sdw::Vec3 >( "outColour", sdw::EntryPoint::eFragment, 0u );
			auto outBentNormal = writer.declOutput< sdw::Vec3 >( "outBentNormal", sdw::EntryPoint::eFragment, 1u );

#define result outColour.r()
#define keyPassThrough outColour.g()

			/** Returns a number on (0, 1) */
			auto unpackKey = writer.implementFunction< sdw::Float >( "unpackKey"
				, [&writer]( sdw::Float const & p )
				{
					writer.returnStmt( p );
				}
				, sdw::InFloat{ writer, "p" } );

			// Reconstruct camera-space P.xyz from screen-space S = (x, y) in
			// pixels and camera-space z < 0.  Assumes that the upper-left pixel center
			// is at (0.5, 0.5) [but that need not be the location at which the sample tap
			// was placed!]
			// Costs 3 MADD.  Error is on the order of 10^3 at the far plane, partly due to z precision.
			auto reconstructCSPosition = writer.implementFunction< sdw::Vec3 >( "reconstructCSPosition"
				, [&writer]( sdw::Vec2 const & S
					, sdw::Float const & z
					, sdw::Vec4 const & projInfo )
				{
					writer.returnStmt( vec3( sdw::fma( S.xy(), projInfo.xy(), projInfo.zw() ) * z, z ) );
				}
				, sdw::InVec2{ writer, "S" }
				, sdw::InFloat{ writer, "z" }
				, sdw::InVec4{ writer, "projInfo" } );

			auto positionFromKey = writer.implementFunction< sdw::Vec3 >( "positionFromKey"
				, [&writer, c3d_ssaoConfigData, &reconstructCSPosition]( sdw::Float const & key
					, sdw::IVec2 const & ssCenter
					, sdw::Vec4 const & projInfo )
				{
					auto z = writer.declLocale( "z"
						, key * c3d_ssaoConfigData.farPlaneZ );
					auto position = writer.declLocale( "position"
						, reconstructCSPosition( vec2( ssCenter ) + vec2( 0.5_f )
							, z
							, projInfo ) );
					writer.returnStmt( position );
				}
				, sdw::InFloat{ writer, "key" }
				, sdw::InIVec2{ writer, "ssCenter" }
				, sdw::InVec4{ writer, "projInfo" } );

			auto getTapInformation = writer.implementFunction< sdw::Vec3 >( "getTapInformation"
				, [&writer, &c3d_mapInput, c3d_mapNormal, c3d_readMultiplyFirst, c3d_readAddSecond, &unpackKey, useNormalsBuffer]( sdw::IVec2 const & tapLoc
					, sdw::Float tapKey
					, sdw::Float value
					, sdw::Vec3 bent )
				{
					auto res = writer.declLocale( "result"
						, vec3( 0.0_f ) );
					auto temp = writer.declLocale( "temp"
						, c3d_mapInput.fetch( tapLoc, 0_i ) );
					tapKey = unpackKey( temp.g() );
					value = temp.r();

					if ( useNormalsBuffer )
					{
						res = c3d_mapNormal.fetch( tapLoc, 0_i ).xyz();
						res = normalize( sdw::fma( res, c3d_readMultiplyFirst.xyz(), c3d_readAddSecond.xyz() ) );
					}

					bent = vec3( 0.0_f );
					writer.returnStmt( res );
				}
				, sdw::InIVec2{ writer, "tapLoc" }
				, sdw::OutFloat{ writer, "tapKey" }
				, sdw::OutFloat{ writer, "value" }
				, sdw::OutVec3{ writer, "bent" } );

			auto square = writer.implementFunction< sdw::Float >( "square"
				, [&writer]( sdw::Float const & x )
				{
					writer.returnStmt( x * x );
				}
				, sdw::InFloat{ writer, "x" } );

			auto calculateBilateralWeight = writer.implementFunction< sdw::Float >( "calculateBilateralWeight"
				, [&writer, &c3d_ssaoConfigData, &positionFromKey, &square, useNormalsBuffer]( sdw::Float const & key
					, sdw::Float const & tapKey
					, sdw::IVec2 const & tapLoc
					, sdw::Vec3 const & normal
					, sdw::Vec3 const & tapNormal
					, sdw::Vec3 const & position )
				{
					auto scale = writer.declLocale( "scale"
						, 1.5_f * c3d_ssaoConfigData.invRadius );

					// The "bilateral" weight. As depth difference increases, decrease weight.
					// The key values are in scene-specific scale. To make them scale-invariant, factor in
					// the AO radius, which should be based on the scene scale.
					auto depthWeight = writer.declLocale( "depthWeight"
						, max( 0.0_f, 1.0_f - ( c3d_ssaoConfigData.edgeSharpness * 2000.0_f ) * abs( tapKey - key ) * scale ) );
					auto k_normal = writer.declLocale( "k_normal"
						, 1.0_f );
					auto k_plane = writer.declLocale( "k_plane"
						, 1.0_f );

					// Prevents blending over creases. 
					auto normalWeight = writer.declLocale( "normalWeight"
						, 1.0_f );
					auto planeWeight = writer.declLocale( "planeWeight"
						, 1.0_f );

					if ( useNormalsBuffer )
					{
						auto normalCloseness = writer.declLocale( "normalCloseness"
							, dot( tapNormal, normal ) );

						sdwIF( writer, c3d_ssaoConfigData.blurHighQuality == 0_i )
						{
							normalCloseness = normalCloseness * normalCloseness;
							normalCloseness = normalCloseness * normalCloseness;
							k_normal = 4.0_f;
						}
						sdwFI

						auto normalError = writer.declLocale( "normalError"
							, ( 1.0_f - normalCloseness ) * k_normal );
						normalWeight = max( 1.0_f - c3d_ssaoConfigData.edgeSharpness * normalError, 0.0_f );

						sdwIF( writer, c3d_ssaoConfigData.blurHighQuality )
						{
							auto lowDistanceThreshold2 = writer.declLocale( "lowDistanceThreshold2"
								, 0.001_f );

							auto tapPosition = writer.declLocale( "tapPosition"
								, positionFromKey( tapKey, tapLoc, c3d_ssaoConfigData.projInfo ) );

							// Change position in camera space
							auto dq = writer.declLocale( "dq"
								, position - tapPosition );

							// How far away is this point from the original sample
							// in camera space? (Max value is unbounded)
							auto distance2 = writer.declLocale( "distance2"
								, dot( dq, dq ) );

							// How far off the expected plane (on the perpendicular) is this point?  Max value is unbounded.
							auto planeError = writer.declLocale( "planeError"
								, max( abs( dot( dq, tapNormal ) ), abs( dot( dq, normal ) ) ) );

							// Minimum distance threshold must be scale-invariant, so factor in the radius
							planeWeight = writer.ternary( distance2 * square( scale ) < lowDistanceThreshold2
								, 1.0_f
								, sdw::Float{ pow( max( 0.0_f
										, 1.0_f - c3d_ssaoConfigData.edgeSharpness * 2.0f * k_plane * planeError / sqrt( distance2 ) )
									, 2.0_f ) } );
						}
						sdwFI
					}

					writer.returnStmt( depthWeight * normalWeight * planeWeight );
				}
				, sdw::InFloat{ writer, "key" }
				, sdw::InFloat{ writer, "tapKey" }
				, sdw::InIVec2{ writer, "tapLoc" }
				, sdw::InVec3{ writer, "normal" }
				, sdw::InVec3{ writer, "tapNormal" }
				, sdw::InVec3{ writer, "position" } );

			writer.implementEntryPointT< shader::Position2FT, sdw::VoidT >( []( sdw::VertexInT< shader::Position2FT > const & in
				, sdw::VertexOut out )
				{
					out.vtx.position = vec4( in.position(), 0.0_f, 1.0_f );
				} );

			writer.implementEntryPoint( [&]( sdw::FragmentIn const & in
				, sdw::FragmentOut const & )
				{
					auto ssCenter = writer.declLocale( "ssCenter"
						, ivec2( in.fragCoord.xy() ) );

					auto temp = writer.declLocale( "temp"
						, c3d_mapInput.fetch( ssCenter, 0_i ) );
					auto sum = writer.declLocale( "sum"
						, temp.r() );
					auto bentNormal = writer.declLocale( "bentNormal"
						, c3d_cameraData.readNormal( c3d_mapBentInput.fetch( ssCenter, 0_i ).xyz() ) );

					keyPassThrough = temp.g();
					auto key = writer.declLocale( "key"
						, unpackKey( keyPassThrough ) );
					auto normal = writer.declLocale( "normal"
						, vec3( 0.0_f ) );

					if ( useNormalsBuffer )
					{
						normal = normalize( c3d_cameraData.readNormal( c3d_mapNormal.fetch( ssCenter, 0_i ).xyz() ) );
					}

					sdwIF( writer, key == 1.0_f )
					{
						// Sky pixel (if you aren't using depth keying, disable this test)
						result = sum;
						outBentNormal = c3d_cameraData.writeNormal( bentNormal );
						writer.returnStmt();
					}
					sdwFI

					// Base weight for depth falloff.  Increase this for more blurriness,
					// decrease it for better edge discrimination
					auto BASE = writer.declLocale( "BASE"
						, c3d_gaussian[0_u][0_u] );
					auto totalWeight = writer.declLocale( "totalWeight"
						, BASE );
					sum *= totalWeight;
					bentNormal *= totalWeight;

					auto position = writer.declLocale( "position"
						, positionFromKey( key, ssCenter, c3d_ssaoConfigData.projInfo ) );
					auto blurRadius = writer.declLocale( "blurRadius"
						, writer.cast< sdw::Int >( c3d_ssaoConfigData.blurRadius ) );

					sdwFOR( writer, sdw::Int, r, -blurRadius, r <= blurRadius, ++r )
					{
						// We already handled the zero case above.  This loop should be unrolled and the static branch optimized out,
						// so the IF statement has no runtime cost
						sdwIF( writer, r != 0_i )
						{
							auto tapLoc = writer.declLocale( "tapLoc"
								, ssCenter + c3d_axis * ( r * writer.cast< sdw::Int >( c3d_ssaoConfigData.blurStepSize ) ) );

							// spatial domain: offset gaussian tap
							auto absR = writer.declLocale( "absR"
								, writer.cast< sdw::UInt >( abs( r ) ) );
							auto weight = writer.declLocale( "weight"
								, 0.3_f + c3d_gaussian[absR % 2_u][absR / 2_u] );

							auto tapKey = writer.declLocale< sdw::Float >( "tapKey" );
							auto value = writer.declLocale< sdw::Float >( "value" );
							auto bent = writer.declLocale( "bent"
								, vec3( 0.0_f ) );
							auto tapNormal = writer.declLocale( "tapNormal"
								, getTapInformation( tapLoc, tapKey, value, bent ) );

							auto bilateralWeight = writer.declLocale( "bilateralWeight"
								, calculateBilateralWeight( key
									, tapKey
									, tapLoc
									, normal
									, tapNormal
									, position ) );

							weight *= bilateralWeight;
							sum += value * weight;
							bentNormal += bent * weight;
							totalWeight += weight;
						}
						sdwFI
					}
					sdwROF

					auto const epsilon = writer.declLocale( "epsilon"
						, 0.0001_f );
					result = sum / ( totalWeight + epsilon );
					bentNormal /= ( totalWeight + epsilon );
					outBentNormal = c3d_cameraData.writeNormal( bentNormal );
				} );
			return writer.getBuilder().releaseShader();

#undef result
#undef keyPassThrough
		}

		static Texture doCreateTexture( RenderDevice const & device
			, crg::ResourcesCache & resources
			, String const & name
			, PixelFormat format
			, Extent2D const & size
			, bool transferDst )
		{
			return Texture{ device
				, resources
				, name
				, { ImageCreateFlags::eNone
					, { size.width, size.height, 1u }, 1u, 1u
					, format
					, ( ImageUsageFlags::eColorAttachment
						| ImageUsageFlags::eSampled
						| ImageUsageFlags::eTransferSrc
						| ( transferDst ? ImageUsageFlags::eTransferDst : ImageUsageFlags::eNone ) ) }
				, {} };
		}

		static String getName( bool useNormalsBuffer )
		{
			return cuT( "SsaoBlur" ) + ( useNormalsBuffer ? String{ cuT( "Nml" ) } : String{} );
		}

		static crg::rq::Config getRqConfig( Extent2D const & renderSize
			, SsaoConfig const & ssaoConfig
			, uint32_t const & passIndex
			, ashes::PipelineShaderStageCreateInfoArray const & stages0
			, ashes::PipelineShaderStageCreateInfoArray const & stages1 )
		{
			crg::rq::Config result;
			result.renderSize( renderSize );
			result.enabled( &ssaoConfig.enabled );
			result.passIndex( &passIndex );
			result.programs( { crg::makeVkArray< VkPipelineShaderStageCreateInfo >( stages0 )
				, crg::makeVkArray< VkPipelineShaderStageCreateInfo >( stages1 ) } );
			return result;
		}

		static crg::ru::Config getRuConfig( bool isVertical
			, crg::Attachment const & attach
			, crg::Attachment const & bentAttach )
		{
			crg::ru::Config result{ 2u, false };

			if ( isVertical )
			{
				result.implicitAction( attach.view()
					, crg::RecordContext::clearAttachment( attach ) );
				result.implicitAction( bentAttach.view()
					, crg::RecordContext::clearAttachment( bentAttach ) );
			}

			return result;
		}
	}

	//*********************************************************************************************

	SsaoBlurPass::RenderQuad::RenderQuad( crg::FramePass const & pass
		, crg::GraphContext & context
		, crg::RunnableGraph & graph
		, crg::rq::Config rqConfig
		, crg::ru::Config const & ruConfig
		, SsaoConfig const & ssaoConfig )
		: crg::RenderQuad{ pass
			, context
			, graph
			, ruConfig
			, c3d::move( rqConfig ) }
		, ssaoConfig{ ssaoConfig }
	{
	}

	uint32_t SsaoBlurPass::RenderQuad::doGetPassIndex()const
	{
		return ssaoConfig.useNormalsBuffer ? 1u : 0u;
	}

	//*********************************************************************************************

	SsaoBlurPass::Program::Program( RenderDevice const & device
		, bool useNormalsBuffer
		, String const & prefix )
		: shader{ prefix + ssaoblr::getName( useNormalsBuffer ), ssaoblr::getProgram( device, useNormalsBuffer ) }
		, stages{ makeProgramStates( device, shader ) }
	{
	}

	//*********************************************************************************************

	SsaoBlurPass::SsaoBlurPass( crg::FramePassGroup & graph
		, RenderDevice const & device
		, ProgressBar * progress
		, String const & prefix
		, Extent2D const & size
		, SsaoConfig const & config
		, SsaoConfigUbo & ssaoConfigUbo
		, CameraUbo const & cameraUbo
		, Point2i const & axis
		, Texture const & input
		, Texture const & bentInput
		, Texture const & normals
		, uint32_t const & passIndex )
		: m_device{ device }
		, m_graph{ graph }
		, m_ssaoConfigUbo{ ssaoConfigUbo }
		, m_cameraUbo{ cameraUbo }
		, m_bentInput{ bentInput }
		, m_config{ config }
		, m_size{ size }
		, m_result{ ssaoblr::doCreateTexture( m_device, *input.resources, makeString( m_graph.getName() ) + cuT( "SsaoBlur" ) + prefix, input.getFormat(), m_size, axis->y != 0 ) }
		, m_bentResult{ ssaoblr::doCreateTexture( m_device, *input.resources, makeString( m_graph.getName() ) + cuT( "SsaoBentNormals" ) + prefix, m_bentInput.getFormat(), m_size, axis->y != 0 ) }
		, m_configurationUbo{ m_device.uboPool->getBuffer< Configuration >( MemoryPropertyFlags::eNone ) }
		, m_programs{ Program{ device, false, makeString( m_graph.getName() ) }
			, Program{ device, true, makeString( m_graph.getName() ) } }
	{
		stepProgressBarLocal( progress, cuT( "Creating " ) + makeString( m_graph.getName() ) + cuT( " SSAO " ) + prefix + cuT( " blur pass" ) );
		auto & configuration = m_configurationUbo.getData();
		configuration.axis = axis;
		auto & pass = m_graph.createPass( "Blur" + toUtf8( prefix )
			, [this, &passIndex, progress, prefix, config, axis]( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & runnable )
			{
				stepProgressBarLocal( progress, cuT( "Initialising SSAO " ) + prefix + cuT( " blur pass" ) );
				auto bentResIt = pass.getTargets().rbegin();
				auto resIt = std::next( bentResIt );
				auto result = makeRawUnique< RenderQuad >( pass
					, context
					, runnable
					, ssaoblr::getRqConfig( m_size
						, config
						, passIndex
						, m_programs[0].stages
						, m_programs[1].stages )
					, ssaoblr::getRuConfig( axis->y != 0
						, **resIt
						, **bentResIt )
					, m_config );
				m_device.renderSystem.getEngine()->registerTimer( makeString( pass.getFullName() )
					, result->getTimer() );
				return result;
			} );
		m_ssaoConfigUbo.createPassBinding( pass, uint32_t( ssaoblr::Idx::SsaoCfgUboIdx ) );
		m_cameraUbo.createPassBinding( pass, uint32_t( ssaoblr::Idx::CameraUboIdx ) );
		m_configurationUbo.createPassBinding( pass, uint32_t( ssaoblr::Idx::BlurCfgUboIdx ) );
		pass.addInputSampled( *normals.getSampledLastAttach(), uint32_t( ssaoblr::Idx::NmlImgIdx ) );
		pass.addInputSampled( *input.getSampledLastAttach(), uint32_t( ssaoblr::Idx::InpImgIdx ) );
		pass.addInputSampled( *bentInput.getSampledLastAttach(), uint32_t( ssaoblr::Idx::BntImgIdx ) );
		m_result.setLastAttach( pass.addOutputColourTarget( m_result.getTargetViewId(), opaqueWhiteClearColor ) );
		m_bentResult.setLastAttach( pass.addOutputColourTarget( m_bentResult.getTargetViewId(), transparentBlackClearColor ) );
		m_result.create();
		m_bentResult.create();
	}

	SsaoBlurPass::~SsaoBlurPass()noexcept
	{
		m_bentResult.destroy();
		m_result.destroy();
		m_device.uboPool->putBuffer( m_configurationUbo );
	}

	void SsaoBlurPass::update( CpuUpdater const & )
	{
		if ( m_config.blurRadius.isDirty() )
		{
			auto & configuration = m_configurationUbo.getData();

			switch ( m_config.blurRadius.value().value() )
			{
			case 1u:
				configuration.gaussian[0][0] = 0.5f;
				configuration.gaussian[0][1] = 0.25f;
				break;
			case 2u:
				configuration.gaussian[0][0] = 0.153170f;
				configuration.gaussian[0][1] = 0.144893f;
				configuration.gaussian[0][2] = 0.122649f;
				break;
			case 3u:
				configuration.gaussian[0][0] = 0.153170f;
				configuration.gaussian[0][1] = 0.144893f;
				configuration.gaussian[0][2] = 0.122649f;
				configuration.gaussian[0][3] = 0.092902f;
				break;
			case 4u:
				configuration.gaussian[0][0] = 0.153170f;
				configuration.gaussian[0][1] = 0.144893f;
				configuration.gaussian[0][2] = 0.122649f;
				configuration.gaussian[0][3] = 0.092902f;
				configuration.gaussian[1][0] = 0.062970f;
				break;
			case 5u:
				configuration.gaussian[0][0] = 0.111220f;
				configuration.gaussian[0][1] = 0.107798f;
				configuration.gaussian[0][2] = 0.098151f;
				configuration.gaussian[0][3] = 0.083953f;
				configuration.gaussian[1][0] = 0.067458f;
				configuration.gaussian[1][1] = 0.050920f;
				break;
			default:
				configuration.gaussian[0][0] = 0.111220f;
				configuration.gaussian[0][1] = 0.107798f;
				configuration.gaussian[0][2] = 0.098151f;
				configuration.gaussian[0][3] = 0.083953f;
				configuration.gaussian[1][0] = 0.067458f;
				configuration.gaussian[1][1] = 0.050920f;
				configuration.gaussian[1][2] = 0.036108f;
				break;
			}
		}
	}

	void SsaoBlurPass::accept( bool horizontal
		, ConfigurationVisitorBase & visitor )
	{
		if ( horizontal )
		{
			visitor.visit( cuT( "SSAO HBlurred AO" )
				, getResult()
				, m_graph.getFinalLayoutState( getResult().getSampledViewId() ).layout
				, TextureFactors{}.invert( true ) );
		}
		else
		{
			visitor.visit( cuT( "SSAO Blurred AO" )
				, getResult()
				, m_graph.getFinalLayoutState( getResult().getSampledViewId() ).layout
				, TextureFactors{}.invert( true ) );
		}

		if ( horizontal )
		{
			visitor.visit( cuT( "HBlurred Bent Normals" )
				, getBentResult()
				, m_graph.getFinalLayoutState( getBentResult().getSampledViewId() ).layout
				, TextureFactors{}.invert( true ) );
		}
		else
		{
			visitor.visit( cuT( "Blurred Bent Normals" )
				, getBentResult()
				, m_graph.getFinalLayoutState( getBentResult().getSampledViewId() ).layout
				, TextureFactors{}.invert( true ) );
		}

		if ( m_config.useNormalsBuffer )
		{
			visitor.visit( m_programs[1].shader );
		}
		else
		{
			visitor.visit( m_programs[0].shader );
		}
	}

	uint32_t SsaoBlurPass::countInitialisationSteps()noexcept
	{
		return 1u;
	}
}
