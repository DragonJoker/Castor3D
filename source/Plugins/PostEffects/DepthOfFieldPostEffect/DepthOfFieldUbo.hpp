/*
See LICENSE file in root folder
*/
#ifndef ___C3DDOF_DepthOfFieldUbo_H___
#define ___C3DDOF_DepthOfFieldUbo_H___

#include <Castor3D/Castor3DModule.hpp>
#include <Castor3D/Buffer/UniformBufferOffset.hpp>

#include <ShaderWriter/BaseTypes/Array.hpp>
#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/BaseTypes/UInt.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/VecTypes/Vec2.hpp>

namespace dof
{
	struct DepthOfFieldConfig
	{
		void setParameters( c3d::Parameters parameters );
		void accept( c3d::ConfigurationVisitorBase & visitor );
		bool write( c3d::StringStream & file, c3d::String const & tabs )const;
		static c3d::AttributeParsers createParsers();
		static c3d::StrSectionIdMap createSections();

		c3d::RangedValue< float > focalDistance{ 10.0f, c3d::makeRange( 0.0f, 100.0f ) };
		c3d::RangedValue< float > focalLength{ 1.0f, c3d::makeRange( 0.0f, 100.0f ) };
		c3d::RangedValue< float > bokehScale{ 1.0f, c3d::makeRange( 0.0f, 10.0f ) };
		bool enableFarBlur;

		c3d::Point2f pixelStepFull{ 1.0f, 1.0f };
		c3d::Point2f pixelStepHalf{ 1.0f, 1.0f };
	};

	struct DepthOfFieldUboConfiguration
	{
		float focalDistance{ 10.0f };
		float focalLength{ 1.0f };
		float bokehScale{ 1.0f };
		uint32_t enableFarBlur{ 1u };

		c3d::Point2f pixelStepFull{ 1.0f, 1.0f };
		c3d::Point2f pixelStepHalf{ 1.0f, 1.0f };

		std::array< c3d::Point4f, 64 > points64;
		std::array< c3d::Point4f, 16 > points16;
	};

	struct DepthOfFieldData
		: public sdw::StructInstanceHelperT< "C3D_DoFData"
			, sdw::type::MemoryLayout::eStd140
			, sdw::FloatField< "focalDistance" >
			, sdw::FloatField< "focalLength" >
			, sdw::FloatField< "bokehScale" >
			, sdw::UIntField< "enableFarBlur" >
			, sdw::Vec2Field< "pixelStepFull" >
			, sdw::Vec2Field< "pixelStepHalf" >
			, sdw::Vec4ArrayField< "points64", 64u >
			, sdw::Vec4ArrayField< "points16", 16u > >
	{
	public:
		SDW_DeclStructInstance( , DepthOfFieldData );

		DepthOfFieldData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			:StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		auto focalDistance()const { return getMember< "focalDistance" >(); }
		auto focalLength()const { return getMember< "focalLength" >(); }
		auto bokehScale()const { return getMember< "bokehScale" >(); }
		auto pixelStepFull()const { return getMember< "pixelStepFull" >(); }
		auto pixelStepHalf()const { return getMember< "pixelStepHalf" >(); }
		auto enableFarBlur()const { return getMember< "enableFarBlur" >() != 0_u; }
		auto points64()const { return getMember< "points64" >(); }
		auto points16()const { return getMember< "points16" >(); }
	};

	class DepthOfFieldUbo
	{
	private:
		using Configuration = DepthOfFieldUboConfiguration;

	public:
		explicit DepthOfFieldUbo( c3d::RenderDevice const & device );
		~DepthOfFieldUbo();
		void cpuUpdate( DepthOfFieldConfig const & data );

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
		static const c3d::MbString Buffer;
		static const c3d::MbString Data;

	private:
		c3d::RenderDevice const & m_device;
		c3d::UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define C3D_DepthOfField( writer, binding, set )\
	sdw::UniformBuffer dofBuffer{ writer, DepthOfFieldUbo::Buffer, binding, set };\
	auto c3d_dofData = dofBuffer.declMember< DepthOfFieldData >( DepthOfFieldUbo::Data );\
	dofBuffer.end()

#endif
