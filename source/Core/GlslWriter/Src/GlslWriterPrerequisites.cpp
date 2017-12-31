#include "GlslWriterPrerequisites.hpp"

#include "GlslWriter.hpp"

namespace glsl
{
	castor::String const TypeTraits< Boolean >::Name = cuT( "bool" );
	castor::String const TypeTraits< Int >::Name = cuT( "int" );
	castor::String const TypeTraits< UInt >::Name = cuT( "uint" );
	castor::String const TypeTraits< Float >::Name = cuT( "float" );
	castor::String const TypeTraits< BVec2 >::Name = cuT( "bvec2" );
	castor::String const TypeTraits< BVec3 >::Name = cuT( "bvec3" );
	castor::String const TypeTraits< BVec4 >::Name = cuT( "bvec4" );
	castor::String const TypeTraits< IVec2 >::Name = cuT( "ivec2" );
	castor::String const TypeTraits< IVec3 >::Name = cuT( "ivec3" );
	castor::String const TypeTraits< IVec4 >::Name = cuT( "ivec4" );
	castor::String const TypeTraits< Vec2 >::Name = cuT( "vec2" );
	castor::String const TypeTraits< Vec3 >::Name = cuT( "vec3" );
	castor::String const TypeTraits< Vec4 >::Name = cuT( "vec4" );
	castor::String const TypeTraits< BMat2 >::Name = cuT( "bmat2" );
	castor::String const TypeTraits< BMat3 >::Name = cuT( "bmat3" );
	castor::String const TypeTraits< BMat4 >::Name = cuT( "bmat4" );
	castor::String const TypeTraits< IMat2 >::Name = cuT( "imat2" );
	castor::String const TypeTraits< IMat3 >::Name = cuT( "imat3" );
	castor::String const TypeTraits< IMat4 >::Name = cuT( "imat4" );
	castor::String const TypeTraits< Mat2 >::Name = cuT( "mat2" );
	castor::String const TypeTraits< Mat3 >::Name = cuT( "mat3" );
	castor::String const TypeTraits< Mat4 >::Name = cuT( "mat4" );
	castor::String const TypeTraits< SamplerBuffer >::Name = cuT( "samplerBuffer" );
	castor::String const TypeTraits< Sampler1D >::Name = cuT( "sampler1D" );
	castor::String const TypeTraits< Sampler2D >::Name = cuT( "sampler2D" );
	castor::String const TypeTraits< Sampler3D >::Name = cuT( "sampler3D" );
	castor::String const TypeTraits< SamplerCube >::Name = cuT( "samplerCube" );
	castor::String const TypeTraits< Sampler2DRect >::Name = cuT( "sampler2DRect" );
	castor::String const TypeTraits< Sampler1DArray >::Name = cuT( "sampler1DArray" );
	castor::String const TypeTraits< Sampler2DArray >::Name = cuT( "sampler2DArray" );
	castor::String const TypeTraits< SamplerCubeArray >::Name = cuT( "samplerCubeArray" );
	castor::String const TypeTraits< Sampler1DShadow >::Name = cuT( "sampler1DShadow" );
	castor::String const TypeTraits< Sampler2DShadow >::Name = cuT( "sampler2DShadow" );
	castor::String const TypeTraits< SamplerCubeShadow >::Name = cuT( "samplerCubeShadow" );
	castor::String const TypeTraits< Sampler2DRectShadow >::Name = cuT( "sampler2DRectShadow" );
	castor::String const TypeTraits< Sampler1DArrayShadow >::Name = cuT( "sampler1DArrayShadow" );
	castor::String const TypeTraits< Sampler2DArrayShadow >::Name = cuT( "sampler2DArrayShadow" );
	castor::String const TypeTraits< SamplerCubeArrayShadow >::Name = cuT( "samplerCubeArrayShadow" );

	void writeLine( GlslWriter & p_writer, castor::String const & p_line )
	{
		p_writer << p_line << Endl{};
	}

	void registerName( GlslWriter & p_writer, castor::String const & p_name, TypeName p_type )
	{
		p_writer.registerName( p_name, p_type );
	}

	void checkNameExists( GlslWriter & p_writer, castor::String const & p_name, TypeName p_type )
	{
		p_writer.checkNameExists( p_name, p_type );
	}
}
