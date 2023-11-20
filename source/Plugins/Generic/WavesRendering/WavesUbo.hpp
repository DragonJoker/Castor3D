/*
See LICENSE file in root folder
*/
#ifndef ___C3DW_WavesUbo_H___
#define ___C3DW_WavesUbo_H___

#include <Castor3D/Castor3DModule.hpp>
#include <Castor3D/Buffer/UniformBufferOffset.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

#include <ShaderWriter/BaseTypes/Array.hpp>
#include <ShaderWriter/BaseTypes/Int.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

namespace waves
{
	static uint32_t constexpr MaxWaves = 8u;

	struct WaveConfigurationData
	{
		explicit WaveConfigurationData( castor::Point3f pdirection = {}
			, float psteepness = {}
			, float plength = {}
			, float pamplitude = {}
			, float pspeed = {} )
			: direction{ pdirection }
			, pad0{}
			, steepness{ psteepness }
			, length{ plength }
			, amplitude{ pamplitude }
			, speed{ pspeed }
		{
		}

		castor::Point3f direction;
		float pad0;
		float steepness;
		float length;
		float amplitude;
		float speed;
	};

	struct WavesConfiguration
	{
		float tessellationFactor{ 7.0f };
		uint32_t numWaves{ 2u };
		float time{ 0.0f };
		float dampeningFactor{ 5.0f };

		std::array< WaveConfigurationData, MaxWaves > waves{ WaveConfigurationData{ castor::Point3f{ 0.3f, 0.0f, -0.7f }, 1.79f, 3.75f, 0.85f, 1.21f }
			, WaveConfigurationData{ castor::Point3f{ 0.5f, 0.0f, -0.2f }, 1.79f, 4.1f, 0.52f, 1.03f }
			, WaveConfigurationData{}
			, WaveConfigurationData{}
			, WaveConfigurationData{}
			, WaveConfigurationData{}
			, WaveConfigurationData{}
			, WaveConfigurationData{} };
	};

	struct Wave
		: sdw::StructInstanceHelperT< "Wave"
			, sdw::type::MemoryLayout::eStd140
			, sdw::Vec3Field< "direction" >
			, sdw::FloatField< "pad" >
			, sdw::FloatField< "steepness" >
			, sdw::FloatField< "length" >
			, sdw::FloatField< "amplitude" >
			, sdw::FloatField< "speed" > >
	{
		Wave( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled = true )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		auto direction()const { return getMember< "direction" >(); }
		auto steepness()const { return getMember< "steepness" >(); }
		auto length()const { return getMember< "length" >(); }
		auto amplitude()const { return getMember< "amplitude" >(); }
		auto speed()const { return getMember< "speed" >(); }
	};

	Writer_Parameter( Wave );

	struct WavesData
		: public sdw::StructInstanceHelperT< "C3D_WavesData"
			, sdw::type::MemoryLayout::eStd140
			, sdw::FloatField< "tessellationFactor" >
			, sdw::UIntField< "numWaves" >
			, sdw::FloatField< "time" >
			, sdw::FloatField< "dampeningFactor" >
			, sdw::StructFieldArrayT< Wave, "waves", MaxWaves > >
	{
		WavesData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		auto waves()const { return getMember< "waves" >(); }
		auto tessellationFactor()const { return getMember< "tessellationFactor" >(); }
		auto numWaves()const { return getMember< "numWaves" >(); }
		auto time()const { return getMember< "time" >(); }
		auto dampeningFactor()const { return getMember< "dampeningFactor" >(); }
	};

	class WavesUbo
	{
	private:
		using Configuration = WavesConfiguration;

	public:
		explicit WavesUbo( castor3d::RenderDevice const & device );
		~WavesUbo();
		void cpuUpdate( WavesConfiguration const & config );

		void createPassBinding( crg::FramePass & pass
			, uint32_t binding )const
		{
			m_ubo.createPassBinding( pass, "WavesCfg", binding );
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

	private:
		castor3d::RenderDevice const & m_device;
		castor3d::UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define C3D_Waves( writer, binding, set )\
	sdw::UniformBuffer wavesBuffer{ writer\
		, "C3D_Waves"\
		, "c3d_wavesData"\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd140 };\
	auto c3d_wavesData = wavesBuffer.declMember< waves::WavesData >( "o" );\
	wavesBuffer.end()

#endif
