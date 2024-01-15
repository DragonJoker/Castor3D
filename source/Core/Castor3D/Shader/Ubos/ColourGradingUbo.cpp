#include "Castor3D/Shader/Ubos/ColourGradingUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPool.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/ToneMapping/ColourGradingConfig.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace shader
	{
		ColourGradingData::ColourGradingData( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
			, m_paramsLogC{ writer.declGlobalArray( "c3d_paramsLogC"
				, 7u
				, std::vector< sdw::Float >{ 0.011361_f /* cut */
					, 5.555556_f /* a */
					, 0.047996_f /* b */
					, 0.244161_f /* c */
					, 0.386036_f /* d */
					, 5.301883_f /* e */
					, 0.092819_f /* f */ } ) }
			, m_linearToLms{ writer.declGlobal( "c3d_linearToLms"
				, mat3( vec3( 3.90405e-1_f, 5.49941e-1_f, 8.92632e-3_f )
					, vec3( 7.08416e-2_f, 9.63172e-1_f, 1.35775e-3_f )
					, vec3( 2.31082e-2_f, 1.28021e-1_f, 9.36245e-1_f ) ) ) }
			, m_lmsToLinear{ writer.declGlobal( "c3d_lmsToLinear"
				, mat3( vec3( 2.85847e+0_f, -1.62879e+0_f, -2.48910e-2_f )
					, vec3( -2.10182e-1_f, 1.15820e+0_f, 3.24281e-4_f )
					, vec3( -4.18120e-2_f, -1.18169e-1_f, 1.06867e+0_f ) ) ) }
		{
		}

		sdw::Vec3 ColourGradingData::colourGrade( sdw::Vec3 const phdrColour )
		{
			auto & writer = *getWriter();

			if ( !m_rgbToHsv )
			{
				m_rgbToHsv = writer.implementFunction< sdw::Vec3 >( "rgbToHsv"
					, [&]( sdw::Vec3 c )
					{
						auto K = writer.declLocale( "K", vec4( 0.0_f, -1.0_f / 3.0_f, 2.0_f / 3.0_f, -1.0_f ) );
						auto p = writer.declLocale( "p", mix( vec4( c.bg(), K.wz() ), vec4( c.gb(), K.xy() ), vec4( step( c.b(), c.g() ) ) ) );
						auto q = writer.declLocale( "q", mix( vec4( p.xyw(), c.r() ), vec4( c.r(), p.yzx() ), vec4( step( p.x(), c.r() ) ) ) );
						auto d = writer.declLocale( "d", q.x() - min( q.w(), q.y() ) );
						auto e = writer.declLocale( "e", 1.0e-4_f );
						writer.returnStmt( vec3( abs( q.z() + ( q.w() - q.y() ) / ( 6.0_f * d + e ) ), d / ( q.x() + e ), q.x() ) );
					}
					, sdw::InVec3{ writer, "colour" } );
			}

			if ( !m_hsvToRgb )
			{
				m_hsvToRgb = writer.implementFunction< sdw::Vec3 >( "hsvToRgb"
					, [&]( sdw::Vec3 c )
					{
						auto K = writer.declLocale( "K", vec4( 1.0_f, 2.0_f / 3.0_f, 1.0_f / 3.0_f, 3.0_f ) );
						auto p = writer.declLocale( "p", abs( fract( vec3( c.x() ) + K.xyz() ) * 6.0_f - K.www() ) );
						writer.returnStmt( c.z() * mix( K.xxx(), clamp( p - K.xxx(), vec3( 0.0_f ), vec3( 1.0_f ) ), vec3( c.y() ) ) );
					}
					, sdw::InVec3{ writer, "colour" } );
			}

			if ( !m_splitToning )
			{
				m_splitToning = writer.implementFunction< sdw::Vec3 >( "splitToning"
					, [&]( sdw::Vec3 colour )
					{
						colour = max( colour, vec3( 0.0_f ) );
						colour = pow( colour, vec3( 1.0_f / 2.2_f ) );
						auto t = writer.declLocale( "t", clamp( getLuminance( clamp( colour, vec3( 0.0_f ), vec3( 1.0_f ) ) ) + splitToningBalance(), 0.0_f, 1.0_f ) );
						auto shadows = writer.declLocale( "shadows", mix( vec3( 0.5_f ), splitToningShadows(), vec3( 1.0_f - t ) ) );
						auto highlights = writer.declLocale( "highlights", mix( vec3( 0.5_f ), splitToningHighlights(), vec3( t ) ) );
						colour = softLight( colour, shadows );
						colour = softLight( colour, highlights );
						writer.returnStmt( pow( colour, vec3( 2.2_f ) ) );
					}
					, sdw::InVec3{ writer, "colour" } );
			}

			if ( !m_shadowsMidtoneHighlight )
			{
				m_shadowsMidtoneHighlight = writer.implementFunction< sdw::Vec3 >( "shadowsMidtoneHighlight"
					, [&]( sdw::Vec3 colour )
					{
						auto luminance = writer.declLocale( "luminance", getLuminance( colour ) );
						auto shadowsWeight = writer.declLocale( "shadowsWeight", 1.0_f - smoothStep( shadowsStart(), shadowsEnd(), luminance ) );
						auto highlightsWeight = writer.declLocale( "highlightsWeight", smoothStep( highlightsStart(), highlightsEnd(), luminance ) );
						auto midtonesWeight = writer.declLocale( "midtonesWeight", 1.0_f - shadowsWeight - highlightsWeight );
						writer.returnStmt( colour * shadows() * shadowsWeight
							+ colour * midtones() * midtonesWeight
							+ colour * highlights() * highlightsWeight );
					}
					, sdw::InVec3{ writer, "colour" } );
			}

			if ( !m_colourGrade )
			{
				m_colourGrade = writer.implementFunction< sdw::Vec3 >( "colourGrade"
					, [&]( sdw::Vec3 hdrColour )
					{
						IF( writer, enabled() )
						{
							hdrColour *= postExposure();
							IF( writer, enableWhiteBalance() )
							{
								hdrColour = linearToLms( hdrColour );
								hdrColour *= whiteBalance();
								hdrColour = lmsToLinear( hdrColour );
							}
							FI;
							IF( writer, enableSplitToning() )
							{
								hdrColour = m_splitToning( hdrColour );
							}
							FI;
							IF( writer, enableChannelMix() )
							{
								hdrColour = vec3( dot( hdrColour, channelMixRed() )
									, dot( hdrColour, channelMixGreen() )
									, dot( hdrColour, channelMixBlue() ) );
							}
							FI;
							IF( writer, enableShadowMidToneHighlight() )
							{
								hdrColour = m_shadowsMidtoneHighlight( hdrColour );
							}
							FI;
							IF( writer, enableContrast() )
							{
								hdrColour = linearToLogC( hdrColour );
								hdrColour = ( hdrColour - midGray() ) * contrast() + midGray();
								hdrColour = logCToLinear( hdrColour );
							}
							FI;
							hdrColour *= colourFilter();
							IF( writer, enableHueShift() )
							{
								hdrColour = m_rgbToHsv( hdrColour );
								auto hue = writer.declLocale( "hue", hdrColour.x() + hueShift() );
								hdrColour.x() = rotateHue( hue, 0.0_f, 1.0_f );
								hdrColour = m_hsvToRgb( hdrColour );
							}
							FI;
							// Saturation
							auto luminance = writer.declLocale( "luminance", getLuminance( hdrColour ) );
							hdrColour = ( hdrColour - luminance ) * saturation() + luminance;
							// Prevent negative colours
							writer.returnStmt( max( vec3( 0.0_f ), hdrColour ) );
						}
						FI;

						writer.returnStmt( hdrColour );
					}
					, sdw::InVec3{ writer, "hdrColour" } );
			}

			return m_colourGrade( phdrColour );
		}

		sdw::Float ColourGradingData::getLuminance( sdw::Vec3 const & colour )
		{
			return dot( colour, vec3( 0.2126729_f, 0.7151522_f, 0.0721750_f ) );
		}

		sdw::Vec3 ColourGradingData::linearToLogC( sdw::Vec3 const & colour )
		{
			return m_paramsLogC[3] * sdw::log( m_paramsLogC[1] * colour + m_paramsLogC[2] ) / sdw::log( 10.0_f ) + m_paramsLogC[4];
		}

		sdw::Vec3 ColourGradingData::logCToLinear( sdw::Vec3 const & colour )
		{
			return ( pow( vec3( 10.0_f ), ( colour - m_paramsLogC[4] ) / m_paramsLogC[3] ) - m_paramsLogC[2] ) / m_paramsLogC[1];
		}

		sdw::Float ColourGradingData::rotateHue( sdw::Float const & value, sdw::Float const & low, sdw::Float const & hi )
		{
			auto & writer = sdw::findWriterMandat( value, low, hi );
			return writer.ternary( value < low
				, value + hi
				, writer.ternary( value > hi, value - hi, value ) );
		}

		sdw::Vec3 ColourGradingData::linearToLms( sdw::Vec3 const & colour )
		{
			return m_linearToLms * colour;
		}

		sdw::Vec3 ColourGradingData::lmsToLinear( sdw::Vec3 const & colour )
		{
			return m_lmsToLinear * colour;
		}

		sdw::Vec3 ColourGradingData::softLight( sdw::Vec3 const & base, sdw::Vec3 const & blend )
		{
			return mix(
				sqrt( base ) * ( 2.0_f * blend - 1.0_f ) + 2.0_f * base * ( 1.0_f - blend ),
				2.0_f * base * blend + base * base * ( 1.0_f - 2.0_f * blend ),
				step( base, vec3( 0.5_f ) )
			);
		}
	}

	//*********************************************************************************************

	ColourGradingUbo::ColourGradingUbo( RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ m_device.uboPool->getBuffer< Configuration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ) }
	{
	}

	ColourGradingUbo::~ColourGradingUbo()
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	void ColourGradingUbo::cpuUpdate( Configuration const & config )
	{
		CU_Require( m_ubo );
		auto & data = m_ubo.getData();
		data.enabled = config.enabled;
		data.enableSplitToning = config.enableSplitToning;
		data.enableShadowMidToneHighlight = config.enableShadowMidToneHighlight;
		data.enableChannelMix = config.enableChannelMix;
		data.enableWhiteBalance = config.enableWhiteBalance;
		data.enableHueShift = config.enableHueShift;
		data.enableContrast = config.enableContrast;
		data.whiteBalance = config.whiteBalance;
		data.postExposure = config.postExposure;
		data.colourFilter = config.colourFilter;
		data.midGray = config.midGray;
		data.splitToningShadows = config.splitToningShadows;
		data.contrast = config.contrast;
		data.splitToningHighlights = config.splitToningHighlights;
		data.splitToningBalance = config.splitToningBalance;
		data.hueShift = config.hueShift;
		data.channelMixRed = config.channelMixRed;
		data.saturation = config.saturation;
		data.channelMixGreen = config.channelMixGreen;
		data.shadowsStart = config.shadowsStart;
		data.channelMixBlue = config.channelMixBlue;
		data.shadowsEnd = config.shadowsEnd;
		data.shadows = config.shadows;
		data.highlightsStart = config.highlightsStart;
		data.midtones = config.midtones;
		data.highlightsEnd = config.highlightsEnd;
		data.highlights = config.highlights;
	}
}
