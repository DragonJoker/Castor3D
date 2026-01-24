#include "Castor3D/Shader/Shaders/GlslClusteredLights.hpp"

#include "Castor3D/Render/Clustered/ClustersConfig.hpp"
#include "Castor3D/Shader/Shaders/GlslDebugOutput.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"
#include "Castor3D/Shader/Ubos/ClustersUbo.hpp"

namespace c3d::shader
{
	//*********************************************************************************************

	namespace clusters
	{
		static bool constexpr C3D_UseWaveIntrinsics = false;

		static void printDebug( sdw::ShaderWriter & writer
			, ClustersData const & clusterData
			, DebugOutputCategory const & debugOutput
			, sdw::UVec3 const & clusterIndex3D
			, sdw::UInt const & pointLightCount
			, sdw::UInt const & spotLightCount )
		{
			debugOutput.registerOutput( cuT( "PointErrors" )
				, vec3( writer.ternary( pointLightCount > MaxLightsPerCluster, 1.0_f, 0.0_f ), 0.0_f, 0.0_f ) );
			debugOutput.registerOutput( cuT( "SpotErrors" )
				, vec3( writer.ternary( spotLightCount > MaxLightsPerCluster, 1.0_f, 0.0_f ), 0.0_f, 0.0_f ) );
			debugOutput.registerOutput( cuT( "ClusterIndex" )
				, vec3( writer.cast< sdw::Float >( clusterIndex3D.x() ) / writer.cast< sdw::Float >( clusterData.dimensions().x() - 1u )
					, writer.cast< sdw::Float >( clusterIndex3D.y() ) / writer.cast< sdw::Float >( clusterData.dimensions().y() - 1u )
					, writer.cast< sdw::Float >( clusterIndex3D.z() ) / writer.cast< sdw::Float >( clusterData.dimensions().z() - 1u ) ) );

			if ( debugOutput.isEnabled() )
			{
				auto I = vec3( 0.0_f, 0.0_f, 1.0_f );
				auto M = vec3( 0.0_f, 1.0_f, 0.0_f );
				auto E = vec3( 1.0_f, 0.0_f, 0.0_f );
				auto pointFactor = writer.declLocale( "pointFactor"
					, 2.0_f * writer.cast< sdw::Float >( pointLightCount ) / sdw::Float( float( MaxLightsPerCluster ) ) );
				auto spotFactor = writer.declLocale( "spotFactor"
					, 2.0_f * writer.cast< sdw::Float >( spotLightCount ) / sdw::Float( float( MaxLightsPerCluster ) ) );
				debugOutput.registerOutput( cuT( "PointLightsCount" )
					, writer.ternary( pointLightCount > 0u
						, writer.ternary( pointFactor > 1.0_f, mix( M, E, vec3( pointFactor - 1.0_f ) ), mix( I, M, vec3( pointFactor ) ) )
						, vec3( 0.0_f ) ) );
				debugOutput.registerOutput( cuT( "SpotLightsCount" )
					, writer.ternary( spotLightCount > 0u
						, writer.ternary( spotFactor > 1.0_f, mix( M, E, vec3( spotFactor - 1.0_f ) ), mix( I, M, vec3( spotFactor ) ) )
						, vec3( 0.0_f ) ) );
			}
		}

		static void computeLightingFastPath( sdw::ShaderWriter & writer
			, ClustersData const & clusterData
			, sdw::UInt32 const & clusterIndex1D
			, sdw::U32Vec3 const & clusterIndex3D
			, sdw::UInt32Array const & pointLightIndices
			, sdw::U32Vec2Array const & pointLightClusters
			, sdw::UInt32Array const & spotLightIndices
			, sdw::U32Vec2Array const & spotLightClusters
			, shader::Lights & lights
			, LightingModel & lightingModel
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::UInt const receivesShadows
			, DebugOutputCategory const & debugOutput
			, Function< void( PointLight const & ) > computePointLight
			, Function< void( SpotLight const & ) > computeSpotLight )
		{
			auto pointClusterLights = writer.declLocale( "pointClusterLights"
				, pointLightClusters[clusterIndex1D] );
			auto pointStartOffset = writer.declLocale( "pointStartOffset"
				, pointClusterLights.x() );
			auto pointLightCount = writer.declLocale( "pointLightCount"
				, pointClusterLights.y() );

			sdwFOR( writer, sdw::UInt, i, 0_u, i < pointLightCount, ++i )
			{
				auto lightIndex = writer.declLocale( "lightIndex"
					, pointLightIndices[pointStartOffset + i] );
				computePointLight( lights.retrievePointLight( lightIndex ) );
			}
			sdwROF

			auto spotClusterLights = writer.declLocale( "spotClusterLights"
				, spotLightClusters[clusterIndex1D] );
			auto spotStartOffset = writer.declLocale( "spotStartOffset"
				, spotClusterLights.x() );
			auto spotLightCount = writer.declLocale( "spotLightCount"
				, spotClusterLights.y() );

			sdwFOR( writer, sdw::UInt, i, 0_u, i < spotLightCount, ++i )
			{
				auto lightIndex = writer.declLocale( "lightIndex"
					, spotLightIndices[spotStartOffset + i] );
				computeSpotLight( lights.retrieveSpotLight( lightIndex ) );
			}
			sdwROF

			printDebug( writer, clusterData, debugOutput.pushBlock( cuT( "Clustered" ) )
				, clusterIndex3D, pointLightCount, spotLightCount );
		}

		static void computeLightingMediumPath( sdw::ShaderWriter & writer
			, ClustersData const & clusterData
			, sdw::UInt32 const & clusterIndex1D
			, sdw::U32Vec3 const & clusterIndex3D
			, sdw::UInt32Array const & pointLightIndices
			, sdw::U32Vec2Array const & pointLightClusters
			, sdw::UInt32Array const & spotLightIndices
			, sdw::U32Vec2Array const & spotLightClusters
			, shader::Lights & lights
			, LightingModel & lightingModel
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::UInt const receivesShadows
			, DebugOutputCategory const & debugOutput
			, Function< void( PointLight const & ) > computePointLight
			, Function< void( SpotLight const & ) > computeSpotLight )
		{
			auto pointClusterLights = writer.declLocale( "pointClusterLights"
				, pointLightClusters[clusterIndex1D] );
			auto pointStartOffset = writer.declLocale( "pointStartOffset"
				, pointClusterLights.x() );
			auto pointLightCount = writer.declLocale( "pointLightCount"
				, pointClusterLights.y() );

			sdwIF( writer, pointLightCount > 0_u )
			{
				auto lightOffset = writer.declLocale( "lightOffset"
					, 0_u );
				auto lightIndex = writer.declLocale( "lightIndex"
					, pointLightIndices[pointStartOffset] );

				sdwWHILE( writer, lightOffset < pointLightCount )
				{
					auto minLightIndex = writer.declLocale( "minLightIndex"
						, sdw::subgroupMin( lightIndex ) );

					sdwIF( writer, minLightIndex >= lightIndex )
					{
						lightOffset++;
						computePointLight( lights.retrievePointLight( minLightIndex ) );
						lightIndex = pointLightIndices[pointStartOffset + lightOffset];
					}
					sdwFI
				}
				sdwELIHW
			}
			sdwFI

			auto spotClusterLights = writer.declLocale( "spotClusterLights"
				, spotLightClusters[clusterIndex1D] );
			auto spotStartOffset = writer.declLocale( "spotStartOffset"
				, spotClusterLights.x() );
			auto spotLightCount = writer.declLocale( "spotLightCount"
				, spotClusterLights.y() );

			sdwIF( writer, spotLightCount > 0_u )
			{
				auto lightOffset = writer.declLocale( "lightOffset"
					, 0_u );
				auto lightIndex = writer.declLocale( "lightIndex"
					, spotLightIndices[spotStartOffset] );

				sdwWHILE( writer, lightOffset < spotLightCount )
				{
					auto minLightIndex = writer.declLocale( "minLightIndex"
						, sdw::subgroupMin( lightIndex ) );

					sdwIF( writer, minLightIndex >= lightIndex )
					{
						lightOffset++;
						computeSpotLight( lights.retrieveSpotLight( minLightIndex ) );
						lightIndex = spotLightIndices[spotStartOffset + lightOffset];
					}
					sdwFI
				}
				sdwELIHW
			}
			sdwFI

			printDebug( writer, clusterData, debugOutput.pushBlock( cuT( "Clustered" ) )
				, clusterIndex3D, pointLightCount, spotLightCount );
		}

		static void computeLighting( sdw::ShaderWriter & writer
			, ClustersData const & clusterData
			, sdw::UInt32 const & clusterIndex1D
			, sdw::U32Vec3 const & clusterIndex3D
			, sdw::UInt32Array const & pointLightIndices
			, sdw::U32Vec2Array const & pointLightClusters
			, sdw::UInt32Array const & spotLightIndices
			, sdw::U32Vec2Array const & spotLightClusters
			, shader::Lights & lights
			, LightingModel & lightingModel
			, BlendComponents const & components
			, LightSurface const & lightSurface
			, sdw::UInt const receivesShadows
			, DebugOutputCategory const & debugOutput
			, Function< void( PointLight const & ) > computePointLight
			, Function< void( SpotLight const & ) > computeSpotLight )
		{
			if constexpr ( C3D_UseWaveIntrinsics )
			{
				sdwIF( writer, clusterData.enableWaveIntrinsics() != 0_u )
				{
					auto firstClusterIndex1D = writer.declLocale( "firstClusterIndex1D"
						, sdw::readFirstInvocation( clusterIndex1D ) );
					auto laneMask = writer.declLocale( "laneMask"
						, sdw::subgroupBallot( clusterIndex1D == firstClusterIndex1D ) );
					auto fastPath = writer.declLocale( "fastPath"
						, all( laneMask == sdw::subgroupBallot( 1_b ) ) );

					sdwIF( writer, fastPath )
					{
						clusters::computeLightingFastPath( writer
							, clusterData
							, firstClusterIndex1D
							, clusterIndex3D
							, pointLightIndices
							, pointLightClusters
							, spotLightIndices
							, spotLightClusters
							, lights
							, lightingModel
							, components
							, lightSurface
							, receivesShadows
							, debugOutput
							, computePointLight
							, computeSpotLight );
					}
					sdwELSE
					{
						clusters::computeLightingMediumPath( writer
						, clusterData
							, clusterIndex1D
							, clusterIndex3D
							, pointLightIndices
							, pointLightClusters
							, spotLightIndices
							, spotLightClusters
							, lights
							, lightingModel
							, components
							, lightSurface
							, receivesShadows
							, debugOutput
							, computePointLight
							, computeSpotLight );
					}
					sdwFI
				}
				sdwELSE
				{
					clusters::computeLightingFastPath( writer
					, clusterData
						, clusterIndex1D
						, clusterIndex3D
						, pointLightIndices
						, pointLightClusters
						, spotLightIndices
						, spotLightClusters
						, lights
						, lightingModel
						, components
						, lightSurface
						, receivesShadows
						, debugOutput
						, computePointLight
						, computeSpotLight );
				}
				sdwFI
			}
			else
			{
				clusters::computeLightingFastPath( writer
					, clusterData
					, clusterIndex1D
					, clusterIndex3D
					, pointLightIndices
					, pointLightClusters
					, spotLightIndices
					, spotLightClusters
					, lights
					, lightingModel
					, components
					, lightSurface
					, receivesShadows
					, debugOutput
					, computePointLight
					, computeSpotLight );
			}
		}
	}

	//*********************************************************************************************

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
			, m_enabled );
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

		m_clusterData = makeUnique< shader::ClustersData >( c3d_clustersData );
		m_clustersLightsData = makeRawUnique< sdw::Vec4 >( c3d_clustersLightsData );
		m_pointLightIndices = makeRawUnique< sdw::UInt32Array >( c3d_pointLightClusterIndex );
		m_pointLightClusters = makeRawUnique< sdw::U32Vec2Array >( c3d_pointLightClusterGrid );
		m_spotLightIndices = makeRawUnique< sdw::UInt32Array >( c3d_spotLightClusterIndex );
		m_spotLightClusters = makeRawUnique< sdw::U32Vec2Array >( c3d_spotLightClusterGrid );
	}

	void ClusteredLights::computeCombinedDifSpec( shader::Lights & lights
		, LightingModel & lightingModel
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::UInt const receivesShadows
		, sdw::Vec2 const screenPosition
		, sdw::Float const viewDepth
		, DebugOutputCategory const & debugOutput
		, DirectLighting & output )
	{
		if ( !m_enabled )
		{
			return;
		}

		auto computePointLight = [&]( PointLight const & point )
			{
				lightingModel.compute( debugOutput
					, point
					, components
					, lightSurface
					, receivesShadows
					, output );
			};
		auto computeSpotLight = [&]( SpotLight const & spot )
			{
				lightingModel.compute( debugOutput
					, spot
					, components
					, lightSurface
					, receivesShadows
					, output );
			};
		auto clusterIndex3D = m_writer.declLocale( "clusterIndex3D"
			, m_clusterData->computeClusterIndex3D( screenPosition
				, viewDepth
				, *m_clustersLightsData ) );
		auto clusterIndex1D = m_writer.declLocale( "clusterIndex1D"
			, m_clusterData->computeClusterIndex1D( clusterIndex3D ) );
		clusters::computeLighting( m_writer
			, *m_clusterData
			, clusterIndex1D
			, clusterIndex3D
			, *m_pointLightIndices
			, *m_pointLightClusters
			, *m_spotLightIndices
			, *m_spotLightClusters
			, lights
			, lightingModel
			, components
			, lightSurface
			, receivesShadows
			, debugOutput
			, computePointLight
			, computeSpotLight );
	}

	void ClusteredLights::computeCombinedAllButDif( shader::Lights & lights
		, LightingModel & lightingModel
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::UInt const receivesShadows
		, sdw::Vec2 const screenPosition
		, sdw::Float const viewDepth
		, DebugOutputCategory const & debugOutput
		, sdw::Vec3 & diffuse
		, DirectLighting & output )
	{
		if ( !m_enabled )
		{
			return;
		}

		auto computePointLight = [&]( PointLight const & point )
			{
				lightingModel.computeAllButDiffuse( debugOutput
					, point
					, components
					, lightSurface
					, receivesShadows
					, diffuse
					, output );
				diffuse = vec3( 0.0_f );
			};
		auto computeSpotLight = [&]( SpotLight const & spot )
			{
				lightingModel.computeAllButDiffuse( debugOutput
					, spot
					, components
					, lightSurface
					, receivesShadows
					, diffuse
					, output );
				diffuse = vec3( 0.0_f );
			};
		auto clusterIndex3D = m_writer.declLocale( "clusterIndex3D"
			, m_clusterData->computeClusterIndex3D( screenPosition
				, viewDepth
				, *m_clustersLightsData ) );
		auto clusterIndex1D = m_writer.declLocale( "clusterIndex1D"
			, m_clusterData->computeClusterIndex1D( clusterIndex3D ) );
		clusters::computeLighting( m_writer
			, *m_clusterData
			, clusterIndex1D
			, clusterIndex3D
			, *m_pointLightIndices
			, *m_pointLightClusters
			, *m_spotLightIndices
			, *m_spotLightClusters
			, lights
			, lightingModel
			, components
			, lightSurface
			, receivesShadows
			, debugOutput
			, computePointLight
			, computeSpotLight );
	}

	void ClusteredLights::computeCombinedDif( Lights & lights
		, LightingModel & lightingModel
		, BlendComponents const & components
		, LightSurface const & lightSurface
		, sdw::UInt const receivesShadows
		, sdw::Vec2 const screenPosition
		, sdw::Float const viewDepth
		, DebugOutputCategory const & debugOutput
		, sdw::Vec3 & output )
	{
		if ( !m_enabled )
		{
			return;
		}

		auto computePointLight = [&]( PointLight const & point )
			{
				output += lightingModel.computeDiffuse( debugOutput
					, point
					, components
					, lightSurface
					, receivesShadows );
			};
		auto computeSpotLight = [&]( SpotLight const & spot )
			{
				output += lightingModel.computeDiffuse( debugOutput
					, spot
					, components
					, lightSurface
					, receivesShadows );
			};
		auto clusterIndex3D = m_writer.declLocale( "clusterIndex3D"
			, m_clusterData->computeClusterIndex3D( screenPosition
				, viewDepth
				, *m_clustersLightsData ) );
		auto clusterIndex1D = m_writer.declLocale( "clusterIndex1D"
			, m_clusterData->computeClusterIndex1D( clusterIndex3D ) );
		clusters::computeLighting( m_writer
			, *m_clusterData
			, clusterIndex1D
			, clusterIndex3D
			, *m_pointLightIndices
			, *m_pointLightClusters
			, *m_spotLightIndices
			, *m_spotLightClusters
			, lights
			, lightingModel
			, components
			, lightSurface
			, receivesShadows
			, debugOutput
			, computePointLight
			, computeSpotLight );
	}

	//*********************************************************************************************
}
