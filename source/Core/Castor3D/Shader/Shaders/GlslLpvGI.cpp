#include "Castor3D/Shader/Shaders/GlslLpvGI.hpp"

#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.hpp"

#include <ShaderWriter/Source.hpp>

using namespace sdw;

namespace castor3d
{
	namespace shader
	{
		LpvGI::LpvGI( sdw::ShaderWriter & writer )
			: m_writer{ writer }
		{
		}

		void LpvGI::declare( uint32_t bindingIndex )
		{
			declare( 0u, bindingIndex, SceneFlag::eLpvGI );
		}

		void LpvGI::declare( uint32_t setIndex
			, uint32_t & bindingIndex
			, SceneFlags sceneFlags )
		{
			using namespace sdw;

			if ( !checkFlag( sceneFlags, SceneFlag::eLpvGI )
				&& !checkFlag( sceneFlags, SceneFlag::eLayeredLpvGI ) )
			{
				return;
			}

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
					, Vec3 minVolumeCorners
					, Float cellSize
					, Vec3 gridSize )
				{
					auto c3d_lpvAccumulatorR = m_writer.getVariable< SampledImage3DRgba16 >( getTextureName( LpvTexture::eR, "Accumulator" ) );
					auto c3d_lpvAccumulatorG = m_writer.getVariable< SampledImage3DRgba16 >( getTextureName( LpvTexture::eG, "Accumulator" ) );
					auto c3d_lpvAccumulatorB = m_writer.getVariable< SampledImage3DRgba16 >( getTextureName( LpvTexture::eB, "Accumulator" ) );

					auto SHintensity = m_writer.declLocale( "SHintensity"
						, m_evalSH( -wsNormal ) );
					auto lpvCellCoords = m_writer.declLocale( "lpvCellCoords"
						, ( wsPosition - minVolumeCorners ) / cellSize / gridSize );
					auto lpvIntensity = m_writer.declLocale( "lpvIntensity"
						, vec3( dot( SHintensity, c3d_lpvAccumulatorR.sample( lpvCellCoords ) )
							, dot( SHintensity, c3d_lpvAccumulatorG.sample( lpvCellCoords ) )
							, dot( SHintensity, c3d_lpvAccumulatorB.sample( lpvCellCoords ) ) ) );
					m_writer.returnStmt( max( lpvIntensity, vec3( 0.0_f ) ) );
				}
				, InVec3{ m_writer, "wsPosition" }
				, InVec3{ m_writer, "wsNormal" }
				, InVec3{ m_writer, "minVolumeCorners" }
				, InFloat{ m_writer, "cellSize" }
				, InVec3{ m_writer, "gridSize" } );
		}

		sdw::Vec3 LpvGI::computeLPVRadiance( sdw::Vec3 wsPosition
			, sdw::Vec3 wsNormal
			, sdw::Vec3 minVolumeCorners
			, sdw::Float cellSize
			, sdw::Vec3 gridSize )
		{
			CU_Require( m_computeLPVRadiance );
			return m_computeLPVRadiance( wsPosition
				, wsNormal
				, minVolumeCorners
				, cellSize
				, gridSize );
		}

		sdw::Vec3 LpvGI::computeResult( SceneFlags sceneFlags
			, sdw::Vec3 wsPosition
			, sdw::Vec3 wsNormal
			, sdw::Vec3 minVolumeCorners
			, sdw::Float cellSize
			, sdw::Vec3 gridSize
			, sdw::Array< sdw::Vec4 > allMinVolumeCorners
			, sdw::Vec4 allCellSizes
			, sdw::UVec4 gridSizes
			, sdw::Vec3 allButAmbient
			, sdw::Vec3 ambient )
		{
			if ( checkFlag( sceneFlags, SceneFlag::eLayeredLpvGI ) )
			{
				return fma( computeLPVRadiance( wsPosition
						, wsNormal
						// l3 is the finest
						, allMinVolumeCorners[3].xyz()
						, allCellSizes.w()
						, vec3( gridSizes.xyz() ) )
					, ambient
					, allButAmbient );
			}

			if ( checkFlag( sceneFlags, SceneFlag::eLpvGI ) )
			{
				return fma( computeLPVRadiance( wsPosition
						, wsNormal
						, minVolumeCorners
						, cellSize
						, gridSize )
					, ambient
					, allButAmbient );
			}

			return allButAmbient + ambient;
		}
	}
}
