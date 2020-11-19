/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LayeredLpvGridConfigUbo_H___
#define ___C3D_LayeredLpvGridConfigUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <CastorUtils/Graphics/Grid.hpp>

namespace castor3d
{
	class LayeredLpvGridConfigUbo
	{
	public:
		using Configuration = LayeredLpvGridConfigUboConfiguration;

	public:
		C3D_API LayeredLpvGridConfigUbo( LayeredLpvGridConfigUbo const & ) = delete;
		C3D_API LayeredLpvGridConfigUbo & operator=( LayeredLpvGridConfigUbo const & ) = delete;
		C3D_API LayeredLpvGridConfigUbo( LayeredLpvGridConfigUbo && ) = default;
		C3D_API LayeredLpvGridConfigUbo & operator=( LayeredLpvGridConfigUbo && ) = delete;

		C3D_API explicit LayeredLpvGridConfigUbo();
		C3D_API explicit LayeredLpvGridConfigUbo( RenderDevice const & device );
		C3D_API ~LayeredLpvGridConfigUbo();

		C3D_API void initialise( RenderDevice const & device );
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Updates UBO data.
		 *\~french
		 *\brief		Met à jour les données de l'UBO.
		 */
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
		C3D_API static const uint32_t BindingPoint;
		C3D_API static const std::string LayeredLpvConfig;
		C3D_API static const std::string AllMinVolumeCorners;
		C3D_API static const std::string AllCellSizes;
		C3D_API static const std::string GridSizes;

	private:
		RenderDevice const * m_device{};
		UniformBufferOffsetT< Configuration > m_ubo{};
	};
}

#define UBO_LAYERED_LPVGRIDCONFIG( writer, binding, set, enabled )\
	sdw::Ubo layeredLpvConfig{ writer\
		, castor3d::LayeredLpvGridConfigUbo::LayeredLpvConfig\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140 };\
	auto c3d_allMinVolumeCorners = layeredLpvConfig.declMember< Vec4 >( castor3d::LayeredLpvGridConfigUbo::AllMinVolumeCorners, castor3d::shader::LpvMaxCascadesCount, enabled );\
	auto c3d_allCellSizes = layeredLpvConfig.declMember< Vec4 >( castor3d::LayeredLpvGridConfigUbo::AllCellSizes, enabled );\
	auto c3d_gridSizesAtt = layeredLpvConfig.declMember< Vec4 >( castor3d::LayeredLpvGridConfigUbo::GridSizes, enabled );\
	layeredLpvConfig.end();\
	auto c3d_gridSizes = c3d_gridSizesAtt.xyz();\
	auto c3d_indirectAttenuations = c3d_gridSizesAtt.w()

#endif
