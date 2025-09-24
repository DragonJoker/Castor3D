/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LpvGridConfigUbo_H___
#define ___C3D_LpvGridConfigUbo_H___

#include "UbosModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <CastorUtils/Graphics/GraphicsModule.hpp>
#include <CastorUtils/Graphics/Grid.hpp>

#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

namespace c3d
{
	namespace shader
	{
		struct LpvGridData
			: sdw::StructInstanceHelperT< "C3D_LpvGridData"
			, sdw::type::MemoryLayout::eStd140
			, sdw::Vec3Field< "minVolumeCorner" >
			, sdw::FloatField< "cellSize" >
			, sdw::Vec3Field< "gridSize" >
			, sdw::FloatField< "indirectAttenuation" >
			, sdw::Vec3Field< "cameraPosition" >
			, sdw::FloatField< "pad0" > >
		{
			SDW_DeclStructInstance( C3D_API, LpvGridData );

			LpvGridData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled )
				: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
				, minVolumeCorner{ StructInstanceHelperT::getMember< "minVolumeCorner" >() }
				, gridSize{ StructInstanceHelperT::getMember< "gridSize" >() }
				, cameraPos{ StructInstanceHelperT::getMember< "cameraPosition" >() }
				, gridWidth{ gridSize.x() }
				, gridHeight{ gridSize.y() }
				, gridDepth{ gridSize.z() }
				, m_cellSize{ StructInstanceHelperT::getMember< "cellSize" >() }
				, m_indirectAttenuation{ StructInstanceHelperT::getMember< "indirectAttenuation" >() }
			{
			}

			C3D_API sdw::IVec3 worldToGrid( sdw::Vec3 const & pos )const;
			C3D_API sdw::IVec3 worldToGrid( sdw::Vec3 const & pos
				, sdw::Vec3 const & nml )const;
			C3D_API sdw::Vec3 worldToTex( sdw::Vec3 const & pos )const;
			C3D_API sdw::Vec2 gridToScreen( sdw::IVec2 const & pos )const;
			C3D_API sdw::Vec3 nextGrid( sdw::IVec3 const & pos
				, sdw::Vec3 const & dir )const;

			sdw::Float const & cellSize()const
			{
				return m_cellSize;
			}

			sdw::Float const & indirectAttenuation()const
			{
				return m_indirectAttenuation;
			}

		public:
			sdw::Vec3 minVolumeCorner;
			sdw::Vec3 gridSize;
			sdw::Vec3 cameraPos;
			sdw::Float gridWidth;
			sdw::Float gridHeight;
			sdw::Float gridDepth;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;

			sdw::Float m_cellSize;
			sdw::Float m_indirectAttenuation;
		};
	}

	class LpvGridConfigUbo
	{
	public:
		using Configuration = LpvGridConfigUboConfiguration;

	public:
		C3D_API LpvGridConfigUbo( LpvGridConfigUbo const & rhs ) = delete;
		C3D_API LpvGridConfigUbo & operator=( LpvGridConfigUbo const & rhs ) = delete;
		C3D_API LpvGridConfigUbo( LpvGridConfigUbo && rhs )noexcept = default;
		C3D_API LpvGridConfigUbo & operator=( LpvGridConfigUbo && rhs )noexcept = delete;
		C3D_API explicit LpvGridConfigUbo( RenderDevice const & device );
		C3D_API ~LpvGridConfigUbo()noexcept;

		C3D_API Grid const & cpuUpdate( BoundingBox const & aabb
			, Point3f const & cameraPos
			, uint32_t gridDim
			, float indirectAttenuation );
		C3D_API Grid const & cpuUpdate( float gridLevelScale
			, Grid const & grid
			, Point3f const & cameraPos
			, Point3f const & cameraDir
			, float indirectAttenuation );

		UniformBufferOffsetT< Configuration > & getUbo()
		{
			return m_ubo;
		}

		UniformBufferOffsetT< Configuration > const & getUbo()const
		{
			return m_ubo;
		}

		void createPassBinding( crg::FramePass & pass
			, uint32_t binding )const
		{
			return m_ubo.createPassBinding( pass, binding );
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

		void addDescriptorWrite( ashes::WriteDescriptorSetArray & descriptorWrites
			, uint32_t & dstBinding
			, uint32_t dstArrayElement = 0u )const
		{
			descriptorWrites.emplace_back( getDescriptorWrite( dstBinding, dstArrayElement ) );
			++dstBinding;
		}

	private:
		RenderDevice const & m_device;
		UniformBufferOffsetT< Configuration > m_ubo;
		Grid m_grid;
	};
}

#define C3D_LpvGridConfig( writer, binding, set, enabled )\
	sdw::UniformBuffer lpvGridConfig{ writer\
		, "C3D_LpvGridConfig"\
		, "c3d_lpvGridConfig"\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd140 };\
	auto c3d_lpvGridData = lpvGridConfig.declMember< c3d::shader::LpvGridData >( "c3d_lpvGridData", enabled );\
	lpvGridConfig.end()

#endif
