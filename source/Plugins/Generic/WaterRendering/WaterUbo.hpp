/*
See LICENSE file in root folder
*/
#ifndef ___C3DWR_WaterUbo_H___
#define ___C3DWR_WaterUbo_H___

#include <Castor3D/Castor3DModule.hpp>
#include <Castor3D/Buffer/UniformBufferOffset.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

#include <ShaderWriter/BaseTypes/Array.hpp>
#include <ShaderWriter/BaseTypes/Int.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

namespace water
{
	struct WaterUboConfiguration
	{
		float time{ 0.0f };
		float dampeningFactor{ 5.0f };
		float depthSofteningDistance{ 0.5f };
		float dummy1{};
		float refractionRatio{ 1.2f };
		float refractionDistortionFactor{ 0.04f };
		float refractionHeightFactor{ 2.5f };
		float refractionDistanceFactor{ 15.0f };
		castor::Point2f normalMapScrollSpeed{ 0.01f, 0.01f };
		castor::Point2f dummy2{};
		castor::Point4f normalMapScroll{ 1.0f, 0.0f, 0.0f, 1.0f };
		float ssrStepSize{ 0.5f };
		float ssrForwardStepsCount{ 20.0f };
		float ssrBackwardStepsCount{ 10.0f };
		float ssrDepthMult{ 20.0f };
	};

	struct WaterData
		: public sdw::StructInstanceHelperT< "WaterData"
			, sdw::type::MemoryLayout::eStd430
			, sdw::FloatField< "time" >
			, sdw::FloatField< "dampeningFactor" >
			, sdw::FloatField< "depthSofteningDistance" >
			, sdw::FloatField< "pad1" >
			, sdw::FloatField< "refractionRatio" >
			, sdw::FloatField< "refractionDistortionFactor" >
			, sdw::FloatField< "refractionHeightFactor" >
			, sdw::FloatField< "refractionDistanceFactor" >
			, sdw::Vec2Field< "normalMapScrollSpeed" >
			, sdw::Vec2Field< "pad2" >
			, sdw::Vec4Field< "normalMapScroll" >
			, sdw::Vec4Field< "ssrSettings" > >
	{
		WaterData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

	public:
		auto time()const { return getMember< "time" >(); }
		auto dampeningFactor()const { return getMember< "dampeningFactor" >(); }
		auto depthSofteningDistance()const { return getMember< "depthSofteningDistance" >(); }
		auto refractionRatio()const { return getMember< "refractionRatio" >(); }
		auto refractionDistortionFactor()const { return getMember< "refractionDistortionFactor" >(); }
		auto refractionHeightFactor()const { return getMember< "refractionHeightFactor" >(); }
		auto refractionDistanceFactor()const { return getMember< "refractionDistanceFactor" >(); }
		auto normalMapScrollSpeed()const { return getMember< "normalMapScrollSpeed" >(); }
		auto normalMapScroll()const { return getMember< "normalMapScroll" >(); }
		auto ssrSettings()const { return getMember< "ssrSettings" >(); }
	};

	class WaterUbo
	{
	private:
		using Configuration = WaterUboConfiguration;

	public:
		explicit WaterUbo( castor3d::RenderDevice const & device );
		~WaterUbo();
		void cpuUpdate( WaterUboConfiguration const & config );

		void createPassBinding( crg::FramePass & pass
			, uint32_t binding )const
		{
			m_ubo.createPassBinding( pass, Buffer + "Cfg", binding );
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

#define C3D_Water( writer, binding, set )\
	auto waterBuffer = writer.declUniformBuffer( water::WaterUbo::Buffer, binding, set );\
	auto c3d_waterData = waterBuffer.declMember< water::WaterData >( water::WaterUbo::Data );\
	waterBuffer.end()

#endif
