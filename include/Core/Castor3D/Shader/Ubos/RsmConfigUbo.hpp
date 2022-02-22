/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RsmConfigUbo_H___
#define ___C3D_RsmConfigUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Render/Technique/Opaque/Lighting/LightingModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/BaseTypes/UInt.hpp>

namespace castor3d
{
	namespace shader
	{
		struct RsmConfigData
			: public sdw::StructInstance
		{
			C3D_API RsmConfigData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			SDW_DeclStructInstance( C3D_API, RsmConfigData );

			C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			sdw::Float rsmIntensity;
			sdw::Float rsmRMax;
			sdw::UInt rsmSampleCount;
			sdw::UInt rsmIndex;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;
		};
	}

	class RsmConfigUbo
	{
	public:
		using Configuration = RsmUboConfiguration;

	public:
		C3D_API explicit RsmConfigUbo( RenderDevice const & device );
		C3D_API RsmConfigUbo( RsmConfigUbo const & rhs ) = delete;
		C3D_API RsmConfigUbo & operator=( RsmConfigUbo const & rhs ) = delete;
		C3D_API RsmConfigUbo( RsmConfigUbo && rhs ) = default;
		C3D_API RsmConfigUbo & operator=( RsmConfigUbo && rhs ) = delete;
		C3D_API ~RsmConfigUbo();
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	light	The light source.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	light	La source lumineuse.
		 */
		C3D_API void update( Light const & light );

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
		C3D_API static std::string const BufferRsmConfig;
		C3D_API static std::string const RsmConfigData;

	private:
		RenderDevice const & m_device;
		UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define UBO_RSM_CONFIG( writer, binding, set )\
	sdw::Ubo rsmConfig{ writer\
		, castor3d::RsmConfigUbo::BufferRsmConfig\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd140\
		, true };\
	auto c3d_rsmConfigData = rsmConfig.declMember< castor3d::shader::RsmConfigData >( castor3d::RsmConfigUbo::RsmConfigData );\
	rsmConfig.end()

#endif
