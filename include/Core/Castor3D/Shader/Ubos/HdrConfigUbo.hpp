/*
See LICENSE file in root folder
*/
#ifndef ___C3D_HdrConfigUbo_H___
#define ___C3D_HdrConfigUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Render/ToneMapping/HdrConfig.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/BaseTypes/Float.hpp>

namespace castor3d
{
	namespace shader
	{
		struct HdrConfigData
			: public sdw::StructInstance
		{
			C3D_API HdrConfigData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			SDW_DeclStructInstance( C3D_API, HdrConfigData );

			C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			C3D_API sdw::Vec3 removeGamma( sdw::Vec3 const & srgb )const;
			C3D_API sdw::Vec3 applyGamma( sdw::Vec3 const & hdr )const;

			sdw::Float const & getExposure()const
			{
				return m_exposure;
			}

			sdw::Float const & getGamma()const
			{
				return m_gamma;
			}

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;

		private:
			sdw::Float m_exposure;
			sdw::Float m_gamma;
		};
	}

	class HdrConfigUbo
	{
	public:
		using Configuration = HdrConfig;

	public:
		C3D_API HdrConfigUbo( HdrConfigUbo const & rhs ) = delete;
		C3D_API HdrConfigUbo & operator=( HdrConfigUbo const & rhs ) = delete;
		C3D_API HdrConfigUbo( HdrConfigUbo && rhs ) = default;
		C3D_API HdrConfigUbo & operator=( HdrConfigUbo && rhs ) = delete;
		C3D_API explicit HdrConfigUbo( RenderDevice const & device );
		C3D_API ~HdrConfigUbo();
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	config	The HDR configuration.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	config	La configuration HDR.
		 */
		C3D_API void cpuUpdate( HdrConfig const & config );

		void createPassBinding( crg::FramePass & pass
			, uint32_t binding )const
		{
			return m_ubo.createPassBinding( pass, "HdrCfg", binding );
		}

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
		C3D_API static castor::String const BufferHdrConfig;
		C3D_API static castor::String const HdrConfigData;

	private:
		RenderDevice const & m_device;
		UniformBufferOffsetT< HdrConfig > m_ubo;
	};
}

#define UBO_HDR_CONFIG( writer, binding, set )\
	sdw::Ubo hdrConfig{ writer\
		, castor3d::HdrConfigUbo::BufferHdrConfig\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140\
		, true };\
	auto c3d_hdrConfigData = hdrConfig.declMember< castor3d::shader::HdrConfigData >( castor3d::HdrConfigUbo::HdrConfigData );\
	hdrConfig.end()

#endif
