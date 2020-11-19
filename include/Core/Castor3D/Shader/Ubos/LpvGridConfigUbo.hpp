/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LpvGridConfigUbo_H___
#define ___C3D_LpvGridConfigUbo_H___

#include "UbosModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <CastorUtils/Graphics/GraphicsModule.hpp>

namespace castor3d
{
	class LpvGridConfigUbo
	{
	public:
		using Configuration = LpvGridConfigUboConfiguration;

	public:
		C3D_API LpvGridConfigUbo( LpvGridConfigUbo const & ) = delete;
		C3D_API LpvGridConfigUbo & operator=( LpvGridConfigUbo const & ) = delete;
		C3D_API LpvGridConfigUbo( LpvGridConfigUbo && ) = default;
		C3D_API LpvGridConfigUbo & operator=( LpvGridConfigUbo && ) = delete;

		C3D_API explicit LpvGridConfigUbo();
		C3D_API explicit LpvGridConfigUbo( RenderDevice const & device );
		C3D_API ~LpvGridConfigUbo();

		C3D_API void initialise( RenderDevice const & device );
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 */
		C3D_API void cpuUpdate( castor::BoundingBox const & aabb
			, castor::Point3f const & cameraPos
			, castor::Point3f const & cameraDir
			, uint32_t gridDim
			, float indirectAttenuation );
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 */
		C3D_API castor::Grid cpuUpdate( uint32_t gridLevel
			, float gridLevelScale
			, castor::Grid const & grid
			, castor::BoundingBox const & aabb
			, castor::Point3f const & cameraPos
			, castor::Point3f const & cameraDir
			, float indirectAttenuation );

		UniformBufferOffsetT< Configuration > & getUbo()
		{
			return m_ubo;
		}

		UniformBufferOffsetT< Configuration > const & getUbo()const
		{
			return m_ubo;
		}

		void createSizedBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & layoutBinding )const
		{
			return m_ubo.createSizedBinding( descriptorSet, layoutBinding );
		}

	public:
		C3D_API static const uint32_t BindingPoint;
		C3D_API static const std::string LpvGridConfig;
		C3D_API static const std::string MinVolumeCornerSize;
		C3D_API static const std::string GridSizes;

	private:
		RenderDevice const * m_device{};
		UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define UBO_LPVGRIDCONFIG( writer, binding, set, enabled )\
	sdw::Ubo lpvGridConfig{ writer\
		, castor3d::LpvGridConfigUbo::LpvGridConfig\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140 };\
	auto c3d_minVolumeCornerSize = lpvGridConfig.declMember< Vec4 >( castor3d::LpvGridConfigUbo::MinVolumeCornerSize, enabled );\
	auto c3d_gridSizeAtt = lpvGridConfig.declMember< Vec4 >( castor3d::LpvGridConfigUbo::GridSizes, enabled );\
	lpvGridConfig.end();\
	auto c3d_minVolumeCorner = c3d_minVolumeCornerSize.xyz();\
	auto c3d_cellSize = c3d_minVolumeCornerSize.w();\
	auto c3d_gridWidth = c3d_gridSizeAtt.x();\
	auto c3d_gridHeight = c3d_gridSizeAtt.y();\
	auto c3d_gridDepth = c3d_gridSizeAtt.z();\
	auto c3d_gridSize = c3d_gridSizeAtt.xyz();\
	auto c3d_indirectAttenuation = c3d_gridSizeAtt.w()

#endif
