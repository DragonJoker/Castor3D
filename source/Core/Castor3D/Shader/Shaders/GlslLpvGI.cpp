#include "Castor3D/Shader/Shaders/GlslLpvGI.hpp"

#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.hpp"

#include <CastorUtils/Math/Angle.hpp>

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

		void LpvGI::declareLayered( uint32_t bindingIndex )
		{
			using namespace sdw;
			auto c3d_lpvAccumulator1R = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eR, "Accumulator1" ), bindingIndex++, 0u );
			auto c3d_lpvAccumulator1G = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eG, "Accumulator1" ), bindingIndex++, 0u );
			auto c3d_lpvAccumulator1B = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eB, "Accumulator1" ), bindingIndex++, 0u );
			auto c3d_lpvAccumulator2R = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eR, "Accumulator2" ), bindingIndex++, 0u );
			auto c3d_lpvAccumulator2G = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eG, "Accumulator2" ), bindingIndex++, 0u );
			auto c3d_lpvAccumulator2B = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eB, "Accumulator2" ), bindingIndex++, 0u );
			auto c3d_lpvAccumulator3R = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eR, "Accumulator3" ), bindingIndex++, 0u );
			auto c3d_lpvAccumulator3G = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eG, "Accumulator3" ), bindingIndex++, 0u );
			auto c3d_lpvAccumulator3B = m_writer.declSampledImage< FImg3DRgba16 >( getTextureName( LpvTexture::eB, "Accumulator3" ), bindingIndex++, 0u );

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
					, Array< Vec4 > allMinVolumeCorners
					, Vec4 cellSizes
					, Vec3 gridSize )
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
						, ( wsPosition - allMinVolumeCorners[0].xyz() ) / cellSizes.x() / gridSize );
					auto lpvCellCoords2 = m_writer.declLocale( "lpvCellCoords2"
						, ( wsPosition - allMinVolumeCorners[1].xyz() ) / cellSizes.y() / gridSize );
					auto lpvCellCoords3 = m_writer.declLocale( "lpvCellCoords3"
						, ( wsPosition - allMinVolumeCorners[2].xyz() ) / cellSizes.z() / gridSize );

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
				, InParam< Array< Vec4 > >{ m_writer, "allMinVolumeCorners", shader::LpvMaxCascadesCount }
				, InVec4{ m_writer, "cellSizes" }
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

		sdw::Vec3 LpvGI::computeLLPVRadiance( sdw::Vec3 wsPosition
			, sdw::Vec3 wsNormal
			, sdw::Array< sdw::Vec4 > allMinVolumeCorners
			, sdw::Vec4 allCellSizes
			, sdw::Vec3 gridSizes )
		{
			CU_Require( m_computeLLPVRadiance );
			return m_computeLLPVRadiance( wsPosition
				, wsNormal
				, allMinVolumeCorners
				, allCellSizes
				, gridSizes );
		}

		sdw::Vec3 LpvGI::computeResult( SceneFlags sceneFlags
			, sdw::Vec3 wsPosition
			, sdw::Vec3 wsNormal
			, sdw::Float indirectAttenuation
			, sdw::Vec3 minVolumeCorners
			, sdw::Float cellSize
			, sdw::Vec3 gridSize
			, sdw::Array< sdw::Vec4 > allMinVolumeCorners
			, sdw::Vec4 allCellSizes
			, sdw::Vec3 gridSizes
			, sdw::Vec3 diffuse
			, sdw::Vec3 allButAmbient
			, sdw::Vec3 ambient )
		{
			if ( checkFlag( sceneFlags, SceneFlag::eLayeredLpvGI ) )
			{
				auto indirect = m_writer.declLocale( "indirect"
					, computeLLPVRadiance( wsPosition
						, wsNormal
						, allMinVolumeCorners
						, allCellSizes
						, gridSizes ) );
				return ( ( indirect * diffuse * ambient / Float{ castor::Pi< float > } )
					+ ( indirect * indirectAttenuation / Float{ castor::Pi< float > } )
					+ allButAmbient );
			}

			if ( checkFlag( sceneFlags, SceneFlag::eLpvGI ) )
			{
				auto indirect = m_writer.declLocale( "indirect"
					, computeLPVRadiance( wsPosition
						, wsNormal
						, minVolumeCorners
						, cellSize
						, gridSize ) );
				return ( ( indirect * diffuse * ambient / Float{ castor::Pi< float > } )
					+ ( indirect * indirectAttenuation / Float{ castor::Pi< float > } )
					+ allButAmbient );
			}

			return allButAmbient + ambient;
		}
	}
}
