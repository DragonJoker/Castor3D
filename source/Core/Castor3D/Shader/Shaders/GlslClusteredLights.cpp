#include "Castor3D/Shader/Shaders/GlslClusteredLights.hpp"

#include "Castor3D/Render/Clustered/ClustersConfig.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Shaders/GlslDebugOutput.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Ubos/ClustersUbo.hpp"

#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	ClusteredLights::ClusteredLights( sdw::ShaderWriter & writer
		, uint32_t & binding
		, uint32_t set
		, ClustersConfig const * config
		, bool enabled )
		: m_writer{ writer }
		, m_enabled{ enabled && config && config->enabled }
	{
		C3D_ClustersEx( writer
			, binding++
			, set
			, m_enabled
			, config );
		C3D_ReducedLightsAABBEx( writer
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
		m_clustersLightsData = std::make_unique< sdw::Vec4 >( c3d_clustersLightsData );
		m_pointLightIndices = std::make_unique< sdw::UInt32Array >( c3d_pointLightClusterIndex );
		m_pointLightClusters = std::make_unique< sdw::U32Vec2Array >( c3d_pointLightClusterGrid );
		m_spotLightIndices = std::make_unique< sdw::UInt32Array >( c3d_spotLightClusterIndex );
		m_spotLightClusters = std::make_unique< sdw::U32Vec2Array >( c3d_spotLightClusterGrid );
	}

	void ClusteredLights::computeCombinedDifSpec( shader::Lights & lights
		, LightingModel & lightingModel
		, BlendComponents const & components
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
			, m_clusterData->computeClusterIndex3D( screenPosition
				, viewDepth
				, *m_clustersLightsData ) );
		auto clusterIndex1D = m_writer.declLocale( "clusterIndex1D"
			, m_clusterData->computeClusterIndex1D( clusterIndex3D ) );
		auto firstClusterIndex1D = m_writer.declLocale( "firstClusterIndex1D"
			, sdw::readFirstInvocation( clusterIndex1D ) );
		auto laneMask = m_writer.declLocale( "laneMask"
			, sdw::subgroupBallot( clusterIndex1D == firstClusterIndex1D ) );
		auto fastPath = m_writer.declLocale( "fastPath"
			, all( laneMask == sdw::subgroupBallot( 1_b ) ) );

		IF( m_writer, fastPath )
		{
			auto pointStartOffset = m_writer.declLocale( "pointStartOffset"
				, ( *m_pointLightClusters )[firstClusterIndex1D].x() );
			auto pointLightCount = m_writer.declLocale( "pointLightCount"
				, ( *m_pointLightClusters )[firstClusterIndex1D].y() );

			FOR( m_writer, sdw::UInt, i, 0_u, i < pointLightCount, ++i )
			{
				auto lightOffset = m_writer.declLocale( "lightOffset"
					, lights.getDirectionalsEnd() + ( *m_pointLightIndices )[pointStartOffset + i] * castor3d::PointLight::LightDataComponents );
				auto light = m_writer.declLocale( "point", lights.getPointLight( lightOffset ) );
				lightingModel.compute( light
					, components
					, lightSurface
					, receivesShadows
					, output );
			}
			ROF;

			auto spotStartOffset = m_writer.declLocale( "spotStartOffset"
				, ( *m_spotLightClusters )[firstClusterIndex1D].x() );
			auto spotLightCount = m_writer.declLocale( "spotLightCount"
				, ( *m_spotLightClusters )[firstClusterIndex1D].y() );

			FOR( m_writer, sdw::UInt, i, 0_u, i < spotLightCount, ++i )
			{
				auto lightOffset = m_writer.declLocale( "lightOffset"
					, lights.getPointsEnd() + ( *m_spotLightIndices )[spotStartOffset + i] * castor3d::SpotLight::LightDataComponents );
				auto light = m_writer.declLocale( "spot", lights.getSpotLight( lightOffset ) );
				lightingModel.compute( light
					, components
					, lightSurface
					, receivesShadows
					, output );
			}
			ROF;

			doPrintDebug( debugOutput, clusterIndex3D, pointLightCount, spotLightCount );
		}
		ELSE
		{
			auto pointStartOffset = m_writer.declLocale( "pointStartOffset"
				, ( *m_pointLightClusters )[clusterIndex1D].x() );
			auto pointLightCount = m_writer.declLocale( "pointLightCount"
				, ( *m_pointLightClusters )[clusterIndex1D].y() );
			auto pointLightOffset = m_writer.declLocale( "pointLightOffset"
				, 0_u );

			WHILE( m_writer, pointLightOffset < pointLightCount )
			{
				auto lightOffset = m_writer.declLocale( "lightOffset"
					, lights.getDirectionalsEnd() + ( *m_pointLightIndices )[pointStartOffset + pointLightOffset] * castor3d::PointLight::LightDataComponents );
				auto minLightOffset = m_writer.declLocale( "minLightOffset"
					, sdw::subgroupMin( lightOffset ) );

				IF( m_writer, minLightOffset >= lightOffset )
				{
					pointLightOffset++;
					auto light = m_writer.declLocale( "point", lights.getPointLight( minLightOffset ) );
					lightingModel.compute( light
						, components
						, lightSurface
						, receivesShadows
						, output );
				}
				FI;
			}
			ELIHW;

			auto spotStartOffset = m_writer.declLocale( "spotStartOffset"
				, ( *m_spotLightClusters )[clusterIndex1D].x() );
			auto spotLightCount = m_writer.declLocale( "spotLightCount"
				, ( *m_spotLightClusters )[clusterIndex1D].y() );
			auto spotLightOffset = m_writer.declLocale( "spotLightOffset"
				, 0_u );

			WHILE( m_writer, spotLightOffset < spotLightCount )
			{
				auto lightOffset = m_writer.declLocale( "lightOffset"
					, lights.getPointsEnd() + ( *m_spotLightIndices )[spotStartOffset + spotLightOffset] * castor3d::SpotLight::LightDataComponents );
				auto minLightOffset = m_writer.declLocale( "minLightOffset"
					, sdw::subgroupMin( lightOffset ) );

				IF( m_writer, minLightOffset >= lightOffset )
				{
					spotLightOffset++;
					auto light = m_writer.declLocale( "spot", lights.getSpotLight( minLightOffset ) );
					lightingModel.compute( light
						, components
						, lightSurface
						, receivesShadows
						, output );
				}
				FI;
			}
			ELIHW;

			doPrintDebug( debugOutput, clusterIndex3D, pointLightCount, spotLightCount );
		}
		FI;
	}

	void ClusteredLights::computeCombinedAllButDif( shader::Lights & lights
		, LightingModel & lightingModel
		, BlendComponents const & components
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
			, m_clusterData->computeClusterIndex3D( screenPosition
				, viewDepth
				, *m_clustersLightsData ) );
		auto clusterIndex1D = m_writer.declLocale( "clusterIndex1D"
			, m_clusterData->computeClusterIndex1D( clusterIndex3D ) );
		auto firstClusterIndex1D = m_writer.declLocale( "firstClusterIndex1D"
			, sdw::readFirstInvocation( clusterIndex1D ) );
		auto laneMask = m_writer.declLocale( "laneMask"
			, sdw::subgroupBallot( clusterIndex1D == firstClusterIndex1D ) );
		auto fastPath = m_writer.declLocale( "fastPath"
			, all( laneMask == sdw::subgroupBallot( 1_b ) ) );

		IF( m_writer, fastPath )
		{
			auto pointStartOffset = m_writer.declLocale( "pointStartOffset"
				, ( *m_pointLightClusters )[firstClusterIndex1D].x() );
			auto pointLightCount = m_writer.declLocale( "pointLightCount"
				, ( *m_pointLightClusters )[firstClusterIndex1D].y() );

			FOR( m_writer, sdw::UInt, i, 0_u, i < pointLightCount, ++i )
			{
				auto lightOffset = m_writer.declLocale( "lightOffset"
					, lights.getDirectionalsEnd() + ( *m_pointLightIndices )[pointStartOffset + i] * castor3d::PointLight::LightDataComponents );
				auto light = m_writer.declLocale( "point", lights.getPointLight( lightOffset ) );
				lightingModel.computeAllButDiffuse( light
					, components
					, lightSurface
					, receivesShadows
					, output );
			}
			ROF;

			auto spotStartOffset = m_writer.declLocale( "spotStartOffset"
				, ( *m_spotLightClusters )[firstClusterIndex1D].x() );
			auto spotLightCount = m_writer.declLocale( "spotLightCount"
				, ( *m_spotLightClusters )[firstClusterIndex1D].y() );

			FOR( m_writer, sdw::UInt, i, 0_u, i < spotLightCount, ++i )
			{
				auto lightOffset = m_writer.declLocale( "lightOffset"
					, lights.getPointsEnd() + ( *m_spotLightIndices )[spotStartOffset + i] * castor3d::SpotLight::LightDataComponents );
				auto light = m_writer.declLocale( "spot", lights.getSpotLight( lightOffset ) );
				lightingModel.computeAllButDiffuse( light
					, components
					, lightSurface
					, receivesShadows
					, output );
			}
			ROF;

			doPrintDebug( debugOutput, clusterIndex3D, pointLightCount, spotLightCount );
		}
		ELSE
		{
			auto pointStartOffset = m_writer.declLocale( "pointStartOffset"
				, ( *m_pointLightClusters )[clusterIndex1D].x() );
			auto pointLightCount = m_writer.declLocale( "pointLightCount"
				, ( *m_pointLightClusters )[clusterIndex1D].y() );
			auto pointLightOffset = m_writer.declLocale( "pointLightOffset"
				, 0_u );

			WHILE( m_writer, pointLightOffset < pointLightCount )
			{
				auto lightOffset = m_writer.declLocale( "lightOffset"
					, lights.getDirectionalsEnd() + ( *m_pointLightIndices )[pointStartOffset + pointLightOffset] * castor3d::PointLight::LightDataComponents );
				auto minLightOffset = m_writer.declLocale( "minLightOffset"
					, sdw::subgroupMin( lightOffset ) );

				IF( m_writer, minLightOffset >= lightOffset )
				{
					pointLightOffset++;
					auto light = m_writer.declLocale( "point", lights.getPointLight( minLightOffset ) );
					lightingModel.computeAllButDiffuse( light
						, components
						, lightSurface
						, receivesShadows
						, output );
				}
				FI;
			}
			ELIHW;

			auto spotStartOffset = m_writer.declLocale( "spotStartOffset"
				, ( *m_spotLightClusters )[clusterIndex1D].x() );
			auto spotLightCount = m_writer.declLocale( "spotLightCount"
				, ( *m_spotLightClusters )[clusterIndex1D].y() );
			auto spotLightOffset = m_writer.declLocale( "spotLightOffset"
				, 0_u );

			WHILE( m_writer, spotLightOffset < spotLightCount )
			{
				auto lightOffset = m_writer.declLocale( "lightOffset"
					, lights.getPointsEnd() + ( *m_spotLightIndices )[spotStartOffset + spotLightOffset] * castor3d::SpotLight::LightDataComponents );
				auto minLightOffset = m_writer.declLocale( "minLightOffset"
					, sdw::subgroupMin( lightOffset ) );

				IF( m_writer, minLightOffset >= lightOffset )
				{
					spotLightOffset++;
					auto light = m_writer.declLocale( "spot", lights.getSpotLight( minLightOffset ) );
					lightingModel.computeAllButDiffuse( light
						, components
						, lightSurface
						, receivesShadows
						, output );
				}
				FI;
			}
			ELIHW;

			doPrintDebug( debugOutput, clusterIndex3D, pointLightCount, spotLightCount );
		}
		FI;
	}

	void ClusteredLights::computeCombinedDif( Lights & lights
		, LightingModel & lightingModel
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::UInt const receivesShadows
		, sdw::Vec2 const screenPosition
		, sdw::Float const viewDepth
		, sdw::Vec3 & output )
	{
		if ( !m_enabled )
		{
			return;
		}

		auto clusterIndex3D = m_writer.declLocale( "clusterIndex3D"
			, m_clusterData->computeClusterIndex3D( screenPosition
				, viewDepth
				, *m_clustersLightsData ) );
		auto clusterIndex1D = m_writer.declLocale( "clusterIndex1D"
			, m_clusterData->computeClusterIndex1D( clusterIndex3D ) );
		auto firstClusterIndex1D = m_writer.declLocale( "firstClusterIndex1D"
			, sdw::readFirstInvocation( clusterIndex1D ) );
		auto laneMask = m_writer.declLocale( "laneMask"
			, sdw::subgroupBallot( clusterIndex1D == firstClusterIndex1D ) );
		auto fastPath = m_writer.declLocale( "fastPath"
			, all( laneMask == sdw::subgroupBallot( 1_b ) ) );

		IF( m_writer, fastPath )
		{
			auto pointStartOffset = m_writer.declLocale( "pointStartOffset"
				, ( *m_pointLightClusters )[firstClusterIndex1D].x() );
			auto pointLightCount = m_writer.declLocale( "pointLightCount"
				, ( *m_pointLightClusters )[firstClusterIndex1D].y() );

			FOR( m_writer, sdw::UInt, i, 0_u, i < pointLightCount, ++i )
			{
				auto lightOffset = m_writer.declLocale( "lightOffset"
					, lights.getDirectionalsEnd() + ( *m_pointLightIndices )[pointStartOffset + i] * castor3d::PointLight::LightDataComponents );
				auto light = m_writer.declLocale( "point", lights.getPointLight( lightOffset ) );
				output += lightingModel.computeDiffuse( light
					, components
					, lightSurface
					, receivesShadows );
			}
			ROF;

			auto spotStartOffset = m_writer.declLocale( "spotStartOffset"
				, ( *m_spotLightClusters )[firstClusterIndex1D].x() );
			auto spotLightCount = m_writer.declLocale( "spotLightCount"
				, ( *m_spotLightClusters )[firstClusterIndex1D].y() );

			FOR( m_writer, sdw::UInt, i, 0_u, i < spotLightCount, ++i )
			{
				auto lightOffset = m_writer.declLocale( "lightOffset"
					, lights.getPointsEnd() + ( *m_spotLightIndices )[spotStartOffset + i] * castor3d::SpotLight::LightDataComponents );
				auto light = m_writer.declLocale( "spot", lights.getSpotLight( lightOffset ) );
				output += lightingModel.computeDiffuse( light
					, components
					, lightSurface
					, receivesShadows );
			}
			ROF;
		}
		ELSE
		{
			auto pointStartOffset = m_writer.declLocale( "pointStartOffset"
				, ( *m_pointLightClusters )[clusterIndex1D].x() );
			auto pointLightCount = m_writer.declLocale( "pointLightCount"
				, ( *m_pointLightClusters )[clusterIndex1D].y() );
			auto pointLightOffset = m_writer.declLocale( "pointLightOffset"
				, 0_u );

			WHILE( m_writer, pointLightOffset < pointLightCount )
			{
				auto lightOffset = m_writer.declLocale( "lightOffset"
					, lights.getDirectionalsEnd() + ( *m_pointLightIndices )[pointStartOffset + pointLightOffset] * castor3d::PointLight::LightDataComponents );
				auto minLightOffset = m_writer.declLocale( "minLightOffset"
					, sdw::subgroupMin( lightOffset ) );

				IF( m_writer, minLightOffset >= lightOffset )
				{
					pointLightOffset++;
					auto light = m_writer.declLocale( "point", lights.getPointLight( minLightOffset ) );
					output += lightingModel.computeDiffuse( light
						, components
						, lightSurface
						, receivesShadows );
				}
				FI;
			}
			ELIHW;

			auto spotStartOffset = m_writer.declLocale( "spotStartOffset"
				, ( *m_spotLightClusters )[clusterIndex1D].x() );
			auto spotLightCount = m_writer.declLocale( "spotLightCount"
				, ( *m_spotLightClusters )[clusterIndex1D].y() );
			auto spotLightOffset = m_writer.declLocale( "spotLightOffset"
				, 0_u );

			WHILE( m_writer, spotLightOffset < spotLightCount )
			{
				auto lightOffset = m_writer.declLocale( "lightOffset"
					, lights.getPointsEnd() + ( *m_spotLightIndices )[spotStartOffset + spotLightOffset] * castor3d::SpotLight::LightDataComponents );
				auto minLightOffset = m_writer.declLocale( "minLightOffset"
					, sdw::subgroupMin( lightOffset ) );

				IF( m_writer, minLightOffset >= lightOffset )
				{
					spotLightOffset++;
					auto light = m_writer.declLocale( "spot", lights.getSpotLight( minLightOffset ) );
					output += lightingModel.computeDiffuse( light
						, components
						, lightSurface
						, receivesShadows );
				}
				FI;
			}
			ELIHW;
		}
		FI;
	}

	void ClusteredLights::doPrintDebug( DebugOutput & debugOutput
		, sdw::UVec3 const & clusterIndex3D
		, sdw::UInt const & pointLightCount
		, sdw::UInt const & spotLightCount )
	{
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
