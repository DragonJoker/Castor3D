/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ColourGradingUbo_H___
#define ___C3D_ColourGradingUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Render/ToneMapping/ColourGradingConfig.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/MatTypes/Mat3.hpp>

namespace castor3d
{
	namespace shader
	{
		struct ColourGradingData
			: sdw::StructInstanceHelperT < "C3D_ColourGradingConfig"
				, sdw::type::MemoryLayout::eStd140
				, sdw::Vec3Field< "whiteBalance" >
				, sdw::FloatField< "postExposure" >
				, sdw::Vec3Field< "colourFilter" >
				, sdw::FloatField< "midGray" >
				, sdw::Vec3Field< "splitToningShadows" >
				, sdw::FloatField< "contrast" >
				, sdw::Vec3Field< "splitToningHighlights" >
				, sdw::FloatField< "splitToningBalance" >
				, sdw::Vec3Field< "channelMixRed" >
				, sdw::FloatField< "saturation" >
				, sdw::Vec3Field< "channelMixGreen" >
				, sdw::FloatField< "shadowsStart" >
				, sdw::Vec3Field< "channelMixBlue" >
				, sdw::FloatField< "shadowsEnd" >
				, sdw::Vec3Field< "shadows" >
				, sdw::FloatField< "highlightsStart" >
				, sdw::Vec3Field< "midtones" >
				, sdw::FloatField< "highlightsEnd" >
				, sdw::Vec3Field< "highlights" >
				, sdw::FloatField< "hueShift" >
				, sdw::UIntField< "enabled" >
				, sdw::UIntField< "enableSplitToning" >
				, sdw::UIntField< "enableShadowMidToneHighlight" >
				, sdw::UIntField< "enableChannelMix" >
				, sdw::UIntField< "enableWhiteBalance" >
				, sdw::UIntField< "enableHueShift" >
				, sdw::UIntField< "enableContrast" >
				, sdw::UIntField< "pad0" > >
		{
			C3D_API ColourGradingData( sdw::ShaderWriter & writer
				, sdw::expr::ExprPtr expr
				, bool enabled );

			C3D_API sdw::Vec3 colourGrade( sdw::Vec3 const  hdrColour );

		private:
			auto enabled()const { return getMember< "enabled" >() != 0_u; }
			auto enableSplitToning()const { return getMember< "enableSplitToning" >() != 0_u; }
			auto enableShadowMidToneHighlight()const { return getMember< "enableShadowMidToneHighlight" >() != 0_u; }
			auto enableChannelMix()const { return getMember< "enableChannelMix" >() != 0_u; }
			auto enableWhiteBalance()const { return getMember< "enableWhiteBalance" >() != 0_u; }
			auto enableHueShift()const { return getMember< "enableHueShift" >() != 0_u; }
			auto enableContrast()const { return getMember< "enableContrast" >() != 0_u; }
			auto whiteBalance()const { return getMember< "whiteBalance" >(); }
			auto postExposure()const { return getMember< "postExposure" >(); }
			auto colourFilter()const { return getMember< "colourFilter" >(); }
			auto midGray()const { return getMember< "midGray" >(); }
			auto splitToningShadows()const { return getMember< "splitToningShadows" >(); }
			auto splitToningBalance()const { return getMember< "splitToningBalance" >(); }
			auto splitToningHighlights()const { return getMember< "splitToningHighlights" >(); }
			auto contrast()const { return getMember< "contrast" >(); }
			auto hueShift()const { return getMember< "hueShift" >(); }
			auto saturation()const { return getMember< "saturation" >(); }
			auto channelMixRed()const { return getMember< "channelMixRed" >(); }
			auto channelMixGreen()const { return getMember< "channelMixGreen" >(); }
			auto channelMixBlue()const { return getMember< "channelMixBlue" >(); }
			auto shadows()const { return getMember< "shadows" >(); }
			auto midtones()const { return getMember< "midtones" >(); }
			auto highlights()const { return getMember< "highlights" >(); }
			auto shadowsStart()const { return getMember< "shadowsStart" >(); }
			auto shadowsEnd()const { return getMember< "shadowsEnd" >(); }
			auto highlightsStart()const { return getMember< "highlightsStart" >(); }
			auto highlightsEnd()const { return getMember< "highlightsEnd" >(); }

			sdw::Float getLuminance( sdw::Vec3 const & colour );
			sdw::Vec3 linearToLogC( sdw::Vec3 const & colour );
			sdw::Vec3 logCToLinear( sdw::Vec3 const & colour );
			sdw::Float rotateHue( sdw::Float const & value, sdw::Float const & low, sdw::Float const & hi );
			sdw::Vec3 linearToLms( sdw::Vec3 const & colour );
			sdw::Vec3 lmsToLinear( sdw::Vec3 const & colour );
			sdw::Vec3 softLight( sdw::Vec3 const & base, sdw::Vec3 const & blend );

		private:
			sdw::FloatArray m_paramsLogC;
			sdw::Mat3x3 m_linearToLms;
			sdw::Mat3x3 m_lmsToLinear;
			sdw::Function< sdw::Vec3, sdw::InVec3 > m_rgbToHsv;
			sdw::Function< sdw::Vec3, sdw::InVec3 > m_hsvToRgb;
			sdw::Function< sdw::Vec3, sdw::InVec3 > m_splitToning;
			sdw::Function< sdw::Vec3, sdw::InVec3 > m_shadowsMidtoneHighlight;
			sdw::Function< sdw::Vec3, sdw::InVec3 > m_colourGrade;
		};
	}

	class ColourGradingUbo
	{
	public:
		using Configuration = ColourGradingConfig;

	public:
		C3D_API ColourGradingUbo( ColourGradingUbo const & rhs ) = delete;
		C3D_API ColourGradingUbo & operator=( ColourGradingUbo const & rhs ) = delete;
		C3D_API ColourGradingUbo( ColourGradingUbo && rhs ) = default;
		C3D_API ColourGradingUbo & operator=( ColourGradingUbo && rhs ) = delete;
		C3D_API explicit ColourGradingUbo( RenderDevice const & device );
		C3D_API ~ColourGradingUbo();
		/**
		 *\~english
		 *\brief		Updates the UBO from given values.
		 *\param[in]	config	The HDR configuration.
		 *\~french
		 *\brief		Met à jour l'UBO avec les valeurs données.
		 *\param[in]	config	La configuration HDR.
		 */
		C3D_API void cpuUpdate( Configuration const & config );

		void createPassBinding( crg::FramePass & pass
			, uint32_t binding )const
		{
			return m_ubo.createPassBinding( pass, "ClrGrdCfg", binding );
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

	private:
		RenderDevice const & m_device;
		UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define C3D_ColourGrading( writer, binding, set )\
	sdw::UniformBuffer colourGradingConfig{ writer\
		, "C3D_ColourGrading"\
		, "c3d_colourGradingBuffer"\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd140\
		, true };\
	auto c3d_colourGrading = colourGradingConfig.declMember< castor3d::shader::ColourGradingData >( "d" );\
	colourGradingConfig.end()

#endif
