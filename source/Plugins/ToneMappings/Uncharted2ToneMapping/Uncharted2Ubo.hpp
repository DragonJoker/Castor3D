/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Uncharted2_Uncharted2Ubo_H___
#define ___C3D_Uncharted2_Uncharted2Ubo_H___

#include <Castor3D/Castor3DModule.hpp>
#include <Castor3D/Buffer/UniformBufferOffset.hpp>

#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>

namespace Uncharted2
{
	struct Uncharted2Data
		: public sdw::StructInstance
	{
		SDW_DeclStructInstance( , Uncharted2Data );
		Uncharted2Data( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );

		static ast::type::StructPtr makeType( ast::type::TypesCache & cache );
		static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

	private:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;

	public:
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
		explicit Uncharted2Ubo( castor3d::RenderDevice const & device );
		~Uncharted2Ubo();

		void createPassBinding( crg::FramePass & pass
			, uint32_t binding )const
		{
			m_ubo.createPassBinding( pass, "Uncharted2Cfg", binding );
		}

		void createSizedBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & layoutBinding )const
		{
			return m_ubo.createSizedBinding( descriptorSet, layoutBinding );
		}

		castor3d::UniformBufferOffsetT< Configuration > const & getUbo()const
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

#define UBO_UNCHARTED2( writer, binding, set )\
	sdw::Ubo uncharted2{ writer\
		, Uncharted2::Uncharted2Ubo::Name\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140\
		, true };\
	auto c3d_uncharted2Data = uncharted2.declStructMember< Uncharted2::Uncharted2Data >( Uncharted2::Uncharted2Ubo::Data );\
	uncharted2.end()

#endif
