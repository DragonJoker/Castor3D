/*
See LICENSE file in root folder
*/
#ifndef ___C3DWR_WaterUbo_H___
#define ___C3DWR_WaterUbo_H___

#include <Castor3D/Render/RenderModule.hpp>
#include <Castor3D/Shader/Ubos/UbosModule.hpp>

#include <Castor3D/Shader/Shaders/GlslBuffer.hpp>

#include <ShaderWriter/MatTypes/Mat4.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>

namespace water::shader
{
	namespace c3d = castor3d::shader;

	struct WaterProfile
		: public sdw::StructInstanceHelperT< "WaterData"
			, sdw::type::MemoryLayout::eStd430
			, sdw::FloatField< "dampeningFactor" >
			, sdw::FloatField< "depthSofteningDistance" >
			, sdw::FloatField< "pad0" >
			, sdw::FloatField< "pad1" >
			, sdw::FloatField< "refractionDistortionFactor" >
			, sdw::FloatField< "refractionHeightFactor" >
			, sdw::FloatField< "refractionDistanceFactor" >
			, sdw::FloatField< "pad2" >
			, sdw::FloatField< "ssrStepSize" >
			, sdw::UIntField< "ssrForwardStepsCount" >
			, sdw::UIntField< "ssrBackwardStepsCount" >
			, sdw::FloatField< "ssrDepthMult" > >
	{
		WaterProfile( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		static std::string const & getName()
		{
			static std::string const name{ "C3D_WaterProfile" };
			return name;
		}

	public:
		auto dampeningFactor()const { return getMember< "dampeningFactor" >(); }
		auto depthSofteningDistance()const { return getMember< "depthSofteningDistance" >(); }
		auto refractionDistortionFactor()const { return getMember< "refractionDistortionFactor" >(); }
		auto refractionHeightFactor()const { return getMember< "refractionHeightFactor" >(); }
		auto refractionDistanceFactor()const { return getMember< "refractionDistanceFactor" >(); }
		auto ssrStepSize()const { return getMember< "ssrStepSize" >(); }
		auto ssrForwardStepsCount()const { return getMember< "ssrForwardStepsCount" >(); }
		auto ssrBackwardStepsCount()const { return getMember< "ssrBackwardStepsCount" >(); }
		auto ssrDepthMult()const { return getMember< "ssrDepthMult" >(); }
	};

	class WaterProfiles
		: public c3d::BufferT< WaterProfile >
	{
	public:
		explicit WaterProfiles( sdw::ShaderWriter & writer
			, uint32_t binding
			, uint32_t set
			, bool enable = true );

		static castor3d::ShaderBufferUPtr create( castor3d::RenderDevice const & device );
		static void update( castor3d::ShaderBuffer & buffer
			, castor3d::Pass const & pass );
		static c3d::BufferBaseUPtr declare( sdw::ShaderWriter & writer
			, uint32_t binding
			, uint32_t set );
	};
}

#endif