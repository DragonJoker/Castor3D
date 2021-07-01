/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LpvLightConfigUbo_H___
#define ___C3D_LpvLightConfigUbo_H___

#include "UbosModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <CastorUtils/Graphics/GraphicsModule.hpp>

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

namespace castor3d
{
	namespace shader
	{
		struct LpvLightData
			: public sdw::StructInstance
		{
			C3D_API LpvLightData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			C3D_API LpvLightData & operator=( LpvLightData const & rhs );

			C3D_API static ast::type::StructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			// Raw values
			sdw::Mat4 lightView;
			sdw::Vec4 lightConfig;
			// Specific values
			sdw::Float texelAreaModifier;
			sdw::Float tanFovXHalf;
			sdw::Float tanFovYHalf;
			sdw::Int lightIndex;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;
		};
	}

	class LpvLightConfigUbo
	{
	public:
		using Configuration = LpvLightConfigUboConfiguration;

	public:
		C3D_API explicit LpvLightConfigUbo( RenderDevice const & device );
		C3D_API ~LpvLightConfigUbo();

		C3D_API void cpuUpdate( Light const & light
			, float lpvCellSize
			, uint32_t faceIndex );
		C3D_API void cpuUpdate( DirectionalLight const & light
			, uint32_t cascadeIndex
			, float lpvCellSize );

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
			return m_ubo.createPassBinding( pass, "LpvLightCfg", binding );
		}

		void createSizedBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & layoutBinding )const
		{
			return m_ubo.createSizedBinding( descriptorSet, layoutBinding );
		}

	public:
		C3D_API static const std::string LpvLightConfig;
		C3D_API static const std::string LpvLightData;

	private:
		RenderDevice const & m_device;
		UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define UBO_LPVLIGHTCONFIG( writer, binding, set )\
	sdw::Ubo lpvLightConfig{ writer\
		, castor3d::LpvLightConfigUbo::LpvLightConfig\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140 };\
	auto c3d_lpvLightData = lpvLightConfig.declStructMember< castor3d::shader::LpvLightData >( castor3d::LpvLightConfigUbo::LpvLightData );\
	lpvLightConfig.end()

#endif
