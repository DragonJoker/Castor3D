/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LayeredLpvGridConfigUbo_H___
#define ___C3D_LayeredLpvGridConfigUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <CastorUtils/Graphics/Grid.hpp>

#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/BaseTypes/Array.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

namespace c3d
{
	namespace shader
	{
		struct LayeredLpvGridData
			: sdw::StructInstanceHelperT< "C3D_LayeredLpvGridData"
				, sdw::type::MemoryLayout::eStd140
				, sdw::Vec4ArrayField< "allMinVolumeCorners", LpvMaxCascadesCount >
				, sdw::Vec4Field< "allCellSizes" >
				, sdw::Vec3Field< "gridSizes" >
				, sdw::FloatField< "indirectAttenuation" > >
		{
			SDW_DeclStructInstance( C3D_API, LayeredLpvGridData );

			LayeredLpvGridData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled )
				: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
				, allMinVolumeCorners{ StructInstanceHelperT::getMember< "allMinVolumeCorners" >() }
				, allCellSizes{ StructInstanceHelperT::getMember< "allCellSizes" >() }
				, gridSizes{ StructInstanceHelperT::getMember< "gridSizes" >() }
				, indirectAttenuation{ StructInstanceHelperT::getMember< "indirectAttenuation" >() }
			{
			}

			sdw::Array< sdw::Vec4 > allMinVolumeCorners;
			sdw::Vec4 allCellSizes;
			sdw::Vec3 gridSizes;
			sdw::Float indirectAttenuation;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;
		};
	}

	class LayeredLpvGridConfigUbo
	{
	public:
		using Configuration = LayeredLpvGridConfigUboConfiguration;

	public:
		C3D_API LayeredLpvGridConfigUbo( LayeredLpvGridConfigUbo const & rhs ) = delete;
		C3D_API LayeredLpvGridConfigUbo & operator=( LayeredLpvGridConfigUbo const & rhs ) = delete;
		C3D_API LayeredLpvGridConfigUbo( LayeredLpvGridConfigUbo && rhs )noexcept = delete;
		C3D_API LayeredLpvGridConfigUbo & operator=( LayeredLpvGridConfigUbo && rhs )noexcept = delete;
		C3D_API explicit LayeredLpvGridConfigUbo( RenderDevice const & device );
		C3D_API ~LayeredLpvGridConfigUbo()noexcept;

		C3D_API void cpuUpdate( Array< Grid const *, LpvMaxCascadesCount > const & grids
			, float indirectAttenuation );

		void createPassBinding( crg::FramePass & pass
			, uint32_t dstBinding )const
		{
			return m_ubo.createPassBinding( pass, dstBinding );
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

		UniformBufferOffsetT< Configuration > const & getUbo()const
		{
			return m_ubo;
		}

	private:
		RenderDevice const & m_device;
		UniformBufferOffsetT< Configuration > m_ubo{};
	};
}

#define C3D_LayeredLpvGridConfig( writer, binding, set, enabled )\
	sdw::UniformBuffer layeredLpvConfig{ writer\
		, "C3D_LayeredLpvConfig"\
		, "c3d_layeredLpvConfig"\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd140 };\
	auto c3d_llpvGridData = layeredLpvConfig.declMember< c3d::shader::LayeredLpvGridData >( "c3d_llpvGridData", enabled );\
	layeredLpvConfig.end()

#endif
