#include "Castor3D/Shader/Shaders/GlslRay.hpp"

#include <ShaderWriter/Writer.hpp>

namespace castor3d::shader
{
	//************************************************************************************************

	Intersection Intersection::create( std::string const & name
		, sdw::ShaderWriter & writer )
	{
		auto result = writer.declLocale< Intersection >( name );
		result.valid() = 0_b;
		result.point() = vec3( 0.0_f );
		return result;
	}

	//************************************************************************************************

	Ray Ray::create( std::string const & name
		, sdw::ShaderWriter & writer
		, sdw::Vec3 const & o
		, sdw::Vec3 const & d )
	{
		auto result = writer.declLocale< Ray >( name );
		result.origin = o;
		result.direction = d;
		return result;
	}

	//************************************************************************************************
}
