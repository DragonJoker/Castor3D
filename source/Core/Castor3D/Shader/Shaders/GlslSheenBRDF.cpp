#include "Castor3D/Shader/Shaders/GlslSheenBRDF.hpp"

#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslOutputComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <CastorUtils/Math/Angle.hpp>

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	//*********************************************************************************************

	SheenBRDF::SheenBRDF( sdw::ShaderWriter & writer
		, Utils & utils )
		: m_writer{ writer }
		, m_utils{ utils }
	{
	}

	sdw::RetVec2 SheenBRDF::compute( sdw::Float const & NdotH
		, sdw::Float const & NdotV
		, sdw::Float const & NdotL
		, sdw::Float const & roughness )
	{
		declareComputeSheen();
		return m_computeSheen( NdotH
			, NdotV
			, NdotL
			, roughness );
	}

	void SheenBRDF::declareDistribution()
	{
		if ( m_distributionCharlie )
		{
			return;
		}

		// Distribution Function
		m_distributionCharlie = m_writer.implementFunction< sdw::Float >( "c3d_sheenDistribution"
			, [this]( sdw::Float const & NdotH
				, sdw::Float roughness )
			{
				// Estevez and Kulla http://www.aconty.com/pdf/s2017_pbs_imageworks_sheen.pdf
				roughness = max( roughness, 0.000001_f ); //clamp (0,1]
				auto alphaG = m_writer.declLocale( "alphaG"
					, roughness * roughness );
				auto invR = m_writer.declLocale( "invR"
					, 1.0_f / alphaG );
				auto cos2h = m_writer.declLocale( "cos2h"
					, NdotH * NdotH );
				auto sin2h = m_writer.declLocale( "sin2h"
					, 1.0_f - cos2h );
				m_writer.returnStmt( ( 2.0_f + invR ) * pow( sin2h, invR * 0.5_f ) / castor::Tau< float > );
			}
			, sdw::InFloat{ m_writer, "product" }
			, sdw::InFloat{ m_writer, "roughness" } );
	}
	
	void SheenBRDF::declareVisibility()
	{
		if ( m_visibility )
		{
			return;
		}

		// Visibility Functions
		m_lambdaSheenNumericHelper = m_writer.implementFunction< sdw::Float >( "c3d_sheenLambdaNumericHelper"
			, [&]( sdw::Float const & x
				, sdw::Float const & alphaG )
			{
				auto oneMinusAlphaSq = m_writer.declLocale( "oneMinusAlphaSq"
					, ( 1.0_f - alphaG ) * ( 1.0_f - alphaG ) );
				auto a = m_writer.declLocale( "a"
					, mix( 21.5473_f, 25.3245_f, oneMinusAlphaSq ) );
				auto b = m_writer.declLocale( "b"
					, mix( 3.82987_f, 3.32435_f, oneMinusAlphaSq ) );
				auto c = m_writer.declLocale( "c"
					, mix( 0.19823_f, 0.16801_f, oneMinusAlphaSq ) );
				auto d = m_writer.declLocale( "d"
					, mix( -1.97760_f, -1.27393_f, oneMinusAlphaSq ) );
				auto e = m_writer.declLocale( "e"
					, mix( -4.32054_f, -4.85967_f, oneMinusAlphaSq ) );
				m_writer.returnStmt( a / ( 1.0_f + b * pow( x, c ) ) + d * x + e );
			}
			, sdw::InFloat{ m_writer, "x" }
			, sdw::InFloat{ m_writer, "alphaG" } );

		m_lambdaSheen = m_writer.implementFunction< sdw::Float >( "c3d_sheenLambda"
			, [&]( sdw::Float const & cosTheta
				, sdw::Float const & alphaG )
			{
				IF( m_writer, abs( cosTheta ) < 0.5_f )
				{
					m_writer.returnStmt( exp( m_lambdaSheenNumericHelper( cosTheta, alphaG ) ) );
				}
				FI;

				m_writer.returnStmt( exp( 2.0_f * m_lambdaSheenNumericHelper( 0.5_f, alphaG )
					- m_lambdaSheenNumericHelper( 1.0_f - cosTheta, alphaG ) ) );
			}
			, sdw::InFloat{ m_writer, "NdotV" }
			, sdw::InFloat{ m_writer, "roughness" } );

		m_visibility = m_writer.implementFunction< sdw::Float >( "c3d_sheenVisibility"
			, [this]( sdw::Float const & NdotL
				, sdw::Float const & NdotV
				, sdw::Float roughness )
			{
				roughness = max( roughness, 0.000001_f ); //clamp (0,1]
				auto alphaG = m_writer.declLocale( "alphaG"
					, roughness * roughness );
				1_u;

				m_writer.returnStmt( clamp( 1.0_f / ( ( 1.0_f + m_lambdaSheen( NdotV, alphaG ) + m_lambdaSheen( NdotL, alphaG ) )
					* ( 4.0_f * NdotV * NdotL ) ), 0.0_f, 1.0_f ) );
			}
			, sdw::InFloat{ m_writer, "NdotL" }
			, sdw::InFloat{ m_writer, "NdotV" }
			, sdw::InFloat{ m_writer, "roughness" } );
	}

	inline sdw::Float operator "" _h( unsigned long long value )
	{
		return sdw::Float{ float( value ) };
	}

	void SheenBRDF::declareComputeSheen()
	{
		if ( m_computeSheen )
		{
			return;
		}

		declareDistribution();
		declareVisibility();
		m_computeSheen = m_writer.implementFunction< sdw::Vec2 >( "c3d_computeSheen"
			, [this]( sdw::Float const & NdotH
				, sdw::Float const & NdotV
				, sdw::Float const & NdotL
				, sdw::Float const & roughness )
			{
				auto albedoSheenScalingLUT = m_writer.declLocaleArray( "albedoSheenScalingLUT"
					, 256u
					, std::vector< sdw::Float >{ 0x000000cd_h, 0x000000a2_h, 0x00000083_h, 0x0000006a_h, 0x00000055_h, 0x00000042_h, 0x00000032_h, 0x00000025_h
						, 0x00000019_h, 0x00000010_h, 0x00000008_h, 0x00000004_h, 0x00000001_h, 0x00000000_h, 0x00000000_h, 0x00000000_h
						, 0x000000df_h, 0x000000c0_h, 0x000000a9_h, 0x00000095_h, 0x00000084_h, 0x00000075_h, 0x00000067_h, 0x00000059_h
						, 0x0000004d_h, 0x00000042_h, 0x00000037_h, 0x0000002d_h, 0x00000023_h, 0x0000001a_h, 0x00000011_h, 0x00000007_h
						, 0x000000e4_h, 0x000000c9_h, 0x000000b5_h, 0x000000a5_h, 0x00000097_h, 0x00000089_h, 0x0000007d_h, 0x00000070_h
						, 0x00000065_h, 0x0000005b_h, 0x00000050_h, 0x00000047_h, 0x0000003c_h, 0x00000032_h, 0x00000028_h, 0x0000001d_h
						, 0x000000e7_h, 0x000000cf_h, 0x000000bd_h, 0x000000af_h, 0x000000a1_h, 0x00000095_h, 0x00000089_h, 0x0000007e_h
						, 0x00000074_h, 0x0000006a_h, 0x00000061_h, 0x00000057_h, 0x0000004d_h, 0x00000043_h, 0x00000039_h, 0x0000002f_h
						, 0x000000e9_h, 0x000000d3_h, 0x000000c2_h, 0x000000b4_h, 0x000000a8_h, 0x0000009c_h, 0x00000091_h, 0x00000087_h
						, 0x0000007d_h, 0x00000074_h, 0x0000006b_h, 0x00000062_h, 0x00000059_h, 0x00000050_h, 0x00000046_h, 0x0000003c_h
						, 0x000000ea_h, 0x000000d5_h, 0x000000c5_h, 0x000000b8_h, 0x000000ac_h, 0x000000a1_h, 0x00000097_h, 0x0000008d_h
						, 0x00000084_h, 0x0000007b_h, 0x00000072_h, 0x0000006a_h, 0x00000062_h, 0x00000059_h, 0x00000050_h, 0x00000047_h
						, 0x000000eb_h, 0x000000d7_h, 0x000000c7_h, 0x000000ba_h, 0x000000af_h, 0x000000a5_h, 0x0000009b_h, 0x00000092_h
						, 0x00000089_h, 0x00000081_h, 0x00000078_h, 0x00000071_h, 0x00000068_h, 0x00000060_h, 0x00000058_h, 0x0000004f_h
						, 0x000000eb_h, 0x000000d8_h, 0x000000c9_h, 0x000000bd_h, 0x000000b1_h, 0x000000a8_h, 0x0000009e_h, 0x00000096_h
						, 0x0000008d_h, 0x00000085_h, 0x0000007d_h, 0x00000076_h, 0x0000006e_h, 0x00000066_h, 0x0000005e_h, 0x00000056_h
						, 0x000000ec_h, 0x000000da_h, 0x000000ca_h, 0x000000bf_h, 0x000000b4_h, 0x000000aa_h, 0x000000a2_h, 0x00000099_h
						, 0x00000091_h, 0x00000089_h, 0x00000081_h, 0x0000007a_h, 0x00000073_h, 0x0000006b_h, 0x00000063_h, 0x0000005c_h
						, 0x000000ed_h, 0x000000da_h, 0x000000cc_h, 0x000000c0_h, 0x000000b6_h, 0x000000ac_h, 0x000000a4_h, 0x0000009b_h
						, 0x00000093_h, 0x0000008c_h, 0x00000085_h, 0x0000007d_h, 0x00000076_h, 0x0000006f_h, 0x00000068_h, 0x00000060_h
						, 0x000000ee_h, 0x000000db_h, 0x000000cd_h, 0x000000c1_h, 0x000000b7_h, 0x000000ae_h, 0x000000a6_h, 0x0000009e_h
						, 0x00000095_h, 0x0000008e_h, 0x00000087_h, 0x00000080_h, 0x00000079_h, 0x00000073_h, 0x0000006c_h, 0x00000065_h
						, 0x000000ed_h, 0x000000db_h, 0x000000ce_h, 0x000000c2_h, 0x000000b8_h, 0x000000af_h, 0x000000a7_h, 0x0000009f_h
						, 0x00000097_h, 0x00000090_h, 0x00000089_h, 0x00000083_h, 0x0000007c_h, 0x00000076_h, 0x0000006f_h, 0x00000068_h
						, 0x000000ef_h, 0x000000dc_h, 0x000000ce_h, 0x000000c3_h, 0x000000b9_h, 0x000000b1_h, 0x000000a8_h, 0x000000a1_h
						, 0x00000099_h, 0x00000092_h, 0x0000008c_h, 0x00000085_h, 0x0000007f_h, 0x00000078_h, 0x00000072_h, 0x0000006b_h
						, 0x000000ef_h, 0x000000dc_h, 0x000000cf_h, 0x000000c4_h, 0x000000bb_h, 0x000000b2_h, 0x000000aa_h, 0x000000a2_h
						, 0x0000009a_h, 0x00000094_h, 0x0000008d_h, 0x00000087_h, 0x00000081_h, 0x0000007b_h, 0x00000074_h, 0x0000006e_h
						, 0x000000ef_h, 0x000000dd_h, 0x000000d0_h, 0x000000c5_h, 0x000000bb_h, 0x000000b3_h, 0x000000ab_h, 0x000000a3_h
						, 0x0000009c_h, 0x00000095_h, 0x0000008f_h, 0x00000089_h, 0x00000083_h, 0x0000007c_h, 0x00000077_h, 0x00000070_h
						, 0x000000f0_h, 0x000000dd_h, 0x000000d1_h, 0x000000c6_h, 0x000000bc_h, 0x000000b4_h, 0x000000ac_h, 0x000000a4_h
						, 0x0000009d_h, 0x00000097_h, 0x00000090_h, 0x0000008a_h, 0x00000085_h, 0x0000007f_h, 0x00000079_h, 0x00000073_h } );
				auto sheenDistribution = m_writer.declLocale( "sheenDistribution"
					, m_distributionCharlie( roughness, NdotH ) );
				auto sheenVisibility = m_writer.declLocale( "sheenVisibility"
					, m_visibility( NdotL, NdotV, roughness ) );
				m_writer.returnStmt( vec2( sheenDistribution * sheenVisibility
					, min( 1.0_f - albedoSheenScalingLUT[m_writer.cast< sdw::UInt >( NdotV * 16.0_f ) + 16_u * m_writer.cast< sdw::UInt >( roughness * 16.0_f )],
						1.0_f - albedoSheenScalingLUT[m_writer.cast< sdw::UInt >( NdotL * 16.0_f ) + 16_u * m_writer.cast< sdw::UInt >( roughness * 16.0_f )] ) ) );
			}
			, sdw::InFloat( m_writer, "NdotH" )
			, sdw::InFloat( m_writer, "NdotV" )
			, sdw::InFloat( m_writer, "NdotL" )
			, sdw::InFloat{ m_writer, "roughness" } );
	}

	//***********************************************************************************************
}
