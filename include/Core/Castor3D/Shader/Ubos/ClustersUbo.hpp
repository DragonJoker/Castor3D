/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ClustersUbo_H___
#define ___C3D_ClustersUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

namespace castor3d
{
	namespace shader
	{
		struct ClustersData
			: public sdw::StructInstanceHelperT< "C3D_ClustersData"
				, sdw::type::MemoryLayout::eStd140
				, sdw::U32Vec3Field< "dimensions" > // 3D grid dimensions
				, sdw::FloatField< "viewNear" > // Distance to the near clipping plane.
				, sdw::U32Vec2Field< "clusterSize" > // Screenspace size of a cluster
				, sdw::FloatField< "nearK" > // ( 1 + ( 2 * tan( fov * 0.5 ) / dimensions.y ) ) // Used to compute the near plane for clusters at depth k.
				, sdw::FloatField< "logGridDimY" > // 1.0f / log( 1 + ( tan( fov * 0.5 ) / dimensions.y )
				, sdw::UIntField< "pointLightLevels" >
				, sdw::UIntField< "spotLightLevels" >
				, sdw::UIntField< "pointLightCount" >
				, sdw::UIntField< "spotLightCount" > >
		{
			ClustersData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled )
				: StructInstanceHelperT{ writer, std::move( expr ), enabled }
			{
			}

			auto dimensions()const { return getMember< "dimensions" >(); }
			auto viewNear()const { return getMember< "viewNear" >(); }
			auto clusterSize()const { return getMember< "clusterSize" >(); }
			auto nearK()const { return getMember< "nearK" >(); }
			auto logGridDimY()const { return getMember< "logGridDimY" >(); }
			auto pointLightLevels()const { return getMember< "pointLightLevels" >(); }
			auto spotLightLevels()const { return getMember< "spotLightLevels" >(); }
			auto pointLightCount()const { return getMember< "pointLightCount" >(); }
			auto spotLightCount()const { return getMember< "spotLightCount" >(); }

			C3D_API sdw::RetU32Vec3 computeClusterIndex3D( sdw::UInt32 const index );
			C3D_API sdw::RetU32Vec3 computeClusterIndex3D( sdw::Vec2 const screenPos
				, sdw::Float viewZ );
			C3D_API sdw::RetUInt32 computeClusterIndex1D( sdw::U32Vec3 const clusterIndex3D );
		private:

			sdw::Function< sdw::U32Vec3
				, sdw::InUInt32 > m_computeClusterIndex3DIdx;
			sdw::Function< sdw::U32Vec3
				, sdw::InVec2
				, sdw::InFloat > m_computeClusterIndex3DPos;
			sdw::Function< sdw::UInt32
				, sdw::InU32Vec3 > m_computeClusterIndex1D;
		};
	}

	class ClustersUbo
	{
	public:
		using Configuration = ClustersUboConfiguration;
		C3D_API ClustersUbo( ClustersUbo const & rhs ) = delete;
		C3D_API ClustersUbo & operator=( ClustersUbo const & rhs ) = delete;
		C3D_API ClustersUbo( ClustersUbo && rhs ) = default;
		C3D_API ClustersUbo & operator=( ClustersUbo && rhs ) = delete;

		C3D_API explicit ClustersUbo( RenderDevice const & device );
		C3D_API ~ClustersUbo();

		C3D_API void cpuUpdate( castor::Point3ui gridDim
			, float viewNear
			, uint32_t clusterSize
			, float nearK
			, uint32_t pointLightsCount
			, uint32_t spotLightsCount );

		void createPassBinding( crg::FramePass & pass
			, uint32_t binding )const
		{
			return m_ubo.createPassBinding( pass, "C3D_Clusters", binding );
		}

		VkDescriptorSetLayoutBinding createLayoutBinding( uint32_t index
			, VkShaderStageFlags stages )const
		{
			return castor3d::makeDescriptorSetLayoutBinding( index
				, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
				, stages );
		}

		void createSizedBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & layoutBinding )const
		{
			return m_ubo.createSizedBinding( descriptorSet, layoutBinding );
		}

		ashes::WriteDescriptorSet getDescriptorWrite( uint32_t dstBinding
			, uint32_t dstArrayElement = 0u )const
		{
			return m_ubo.getDescriptorWrite( dstBinding, dstArrayElement );
		}

		UniformBufferOffsetT< Configuration > const & getUbo()const
		{
			return m_ubo;
		}

	private:
		RenderDevice const & m_device;
		UniformBufferOffsetT< Configuration > m_ubo;
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
	auto c3d_clustersData = clusters.declMember< castor3d::shader::ClustersData >( "c", enabled ); \
	clusters.end()

#define C3D_Clusters( writer, binding, set ) \
	C3D_ClustersEx( writer, binding, set, true )

#endif
