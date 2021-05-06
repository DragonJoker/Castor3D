/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LayeredLpvGridConfigUbo_H___
#define ___C3D_LayeredLpvGridConfigUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <CastorUtils/Graphics/Grid.hpp>

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/BaseTypes/Array.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

namespace castor3d
{
	namespace shader
	{
		struct LayeredLpvGridData
			: public sdw::StructInstance
		{
			C3D_API LayeredLpvGridData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			C3D_API LayeredLpvGridData & operator=( LayeredLpvGridData const & rhs );

			C3D_API static ast::type::StructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			// Raw values
			sdw::Array< sdw::Vec4 > allMinVolumeCorners;
			sdw::Vec4 allCellSizes;
			sdw::Vec4 gridSizesAtt;
			// Specific values
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
		C3D_API explicit LayeredLpvGridConfigUbo( RenderDevice const & device );
		C3D_API ~LayeredLpvGridConfigUbo();

		C3D_API void cpuUpdate( std::array< castor::Grid, shader::LpvMaxCascadesCount > const & grids
			, float indirectAttenuation );

		void createSizedBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & layoutBinding )const
		{
			return m_ubo.createSizedBinding( descriptorSet, layoutBinding );
		}

		UniformBufferOffsetT< Configuration > const & getUbo()const
		{
			return m_ubo;
		}

	public:
		C3D_API static const std::string LayeredLpvConfig;
		C3D_API static const std::string LayeredLpvGridData;

	private:
		RenderDevice const & m_device;
		UniformBufferOffsetT< Configuration > m_ubo{};
	};
}

#define UBO_LAYERED_LPVGRIDCONFIG( writer, binding, set, enabled )\
	sdw::Ubo layeredLpvConfig{ writer\
		, castor3d::LayeredLpvGridConfigUbo::LayeredLpvConfig\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140 };\
	auto c3d_llpvGridData = layeredLpvConfig.declStructMember< castor3d::shader::LayeredLpvGridData >( castor3d::LayeredLpvGridConfigUbo::LayeredLpvGridData, enabled );\
	layeredLpvConfig.end()

#endif
