#include "Castor3D/Shader/Shaders/GlslClusteredLights.hpp"

#include "Castor3D/Shader/Shaders/GlslDebugOutput.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"
#include "Castor3D/Shader/Ubos/ClustersUbo.hpp"

#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	ClusteredLights::ClusteredLights( sdw::ShaderWriter & writer
		, uint32_t & binding
		, uint32_t set
		, bool enabled )
		: m_writer{ writer }
		, m_enabled{ enabled }
	{
		C3D_ClustersEx( writer
			, binding++
			, set
			, m_enabled );
		C3D_PointLightClusterIndexEx( writer
			, binding++
			, set
			, m_enabled );
		C3D_PointLightClusterGridEx( writer
			, binding++
			, set
			, m_enabled );
		C3D_SpotLightClusterIndexEx( writer
			, binding++
			, set
			, m_enabled );
		C3D_SpotLightClusterGridEx( writer
			, binding++
			, set
			, m_enabled );

		m_clusterData = castor::makeUnique< shader::ClustersData >( c3d_clustersData );
		m_pointLightIndices = std::make_unique< sdw::UInt32Array >( c3d_pointLightClusterIndex );
		m_pointLightClusters = std::make_unique< sdw::U32Vec2Array >( c3d_pointLightClusterGrid );
		m_spotLightIndices = std::make_unique< sdw::UInt32Array >( c3d_spotLightClusterIndex );
		m_spotLightClusters = std::make_unique< sdw::U32Vec2Array >( c3d_spotLightClusterGrid );
	}

	void ClusteredLights::computeCombinedDifSpec( shader::Lights & lights
		, LightingModel & lightingModel
		, BlendComponents const & components
		, BackgroundModel & background
		, LightSurface const & lightSurface
		, sdw::UInt const receivesShadows
		, sdw::Vec2 const screenPosition
		, sdw::Float const viewDepth
		, DebugOutput & debugOutput
		, OutputComponents & output )
	{
		if ( !m_enabled )
		{
			return;
		}

		auto clusterIndex3D = m_writer.declLocale( "clusterIndex3D"
			, m_clusterData->computeClusterIndex3D( screenPosition, viewDepth ) );
		auto clusterIndex1D = m_writer.declLocale( "clusterIndex1D"
			, m_clusterData->computeClusterIndex1D( clusterIndex3D ) );

		auto pointStartOffset = m_writer.declLocale( "pointStartOffset"
			, ( *m_pointLightClusters )[clusterIndex1D].x() );
		auto pointLightCount = m_writer.declLocale( "pointLightCount"
			, ( *m_pointLightClusters )[clusterIndex1D].y() );

		FOR( m_writer, sdw::UInt, i, 0_u, i < pointLightCount, ++i )
		{
			auto lightOffset = m_writer.declLocale( "lightOffset"
				, ( *m_pointLightIndices )[pointStartOffset + i] );
			auto light = m_writer.declLocale( "point", lights.getPointLight( lightOffset ) );
			lightingModel.compute( light
				, components
				, lightSurface
				, receivesShadows
				, output );
		}
		ROF;

		auto spotStartOffset = m_writer.declLocale( "spotStartOffset"
			, ( *m_spotLightClusters )[clusterIndex1D].x() );
		auto spotLightCount = m_writer.declLocale( "spotLightCount"
			, ( *m_spotLightClusters )[clusterIndex1D].y() );

		FOR( m_writer, sdw::UInt, i, 0_u, i < spotLightCount, ++i )
		{
			auto lightOffset = m_writer.declLocale( "lightOffset"
				, ( *m_spotLightIndices )[spotStartOffset + i] );
			auto light = m_writer.declLocale( "spot", lights.getSpotLight( lightOffset ) );
			lightingModel.compute( light
				, components
				, lightSurface
				, receivesShadows
				, output );
		}
		ROF;

		debugOutput.registerOutput( "Clustered"
			, "PointErrors"
			, vec3( m_writer.ternary( pointLightCount > MaxLightsPerCluster, 1.0_f, 0.0_f ), 0.0_f, 0.0_f ) );
		debugOutput.registerOutput( "Clustered"
			, "SpotErrors"
			, vec3( m_writer.ternary( spotLightCount > MaxLightsPerCluster, 1.0_f, 0.0_f ), 0.0_f, 0.0_f ) );
		debugOutput.registerOutput( "Clustered"
			, "ClusterIndex"
			, vec3( m_writer.cast< sdw::Float >( clusterIndex3D.x() ) / m_writer.cast< sdw::Float >( m_clusterData->dimensions().x() - 1u )
				, m_writer.cast< sdw::Float >( clusterIndex3D.y() ) / m_writer.cast< sdw::Float >( m_clusterData->dimensions().y() - 1u )
				, m_writer.cast< sdw::Float >( clusterIndex3D.z() ) / m_writer.cast< sdw::Float >( m_clusterData->dimensions().z() - 1u ) ) );

		if ( debugOutput.isEnabled() )
		{
			auto I = vec3( 0.0_f, 0.0_f, 1.0_f );
			auto M = vec3( 0.0_f, 1.0_f, 0.0_f );
			auto E = vec3( 1.0_f, 0.0_f, 0.0_f );
			auto pointFactor = m_writer.declLocale( "pointFactor"
				, 2.0_f * m_writer.cast< sdw::Float >( pointLightCount ) / sdw::Float( float( MaxLightsPerCluster ) ) );
			auto spotFactor = m_writer.declLocale( "spotFactor"
				, 2.0_f * m_writer.cast< sdw::Float >( spotLightCount ) / sdw::Float( float( MaxLightsPerCluster ) ) );
			debugOutput.registerOutput( "Clustered"
				, "PointLightsCount"
				, m_writer.ternary( pointLightCount > 0u
					, m_writer.ternary( pointFactor > 1.0_f, mix( M, E, vec3( pointFactor - 1.0_f ) ), mix( I, M, vec3( pointFactor ) ) )
					, vec3( 0.0_f ) ) );
			debugOutput.registerOutput( "Clustered"
				, "SpotLightsCount"
				, m_writer.ternary( spotLightCount > 0u
					, m_writer.ternary( spotFactor > 1.0_f, mix( M, E, vec3( spotFactor - 1.0_f ) ), mix( I, M, vec3( spotFactor ) ) )
					, vec3( 0.0_f ) ) );
		}
	}
}
