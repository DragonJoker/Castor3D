#include "Castor3D/Shader/Shaders/GlslGlobalIllumination.hpp"

#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

#include <CastorUtils/Math/Angle.hpp>

#include <ShaderWriter/Source.hpp>

using namespace sdw;

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

		void GlobalIllumination::declare( uint32_t setIndex
			, uint32_t & bindingIndex
			, SceneFlags sceneFlags )
		{
			using namespace sdw;

			if ( checkFlag( sceneFlags, SceneFlag::eVoxelConeTracing ) )
			{
				declareVct( bindingIndex, setIndex );
			}
			else
			{
				if ( checkFlag( sceneFlags, SceneFlag::eLpvGI ) )
				{
					declareLpv( bindingIndex, setIndex );
				}

				if ( checkFlag( sceneFlags, SceneFlag::eLayeredLpvGI ) )
				{
					declareLayeredLpv( bindingIndex, setIndex );
				}
			}
		}

		void GlobalIllumination::declareVct( uint32_t & bindingIndex
			, uint32_t setIndex )
		{
			UBO_VOXELIZER( m_writer, VoxelizerUbo::BindingPoint, 0u, true );
			auto c3d_mapVoxels = m_writer.declSampledImage< FImg3DRgba32 >( "c3d_mapVoxels", bindingIndex++, setIndex );
			m_utils.declareFresnelSchlick();
			m_utils.declareTraceCone();
		}

		void GlobalIllumination::declareLpv( uint32_t & bindingIndex
			, uint32_t setIndex )
		{
			UBO_LPVGRIDCONFIG( m_writer, LpvGridConfigUbo::BindingPoint, 0u, true );
			auto c3d_lpvAccumulatorR = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eR, "Accumulator" ), bindingIndex++, setIndex );
			auto c3d_lpvAccumulatorG = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eG, "Accumulator" ), bindingIndex++, setIndex );
			auto c3d_lpvAccumulatorB = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eB, "Accumulator" ), bindingIndex++, setIndex );

			/*Spherical harmonics coefficients - precomputed*/
			auto SH_C0 = m_writer.declConstant( "SH_C0"
				, Float{ 1.0f / ( 2.0f * sqrt( castor::Pi< float > ) ) } );
			auto SH_C1 = m_writer.declConstant( "SH_C1"
				, Float{ sqrt( 3.0f / castor::Pi< float > ) / 2.0f } );

			// no normalization
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

			m_computeLPVRadiance = m_writer.implementFunction< sdw::Vec3 >( "computeLPVRadiance"
				, [this]( Vec3 wsPosition
					, Vec3 wsNormal
					, LpvGridData lpvGridData )
				{
					auto c3d_lpvAccumulatorR = m_writer.getVariable< SampledImage3DRgba16 >( getTextureName( LpvTexture::eR, "Accumulator" ) );
					auto c3d_lpvAccumulatorG = m_writer.getVariable< SampledImage3DRgba16 >( getTextureName( LpvTexture::eG, "Accumulator" ) );
					auto c3d_lpvAccumulatorB = m_writer.getVariable< SampledImage3DRgba16 >( getTextureName( LpvTexture::eB, "Accumulator" ) );

					auto SHintensity = m_writer.declLocale( "SHintensity"
						, m_evalSH( -wsNormal ) );
					auto lpvCellCoords = m_writer.declLocale( "lpvCellCoords"
						, ( wsPosition - lpvGridData.minVolumeCorner ) / lpvGridData.cellSize / lpvGridData.gridSize );
					auto lpvIntensity = m_writer.declLocale( "lpvIntensity"
						, vec3( dot( SHintensity, c3d_lpvAccumulatorR.sample( lpvCellCoords ) )
							, dot( SHintensity, c3d_lpvAccumulatorG.sample( lpvCellCoords ) )
							, dot( SHintensity, c3d_lpvAccumulatorB.sample( lpvCellCoords ) ) ) );
					m_writer.returnStmt( max( lpvIntensity, vec3( 0.0_f ) ) );
				}
				, InVec3{ m_writer, "wsPosition" }
				, InVec3{ m_writer, "wsNormal" }
				, InLpvGridData{ m_writer, "lpvGridData" } );
		}

		void GlobalIllumination::declareLayeredLpv( uint32_t & bindingIndex
			, uint32_t setIndex )
		{
			using namespace sdw;
			UBO_LAYERED_LPVGRIDCONFIG( m_writer, LayeredLpvGridConfigUbo::BindingPoint, 0u, true );
			auto c3d_lpvAccumulator1R = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eR, "Accumulator1" ), bindingIndex++, setIndex );
			auto c3d_lpvAccumulator1G = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eG, "Accumulator1" ), bindingIndex++, setIndex );
			auto c3d_lpvAccumulator1B = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eB, "Accumulator1" ), bindingIndex++, setIndex );
			auto c3d_lpvAccumulator2R = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eR, "Accumulator2" ), bindingIndex++, setIndex );
			auto c3d_lpvAccumulator2G = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eG, "Accumulator2" ), bindingIndex++, setIndex );
			auto c3d_lpvAccumulator2B = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eB, "Accumulator2" ), bindingIndex++, setIndex );
			auto c3d_lpvAccumulator3R = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eR, "Accumulator3" ), bindingIndex++, setIndex );
			auto c3d_lpvAccumulator3G = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eG, "Accumulator3" ), bindingIndex++, setIndex );
			auto c3d_lpvAccumulator3B = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eB, "Accumulator3" ), bindingIndex++, setIndex );

			/*Spherical harmonics coefficients - precomputed*/
			auto SH_C0 = m_writer.declConstant( "SH_C0"
				, Float{ 1.0f / ( 2.0f * sqrt( castor::Pi< float > ) ) } );
			auto SH_C1 = m_writer.declConstant( "SH_C1"
				, Float{ sqrt( 3.0f / castor::Pi< float > ) / 2.0f } );

			// no normalization
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

			m_computeLLPVRadiance = m_writer.implementFunction< sdw::Vec3 >( "computeLPVRadiance"
				, [this]( Vec3 wsPosition
					, Vec3 wsNormal
					, LayeredLpvGridData llpvGridData )
				{
					auto c3d_lpvAccumulator1R = m_writer.getVariable< SampledImage3DRgba16 >( getTextureName( LpvTexture::eR, "Accumulator1" ) );
					auto c3d_lpvAccumulator1G = m_writer.getVariable< SampledImage3DRgba16 >( getTextureName( LpvTexture::eG, "Accumulator1" ) );
					auto c3d_lpvAccumulator1B = m_writer.getVariable< SampledImage3DRgba16 >( getTextureName( LpvTexture::eB, "Accumulator1" ) );
					auto c3d_lpvAccumulator2R = m_writer.getVariable< SampledImage3DRgba16 >( getTextureName( LpvTexture::eR, "Accumulator2" ) );
					auto c3d_lpvAccumulator2G = m_writer.getVariable< SampledImage3DRgba16 >( getTextureName( LpvTexture::eG, "Accumulator2" ) );
					auto c3d_lpvAccumulator2B = m_writer.getVariable< SampledImage3DRgba16 >( getTextureName( LpvTexture::eB, "Accumulator2" ) );
					auto c3d_lpvAccumulator3R = m_writer.getVariable< SampledImage3DRgba16 >( getTextureName( LpvTexture::eR, "Accumulator3" ) );
					auto c3d_lpvAccumulator3G = m_writer.getVariable< SampledImage3DRgba16 >( getTextureName( LpvTexture::eG, "Accumulator3" ) );
					auto c3d_lpvAccumulator3B = m_writer.getVariable< SampledImage3DRgba16 >( getTextureName( LpvTexture::eB, "Accumulator3" ) );

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

					m_writer.returnStmt( max( lpvIntensity1 + lpvIntensity2 + lpvIntensity3, vec3( 0.0_f ) ) );
				}
				, InVec3{ m_writer, "wsPosition" }
				, InVec3{ m_writer, "wsNormal" }
				, InLayeredLpvGridData{ m_writer, "llpvGridData" } );
		}

		sdw::Vec3 GlobalIllumination::computeLPVRadiance( sdw::Vec3 wsPosition
			, sdw::Vec3 wsNormal
			, LpvGridData lpvGridData )
		{
			CU_Require( m_computeLPVRadiance );
			return m_computeLPVRadiance( wsPosition
				, wsNormal
				, lpvGridData );
		}

		sdw::Vec3 GlobalIllumination::computeLLPVRadiance( sdw::Vec3 wsPosition
			, sdw::Vec3 wsNormal
			, LayeredLpvGridData llpvGridData )
		{
			CU_Require( m_computeLLPVRadiance );
			return m_computeLLPVRadiance( wsPosition
				, wsNormal
				, llpvGridData );
		}

		sdw::Vec3 GlobalIllumination::computeDiffuse( SceneFlags sceneFlags
			, sdw::Vec3 wsPosition
			, sdw::Vec3 wsNormal
			, sdw::Vec3 diffuse
			, sdw::Vec3 allButAmbient
			, sdw::Vec3 ambient )
		{
			if ( checkFlag( sceneFlags, SceneFlag::eVoxelConeTracing ) )
			{
				auto voxelData = m_writer.getVariable< VoxelData >( "c3d_voxelData" );
				auto mapVoxels = m_writer.getVariable< SampledImage3DRgba32 >( "c3d_mapVoxels" );

				auto vxlPosition = m_writer.declLocale( "vxlPosition"
					, wsPosition );
				vxlPosition *= voxelData.sizeInv;
				vxlPosition *= voxelData.resolutionInv;
				vxlPosition = clamp( abs( vxlPosition ), vec3( 0.0_f ), vec3( 1.0_f ) );
				auto vxlBlend = m_writer.declLocale( "vxlBlend"
					, 1.0_f - pow( max( vxlPosition.x(), max( vxlPosition.y(), vxlPosition.z() ) ), 4.0_f ) );

				auto vxlRadiance = m_writer.declLocale( "vxlRadiance"
					, m_utils.traceConeRadiance( mapVoxels
						, wsPosition
						, wsNormal
						, voxelData ) );
				auto indirect = m_writer.declLocale( "indirect"
					, mix( vec3( 0.0_f ), vxlRadiance.xyz(), vec3( vxlRadiance.a() * vxlBlend ) ) );
				return ( indirect * ambient ) + allButAmbient;
			}
			else
			{
				if ( checkFlag( sceneFlags, SceneFlag::eLayeredLpvGI ) )
				{
					auto llpvGridData = m_writer.getVariable< LayeredLpvGridData >( "c3d_llpvGridData" );
					auto indirect = m_writer.declLocale( "indirect"
						, computeLLPVRadiance( wsPosition
							, wsNormal
							, llpvGridData ) );
					return ( ( indirect * diffuse * ambient / Float{ castor::Pi< float > } )
						+ ( indirect * llpvGridData.indirectAttenuation / Float{ castor::Pi< float > } )
						+ allButAmbient );
				}

				if ( checkFlag( sceneFlags, SceneFlag::eLpvGI ) )
				{
					auto lpvGridData = m_writer.getVariable< LpvGridData >( "c3d_lpvGridData" );
					auto indirect = m_writer.declLocale( "indirect"
						, computeLPVRadiance( wsPosition
							, wsNormal
							, lpvGridData ) );
					return ( ( indirect * diffuse * ambient / Float{ castor::Pi< float > } )
						+ ( indirect * lpvGridData.indirectAttenuation / Float{ castor::Pi< float > } )
						+ allButAmbient );
				}
			}

			return ambient + allButAmbient;
		}

		sdw::Vec3 GlobalIllumination::computeSpecular( SceneFlags sceneFlags
			, sdw::Vec3 wsCamera
			, sdw::Vec3 wsPosition
			, sdw::Vec3 wsNormal
			, sdw::Float roughness
			, sdw::Vec3 f0
			, sdw::Vec3 specular )
		{
			if ( checkFlag( sceneFlags, SceneFlag::eVoxelConeTracing ) )
			{
				auto voxelData = m_writer.getVariable< VoxelData >( "c3d_voxelData" );
				auto mapVoxels = m_writer.getVariable< SampledImage3DRgba32 >( "c3d_mapVoxels" );
				auto vxlPosition = m_writer.getVariable< sdw::Vec3 >( "vxlPosition" );
				auto vxlBlend = m_writer.getVariable< sdw::Float >( "vxlBlend" );

				auto vxlReflection = m_writer.declLocale( "vxlReflection"
					, m_utils.traceConeReflection( mapVoxels
						, wsPosition
						, wsNormal
						, wsCamera - wsPosition
						, roughness
						, voxelData ) );
				auto indirectSpecular = m_writer.declLocale( "indirectSpecular"
					, mix( vec3( 0.0_f ), vxlReflection.xyz(), vec3( vxlReflection.a() * vxlBlend ) ) );
				auto V = m_writer.declLocale( "V"
					, normalize( wsCamera - wsPosition ) );
				auto NdotV = m_writer.declLocale( "NdotV"
					, max( 0.0_f, dot( wsNormal, V ) ) );
				return specular + indirectSpecular * m_utils.fresnelSchlick( NdotV, f0, roughness );
			}

			return specular;
		}
	}
}
