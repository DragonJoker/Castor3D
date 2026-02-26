/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslLightsBVH_H___
#define ___C3D_GlslLightsBVH_H___

#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Shader/Shaders/GlslAABB.hpp"
#include "Castor3D/Shader/Shaders/GlslAppendBuffer.hpp"
#include "Castor3D/Shader/Shaders/GlslBVHBase.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"

namespace c3d::shader
{
	template< typename BVHContT >
	struct LightsBVHT
	{
		LightsBVHT( sdw::ShaderWriter & writer
			, sdw::Array< AABB > const & allLightsAABB
			, u32 maxLightsInList )
			: m_writer{ writer }
			, m_allLightsAABB{ allLightsAABB }
			, m_maxLightsInList{ maxLightsInList }
			, m_sharedLightStartOffset{ m_writer.declSharedVariable< sdw::UInt >( "gsLightStartOffset" ) }
			, m_sharedLights{ m_writer.declSharedVariable< AppendArrayT< sdw::UInt > >( "gsLights", true, "U32", m_maxLightsInList ) }
		{
		}

		void resetTraversal()
		{
			m_sharedLights.resetCount();
		}

		void traversePointLights( sdw::Array< sdw::UInt > const & pointLightsIndices
			, sdw::Array< AABB > const & pointLightBVH
			, BVHBaseT< BVHContT > & bvh
			, BVHContT const & currentCont
			, sdw::UInt const & pointLightLevels
			, sdw::UInt const & pointLightCount
			, sdw::UInt const & groupIndex )
		{
			auto childOffset = m_writer.declLocale( "childOffsetPoint", groupIndex );

			sdwDOWHILE( m_writer, bvh.hasParentNode() )
			{
				auto childIndex = m_writer.declLocale( "childIndex"
					, bvh.getFirstChild( pointLightLevels ) + childOffset );

				sdwIF( m_writer, bvh.isLeafNode( childIndex, pointLightLevels ) )
				{
					auto leafIndex = m_writer.declLocale( "leafIndex"
						, bvh.getLeafIndex( childIndex, pointLightLevels ) );

					sdwIF( m_writer, leafIndex < pointLightCount )
					{
						doProcessPointLightAABB( pointLightsIndices, leafIndex, currentCont );
					}
					sdwFI
				}
				sdwELSEIF( currentCont.intersectAABB( pointLightBVH[childIndex] ) )
				{
					bvh.pushNode( childIndex );
				}
				sdwFI

				groupMemoryBarrierWithGroupSync( m_writer );

				bvh.popChild( groupIndex );

				groupMemoryBarrierWithGroupSync( m_writer );
			}
			sdwELIHWOD
		}

		void traverseSpotLights( sdw::Array< sdw::UInt > const & spotLightsIndices
			, LightsBuffer & lights
			, CameraData const & cameraData
			, sdw::Array< AABB > const & spotLightBVH
			, BVHBaseT< BVHContT > & bvh
			, BVHContT const & currentCont
			, sdw::UInt const & pointLightCount
			, sdw::UInt const & spotLightLevels
			, sdw::UInt const & spotLightCount
			, sdw::UInt const & groupIndex )
		{
			auto childOffset = m_writer.declLocale( "childOffsetSpot", groupIndex );

			sdwDOWHILE( m_writer, bvh.hasParentNode() )
			{
				auto childIndex = m_writer.declLocale( "childIndex"
					, bvh.getFirstChild( spotLightLevels ) + childOffset );

				sdwIF( m_writer, bvh.isLeafNode( childIndex, spotLightLevels ) )
				{
					auto leafIndex = m_writer.declLocale( "leafIndex"
						, bvh.getLeafIndex( childIndex, spotLightLevels ) );

					sdwIF( m_writer, leafIndex < spotLightCount )
					{
						doProcessSpotLightAABB( spotLightsIndices, lights, cameraData, leafIndex, pointLightCount, currentCont );
					}
					sdwFI
				}
				sdwELSEIF( currentCont.intersectAABB( spotLightBVH[childIndex] ) )
				{
					bvh.pushNode( childIndex );
				}
				sdwFI

				groupMemoryBarrierWithGroupSync( m_writer );

				bvh.popChild( groupIndex );

				groupMemoryBarrierWithGroupSync( m_writer );
			}
			sdwELIHWOD
		}

		sdw::UInt getTraversedLightCount()const
		{
			return m_sharedLights.getCount();
		}

		sdw::UInt getTraversedLightID( sdw::UInt const & index )const
		{
			return m_sharedLights[index];
		}

	private:
		void doProcessPointLightAABB( sdw::Array< sdw::UInt > const & pointLightsIndices
			, sdw::UInt const & leafIndex
			, BVHContT const & pcurrentCont )
		{
			if ( !m_processPointLightAABB )
				m_processPointLightAABB = m_writer.implementFunction< sdw::Void >( "c3d_processPointLightAABB"
					, [this]( sdw::UInt const & lightIndex
						, BVHContT const & currentCont )
					{
						auto aabb = m_writer.declLocale( "aabb"
							, m_allLightsAABB[lightIndex] );
						auto center = m_writer.declLocale( "center"
							, aabb.min().xyz() + ( aabb.max().xyz() - aabb.min().xyz() ) / 2.0_f );

						sdwIF( m_writer, currentCont.intersectSphere( vec4( center, aabb.min().w() ) ) )
						{
							m_sharedLights.appendData( lightIndex, m_maxLightsInList );
						}
						sdwFI
					}
					, sdw::InUInt{ m_writer, "lightIndex" }
					, sdw::InParam< BVHContT >{ m_writer, "currentCont" } );
			m_processPointLightAABB( pointLightsIndices[leafIndex], pcurrentCont );
		}

		void doProcessSpotLightAABB( sdw::Array< sdw::UInt > const & spotLightsIndices
			, LightsBuffer & lights
			, CameraData const & cameraData
			, sdw::UInt const & leafIndex
			, sdw::UInt const & ppointLightCount
			, BVHContT const & pcurrentCont )
		{
			if ( !m_processSpotLightAABB )
				m_processSpotLightAABB = m_writer.implementFunction< sdw::Void >( "c3d_processSpotLightAABB"
					, [this, &lights, &cameraData]( sdw::UInt const & lightIndex
						, sdw::UInt const & pointLightCount
						, BVHContT const & currentCont )
					{
						auto aabb = m_writer.declLocale( "aabb"
							, m_allLightsAABB[pointLightCount + lightIndex] );

						sdwIF( m_writer, currentCont.intersectAABBCoarse( aabb ) )
						{
							auto spot = m_writer.declLocale( "spot"
								, lights.getSpotLight( lights.getPointsEnd() + lightIndex * SpotLightInstance::LightDataComponents ) );
							auto cone = m_writer.declLocale( "cone"
								, Cone{ cameraData.worldToCurView( vec4( spot.position(), 1.0_f ) ).xyz()
									, cameraData.worldToCurView( -spot.direction() )
									, computeRange( spot )
									, spot.outerCutOffCos()
									, spot.outerCutOffSin()
									, spot.outerCutOffTan() } );

							sdwIF( m_writer, currentCont.intersectCone( cone ) )
							{
								m_sharedLights.appendData( lightIndex, m_maxLightsInList );
							}
							sdwFI
						}
						sdwFI
					}
					, sdw::InUInt{ m_writer, "lightIndex" }
					, sdw::InUInt{ m_writer, "pointLightCount" }
					, sdw::InParam< BVHContT >{ m_writer, "currentCont" } );
			m_processSpotLightAABB( spotLightsIndices[leafIndex], ppointLightCount, pcurrentCont );
		}

	private:
		sdw::ShaderWriter & m_writer;
		sdw::Array< AABB > const & m_allLightsAABB;
		u32 m_maxLightsInList;

		sdw::Int m_sharedLightStartOffset;
		AppendArrayT< sdw::UInt > m_sharedLights;

		mutable sdw::Function< sdw::Void, sdw::InUInt, sdw::InParam< BVHContT > > m_processPointLightAABB;
		mutable sdw::Function< sdw::Void, sdw::InUInt, sdw::InUInt, sdw::InParam< BVHContT > > m_processSpotLightAABB;
	};
}

#endif
