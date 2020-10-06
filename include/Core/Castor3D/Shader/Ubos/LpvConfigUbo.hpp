/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LpvConfigUbo_H___
#define ___C3D_LpvConfigUbo_H___

#include "UbosModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <CastorUtils/Graphics/GraphicsModule.hpp>

namespace castor3d
{
	class LpvConfigUbo
	{
	public:
		using Configuration = LpvConfiguration;

	public:
		/**
		 *\~english
		 *\name			Copy/Move construction/assignment operation.
		 *\~french
		 *\name			Constructeurs/Opérateurs d'affectation par copie/déplacement.
		 */
		/**@{*/
		C3D_API LpvConfigUbo( LpvConfigUbo const & ) = delete;
		C3D_API LpvConfigUbo & operator=( LpvConfigUbo const & ) = delete;
		C3D_API LpvConfigUbo( LpvConfigUbo && ) = default;
		C3D_API LpvConfigUbo & operator=( LpvConfigUbo && ) = delete;
		/**@}*/
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		explicit LpvConfigUbo( RenderDevice const & device
			, uint32_t index = 0u );
		/**@}*/
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~LpvConfigUbo();
		/**
		 *\~english
		 *\brief		Updates UBO data.
		 *\~french
		 *\brief		Met à jour les données de l'UBO.
		 */
		C3D_API void cpuUpdate( Configuration const & config );

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
		C3D_API static const std::string LpvConfig;
		C3D_API static const std::string LightView;
		C3D_API static const std::string MinVolumeCorner;
		C3D_API static const std::string GridSizes;
		C3D_API static const std::string Config;

	private:
		RenderDevice const & m_device;
		UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define UBO_LPVCONFIG( writer, binding, set )\
	sdw::Ubo lpvConfig{ writer\
		, castor3d::LpvConfigUbo::LpvConfig\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140 };\
	auto c3d_lightView = lpvConfig.declMember< Mat4 >( castor3d::LpvConfigUbo::LightView );\
	auto c3d_minVolumeCorner = lpvConfig.declMember< Vec4 >( castor3d::LpvConfigUbo::MinVolumeCorner );\
	auto c3d_gridSizes = lpvConfig.declMember< Vec4 >( castor3d::LpvConfigUbo::GridSizes );\
	auto c3d_lpvConfig = lpvConfig.declMember< Vec4 >( castor3d::LpvConfigUbo::Config );\
	lpvConfig.end();\
	auto c3d_gridWidth = c3d_gridSizes.x();\
	auto c3d_gridHeight = c3d_gridSizes.y();\
	auto c3d_gridDepth = c3d_gridSizes.z();\
	auto c3d_gridSize = c3d_gridSizes.xyz();\
	auto c3d_lightIndex = writer.cast< sdw::Int >( c3d_gridSizes.w() );\
	auto c3d_lpvIndirectAttenuation = c3d_lpvConfig.x();\
	auto c3d_lpvTexelAreaModifier = c3d_lpvConfig.y();\
	auto c3d_lpvTanFovXHalf = c3d_lpvConfig.z();\
	auto c3d_lpvTanFovYHalf = c3d_lpvConfig.w()

#endif
