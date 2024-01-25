/*
See LICENSE file in root folder
*/
#ifndef ___C3DORFFT_OceanUbo_H___
#define ___C3DORFFT_OceanUbo_H___

#include "OceanFFTRenderingPrerequisites.hpp"

#include <Castor3D/Castor3DModule.hpp>
#include <Castor3D/Buffer/UniformBufferOffset.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

#include <ShaderWriter/BaseTypes/Array.hpp>
#include <ShaderWriter/BaseTypes/Int.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

namespace ocean_fft
{
	struct OceanUboData
		: public sdw::StructInstanceHelperT< "OceanUboData"
			, sdw::type::MemoryLayout::eStd430
			, sdw::FloatField< "time" >
			, sdw::FloatField< "distanceMod" >
			, sdw::FloatField< "amplitude" >
			, sdw::FloatField< "maxWaveLength" >
			, sdw::Vec2Field< "otherMod" >
			, sdw::Vec2Field< "normalMod" >
			, sdw::Vec2Field< "tileScale" >
			, sdw::Vec2Field< "normalScale" >
			, sdw::Vec2Field< "blockOffset" >
			, sdw::Vec2Field< "fftScale" >
			, sdw::Vec2Field< "patchSize" >
			, sdw::Vec2Field< "maxTessLevel" >
			, sdw::Vec2Field< "invHeightmapSize" >
			, sdw::Vec2Field< "windDirection" >
			, sdw::U32Vec2Field< "heightMapSamples" >
			, sdw::Int32Field< "displacementDownsample" >
			, sdw::FloatField< "L" > >
	{
		OceanUboData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, castor::move( expr ), enabled }
		{
		}

	public:
		auto time()const { return getMember< "time" >(); }
		auto distanceMod()const { return getMember< "distanceMod" >(); }
		auto amplitude()const { return getMember< "amplitude" >(); }
		auto maxWaveLength()const { return getMember< "maxWaveLength" >(); }
		auto otherMod()const { return getMember< "otherMod" >(); }
		auto normalMod()const { return getMember< "normalMod" >(); }
		auto tileScale()const { return getMember< "tileScale" >(); }
		auto normalScale()const { return getMember< "normalScale" >(); }
		auto blockOffset()const { return getMember< "blockOffset" >(); }
		auto fftScale()const { return getMember< "fftScale" >(); }
		auto patchSize()const { return getMember< "patchSize" >(); }
		auto maxTessLevel()const { return getMember< "maxTessLevel" >(); }
		auto invHeightmapSize()const { return getMember< "invHeightmapSize" >(); }
		auto windDirection()const { return getMember< "windDirection" >(); }
		auto heightMapSamples()const { return getMember< "heightMapSamples" >(); }
		auto L()const { return getMember< "L" >(); }
		auto displacementDownsample()const { return getMember< "displacementDownsample" >(); }
	};

	class OceanUbo
	{
	private:
		using Configuration = OceanUboConfiguration;

	public:
		explicit OceanUbo( castor3d::RenderDevice const & device );
		~OceanUbo();
		void cpuUpdate( Configuration const & config
			, OceanFFTConfig const & fftConfig
			, castor::Point3f const & cameraPosition );

		void createPassBinding( crg::FramePass & pass
			, uint32_t binding )const
		{
			m_ubo.createPassBinding( pass, "FFTWaterCfg", binding );
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

		void addDescriptorWrite( ashes::WriteDescriptorSetArray & bindings
			, uint32_t & dstBinding
			, uint32_t dstArrayElement = 0u )const
		{
			bindings.emplace_back( getDescriptorWrite( dstBinding, dstArrayElement ) );
			++dstBinding;
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
		static const castor::MbString Buffer;
		static const castor::MbString Data;

	private:
		castor3d::RenderDevice const & m_device;
		castor3d::UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define C3D_FftOcean( writer, binding, set )\
	auto oceanBuffer = writer.declUniformBuffer( ocean_fft::OceanUbo::Buffer, binding, set );\
	auto c3d_oceanData = oceanBuffer.declMember< ocean_fft::OceanUboData >( ocean_fft::OceanUbo::Data );\
	oceanBuffer.end()

#endif
