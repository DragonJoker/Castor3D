/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ClustersUbo_H___
#define ___C3D_ClustersUbo_H___

#include "Castor3D/Render/Clustered/ClusteredModule.hpp"

#include "Castor3D/Shader/Shaders/GlslAABB.hpp"
#include "Castor3D/Shader/Ubos/Ubo.hpp"

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

namespace c3d
{
	namespace shader
	{
		struct ClustersData
			: public sdw::StructInstanceHelperT< "C3D_ClustersData"
				, sdw::type::MemoryLayout::eStd140
				, sdw::U32Vec3Field< "dimensions" >
				, sdw::UIntField< "splitScheme" >
				, sdw::U32Vec2Field< "clusterSize" >
				, sdw::Vec2Field< "viewNearFar" >
				, sdw::UIntField< "pointLightLevels" >
				, sdw::UIntField< "spotLightLevels" >
				, sdw::UIntField< "pointLightCount" >
				, sdw::UIntField< "spotLightCount" >
				, sdw::FloatField< "minDistance" >
				, sdw::UIntField< "enableWaveIntrinsics" >
				, sdw::FloatField< "pad1" >
				, sdw::FloatField< "pad2" > >
		{
			SDW_DeclStructInstance( C3D_API, ClustersData );

			ClustersData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled )
				: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
			{
			}

			auto dimensions()const { return getMember< "dimensions" >(); }
			auto clusterSize()const { return getMember< "clusterSize" >(); }
			auto viewNear()const { return getMember< "viewNearFar" >().x(); }
			auto viewFar()const { return getMember< "viewNearFar" >().y(); }
			auto pointLightLevels()const { return getMember< "pointLightLevels" >(); }
			auto spotLightLevels()const { return getMember< "spotLightLevels" >(); }
			auto pointLightCount()const { return getMember< "pointLightCount" >(); }
			auto spotLightCount()const { return getMember< "spotLightCount" >(); }
			auto splitScheme()const { return getMember< "splitScheme" >(); }
			auto minDistance()const { return getMember< "minDistance" >(); }
			auto enableWaveIntrinsics()const { return getMember< "enableWaveIntrinsics" >(); }

			C3D_API sdw::RetU32Vec3 computeClusterIndex3D( sdw::UInt32 const index );
			C3D_API sdw::RetU32Vec3 computeClusterIndex3D( sdw::Vec2 const screenPos
				, sdw::Float viewZ
				, sdw::Vec4 const clustersLightsData );
			C3D_API sdw::UInt32 computeClusterIndex1D( sdw::U32Vec3 const clusterIndex3D );
			C3D_API sdw::RetVec2 getClusterDepthBounds( sdw::U32Vec3 const clusterIndex3D
				, sdw::Vec4 const clustersLightsData
				, sdw::Vec4 const lightsAABBRange );
			C3D_API sdw::RetVoid computeGlobalLightsData( sdw::Vec4 const lightsMin
				, sdw::Vec4 const lightsMax
				, sdw::Float const nearPlane
				, sdw::Float const farPlane
				, sdw::Vec4 & clustersLightsData
				, sdw::Vec4 & lightsAABBRange );

		private:
			sdw::Function< sdw::U32Vec3
				, sdw::InUInt32 > m_computeClusterIndex3DIdx;
			sdw::Function< sdw::U32Vec3
				, sdw::InVec2
				, sdw::InFloat
				, sdw::InVec4 > m_computeClusterIndex3DPos;
			sdw::Function< sdw::UInt32
				, sdw::InU32Vec3 > m_computeClusterIndex1D;
			sdw::Function< sdw::Vec2
				, sdw::InU32Vec3
				, sdw::InVec4
				, sdw::InVec4 > m_getClusterDepthBounds;
			sdw::Function< sdw::Void
				, sdw::PVec4
				, sdw::PVec4
				, sdw::InFloat
				, sdw::InFloat
				, sdw::OutVec4
				, sdw::OutVec4 > m_computeGlobalLightsData;
		};
	}

	class ClustersUbo
		: public UboT< ClustersUboConfiguration >
	{
	public:
		using Configuration = ClustersUboConfiguration;

	public:
		C3D_API explicit ClustersUbo( RenderDevice const & device );

		C3D_API void cpuUpdate( Point3ui gridDim
			, Point2ui clusterSize
			, float viewNear
			, float viewFar
			, uint32_t pointLightsCount
			, uint32_t spotLightsCount
			, ClusterSplitScheme splitScheme
			, float minDistance
			, bool enableWaveIntrinsics );
	};
}

#define C3D_ClustersEx( writer, binding, set, enabled ) \
	sdw::UniformBuffer clusters{ writer \
		, "C3D_Clusters" \
		, "c3d_clusters" \
		, ( enabled ? uint32_t( binding ) : 0u ) \
		, uint32_t( set ) \
		, ast::type::MemoryLayout::eStd140 \
		, enabled }; \
	auto c3d_clustersData = clusters.declMember< c3d::shader::ClustersData >( "c", enabled ); \
	clusters.end()

#define C3D_Clusters( writer, binding, set ) \
	C3D_ClustersEx( writer, binding, set, true )

#endif
