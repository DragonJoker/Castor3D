/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LpvLightConfigUbo_H___
#define ___C3D_LpvLightConfigUbo_H___

#include "UbosModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <CastorUtils/Graphics/GraphicsModule.hpp>

#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>

namespace castor3d
{
	namespace shader
	{
		struct LpvLightData
			: public sdw::StructInstanceHelperT< "C3D_LpvLightData"
				, sdw::type::MemoryLayout::eStd140
				, sdw::Mat4x4Field< "lightView" >
				, sdw::FloatField< "texelAreaModifier" >
				, sdw::FloatField< "tanFovXHalf" >
				, sdw::FloatField< "tanFovYHalf" >
				, sdw::IntField< "lightOffset" > >
		{
			C3D_API LpvLightData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled )
				: StructInstanceHelperT{ writer, castor::move( expr ), enabled }
			{
			}

			auto lightView()const { return getMember< "lightView" >(); }
			auto texelAreaModifier()const { return getMember< "texelAreaModifier" >(); }
			auto tanFovXHalf()const { return getMember< "tanFovXHalf" >(); }
			auto tanFovYHalf()const { return getMember< "tanFovYHalf" >(); }
			auto lightOffset()const { return getMember< "lightOffset" >(); }
		};
	}

	class LpvLightConfigUbo
	{
	public:
		using Configuration = LpvLightConfigUboConfiguration;

	public:
		C3D_API LpvLightConfigUbo( LpvLightConfigUbo const & rhs ) = delete;
		C3D_API LpvLightConfigUbo & operator=( LpvLightConfigUbo const & rhs ) = delete;
		C3D_API LpvLightConfigUbo( LpvLightConfigUbo && rhs )noexcept = default;
		C3D_API LpvLightConfigUbo & operator=( LpvLightConfigUbo && rhs )noexcept = delete;
		C3D_API explicit LpvLightConfigUbo( RenderDevice const & device );
		C3D_API ~LpvLightConfigUbo()noexcept;

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

	private:
		RenderDevice const & m_device;
		UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define C3D_LpvLightConfig( writer, binding, set )\
	sdw::UniformBuffer lpvLightConfig{ writer\
		, "C3D_LpvLightConfig"\
		, "c3d_lpvLightConfig"\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd140 };\
	auto c3d_lpvLightData = lpvLightConfig.declMember< castor3d::shader::LpvLightData >( "d" );\
	lpvLightConfig.end()

#endif
