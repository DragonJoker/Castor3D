/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LpvLightConfigUbo_H___
#define ___C3D_LpvLightConfigUbo_H___

#include "UbosModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <CastorUtils/Graphics/GraphicsModule.hpp>

namespace castor3d
{
	class LpvLightConfigUbo
	{
	public:
		using Configuration = LpvLightConfigUboConfiguration;

	public:
		C3D_API LpvLightConfigUbo( LpvLightConfigUbo const & ) = delete;
		C3D_API LpvLightConfigUbo & operator=( LpvLightConfigUbo const & ) = delete;
		C3D_API LpvLightConfigUbo( LpvLightConfigUbo && ) = default;
		C3D_API LpvLightConfigUbo & operator=( LpvLightConfigUbo && ) = delete;

		C3D_API explicit LpvLightConfigUbo();
		C3D_API explicit LpvLightConfigUbo( RenderDevice const & device );
		C3D_API ~LpvLightConfigUbo();

		C3D_API void initialise( RenderDevice const & device );
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 */
		C3D_API void cpuUpdate( Light const & light );
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 */
		C3D_API void cpuUpdate( DirectionalLight const & light
			, uint32_t cascadeIndex );

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
		C3D_API static const std::string LpvLightConfig;
		C3D_API static const std::string LightView;
		C3D_API static const std::string Config;

	private:
		RenderDevice const * m_device{};
		UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define UBO_LPVLIGHTCONFIG( writer, binding, set )\
	sdw::Ubo lpvLightConfig{ writer\
		, castor3d::LpvLightConfigUbo::LpvLightConfig\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140 };\
	auto c3d_lightView = lpvLightConfig.declMember< Mat4 >( castor3d::LpvLightConfigUbo::LightView );\
	auto c3d_lpvLightConfig = lpvLightConfig.declMember< Vec4 >( castor3d::LpvLightConfigUbo::Config );\
	lpvLightConfig.end();\
	auto c3d_lpvTexelAreaModifier = c3d_lpvLightConfig.x();\
	auto c3d_lpvTanFovXHalf = c3d_lpvLightConfig.y();\
	auto c3d_lpvTanFovYHalf = c3d_lpvLightConfig.z();\
	auto c3d_lightIndex = writer.cast< sdw::Int >( c3d_lpvLightConfig.w() )

#endif
