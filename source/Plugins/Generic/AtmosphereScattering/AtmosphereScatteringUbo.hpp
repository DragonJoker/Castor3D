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
		: public sdw::StructInstanceHelperT< "C3D_ATM_AtmosphereData"
			, sdw::type::MemoryLayout::eStd430
			, sdw::Vec3Field< "sunDirection" >
			, sdw::FloatField< "pad0" >
			, sdw::Vec3Field< "solarIrradiance" >
			, sdw::FloatField< "sunAngularRadius" >
			, sdw::Vec3Field< "sunIlluminance" >
			, sdw::FloatField< "sunIlluminanceScale" >
			, sdw::Vec2Field< "rayMarchMinMaxSPP" >
			, sdw::Vec2Field< "pad2" >
			, sdw::Vec3Field< "absorptionExtinction" >
			, sdw::FloatField< "muSMin" >
			, sdw::Vec3Field< "rayleighScattering" >
			, sdw::FloatField< "miePhaseFunctionG" >
			, sdw::Vec3Field< "mieScattering" >
			, sdw::FloatField< "bottomRadius" >
			, sdw::Vec3Field< "mieExtinction" >
			, sdw::FloatField< "topRadius" >
			, sdw::Vec3Field< "mieAbsorption" >
			, sdw::FloatField< "multipleScatteringFactor" >
			, sdw::Vec3Field< "groundAlbedo" >
			, sdw::FloatField< "multiScatteringLUTRes" >
			, sdw::FloatField< "rayleighDensity0LayerWidth" >
			, sdw::FloatField< "rayleighDensity0ExpTerm" >
			, sdw::FloatField< "rayleighDensity0ExpScale" >
			, sdw::FloatField< "rayleighDensity0LinearTerm" >
			, sdw::FloatField< "rayleighDensity0ConstantTerm" >
			, sdw::FloatField< "rayleighDensity1LayerWidth" >
			, sdw::FloatField< "rayleighDensity1ExpTerm" >
			, sdw::FloatField< "rayleighDensity1ExpScale" >
			, sdw::FloatField< "rayleighDensity1LinearTerm" >
			, sdw::FloatField< "rayleighDensity1ConstantTerm" >
			, sdw::FloatField< "mieDensity0LayerWidth" >
			, sdw::FloatField< "mieDensity0ExpTerm" >
			, sdw::FloatField< "mieDensity0ExpScale" >
			, sdw::FloatField< "mieDensity0LinearTerm" >
			, sdw::FloatField< "mieDensity0ConstantTerm" >
			, sdw::FloatField< "mieDensity1LayerWidth" >
			, sdw::FloatField< "mieDensity1ExpTerm" >
			, sdw::FloatField< "mieDensity1ExpScale" >
			, sdw::FloatField< "mieDensity1LinearTerm" >
			, sdw::FloatField< "mieDensity1ConstantTerm" >
			, sdw::FloatField< "absorptionDensity0LayerWidth" >
			, sdw::FloatField< "absorptionDensity0ExpTerm" >
			, sdw::FloatField< "absorptionDensity0ExpScale" >
			, sdw::FloatField< "absorptionDensity0LinearTerm" >
			, sdw::FloatField< "absorptionDensity0ConstantTerm" >
			, sdw::FloatField< "absorptionDensity1LayerWidth" >
			, sdw::FloatField< "absorptionDensity1ExpTerm" >
			, sdw::FloatField< "absorptionDensity1ExpScale" >
			, sdw::FloatField< "absorptionDensity1LinearTerm" >
			, sdw::FloatField< "absorptionDensity1ConstantTerm" > >
	{
		AtmosphereData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

	public:
		auto sunDirection()const { return getMember< "sunDirection" >(); }
		auto solarIrradiance()const { return getMember< "solarIrradiance" >(); }
		auto sunAngularRadius()const { return getMember< "sunAngularRadius" >(); }
		auto sunIlluminance()const { return getMember< "sunIlluminance" >(); }
		auto sunIlluminanceScale()const { return getMember< "sunIlluminanceScale" >(); }
		auto rayMarchMinMaxSPP()const { return getMember< "rayMarchMinMaxSPP" >(); }
		auto absorptionExtinction()const { return getMember< "absorptionExtinction" >(); }
		auto muSMin()const { return getMember< "muSMin" >(); }
		auto rayleighScattering()const { return getMember< "rayleighScattering" >(); }
		auto miePhaseFunctionG()const { return getMember< "miePhaseFunctionG" >(); }
		auto mieScattering()const { return getMember< "mieScattering" >(); }
		auto bottomRadius()const { return getMember< "bottomRadius" >(); }
		auto mieExtinction()const { return getMember< "mieExtinction" >(); }
		auto topRadius()const { return getMember< "topRadius" >(); }
		auto mieAbsorption()const { return getMember< "mieAbsorption" >(); }
		auto multipleScatteringFactor()const { return getMember< "multipleScatteringFactor" >(); }
		auto groundAlbedo()const { return getMember< "groundAlbedo" >(); }
		auto multiScatteringLUTRes()const { return getMember< "multiScatteringLUTRes" >(); }
		auto rayleighDensity0LayerWidth()const { return getMember< "rayleighDensity0LayerWidth" >(); }
		auto rayleighDensity0ExpTerm()const { return getMember< "rayleighDensity0ExpTerm" >(); }
		auto rayleighDensity0ExpScale()const { return getMember< "rayleighDensity0ExpScale" >(); }
		auto rayleighDensity0LinearTerm()const { return getMember< "rayleighDensity0LinearTerm" >(); }
		auto rayleighDensity0ConstantTerm()const { return getMember< "rayleighDensity0ConstantTerm" >(); }
		auto rayleighDensity1LayerWidth()const { return getMember< "rayleighDensity1LayerWidth" >(); }
		auto rayleighDensity1ExpTerm()const { return getMember< "rayleighDensity1ExpTerm" >(); }
		auto rayleighDensity1ExpScale()const { return getMember< "rayleighDensity1ExpScale" >(); }
		auto rayleighDensity1LinearTerm()const { return getMember< "rayleighDensity1LinearTerm" >(); }
		auto rayleighDensity1ConstantTerm()const { return getMember< "rayleighDensity1ConstantTerm" >(); }
		auto mieDensity0LayerWidth()const { return getMember< "mieDensity0LayerWidth" >(); }
		auto mieDensity0ExpTerm()const { return getMember< "mieDensity0ExpTerm" >(); }
		auto mieDensity0ExpScale()const { return getMember< "mieDensity0ExpScale" >(); }
		auto mieDensity0LinearTerm()const { return getMember< "mieDensity0LinearTerm" >(); }
		auto mieDensity0ConstantTerm()const { return getMember< "mieDensity0ConstantTerm" >(); }
		auto mieDensity1LayerWidth()const { return getMember< "mieDensity1LayerWidth" >(); }
		auto mieDensity1ExpTerm()const { return getMember< "mieDensity1ExpTerm" >(); }
		auto mieDensity1ExpScale()const { return getMember< "mieDensity1ExpScale" >(); }
		auto mieDensity1LinearTerm()const { return getMember< "mieDensity1LinearTerm" >(); }
		auto mieDensity1ConstantTerm()const { return getMember< "mieDensity1ConstantTerm" >(); }
		auto absorptionDensity0LayerWidth()const { return getMember< "absorptionDensity0LayerWidth" >(); }
		auto absorptionDensity0ExpTerm()const { return getMember< "absorptionDensity0ExpTerm" >(); }
		auto absorptionDensity0ExpScale()const { return getMember< "absorptionDensity0ExpScale" >(); }
		auto absorptionDensity0LinearTerm()const { return getMember< "absorptionDensity0LinearTerm" >(); }
		auto absorptionDensity0ConstantTerm()const { return getMember< "absorptionDensity0ConstantTerm" >(); }
		auto absorptionDensity1LayerWidth()const { return getMember< "absorptionDensity1LayerWidth" >(); }
		auto absorptionDensity1ExpTerm()const { return getMember< "absorptionDensity1ExpTerm" >(); }
		auto absorptionDensity1ExpScale()const { return getMember< "absorptionDensity1ExpScale" >(); }
		auto absorptionDensity1LinearTerm()const { return getMember< "absorptionDensity1LinearTerm" >(); }
		auto absorptionDensity1ConstantTerm()const { return getMember< "absorptionDensity1ConstantTerm" >(); }
	};

	class AtmosphereScatteringUbo
	{
	private:
		using Configuration = AtmosphereScatteringConfig;

	public:
		AtmosphereScatteringUbo( castor3d::RenderDevice const & device
			, bool & dirty );
		~AtmosphereScatteringUbo();
		castor::Point3f cpuUpdate( Configuration const & config
			, castor3d::SceneNode const & node );

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
		bool & m_dirty;
		CheckedAtmosphereScatteringConfig m_config;
		castor::GroupChangeTracked< castor::Point3f > m_sunDirection;
		castor::GroupChangeTracked< castor::Point3f > m_mieAbsorption;
	};
}

#define C3D_AtmosphereScattering( writer, binding, set )\
	auto atmosphereBuffer = writer.declUniformBuffer<>( atmosphere_scattering::AtmosphereScatteringUbo::Buffer, binding, set );\
	auto c3d_atmosphereData = atmosphereBuffer.declMember< atmosphere_scattering::AtmosphereData >( atmosphere_scattering::AtmosphereScatteringUbo::Data );\
	atmosphereBuffer.end()

#endif
