#include "Castor3D/Miscellaneous/GpuInformations.hpp"

namespace castor3d
{
	castor::OutputStream & operator<<( castor::OutputStream & stream, GpuInformations const & object )
	{
		auto support = []( bool supported )
		{
			return ( supported ? "supported" : "not supported" );
		};
		castor::Array< castor::String, size_t( GpuMin::eCount ) > const minNames
		{
			cuT( "Min buffer map size" ),
			cuT( "Min uniform buffer offset alignment" ),
		};
		castor::Array< castor::String, size_t( GpuMax::eCount ) > const maxNames
		{
			cuT( "Max image 1D size" ),
			cuT( "Max image 2D size" ),
			cuT( "Max image 3D size" ),
			cuT( "Max image cube size" ),
			cuT( "Max image layers count" ),
			cuT( "Max sampler lod bias" ),
			cuT( "Max clip distances" ),
			cuT( "Max framebuffer width" ),
			cuT( "Max framebuffer height" ),
			cuT( "Max framebuffer layers" ),
			cuT( "Max framebuffer samples" ),
			cuT( "Max texel buffer range" ),
			cuT( "Max uniform buffer range" ),
			cuT( "Max storage buffer range" ),
			cuT( "Max viewport width" ),
			cuT( "Max viewport height" ),
			cuT( "Max viewports count" ),
			cuT( "Max work group count X" ),
			cuT( "Max work group count Y" ),
			cuT( "Max work group count Z" ),
			cuT( "Max work group size X" ),
			cuT( "Max work group size Y" ),
			cuT( "Max work group size Z" ),
			cuT( "Max work group invocations" ),
			cuT( "Max mesh work group invocations" ),
			cuT( "Max mesh work group size X" ),
			cuT( "Max mesh work group size Y" ),
			cuT( "Max mesh work group size Z" ),
			cuT( "Max mesh output vertices" ),
			cuT( "Max mesh output primitives" ),
			cuT( "Max task work group invocations" ),
			cuT( "Max task work group size X" ),
			cuT( "Max task work group size Y" ),
			cuT( "Max task work group size Z" ),
		};
		stream << "GPU informations:" << "\n";
		stream << "    Vendor: " << object.getVendor() << "\n";
		stream << "    API name: " << object.getRenderer() << "\n";
		stream << "    API version: " << object.getVersion() << "\n";
		stream << "    Storage Buffers: " << support( object.hasFeature( GpuFeature::eShaderStorageBuffers ) ) << "\n";
		stream << "    Stereo Rendering: " << support( object.hasFeature( GpuFeature::eStereoRendering ) ) << "\n";
		stream << "    Vertex shaders: " << support( object.hasShaderType( VK_SHADER_STAGE_VERTEX_BIT ) ) << "\n";
		stream << "    Tessellation Control shaders: " << support( object.hasShaderType( VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT ) ) << "\n";
		stream << "    Tessellation Evaluation shaders: " << support( object.hasShaderType( VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT ) ) << "\n";
		stream << "    Geometry shaders: " << support( object.hasShaderType( VK_SHADER_STAGE_GEOMETRY_BIT ) ) << "\n";
		stream << "    Fragment shaders: " << support( object.hasShaderType( VK_SHADER_STAGE_FRAGMENT_BIT ) ) << "\n";
		stream << "    Compute shaders: " << support( object.hasShaderType( VK_SHADER_STAGE_COMPUTE_BIT ) ) << "\n";
		stream << "    Mesh shaders: " << support( object.hasShaderType( VK_SHADER_STAGE_MESH_BIT_NV ) ) << "\n";
		stream << "    Task shaders: " << support( object.hasShaderType( VK_SHADER_STAGE_TASK_BIT_NV ) ) << "\n";
		stream << "    Ray Generation shaders: " << support( object.hasShaderType( VK_SHADER_STAGE_RAYGEN_BIT_KHR ) ) << "\n";
		stream << "    Any Hit shaders: " << support( object.hasShaderType( VK_SHADER_STAGE_ANY_HIT_BIT_KHR ) ) << "\n";
		stream << "    Closest Hit shaders: " << support( object.hasShaderType( VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR ) ) << "\n";
		stream << "    Miss shaders: " << support( object.hasShaderType( VK_SHADER_STAGE_MISS_BIT_KHR ) ) << "\n";
		stream << "    Intersection shaders: " << support( object.hasShaderType( VK_SHADER_STAGE_INTERSECTION_BIT_KHR ) ) << "\n";
		stream << "    Callable shaders: " << support( object.hasShaderType( VK_SHADER_STAGE_CALLABLE_BIT_KHR ) ) << "\n";

		for ( size_t i = 0; i < size_t( GpuMin::eCount ); ++i )
		{
			stream << "    " << minNames[i] << ": " << object.getValue( GpuMin( i ) ) << "\n";
		}

		for ( size_t i = 0; i < size_t( GpuMax::eCount ); ++i )
		{
			stream << "    " << maxNames[i] << ": " << object.getValue( GpuMax( i ) ) << "\n";
		}

		stream << std::flush;
		return stream;
	}
}
