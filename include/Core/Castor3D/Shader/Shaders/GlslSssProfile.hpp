/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslSssProfile_H___
#define ___C3D_GlslSssProfile_H___

#include "Castor3D/Limits.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"
#include "Castor3D/Shader/Shaders/GlslBuffer.hpp"

#include <ShaderWriter/BaseTypes/Array.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>

namespace castor3d::shader
{
	struct SssProfile
		: public sdw::StructInstanceHelperT< "C3D_SssProfile"
			, sdw::type::MemoryLayout::eStd140
			, sdw::IntField< "transmittanceProfileSize" >
			, sdw::FloatField< "gaussianWidth" >
			, sdw::FloatField< "subsurfaceScatteringStrength" >
			, sdw::FloatField< "pad" >
			, sdw::Vec4ArrayField< "transmittanceProfile", TransmittanceProfileSize > >
	{
		inline SssProfile( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		auto transmittanceProfileSize()const { return getMember< "transmittanceProfileSize" >(); }
		auto gaussianWidth()const { return getMember< "gaussianWidth" >(); }
		auto subsurfaceScatteringStrength()const { return getMember< "subsurfaceScatteringStrength" >(); }
		auto transmittanceProfile()const { return getMember< "transmittanceProfile" >(); }
	};

	class SssProfiles
		: public BufferT< SssProfile >
	{
	public:
		C3D_API explicit SssProfiles( sdw::ShaderWriter & writer
			, uint32_t binding
			, uint32_t set
			, bool enable = true );

		SssProfile getProfile( sdw::UInt const & index )const
		{
			return BufferT< SssProfile >::getData( index - 1u );
		}
	};
}

#endif
