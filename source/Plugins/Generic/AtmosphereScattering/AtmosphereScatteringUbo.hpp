/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereScatteringUbo_H___
#define ___C3DAS_AtmosphereScatteringUbo_H___

#include "AtmosphereScatteringConfig.hpp"

#include <Castor3D/Castor3DModule.hpp>
#include <Castor3D/Buffer/UniformBufferOffset.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

#include <ShaderWriter/BaseTypes/Array.hpp>
#include <ShaderWriter/BaseTypes/Int.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/MatTypes/Mat4.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

namespace atmosphere_scattering
{
	struct AtmosphereData
		: public sdw::StructInstance
	{
		AtmosphereData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );

		SDW_DeclStructInstance( , AtmosphereData );

		static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

	private:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;

	public:
		sdw::Vec3 sunDirection;
		sdw::Float pad0;

		sdw::Vec3 solarIrradiance;
		sdw::Float sunAngularRadius;

		sdw::Vec3 sunIlluminance;
		sdw::Float sunIlluminanceScale;

		sdw::Vec2 rayMarchMinMaxSPP;
		sdw::Vec2 pad1;

		sdw::Vec3 absorptionExtinction;
		sdw::Float muSMin;

		sdw::Vec3 rayleighScattering;
		sdw::Float miePhaseFunctionG;

		sdw::Vec3 mieScattering;
		sdw::Float bottomRadius;

		sdw::Vec3 mieExtinction;
		sdw::Float topRadius;

		sdw::Vec3 mieAbsorption;
		sdw::Float multipleScatteringFactor;

		sdw::Vec3 groundAlbedo;
		sdw::Float multiScatteringLUTRes;

		sdw::Float rayleighDensity0LayerWidth;
		sdw::Float rayleighDensity0ExpTerm;
		sdw::Float rayleighDensity0ExpScale;
		sdw::Float rayleighDensity0LinearTerm;
		sdw::Float rayleighDensity0ConstantTerm;

		sdw::Float rayleighDensity1LayerWidth;
		sdw::Float rayleighDensity1ExpTerm;
		sdw::Float rayleighDensity1ExpScale;
		sdw::Float rayleighDensity1LinearTerm;
		sdw::Float rayleighDensity1ConstantTerm;

		sdw::Float mieDensity0LayerWidth;
		sdw::Float mieDensity0ExpTerm;
		sdw::Float mieDensity0ExpScale;
		sdw::Float mieDensity0LinearTerm;
		sdw::Float mieDensity0ConstantTerm;

		sdw::Float mieDensity1LayerWidth;
		sdw::Float mieDensity1ExpTerm;
		sdw::Float mieDensity1ExpScale;
		sdw::Float mieDensity1LinearTerm;
		sdw::Float mieDensity1ConstantTerm;

		sdw::Float absorptionDensity0LayerWidth;
		sdw::Float absorptionDensity0ExpTerm;
		sdw::Float absorptionDensity0ExpScale;
		sdw::Float absorptionDensity0LinearTerm;
		sdw::Float absorptionDensity0ConstantTerm;

		sdw::Float absorptionDensity1LayerWidth;
		sdw::Float absorptionDensity1ExpTerm;
		sdw::Float absorptionDensity1ExpScale;
		sdw::Float absorptionDensity1LinearTerm;
		sdw::Float absorptionDensity1ConstantTerm;
	};

	class AtmosphereScatteringUbo
	{
	private:
		using Configuration = AtmosphereScatteringConfig;

	public:
		explicit AtmosphereScatteringUbo( castor3d::RenderDevice const & device );
		~AtmosphereScatteringUbo();
		void cpuUpdate( Configuration const & config );

		void createPassBinding( crg::FramePass & pass
			, uint32_t binding )const
		{
			m_ubo.createPassBinding( pass, "AtmosphereCfg", binding );
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

		castor3d::UniformBufferOffsetT< Configuration > const & getUbo()const
		{
			return m_ubo;
		}

		castor3d::UniformBufferOffsetT< Configuration > & getUbo()
		{
			return m_ubo;
		}

	public:
		static const castor::String Buffer;
		static const castor::String Data;

	private:
		castor3d::RenderDevice const & m_device;
		castor3d::UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define C3D_AtmosphereScattering( writer, binding, set )\
	auto atmosphereBuffer = writer.declUniformBuffer<>( atmosphere_scattering::AtmosphereScatteringUbo::Buffer, binding, set );\
	auto c3d_atmosphereData = atmosphereBuffer.declMember< atmosphere_scattering::AtmosphereData >( atmosphere_scattering::AtmosphereScatteringUbo::Data );\
	atmosphereBuffer.end()

#endif
