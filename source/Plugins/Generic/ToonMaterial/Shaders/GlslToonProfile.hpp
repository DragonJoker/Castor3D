/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslToonProfile_H___
#define ___C3D_GlslToonProfile_H___

#include <Castor3D/Render/RenderModule.hpp>
#include <Castor3D/Shader/Ubos/UbosModule.hpp>

#include <Castor3D/Shader/Shaders/GlslBuffer.hpp>

#include <ShaderWriter/MatTypes/Mat4.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>

namespace toon::shader
{
	namespace c3d = castor3d::shader;

	struct ToonProfile
		: public sdw::StructInstanceHelperT < "C3D_ToonProfile"
			, sdw::type::MemoryLayout::eStd430
			, sdw::FloatField< "edgeWidth" >
			, sdw::FloatField< "depthFactor" >
			, sdw::FloatField< "normalFactor" >
			, sdw::FloatField< "objectFactor" >
			, sdw::Vec4Field< "edgeColour" >
			, sdw::FloatField< "smoothBand" >
			, sdw::FloatField< "pad0" >
			, sdw::FloatField< "pad1" >
			, sdw::FloatField< "pad2" > >
	{
		friend class ToonProfiles;
		ToonProfile( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		static std::string const & getName()
		{
			static std::string const name{ "C3D_ToonProfile" };
			return name;
		}
		
		auto edgeWidth()const { return getMember< "edgeWidth" >(); }
		auto depthFactor()const { return getMember< "depthFactor" >(); }
		auto normalFactor()const { return getMember< "normalFactor" >(); }
		auto objectFactor()const { return getMember< "objectFactor" >(); }
		auto edgeColour()const { return getMember< "edgeColour" >(); }
		auto smoothBand()const { return getMember< "smoothBand" >(); }
	};

	class ToonProfiles
		: public c3d::BufferT< ToonProfile >
	{
	public:
		explicit ToonProfiles( sdw::ShaderWriter & writer
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
