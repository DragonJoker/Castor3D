#include "Castor3D/Shader/Shaders/GlslLpvGI.hpp"

#include <ShaderWriter/Source.hpp>

using namespace sdw;

namespace castor3d
{
	namespace shader
	{
		LpvGI::LpvGI( sdw::ShaderWriter & writer )
			: m_writer{ writer }
		{
			using namespace sdw;

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
					, Vec3 gridSize
					, SampledImage3DRgba16 lpvAccumulatorR
					, SampledImage3DRgba16 lpvAccumulatorG
					, SampledImage3DRgba16 lpvAccumulatorB )
				{
					auto SHintensity = m_writer.declLocale( "SHintensity"
						, m_evalSH( -wsNormal ) );
					auto lpvCellCoords = m_writer.declLocale( "lpvCellCoords"
						, ( wsPosition - minVolumeCorners ) / cellSize / gridSize );
					auto lpvIntensity = m_writer.declLocale( "lpvIntensity"
						, vec3( dot( SHintensity, lpvAccumulatorR.sample( lpvCellCoords ) )
							, dot( SHintensity, lpvAccumulatorG.sample( lpvCellCoords ) )
							, dot( SHintensity, lpvAccumulatorB.sample( lpvCellCoords ) ) ) );
					m_writer.returnStmt( max( lpvIntensity, vec3( 0.0_f ) ) );
				}
				, InVec3{ m_writer, "wsPosition" }
				, InVec3{ m_writer, "wsNormal" }
				, InVec3{ m_writer, "minVolumeCorners" }
				, InFloat{ m_writer, "cellSize" }
				, InVec3{ m_writer, "gridSize" }
				, InSampledImage3DRgba16{ m_writer, "lpvAccumulatorR" }
				, InSampledImage3DRgba16{ m_writer, "lpvAccumulatorG" }
				, InSampledImage3DRgba16{ m_writer, "lpvAccumulatorB" } );

		}

		sdw::Vec3 LpvGI::computeLPVRadiance( sdw::Vec3 wsPosition
			, sdw::Vec3 wsNormal
			, sdw::Vec3 minVolumeCorners
			, sdw::Float cellSize
			, sdw::Vec3 gridSize
			, sdw::SampledImage3DRgba16 lpvAccumulatorR
			, sdw::SampledImage3DRgba16 lpvAccumulatorG
			, sdw::SampledImage3DRgba16 lpvAccumulatorB )
		{
			return m_computeLPVRadiance( wsPosition
				, wsNormal
				, minVolumeCorners
				, cellSize
				, gridSize
				, lpvAccumulatorR
				, lpvAccumulatorG
				, lpvAccumulatorB );
		}
	}
}
