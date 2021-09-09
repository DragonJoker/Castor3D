#include "Castor3D/Shader/Shaders/GlslGlobalIllumination.hpp"

#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"
#include "Castor3D/Shader/Ubos/VoxelizerUbo.hpp"

#include <CastorUtils/Math/Angle.hpp>

#include <ShaderWriter/Source.hpp>

using namespace sdw;

namespace castor3d
{
	namespace shader
	{
		GlobalIllumination::GlobalIllumination( sdw::ShaderWriter & writer
			, Utils & utils
			, bool deferred )
			: m_writer{ writer }
			, m_utils{ utils }
			, m_deferred{ deferred }
		{
		}

		void GlobalIllumination::declare( uint32_t & bindingIndex
			, uint32_t setIndex
			, SceneFlags sceneFlags )
		{
			m_utils.declareFresnelSchlick();

			if ( checkFlag( sceneFlags, SceneFlag::eVoxelConeTracing ) )
			{
				declareVct( bindingIndex, bindingIndex, setIndex, setIndex );
			}
			else if ( !m_deferred )
			{
				if ( checkFlag( sceneFlags, SceneFlag::eLpvGI ) )
				{
					declareLpv( bindingIndex, bindingIndex, setIndex, setIndex );
				}

				if ( checkFlag( sceneFlags, SceneFlag::eLayeredLpvGI ) )
				{
					declareLayeredLpv( bindingIndex, bindingIndex, setIndex, setIndex );
				}
			}
		}

		void GlobalIllumination::declare( uint32_t vctUboBindingIndex
			, uint32_t lpvUboBindingIndex
			, uint32_t llpvUboBindingIndex
			, uint32_t & vctTexBindingIndex
			, uint32_t & lpvTexBindingIndex
			, uint32_t & llpvTexBindingIndex
			, uint32_t texSetIndex
			, SceneFlags sceneFlags )
		{
			using namespace sdw;
			m_utils.declareFresnelSchlick();

			if ( checkFlag( sceneFlags, SceneFlag::eVoxelConeTracing ) )
			{
				declareVct( vctUboBindingIndex, vctTexBindingIndex, 0u, texSetIndex );
			}
			else
			{
				if ( checkFlag( sceneFlags, SceneFlag::eLpvGI ) )
				{
					declareLpv( lpvUboBindingIndex, lpvTexBindingIndex, 0u, texSetIndex );
				}

				if ( checkFlag( sceneFlags, SceneFlag::eLayeredLpvGI ) )
				{
					declareLayeredLpv( llpvUboBindingIndex, llpvTexBindingIndex, 0u, texSetIndex );
				}
			}
		}

		void GlobalIllumination::declareTraceConeRadiance()
		{
			declareTraceCone();
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
				, [&]( sdw::SampledImage3DRgba32 const & voxels
					, Surface surface
					, shader::VoxelData const & voxelData )
				{
					auto radiance = m_writer.declLocale( "radiance"
						, vec4( 0.0_f ) );

					FOR( m_writer, sdw::UInt, cone, 0_u, cone < voxelData.radianceNumCones, ++cone ) // quality is between 1 and 16 cones
					{
						// approximate a hemisphere from random points inside a sphere:
						//  (and modulate cone with surface normal, no banding this way)
						auto wsConeDirection = m_writer.declLocale( "wsConeDirection"
							, normalize( cones[cone] + surface.worldNormal ) );
						// if point on sphere is facing below normal (so it's located on bottom hemisphere), put it on the opposite hemisphere instead:
						wsConeDirection *= m_writer.ternary( dot( wsConeDirection, surface.worldNormal ) < 0.0_f, -1.0_f, 1.0_f );

						radiance += m_traceCone( voxels
							, surface
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
				, sdw::InSampledImage3DRgba32{ m_writer, "voxels" }
				, InSurface{ m_writer, "surface" }
				, InVoxelData{ m_writer, "voxelData" } );
		}

		void GlobalIllumination::declareVct( uint32_t & uboBindingIndex
			, uint32_t & texBindingIndex
			, uint32_t uboSetIndex
			, uint32_t texSetIndex )
		{
			UBO_VOXELIZER( m_writer, uboBindingIndex++, uboSetIndex, true );
			m_writer.declSampledImage< FImg3DRgba32 >( "c3d_mapVoxelsFirstBounce", texBindingIndex++, texSetIndex );
			m_writer.declSampledImage< FImg3DRgba32 >( "c3d_mapVoxelsSecondaryBounce", texBindingIndex++, texSetIndex );
			declareTraceConeRadiance();
			declareTraceConeReflection();
			declareTraceConeOcclusion();
		}

		void GlobalIllumination::declareLpv( uint32_t & uboBindingIndex
			, uint32_t & texBindingIndex
			, uint32_t uboSetIndex
			, uint32_t texSetIndex
			, bool declUbo )
		{
			if ( declUbo )
			{
				UBO_LPVGRIDCONFIG( m_writer, uboBindingIndex++, uboSetIndex, true );
			}

			auto c3d_lpvAccumulatorR = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eR, "Accumulator" ), texBindingIndex++, texSetIndex );
			auto c3d_lpvAccumulatorG = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eG, "Accumulator" ), texBindingIndex++, texSetIndex );
			auto c3d_lpvAccumulatorB = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eB, "Accumulator" ), texBindingIndex++, texSetIndex );

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

			m_computeLPVRadiance = m_writer.implementFunction< sdw::Vec4 >( "computeLPVRadiance"
				, [this]( Surface surface
					, LpvGridData lpvGridData )
				{
					auto c3d_lpvAccumulatorR = m_writer.getVariable< SampledImage3DRgba16 >( getTextureName( LpvTexture::eR, "Accumulator" ) );
					auto c3d_lpvAccumulatorG = m_writer.getVariable< SampledImage3DRgba16 >( getTextureName( LpvTexture::eG, "Accumulator" ) );
					auto c3d_lpvAccumulatorB = m_writer.getVariable< SampledImage3DRgba16 >( getTextureName( LpvTexture::eB, "Accumulator" ) );

					auto SHintensity = m_writer.declLocale( "SHintensity"
						, m_evalSH( -surface.worldNormal ) );
					auto lpvCellCoords = m_writer.declLocale( "lpvCellCoords"
						, lpvGridData.worldToTex( surface.worldPosition ) );
					auto lpvIntensity = m_writer.declLocale( "lpvIntensity"
						, vec3( dot( SHintensity, c3d_lpvAccumulatorR.sample( lpvCellCoords ) )
							, dot( SHintensity, c3d_lpvAccumulatorG.sample( lpvCellCoords ) )
							, dot( SHintensity, c3d_lpvAccumulatorB.sample( lpvCellCoords ) ) ) );
					m_writer.returnStmt( vec4( max( lpvIntensity, vec3( 0.0_f ) ), 1.0f ) );
				}
				, InSurface{ m_writer, "surface" }
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
				UBO_LAYERED_LPVGRIDCONFIG( m_writer, uboBindingIndex++, uboSetIndex, true );
			}

			auto c3d_lpvAccumulator1R = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eR, "Accumulator1" ), texBindingIndex++, texSetIndex );
			auto c3d_lpvAccumulator1G = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eG, "Accumulator1" ), texBindingIndex++, texSetIndex );
			auto c3d_lpvAccumulator1B = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eB, "Accumulator1" ), texBindingIndex++, texSetIndex );
			auto c3d_lpvAccumulator2R = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eR, "Accumulator2" ), texBindingIndex++, texSetIndex );
			auto c3d_lpvAccumulator2G = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eG, "Accumulator2" ), texBindingIndex++, texSetIndex );
			auto c3d_lpvAccumulator2B = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eB, "Accumulator2" ), texBindingIndex++, texSetIndex );
			auto c3d_lpvAccumulator3R = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eR, "Accumulator3" ), texBindingIndex++, texSetIndex );
			auto c3d_lpvAccumulator3G = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eG, "Accumulator3" ), texBindingIndex++, texSetIndex );
			auto c3d_lpvAccumulator3B = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eB, "Accumulator3" ), texBindingIndex++, texSetIndex );

			/*Spherical harmonics coefficients - precomputed*/
			auto SH_C0 = m_writer.declConstant( "SH_C0"
				, Float{ 1.0f / ( 2.0f * sqrt( castor::Pi< float > ) ) } );
			auto SH_C1 = m_writer.declConstant( "SH_C1"
				, Float{ sqrt( 3.0f / castor::Pi< float > ) / 2.0f } );

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
				, [this]( Surface surface
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
						, m_evalSH( -surface.worldNormal ) );
					auto lpvCellCoords1 = m_writer.declLocale( "lpvCellCoords1"
						, ( surface.worldPosition - llpvGridData.allMinVolumeCorners[0].xyz() ) / llpvGridData.allCellSizes.x() / llpvGridData.gridSizes );
					auto lpvCellCoords2 = m_writer.declLocale( "lpvCellCoords2"
						, ( surface.worldPosition - llpvGridData.allMinVolumeCorners[1].xyz() ) / llpvGridData.allCellSizes.y() / llpvGridData.gridSizes );
					auto lpvCellCoords3 = m_writer.declLocale( "lpvCellCoords3"
						, ( surface.worldPosition - llpvGridData.allMinVolumeCorners[2].xyz() ) / llpvGridData.allCellSizes.z() / llpvGridData.gridSizes );

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
				, InSurface{ m_writer, "surface" }
				, InLayeredLpvGridData{ m_writer, "llpvGridData" } );
		}

		sdw::Vec4 GlobalIllumination::computeLPVRadiance( Surface surface
			, LpvGridData lpvGridData )
		{
			CU_Require( m_computeLPVRadiance );
			return m_computeLPVRadiance( surface
				, lpvGridData );
		}

		sdw::Vec4 GlobalIllumination::computeLLPVRadiance( Surface surface
			, LayeredLpvGridData llpvGridData )
		{
			CU_Require( m_computeLLPVRadiance );
			return m_computeLLPVRadiance( surface
				, llpvGridData );
		}

		sdw::Float GlobalIllumination::computeOcclusion( SceneFlags sceneFlags
			, LightType lightType
			, Surface surface )
		{
			auto vxlOcclusion = m_writer.declLocale( "vxlOcclusion"
				, 1.0_f );

			if ( checkFlag( sceneFlags, SceneFlag::eVoxelConeTracing ) )
			{
				auto voxelData = m_writer.getVariable< VoxelData >( "c3d_voxelData" );
				auto mapVoxelsFirstBounce = m_writer.getVariable< SampledImage3DRgba32 >( "c3d_mapVoxelsFirstBounce" );
				auto mapVoxelsSecondaryBounce = m_writer.getVariable< SampledImage3DRgba32 >( "c3d_mapVoxelsSecondaryBounce" );

				IF( m_writer, voxelData.enableOcclusion )
				{
					IF( m_writer, voxelData.enableSecondaryBounce )
					{
						switch ( lightType )
						{
						case LightType::eDirectional:
							{
								auto c3d_light = m_writer.getVariable< shader::DirectionalLight >( "c3d_light" );
								vxlOcclusion = traceConeOcclusion( mapVoxelsSecondaryBounce
									, surface
									, c3d_light.m_direction
									, voxelData );
							}
							break;
						case LightType::ePoint:
							{
								auto c3d_light = m_writer.getVariable< shader::PointLight >( "c3d_light" );
								vxlOcclusion = traceConeOcclusion( mapVoxelsSecondaryBounce
									, surface
									, c3d_light.m_position - surface.worldPosition
									, voxelData );
							}
							break;
						case LightType::eSpot:
							{
								auto c3d_light = m_writer.getVariable< shader::SpotLight >( "c3d_light" );
								vxlOcclusion = traceConeOcclusion( mapVoxelsSecondaryBounce
									, surface
									, c3d_light.m_position - surface.worldPosition
									, voxelData );
							}
							break;
						default:
							CU_Failure( "Unsupported LightType" );
							break;
						}
					}
					ELSE
					{
						switch ( lightType )
						{
						case LightType::eDirectional:
							{
								auto c3d_light = m_writer.getVariable< shader::DirectionalLight >( "c3d_light" );
								vxlOcclusion = traceConeOcclusion( mapVoxelsFirstBounce
									, surface
									, c3d_light.m_direction
									, voxelData );
							}
							break;
						case LightType::ePoint:
							{
								auto c3d_light = m_writer.getVariable< shader::PointLight >( "c3d_light" );
								vxlOcclusion = traceConeOcclusion( mapVoxelsFirstBounce
									, surface
									, c3d_light.m_position - surface.worldPosition
									, voxelData );
							}
							break;
						case LightType::eSpot:
							{
								auto c3d_light = m_writer.getVariable< shader::SpotLight >( "c3d_light" );
								vxlOcclusion = traceConeOcclusion( mapVoxelsFirstBounce
									, surface
									, c3d_light.m_position - surface.worldPosition
									, voxelData );
							}
							break;
						default:
							CU_Failure( "Unsupported LightType" );
							break;
						}
					}
					FI;
				}
				FI;
			}

			return vxlOcclusion;
		}

		sdw::Vec4 GlobalIllumination::computeVCTRadiance( Surface const & surface
			, VoxelData const & voxelData
			, sdw::Float const & indirectOcclusion )const
		{
			auto mapVoxelsFirstBounce = m_writer.getVariable< SampledImage3DRgba32 >( "c3d_mapVoxelsFirstBounce" );
			auto mapVoxelsSecondaryBounce = m_writer.getVariable< SampledImage3DRgba32 >( "c3d_mapVoxelsSecondaryBounce" );

			auto vxlRadiance( m_writer.declLocale< sdw::Vec4 >( "vxlRadiance" ) );

			IF( m_writer, voxelData.enableSecondaryBounce )
			{
				vxlRadiance = traceConeRadiance( mapVoxelsSecondaryBounce
					, surface
					, voxelData );
			}
			ELSE
			{
				vxlRadiance = traceConeRadiance( mapVoxelsFirstBounce
				, surface
					, voxelData );
			}
			FI;

			auto vxlPosition = m_writer.declLocale( "vxlPosition"
				, clamp( abs( voxelData.worldToClip( surface.worldPosition ) ), vec3( -1.0_f ), vec3( 1.0_f ) ) );
			auto vxlBlend = m_writer.declLocale( "vxlBlend"
				, 1.0_f - pow( max( vxlPosition.x(), max( vxlPosition.y(), vxlPosition.z() ) ), 4.0_f ) );
			return vec4( mix( vec3( 0.0_f )
					, vxlRadiance.xyz() * indirectOcclusion
					, vec3( vxlRadiance.a() * vxlBlend * indirectOcclusion ) )
				, vxlBlend );
		}

		sdw::Vec3 GlobalIllumination::computeVCTSpecular( sdw::Vec3 const & wsCamera
			, sdw::Vec3 const & vsPosition
			, Surface const & surface
			, sdw::Float const & roughness
			, sdw::Float const & indirectOcclusion
			, sdw::Float const & indirectBlend
			, VoxelData const & voxelData )const
		{
			auto mapVoxelsFirstBounce = m_writer.getVariable< SampledImage3DRgba32 >( "c3d_mapVoxelsFirstBounce" );
			auto mapVoxelsSecondaryBounce = m_writer.getVariable< SampledImage3DRgba32 >( "c3d_mapVoxelsSecondaryBounce" );
			auto vxlReflection( m_writer.declLocale< sdw::Vec4 >( "vxlReflection" ) );

			IF( m_writer, voxelData.enableSecondaryBounce )
			{
				vxlReflection = traceConeReflection( mapVoxelsSecondaryBounce
					, surface
					, wsCamera - surface.worldPosition
					, roughness
					, voxelData );
			}
			ELSE
			{
				vxlReflection = traceConeReflection( mapVoxelsFirstBounce
					, surface
					, wsCamera - surface.worldPosition
					, roughness
					, voxelData );
			}
			FI;

			return mix( vec3( 0.0_f )
					, vxlReflection.xyz()
					, vec3( vxlReflection.a() * indirectBlend * indirectOcclusion ) );
		}

		sdw::Vec4 GlobalIllumination::computeDiffuse( SceneFlags sceneFlags
			, Surface surface
			, sdw::Float indirectOcclusion )
		{
			auto indirectDiffuse = m_writer.declLocale( "lightIndirectDiffuse"
				, vec4( 0.0_f ) );

			if ( checkFlag( sceneFlags, SceneFlag::eVoxelConeTracing ) )
			{
				auto voxelData = m_writer.getVariable< VoxelData >( "c3d_voxelData" );
				indirectDiffuse += computeVCTRadiance( surface, voxelData, indirectOcclusion );
			}
			else
			{
				if ( checkFlag( sceneFlags, SceneFlag::eLayeredLpvGI ) )
				{
					auto llpvGridData = m_writer.getVariable< LayeredLpvGridData >( "c3d_llpvGridData" );
					indirectDiffuse += ( computeLLPVRadiance( surface, llpvGridData ) * llpvGridData.indirectAttenuation ) / Float{ castor::Pi< float > };
				}

				if ( checkFlag( sceneFlags, SceneFlag::eLpvGI ) )
				{
					auto lpvGridData = m_writer.getVariable< LpvGridData >( "c3d_lpvGridData" );
					indirectDiffuse += ( computeLPVRadiance( surface, lpvGridData ) * lpvGridData.indirectAttenuation() ) / Float{ castor::Pi< float > };
				}
			}

			return indirectDiffuse;
		}

		sdw::Vec3 GlobalIllumination::computeAmbient( SceneFlags sceneFlags
			, sdw::Vec3 const & indirectDiffuse )
		{
			if ( checkFlag( sceneFlags, SceneFlag::eVoxelConeTracing ) )
			{
				return indirectDiffuse;
			}

			if ( checkFlag( sceneFlags, SceneFlag::eLayeredLpvGI ) )
			{
				auto llpvGridData = m_writer.getVariable< LayeredLpvGridData >( "c3d_llpvGridData" );
				return indirectDiffuse / llpvGridData.indirectAttenuation;
			}

			if ( checkFlag( sceneFlags, SceneFlag::eLpvGI ) )
			{
				auto lpvGridData = m_writer.getVariable< LpvGridData >( "c3d_lpvGridData" );
				return indirectDiffuse / lpvGridData.indirectAttenuation();
			}

			return vec3( 1.0_f );
		}

		sdw::Vec3 GlobalIllumination::computeSpecular( SceneFlags sceneFlags
			, sdw::Vec3 wsCamera
			, sdw::Vec3 vsPosition
			, Surface surface
			, sdw::Vec3 specular
			, sdw::Float roughness
			, sdw::Float indirectOcclusion
			, sdw::Float indirectBlend )
		{
			auto indirectSpecular = m_writer.declLocale< sdw::Vec3 >( "lightIndirectSpecular"
				, vec3( 0.0_f ) );

			if ( checkFlag( sceneFlags, SceneFlag::eVoxelConeTracing ) )
			{
				auto voxelData = m_writer.getVariable< VoxelData >( "c3d_voxelData" );
				indirectSpecular = computeVCTSpecular( wsCamera
					, vsPosition
					, surface
					, roughness
					, indirectOcclusion
					, indirectBlend
					, voxelData );
				auto V = m_writer.declLocale( "V"
					, normalize( vsPosition ) );
				auto NdotV = m_writer.declLocale( "NdotV"
					, max( 0.0_f, dot( surface.worldNormal, V ) ) );
				indirectSpecular *= m_utils.fresnelSchlick( NdotV
					, indirectSpecular.xyz() );
			}

			return indirectSpecular;
		}

		sdw::Vec4 GlobalIllumination::traceConeRadiance( sdw::SampledImage3DRgba32 const & voxels
			, Surface surface
			, VoxelData const & voxelData )const
		{
			return m_traceConeRadiance( voxels
				, surface
				, voxelData );
		}

		void GlobalIllumination::declareTraceCone()
		{
			if ( m_traceCone )
			{
				return;
			}

			m_utils.declareIsSaturated();

			if ( !m_deferred )
			{
				m_utils.declareFresnelSchlick();
			}

			m_traceCone = m_writer.implementFunction< sdw::Vec4 >( "traceCone"
				, [&]( sdw::SampledImage3DRgba32 const & voxels
					, Surface surface
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
						, surface.worldPosition + surface.worldNormal * vec3( voxelData.gridToWorld * 2.0f * sqrt( 2.0f ) ) ); // sqrt2 is diagonal voxel half-extent

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
				, sdw::InSampledImage3DRgba32{ m_writer, "voxels" }
				, InSurface{ m_writer, "surface" }
				, sdw::InVec3{ m_writer, "wsConeDirection" }
				, sdw::InFloat{ m_writer, "coneAperture" }
				, InVoxelData{ m_writer, "voxelData" } );
		}

		void GlobalIllumination::declareTraceConeReflection()
		{
			m_traceConeReflection = m_writer.implementFunction< sdw::Vec4 >( "traceConeReflection"
				, [&]( sdw::SampledImage3DRgba32 const & voxels
					, Surface surface
					, sdw::Vec3 const & wsViewVector
					, sdw::Float const & roughness
					, shader::VoxelData const & voxelData )
				{
					auto aperture = m_writer.declLocale( "aperture"
						, tan( roughness * sdw::Float{ castor::PiDiv2< float > / 10 } ) );
					auto wsConeDirection = m_writer.declLocale( "wsConeDirection"
						, reflect( -wsViewVector, surface.worldNormal ) );

					auto reflection = m_writer.declLocale( "reflection"
						, m_traceCone( voxels
							, surface
							, wsConeDirection
							, aperture
							, voxelData ) );

					m_writer.returnStmt( vec4( max( vec3( 0.0_f ), reflection.rgb() )
						, clamp( reflection.a() * ( 1.0_f - roughness ), 0.0_f, 1.0_f ) ) );
				}
				, sdw::InSampledImage3DRgba32{ m_writer, "voxels" }
				, InSurface{ m_writer, "surface" }
				, sdw::InVec3{ m_writer, "wsViewVector" }
				, sdw::InFloat{ m_writer, "roughness" }
				, InVoxelData{ m_writer, "voxelData" } );
		}

		void GlobalIllumination::declareTraceConeOcclusion()
		{
			m_traceConeOcclusion = m_writer.implementFunction< sdw::Float >( "traceConeOcclusion"
				, [&]( sdw::SampledImage3DRgba32 const & voxels
					, Surface surface
					, sdw::Vec3 const & wsConeDirection
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
						, surface.worldPosition + surface.worldNormal * vec3( voxelData.gridToWorld * 2.0f * sqrt( 2.0f ) ) ); // sqrt2 is diagonal voxel half-extent

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
						occlusion += a * sam.a() * smoothStep( 0.0_f, voxelData.radianceMaxDistance, sqrt( wsDist ) );

						// step along ray:
						wsDist += wsDiameter * voxelData.rayStepSize;
					}
					ELIHW;

					m_writer.returnStmt( occlusion );
				}
				, sdw::InSampledImage3DRgba32{ m_writer, "voxels" }
				, InSurface{ m_writer, "surface" }
				, sdw::InVec3{ m_writer, "wsConeDirection" }
				, InVoxelData{ m_writer, "voxelData" } );
		}

		sdw::Vec4 GlobalIllumination::traceConeReflection( sdw::SampledImage3DRgba32 const & voxels
			, Surface surface
			, sdw::Vec3 const & wsViewVector
			, sdw::Float const & roughness
			, VoxelData const & voxelData )const
		{
			return m_traceConeReflection( voxels
				, surface
				, wsViewVector
				, roughness
				, voxelData );
		}

		sdw::Float GlobalIllumination::traceConeOcclusion( sdw::SampledImage3DRgba32 const & voxels
			, Surface surface
			, sdw::Vec3 const & wsConeDirection
			, VoxelData const & voxelData )const
		{
			return m_traceConeOcclusion( voxels
				, surface
				, wsConeDirection
				, voxelData );
		}
	}
}
