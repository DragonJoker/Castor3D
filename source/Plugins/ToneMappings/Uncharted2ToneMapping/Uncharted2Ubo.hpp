/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Uncharted2_Uncharted2Ubo_H___
#define ___C3D_Uncharted2_Uncharted2Ubo_H___

#include <Castor3D/Castor3DModule.hpp>
#include <Castor3D/Buffer/UniformBufferOffset.hpp>

#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>

namespace Uncharted2
{
	struct Uncharted2Data
		: public sdw::StructInstanceHelperT< "C3D_Uncharted2Data"
			, sdw::type::MemoryLayout::eStd140
			, sdw::FloatField< "shoulderStrength" >
			, sdw::FloatField< "linearStrength" >
			, sdw::FloatField< "linearAngle" >
			, sdw::FloatField< "toeStrength" >
			, sdw::FloatField< "toeNumerator" >
			, sdw::FloatField< "toeDenominator" >
			, sdw::FloatField< "linearWhitePointValue" >
			, sdw::FloatField< "exposureBias" > >
	{
		SDW_DeclStructInstance( , Uncharted2Data );

		Uncharted2Data( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
			, shoulderStrength{ getMember< "shoulderStrength" >() }
			, linearStrength{ getMember< "linearStrength" >() }
			, linearAngle{ getMember< "linearAngle" >() }
			, toeStrength{ getMember< "toeStrength" >() }
			, toeNumerator{ getMember< "toeNumerator" >() }
			, toeDenominator{ getMember< "toeDenominator" >() }
			, linearWhitePointValue{ getMember< "linearWhitePointValue" >() }
			, exposureBias{ getMember< "exposureBias" >() }
		{
		}

		sdw::Float shoulderStrength;
		sdw::Float linearStrength;
		sdw::Float linearAngle;
		sdw::Float toeStrength;
		sdw::Float toeNumerator;
		sdw::Float toeDenominator;
		sdw::Float linearWhitePointValue;
		sdw::Float exposureBias;
	};

	struct Uncharted2UboConfiguration
	{
		float shoulderStrength{ 0.15f };
		float linearStrength{ 0.50f };
		float linearAngle{ 0.10f };
		float toeStrength{ 0.20f };
		float toeNumerator{ 0.02f };
		float toeDenominator{ 0.30f };
		float linearWhitePointValue{ 11.2f };
		float exposureBias{ 2.0f };
	};

	class Uncharted2Ubo
	{
	private:
		using Configuration = Uncharted2UboConfiguration;

	public:
		Uncharted2Ubo( Uncharted2Ubo const & rhs ) = delete;
		Uncharted2Ubo & operator=( Uncharted2Ubo const & rhs ) = delete;
		Uncharted2Ubo( Uncharted2Ubo && rhs ) = default;
		Uncharted2Ubo & operator=( Uncharted2Ubo && rhs ) = delete;
		explicit Uncharted2Ubo( c3d::RenderDevice const & device );
		~Uncharted2Ubo();

		void createPassBinding( crg::FramePass & pass
			, uint32_t binding )const
		{
			m_ubo.createPassBinding( pass, binding );
		}

		void createSizedBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & layoutBinding )const
		{
			return m_ubo.createSizedBinding( descriptorSet, layoutBinding );
		}

		c3d::UniformBufferOffsetT< Configuration > const & getUbo()const
		{
			return m_ubo;
		}

	public:
		static const c3d::String Buffer;
		static const c3d::String Data;

	private:
		c3d::RenderDevice const & m_device;
		c3d::UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define C3D_Uncharted2( writer, binding, set )\
	sdw::UniformBuffer uncharted2{ writer\
		, Uncharted2::Uncharted2Ubo::Name\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140\
		, true };\
	auto c3d_uncharted2Data = uncharted2.declMember< Uncharted2::Uncharted2Data >( Uncharted2::Uncharted2Ubo::Data );\
	uncharted2.end()

#endif
