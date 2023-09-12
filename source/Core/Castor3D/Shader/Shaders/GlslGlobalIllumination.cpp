#include "Castor3D/Shader/Shaders/GlslGlobalIllumination.hpp"

#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.hpp"
#include "Castor3D/Shader/Shaders/GlslDebugOutput.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

#include <CastorUtils/Math/Angle.hpp>

#include <ShaderWriter/Source.hpp>

#pragma GCC diagnostic ignored "-Wuseless-cast"

namespace castor3d
{
	namespace shader
	{
		GlobalIllumination::GlobalIllumination( sdw::ShaderWriter & writer
			, Utils & utils )
			: m_writer{ writer }
			, m_utils{ utils }
		{
		}

		GlobalIllumination::GlobalIllumination( sdw::ShaderWriter & writer
			, Utils & utils
			, uint32_t & bindingIndex
			, uint32_t setIndex
			, SceneFlags sceneFlags
			, IndirectLightingData const * indirectLighting )
			: GlobalIllumination{ writer, utils }
		{
			declare( bindingIndex
				, setIndex
				, sceneFlags
				, indirectLighting );
		}

		void GlobalIllumination::declare( uint32_t & bindingIndex
			, uint32_t setIndex
			, SceneFlags sceneFlags
			, IndirectLightingData const * indirectLighting )
		{
			if ( checkFlag( sceneFlags, SceneFlag::eVoxelConeTracing ) )
			{
				declareVct( bindingIndex, bindingIndex, setIndex, setIndex );
			}
			else
			{
				if ( indirectLighting && indirectLighting->vctConfigUbo )
				{
					bindingIndex += 3u; // VCT: UBO + FirstBounce + SecondBounce.
				}

				if ( checkFlag( sceneFlags, SceneFlag::eLpvGI ) )
				{
					declareLpv( bindingIndex, bindingIndex, setIndex, setIndex );
				}
				else if ( indirectLighting && indirectLighting->lpvConfigUbo )
				{
					bindingIndex += 4u; // LPV: UBO + AccumR + AccumG + AccumB.
				}

				if ( checkFlag( sceneFlags, SceneFlag::eLayeredLpvGI ) )
				{
					declareLayeredLpv( bindingIndex, bindingIndex, setIndex, setIndex );
				}
				else if ( indirectLighting && indirectLighting->llpvConfigUbo )
				{
					bindingIndex += 1u; // LLPV: UBO
					bindingIndex += 3u; // LLPV: Accum1R + Accum1G + Accum1B.
					bindingIndex += 3u; // LLPV: Accum2R + Accum2G + Accum2B.
					bindingIndex += 3u; // LLPV: Accum3R + Accum3G + Accum3B.
				}
			}
		}

		void GlobalIllumination::declareVct( uint32_t & uboBindingIndex
			, uint32_t & texBindingIndex
			, uint32_t uboSetIndex
			, uint32_t texSetIndex )
		{
			C3D_Voxelizer( m_writer, uboBindingIndex++, uboSetIndex, true );
			m_writer.declCombinedImg< FImg3DRgba32 >( "c3d_mapVoxelsFirstBounce", texBindingIndex++, texSetIndex );
			m_writer.declCombinedImg< FImg3DRgba32 >( "c3d_mapVoxelsSecondaryBounce", texBindingIndex++, texSetIndex );
		}

		void GlobalIllumination::declareLpv( uint32_t & uboBindingIndex
			, uint32_t & texBindingIndex
			, uint32_t uboSetIndex
			, uint32_t texSetIndex
			, bool declUbo )
		{
			if ( declUbo )
			{
				C3D_LpvGridConfig( m_writer, uboBindingIndex++, uboSetIndex, true );
			}

			auto c3d_lpvAccumulatorR = m_writer.declCombinedImg< FImg3DRgba16 >( getTextureName( LpvTexture::eR, "Accumulator" ), texBindingIndex++, texSetIndex );
			auto c3d_lpvAccumulatorG = m_writer.declCombinedImg< FImg3DRgba16 >( getTextureName( LpvTexture::eG, "Accumulator" ), texBindingIndex++, texSetIndex );
			auto c3d_lpvAccumulatorB = m_writer.declCombinedImg< FImg3DRgba16 >( getTextureName( LpvTexture::eB, "Accumulator" ), texBindingIndex++, texSetIndex );

			/*Spherical harmonics coefficients - precomputed*/
			auto SH_C0 = m_writer.declConstant( "SH_C0"
				, sdw::Float{ 1.0f / float( 2.0f * sqrt( castor::Pi< float > ) ) } );
			auto SH_C1 = m_writer.declConstant( "SH_C1"
				, sdw::Float{ float( sqrt( 3.0f / castor::Pi< float > ) / 2.0f ) } );

			// no normalization
			m_evalSH = m_writer.implementFunction< sdw::Vec4 >( "evalSH"
				, [this]( sdw::Vec3 direction )
				{
					auto SH_C0 = m_writer.getVariable< sdw::Float >( "SH_C0" );
					auto SH_C1 = m_writer.getVariable< sdw::Float >( "SH_C1" );

					m_writer.returnStmt( vec4( SH_C0
						, -SH_C1 * direction.y()
						, SH_C1 * direction.z()
						, -SH_C1 * direction.x() ) );
				}
				, sdw::InVec3{ m_writer, "direction" } );

			m_computeLPVRadiance = m_writer.implementFunction< sdw::Vec4 >( "computeLPVRadiance"
				, [this]( sdw::Vec3 const & wsNormal
					, sdw::Vec3 const & wsPosition
					, LpvGridData lpvGridData )
				{
					auto c3d_lpvAccumulatorR = m_writer.getVariable< sdw::CombinedImage3DRgba16 >( getTextureName( LpvTexture::eR, "Accumulator" ) );
					auto c3d_lpvAccumulatorG = m_writer.getVariable< sdw::CombinedImage3DRgba16 >( getTextureName( LpvTexture::eG, "Accumulator" ) );
					auto c3d_lpvAccumulatorB = m_writer.getVariable< sdw::CombinedImage3DRgba16 >( getTextureName( LpvTexture::eB, "Accumulator" ) );

					auto SHintensity = m_writer.declLocale( "SHintensity"
						, m_evalSH( -wsNormal ) );
					auto lpvCellCoords = m_writer.declLocale( "lpvCellCoords"
						, lpvGridData.worldToTex( wsPosition ) );
					auto lpvIntensity = m_writer.declLocale( "lpvIntensity"
						, vec3( dot( SHintensity, c3d_lpvAccumulatorR.sample( lpvCellCoords ) )
							, dot( SHintensity, c3d_lpvAccumulatorG.sample( lpvCellCoords ) )
							, dot( SHintensity, c3d_lpvAccumulatorB.sample( lpvCellCoords ) ) ) );
					m_writer.returnStmt( vec4( max( lpvIntensity, vec3( 0.0_f ) ), 1.0f ) );
				}
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InVec3{ m_writer, "wsPosition" }
				, InLpvGridData{ m_writer, "lpvGridData" } );
		}

		void GlobalIllumination::declareLayeredLpv( uint32_t & uboBindingIndex
			, uint32_t & texBindingIndex
			, uint32_t uboSetIndex
			, uint32_t texSetIndex
			, bool declUbo )
		{
			using namespace sdw;

			if ( declUbo )
			{
				C3D_LayeredLpvGridConfig( m_writer, uboBindingIndex++, uboSetIndex, true );
			}

			auto c3d_lpvAccumulator1R = m_writer.declCombinedImg< FImg3DRgba16 >( getTextureName( LpvTexture::eR, "Accumulator1" ), texBindingIndex++, texSetIndex );
			auto c3d_lpvAccumulator1G = m_writer.declCombinedImg< FImg3DRgba16 >( getTextureName( LpvTexture::eG, "Accumulator1" ), texBindingIndex++, texSetIndex );
			auto c3d_lpvAccumulator1B = m_writer.declCombinedImg< FImg3DRgba16 >( getTextureName( LpvTexture::eB, "Accumulator1" ), texBindingIndex++, texSetIndex );
			auto c3d_lpvAccumulator2R = m_writer.declCombinedImg< FImg3DRgba16 >( getTextureName( LpvTexture::eR, "Accumulator2" ), texBindingIndex++, texSetIndex );
			auto c3d_lpvAccumulator2G = m_writer.declCombinedImg< FImg3DRgba16 >( getTextureName( LpvTexture::eG, "Accumulator2" ), texBindingIndex++, texSetIndex );
			auto c3d_lpvAccumulator2B = m_writer.declCombinedImg< FImg3DRgba16 >( getTextureName( LpvTexture::eB, "Accumulator2" ), texBindingIndex++, texSetIndex );
			auto c3d_lpvAccumulator3R = m_writer.declCombinedImg< FImg3DRgba16 >( getTextureName( LpvTexture::eR, "Accumulator3" ), texBindingIndex++, texSetIndex );
			auto c3d_lpvAccumulator3G = m_writer.declCombinedImg< FImg3DRgba16 >( getTextureName( LpvTexture::eG, "Accumulator3" ), texBindingIndex++, texSetIndex );
			auto c3d_lpvAccumulator3B = m_writer.declCombinedImg< FImg3DRgba16 >( getTextureName( LpvTexture::eB, "Accumulator3" ), texBindingIndex++, texSetIndex );

			/*Spherical harmonics coefficients - precomputed*/
			auto SH_C0 = m_writer.declConstant( "SH_C0"
				, Float{ 1.0f / float( 2.0f * sqrt( castor::Pi< float > ) ) } );
			auto SH_C1 = m_writer.declConstant( "SH_C1"
				, Float{ float( sqrt( 3.0f / castor::Pi< float > ) / 2.0f ) } );

			// no normalization
			if ( !m_evalSH )
			{
				m_evalSH = m_writer.implementFunction< Vec4 >( "evalSH"
					, [this]( Vec3 direction )
					{
						auto SH_C0 = m_writer.getVariable< Float >( "SH_C0" );
						auto SH_C1 = m_writer.getVariable< Float >( "SH_C1" );

						m_writer.returnStmt( vec4( SH_C0
							, -SH_C1 * direction.y()
							, SH_C1 * direction.z()
							, -SH_C1 * direction.x() ) );
					}
					, InVec3{ m_writer, "direction" } );
			}

			m_computeLLPVRadiance = m_writer.implementFunction< sdw::Vec4 >( "computeLLPVRadiance"
				, [this]( sdw::Vec3 const & wsNormal
					, sdw::Vec3 const & wsPosition
					, LayeredLpvGridData llpvGridData )
				{
					auto c3d_lpvAccumulator1R = m_writer.getVariable< CombinedImage3DRgba16 >( getTextureName( LpvTexture::eR, "Accumulator1" ) );
					auto c3d_lpvAccumulator1G = m_writer.getVariable< CombinedImage3DRgba16 >( getTextureName( LpvTexture::eG, "Accumulator1" ) );
					auto c3d_lpvAccumulator1B = m_writer.getVariable< CombinedImage3DRgba16 >( getTextureName( LpvTexture::eB, "Accumulator1" ) );
					auto c3d_lpvAccumulator2R = m_writer.getVariable< CombinedImage3DRgba16 >( getTextureName( LpvTexture::eR, "Accumulator2" ) );
					auto c3d_lpvAccumulator2G = m_writer.getVariable< CombinedImage3DRgba16 >( getTextureName( LpvTexture::eG, "Accumulator2" ) );
					auto c3d_lpvAccumulator2B = m_writer.getVariable< CombinedImage3DRgba16 >( getTextureName( LpvTexture::eB, "Accumulator2" ) );
					auto c3d_lpvAccumulator3R = m_writer.getVariable< CombinedImage3DRgba16 >( getTextureName( LpvTexture::eR, "Accumulator3" ) );
					auto c3d_lpvAccumulator3G = m_writer.getVariable< CombinedImage3DRgba16 >( getTextureName( LpvTexture::eG, "Accumulator3" ) );
					auto c3d_lpvAccumulator3B = m_writer.getVariable< CombinedImage3DRgba16 >( getTextureName( LpvTexture::eB, "Accumulator3" ) );

					auto SHintensity = m_writer.declLocale( "SHintensity"
						, m_evalSH( -wsNormal ) );
					auto lpvCellCoords1 = m_writer.declLocale( "lpvCellCoords1"
						, ( wsPosition - llpvGridData.allMinVolumeCorners[0].xyz() ) / llpvGridData.allCellSizes.x() / llpvGridData.gridSizes );
					auto lpvCellCoords2 = m_writer.declLocale( "lpvCellCoords2"
						, ( wsPosition - llpvGridData.allMinVolumeCorners[1].xyz() ) / llpvGridData.allCellSizes.y() / llpvGridData.gridSizes );
					auto lpvCellCoords3 = m_writer.declLocale( "lpvCellCoords3"
						, ( wsPosition - llpvGridData.allMinVolumeCorners[2].xyz() ) / llpvGridData.allCellSizes.z() / llpvGridData.gridSizes );

					auto lpvIntensity1 = m_writer.declLocale( "lpvIntensity1"
						, vec3( dot( SHintensity, c3d_lpvAccumulator1R.sample( lpvCellCoords1 ) )
							, dot( SHintensity, c3d_lpvAccumulator1G.sample( lpvCellCoords1 ) )
							, dot( SHintensity, c3d_lpvAccumulator1B.sample( lpvCellCoords1 ) ) ) );
					auto lpvIntensity2 = m_writer.declLocale( "lpvIntensity2"
						, vec3( dot( SHintensity, c3d_lpvAccumulator2R.sample( lpvCellCoords2 ) )
							, dot( SHintensity, c3d_lpvAccumulator2G.sample( lpvCellCoords2 ) )
							, dot( SHintensity, c3d_lpvAccumulator2B.sample( lpvCellCoords2 ) ) ) );
					auto lpvIntensity3 = m_writer.declLocale( "lpvIntensity3"
						, vec3( dot( SHintensity, c3d_lpvAccumulator3R.sample( lpvCellCoords3 ) )
							, dot( SHintensity, c3d_lpvAccumulator3G.sample( lpvCellCoords3 ) )
							, dot( SHintensity, c3d_lpvAccumulator3B.sample( lpvCellCoords3 ) ) ) );

					m_writer.returnStmt( vec4( max( lpvIntensity1 + lpvIntensity2 + lpvIntensity3, vec3( 0.0_f ) ), 1.0f ) );
				}
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InVec3{ m_writer, "wsPosition" }
				, InLayeredLpvGridData{ m_writer, "llpvGridData" } );
		}

		sdw::Vec4 GlobalIllumination::computeLPVRadiance( LightSurface lightSurface
			, LpvGridData lpvGridData )
		{
			CU_Require( m_computeLPVRadiance );
			return m_computeLPVRadiance( lightSurface.N()
				, lightSurface.worldPosition()
				, lpvGridData );
		}

		sdw::Vec4 GlobalIllumination::computeLLPVRadiance( LightSurface lightSurface
			, LayeredLpvGridData llpvGridData )
		{
			CU_Require( m_computeLLPVRadiance );
			return m_computeLLPVRadiance( lightSurface.N()
				, lightSurface.worldPosition()
				, llpvGridData );
		}

		sdw::Float GlobalIllumination::computeOcclusion( SceneFlags sceneFlags
			, LightSurface lightSurface
			, DebugOutput & debugOutput )
		{
			auto indirectOcclusion = m_writer.declLocale( "indirectOcclusion"
				, 1.0_f );

			if ( checkFlag( sceneFlags, SceneFlag::eVoxelConeTracing ) )
			{
				auto voxelData = m_writer.getVariable< VoxelData >( "c3d_voxelData" );
				auto mapVoxelsFirstBounce = m_writer.getVariable< sdw::CombinedImage3DRgba32 >( "c3d_mapVoxelsFirstBounce" );
				auto mapVoxelsSecondaryBounce = m_writer.getVariable< sdw::CombinedImage3DRgba32 >( "c3d_mapVoxelsSecondaryBounce" );

				IF( m_writer, voxelData.enableOcclusion )
				{
					IF( m_writer, voxelData.enableSecondaryBounce )
					{
						indirectOcclusion = traceConeOcclusion( mapVoxelsSecondaryBounce
							, lightSurface
							, voxelData );
					}
					ELSE
					{
						indirectOcclusion = traceConeOcclusion( mapVoxelsFirstBounce
							, lightSurface
							, voxelData );
					}
					FI;
				}
				FI;
			}

			debugOutput.registerOutput( "Indirect", "Occlusion", indirectOcclusion );
			return indirectOcclusion;
		}

		sdw::Vec4 GlobalIllumination::computeVCTRadiance( LightSurface const & lightSurface
			, VoxelData const & voxelData
			, sdw::Float const & indirectOcclusion )
		{
			auto mapVoxelsFirstBounce = m_writer.getVariable< sdw::CombinedImage3DRgba32 >( "c3d_mapVoxelsFirstBounce" );
			auto mapVoxelsSecondaryBounce = m_writer.getVariable< sdw::CombinedImage3DRgba32 >( "c3d_mapVoxelsSecondaryBounce" );

			auto vxlRadiance( m_writer.declLocale< sdw::Vec4 >( "vxlRadiance" ) );

			IF( m_writer, voxelData.enableSecondaryBounce )
			{
				vxlRadiance = traceConeRadiance( mapVoxelsSecondaryBounce
					, lightSurface
					, voxelData );
			}
			ELSE
			{
				vxlRadiance = traceConeRadiance( mapVoxelsFirstBounce
					, lightSurface
					, voxelData );
			}
			FI;

			auto vxlPosition = m_writer.declLocale( "vxlPosition"
				, clamp( abs( voxelData.worldToClip( lightSurface.worldPosition() ) ), vec3( -1.0_f ), vec3( 1.0_f ) ) );
			auto vxlBlend = m_writer.declLocale( "vxlBlend"
				, 1.0_f - pow( max( vxlPosition.x(), max( vxlPosition.y(), vxlPosition.z() ) ), 4.0_f ) );
			return vec4( mix( vec3( 0.0_f )
					, vxlRadiance.xyz() * indirectOcclusion
					, vec3( vxlRadiance.a() * vxlBlend * indirectOcclusion ) )
				, vxlBlend );
		}

		sdw::Vec3 GlobalIllumination::computeVCTSpecular( LightSurface const & lightSurface
			, sdw::Float const & roughness
			, sdw::Float const & indirectOcclusion
			, sdw::Float const & indirectBlend
			, VoxelData const & voxelData )
		{
			auto mapVoxelsFirstBounce = m_writer.getVariable< sdw::CombinedImage3DRgba32 >( "c3d_mapVoxelsFirstBounce" );
			auto mapVoxelsSecondaryBounce = m_writer.getVariable< sdw::CombinedImage3DRgba32 >( "c3d_mapVoxelsSecondaryBounce" );
			auto vxlReflection( m_writer.declLocale< sdw::Vec4 >( "vxlReflection" ) );

			IF( m_writer, voxelData.enableSecondaryBounce )
			{
				vxlReflection = traceConeReflection( mapVoxelsSecondaryBounce
					, lightSurface
					, roughness
					, voxelData );
			}
			ELSE
			{
				vxlReflection = traceConeReflection( mapVoxelsFirstBounce
					, lightSurface
					, roughness
					, voxelData );
			}
			FI;

			return mix( vec3( 0.0_f )
					, vxlReflection.xyz()
					, vec3( vxlReflection.a() * indirectBlend * indirectOcclusion ) );
		}

		sdw::Vec4 GlobalIllumination::computeDiffuse( SceneFlags sceneFlags
			, LightSurface lightSurface
			, sdw::Float indirectOcclusion
			, DebugOutput & debugOutput )
		{
			auto indirectDiffuse = m_writer.declLocale( "lightIndirectDiffuse"
				, vec4( 0.0_f ) );

			if ( checkFlag( sceneFlags, SceneFlag::eVoxelConeTracing ) )
			{
				auto voxelData = m_writer.getVariable< VoxelData >( "c3d_voxelData" );
				indirectDiffuse += computeVCTRadiance( lightSurface, voxelData, indirectOcclusion );
			}
			else
			{
				if ( checkFlag( sceneFlags, SceneFlag::eLayeredLpvGI ) )
				{
					auto llpvGridData = m_writer.getVariable< LayeredLpvGridData >( "c3d_llpvGridData" );
					indirectDiffuse += ( computeLLPVRadiance( lightSurface, llpvGridData ) * llpvGridData.indirectAttenuation ) / sdw::Float{ castor::Pi< float > };
				}

				if ( checkFlag( sceneFlags, SceneFlag::eLpvGI ) )
				{
					auto lpvGridData = m_writer.getVariable< LpvGridData >( "c3d_lpvGridData" );
					indirectDiffuse += ( computeLPVRadiance( lightSurface, lpvGridData ) * lpvGridData.indirectAttenuation() ) / sdw::Float{ castor::Pi< float > };
				}
			}

			debugOutput.registerOutput( "Indirect", "Raw Diffuse", indirectDiffuse );
			return indirectDiffuse;
		}

		sdw::Vec3 GlobalIllumination::computeAmbient( SceneFlags sceneFlags
			, sdw::Vec3 const & indirectDiffuse
			, DebugOutput & debugOutput )
		{
			auto indirectAmbient = m_writer.declLocale( "indirectAmbient"
				, vec3( 1.0_f ) );

			if ( checkFlag( sceneFlags, SceneFlag::eVoxelConeTracing ) )
			{
				indirectAmbient = indirectDiffuse;
			}
			else if ( checkFlag( sceneFlags, SceneFlag::eLayeredLpvGI ) )
			{
				auto llpvGridData = m_writer.getVariable< LayeredLpvGridData >( "c3d_llpvGridData" );
				indirectAmbient = indirectDiffuse / llpvGridData.indirectAttenuation;
			}
			else if ( checkFlag( sceneFlags, SceneFlag::eLpvGI ) )
			{
				auto lpvGridData = m_writer.getVariable< LpvGridData >( "c3d_lpvGridData" );
				indirectAmbient = indirectDiffuse / lpvGridData.indirectAttenuation();
			}

			debugOutput.registerOutput( "Indirect", "Ambient", indirectAmbient );
			return indirectAmbient;
		}

		sdw::Vec3 GlobalIllumination::computeSpecular( SceneFlags sceneFlags
			, LightSurface lightSurface
			, sdw::Float roughness
			, sdw::Float indirectOcclusion
			, sdw::Float indirectBlend
			, sdw::CombinedImage2DRgba32 brdfMap
			, DebugOutput & debugOutput )
		{
			auto indirectSpecular = m_writer.declLocale( "lightIndirectSpecular"
				, vec3( 0.0_f ) );

			if ( checkFlag( sceneFlags, SceneFlag::eVoxelConeTracing ) )
			{
				auto voxelData = m_writer.getVariable< VoxelData >( "c3d_voxelData" );
				indirectSpecular = computeVCTSpecular( lightSurface
					, roughness
					, indirectOcclusion
					, indirectBlend
					, voxelData );
				auto envBRDF = m_writer.declLocale( "envBRDF"
					, brdfMap.sample( vec2( lightSurface.NdotV(), roughness ) ) );
				indirectSpecular *= sdw::fma( m_utils.conductorFresnel( lightSurface.NdotV(), indirectSpecular.xyz() )
					, vec3( envBRDF.x() )
					, vec3( envBRDF.y() ) );
			}

			debugOutput.registerOutput( "Indirect", "Specular", indirectSpecular );
			return indirectSpecular;
		}

		sdw::Vec4 GlobalIllumination::traceConeRadiance( sdw::CombinedImage3DRgba32 const & pvoxels
			, LightSurface plightSurface
			, VoxelData const & pvoxelData )
		{
			if ( !m_traceConeRadiance )
			{
				auto cones = m_writer.declConstantArray( "cones"
					, std::vector< sdw::Vec3 >{ vec3( 0.57735_f, 0.57735, 0.57735 )
					, vec3( 0.57735_f, -0.57735, -0.57735 )
					, vec3( -0.57735_f, 0.57735, -0.57735 )
					, vec3( -0.57735_f, -0.57735, 0.57735 )
					, vec3( -0.903007_f, -0.182696, -0.388844 )
					, vec3( -0.903007_f, 0.182696, 0.388844 )
					, vec3( 0.903007_f, -0.182696, 0.388844 )
					, vec3( 0.903007_f, 0.182696, -0.388844 )
					, vec3( -0.388844_f, -0.903007, -0.182696 )
					, vec3( 0.388844_f, -0.903007, 0.182696 )
					, vec3( 0.388844_f, 0.903007, -0.182696 )
					, vec3( -0.388844_f, 0.903007, 0.182696 )
					, vec3( -0.182696_f, -0.388844, -0.903007 )
					, vec3( 0.182696_f, 0.388844, -0.903007 )
					, vec3( -0.182696_f, 0.388844, 0.903007 )
					, vec3( 0.182696_f, -0.388844, 0.903007 ) } );

				m_traceConeRadiance = m_writer.implementFunction< sdw::Vec4 >( "traceConeRadiance"
					, [&]( sdw::CombinedImage3DRgba32 const & voxels
						, sdw::Vec3 const & wsNormal
						, sdw::Vec3 const & wsPosition
						, shader::VoxelData const & voxelData )
					{
						auto radiance = m_writer.declLocale( "radiance"
							, vec4( 0.0_f ) );

						FOR( m_writer, sdw::UInt, cone, 0_u, cone < voxelData.radianceNumCones, ++cone ) // quality is between 1 and 16 cones
						{
							// approximate a hemisphere from random points inside a sphere:
							//  (and modulate cone with surface normal, no banding this way)
							auto wsConeDirection = m_writer.declLocale( "wsConeDirection"
								, normalize( cones[cone] + wsNormal ) );
							// if point on sphere is facing below normal (so it's located on bottom hemisphere), put it on the opposite hemisphere instead:
							wsConeDirection *= m_writer.ternary( dot( wsConeDirection, wsNormal ) < 0.0_f, -1.0_f, 1.0_f );

							radiance += traceCone( voxels
								, wsNormal
								, wsPosition
								, wsConeDirection
								, sdw::Float{ castor::Angle::fromRadians( castor::PiDiv2< float > / 3 ).tan() }
								, voxelData );
						}
						ROF;

						// final radiance is average of all the cones radiances
						radiance *= voxelData.radianceNumConesInv;
						radiance.a() = clamp( radiance.a(), 0.0_f, 1.0_f );

						m_writer.returnStmt( max( vec4( 0.0_f ), radiance ) );
					}
					, sdw::InCombinedImage3DRgba32{ m_writer, "voxels" }
					, sdw::InVec3{ m_writer, "wsNormal" }
					, sdw::InVec3{ m_writer, "wsPosition" }
					, InVoxelData{ m_writer, "voxelData" } );
			}

			return m_traceConeRadiance( pvoxels
				, plightSurface.N()
				, plightSurface.worldPosition()
				, pvoxelData );
		}

		sdw::Vec4 GlobalIllumination::traceCone( sdw::CombinedImage3DRgba32 const & pvoxels
			, sdw::Vec3 const & pwsNormal
			, sdw::Vec3 const & pwsPosition
			, sdw::Vec3 const & pwsConeDirection
			, sdw::Float const & pconeAperture
			, VoxelData const & pvoxelData )
		{
			if ( !m_traceCone )
			{
				m_traceCone = m_writer.implementFunction< sdw::Vec4 >( "traceCone"
					, [&]( sdw::CombinedImage3DRgba32 const & voxels
						, sdw::Vec3 const & wsNormal
						, sdw::Vec3 const & wsPosition
						, sdw::Vec3 const & wsConeDirection
						, sdw::Float const & coneAperture
						, VoxelData const & voxelData )
					{
						auto color = m_writer.declLocale( "color"
							, vec3( 0.0_f ) );
						auto occlusion = m_writer.declLocale( "occlusion"
							, 0.0_f );

						// We need to offset the cone start position to avoid sampling its own voxel (self-occlusion):
						//	Unfortunately, it will result in disconnection between nearby surfaces :(
						auto wsDist = m_writer.declLocale( "wsDist"
							, voxelData.gridToWorld ); // offset by cone dir so that first sample of all cones are not the same
						auto wsStartPos = m_writer.declLocale( "wsStartPos"
							, wsPosition + wsNormal * vec3( voxelData.gridToWorld * 2.0f * float( sqrt( 2.0f ) ) ) ); // sqrt2 is diagonal voxel half-extent

						// We will break off the loop if the sampling distance is too far for performance reasons:
						WHILE( m_writer, wsDist < voxelData.radianceMaxDistance && occlusion < 1.0_f )
						{
							auto wsDiameter = m_writer.declLocale( "wsDiameter"
								, max( voxelData.gridToWorld, 2.0_f * coneAperture * wsDist ) );
							auto mip = m_writer.declLocale( "mip"
								, log2( wsDiameter * voxelData.worldToGrid ) );

							auto tsCoord = m_writer.declLocale( "tsCoord"
								, voxelData.worldToTex( wsStartPos + wsConeDirection * vec3( wsDist ) ) );

							// break if the ray exits the voxel grid, or we sample from the last mip:
							IF( m_writer, !m_utils.isSaturated( tsCoord ) || mip >= voxelData.radianceMips )
							{
								m_writer.loopBreakStmt();
							}
							FI;

							auto sam = m_writer.declLocale( "sam"
								, voxels.lod( tsCoord, mip ) );

							// this is the correct blending to avoid black-staircase artifact (ray stepped front-to back, so blend front to back):
							auto a = m_writer.declLocale( "a"
								, 1.0_f - occlusion );
							color += a * sam.rgb();
							occlusion += a * sam.a();

							// step along ray:
							wsDist += wsDiameter * voxelData.rayStepSize;
						}
						ELIHW;

						m_writer.returnStmt( vec4( color, occlusion ) );
					}
					, sdw::InCombinedImage3DRgba32{ m_writer, "voxels" }
					, sdw::InVec3{ m_writer, "wsNormal" }
					, sdw::InVec3{ m_writer, "wsPosition" }
					, sdw::InVec3{ m_writer, "wsConeDirection" }
					, sdw::InFloat{ m_writer, "coneAperture" }
					, InVoxelData{ m_writer, "voxelData" } );
			}

			return m_traceCone( pvoxels
				, pwsNormal
				, pwsPosition
				, pwsConeDirection
				, pconeAperture
				, pvoxelData );
		}

		sdw::Vec4 GlobalIllumination::traceConeReflection( sdw::CombinedImage3DRgba32 const & pvoxels
			, LightSurface plightSurface
			, sdw::Float const & proughness
			, VoxelData const & pvoxelData )
		{
			if ( !m_traceConeReflection )
			{
				m_traceConeReflection = m_writer.implementFunction< sdw::Vec4 >( "traceConeReflection"
					, [&]( sdw::CombinedImage3DRgba32 const & voxels
						, sdw::Vec3 const & wsNormal
						, sdw::Vec3 const & wsPosition
						, sdw::Vec3 const & V
						, sdw::Float const & roughness
						, shader::VoxelData const & voxelData )
					{
						auto aperture = m_writer.declLocale( "aperture"
							, tan( roughness * sdw::Float{ castor::PiDiv2< float > / 10 } ) );
						auto wsConeDirection = m_writer.declLocale( "wsConeDirection"
							, reflect( -V, wsNormal ) );

						auto reflection = m_writer.declLocale( "reflection"
							, traceCone( voxels
								, wsNormal
								, wsPosition
								, wsConeDirection
								, aperture
								, voxelData ) );
						m_writer.returnStmt( vec4( max( vec3( 0.0_f ), reflection.rgb() )
							, clamp( reflection.a() * ( 1.0_f - roughness ), 0.0_f, 1.0_f ) ) );
					}
					, sdw::InCombinedImage3DRgba32{ m_writer, "voxels" }
					, sdw::InVec3{ m_writer, "wsNormal" }
					, sdw::InVec3{ m_writer, "wsPosition" }
					, sdw::InVec3{ m_writer, "V" }
					, sdw::InFloat{ m_writer, "roughness" }
					, InVoxelData{ m_writer, "voxelData" } );
			}

			return m_traceConeReflection( pvoxels
				, plightSurface.N()
				, plightSurface.worldPosition()
				, plightSurface.V()
				, proughness
				, pvoxelData );
		}

		sdw::Float GlobalIllumination::traceConeOcclusion( sdw::CombinedImage3DRgba32 const & pvoxels
			, LightSurface plightSurface
			, VoxelData const & pvoxelData )
		{
			if ( !m_traceConeOcclusion )
			{
				m_traceConeOcclusion = m_writer.implementFunction< sdw::Float >( "traceConeOcclusion"
					, [&]( sdw::CombinedImage3DRgba32 const & voxels
						, sdw::Vec3 const & wsNormal
						, sdw::Vec3 const & wsPosition
						, sdw::Vec3 const & L
						, shader::VoxelData const & voxelData )
					{
						auto coneAperture = m_writer.declLocale( "coneAperture"
							, sdw::Float{ castor::Angle::fromRadians( castor::PiDiv2< float > ).tan() } );
						auto occlusion = m_writer.declLocale( "occlusion"
							, 0.0_f );
						// We need to offset the cone start position to avoid sampling its own voxel (self-occlusion):
						//	Unfortunately, it will result in disconnection between nearby surfaces :(
						auto wsDist = m_writer.declLocale( "wsDist"
							, voxelData.gridToWorld ); // offset by cone dir so that first sample of all cones are not the same
						auto wsStartPos = m_writer.declLocale( "wsStartPos"
							, wsPosition + wsNormal * vec3( voxelData.gridToWorld * 2.0f * float( sqrt( 2.0f ) ) ) ); // sqrt2 is diagonal voxel half-extent

						// We will break off the loop if the sampling distance is too far for performance reasons:
						WHILE( m_writer, wsDist < voxelData.radianceMaxDistance && occlusion < 1.0_f )
						{
							auto wsDiameter = m_writer.declLocale( "wsDiameter"
								, max( voxelData.gridToWorld, 2.0_f * coneAperture * wsDist ) );
							auto mip = m_writer.declLocale( "mip"
								, log2( wsDiameter * voxelData.worldToGrid ) );

							auto tsCoord = m_writer.declLocale( "tsCoord"
								, voxelData.worldToTex( wsStartPos - L * vec3( wsDist ) ) );

							// break if the ray exits the voxel grid, or we sample from the last mip:
							IF( m_writer, !m_utils.isSaturated( tsCoord ) || mip >= voxelData.radianceMips )
							{
								m_writer.loopBreakStmt();
							}
							FI;

							auto sam = m_writer.declLocale( "sam"
								, voxels.lod( tsCoord, mip ) );

							// this is the correct blending to avoid black-staircase artifact (ray stepped front-to back, so blend front to back):
							auto a = m_writer.declLocale( "a"
								, 1.0_f - occlusion );
							occlusion += a * sam.a() * smoothStep( 0.0_f, voxelData.radianceMaxDistance, sqrt( wsDist ) );

							// step along ray:
							wsDist += wsDiameter * voxelData.rayStepSize;
						}
						ELIHW;

						m_writer.returnStmt( occlusion );
					}
					, sdw::InCombinedImage3DRgba32{ m_writer, "voxels" }
					, sdw::InVec3{ m_writer, "wsNormal" }
					, sdw::InVec3{ m_writer, "wsPosition" }
					, sdw::InVec3{ m_writer, "L" }
					, InVoxelData{ m_writer, "voxelData" } );
			}

			return m_traceConeOcclusion( pvoxels
				, plightSurface.N()
				, plightSurface.worldPosition()
				, plightSurface.L()
				, pvoxelData );
		}
	}
}
