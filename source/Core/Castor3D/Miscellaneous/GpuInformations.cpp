#include "Castor3D/Miscellaneous/GpuInformations.hpp"

namespace castor3d
{
	std::ostream & operator<<( std::ostream & stream, GpuInformations const & object )
	{
		auto support = []( bool supported )
		{
			return ( supported ? "supported" : "not supported" );
		};
		std::array< std::string, size_t( GpuMin::eCount ) > const minNames
		{
			"Min buffer map size",
			"Min uniform buffer offset alignment",
		};
		std::array< std::string, size_t( GpuMax::eCount ) > const maxNames
		{
			"Max image 1D size",
			"Max image 2D size",
			"Max image 3D size",
			"Max image cube size",
			"Max image layers count",
			"Max sampler lod bias",
			"Max clip distances",
			"Max framebuffer width",
			"Max framebuffer height",
			"Max framebuffer layers",
			"Max framebuffer samples",
			"Max texel buffer range",
			"Max uniform buffer range",
			"Max storage buffer range",
			"Max viewport width",
			"Max viewport height",
			"Max viewports count",
			"Max work group count X",
			"Max work group count Y",
			"Max work group count Z",
			"Max work group size X",
			"Max work group size Y",
			"Max work group size Z",
			"Max work group invocations",
			"Max mesh work group invocations",
			"Max mesh work group size X",
			"Max mesh work group size Y",
			"Max mesh work group size Z",
			"Max mesh total memory size",
			"Max mesh output vertices",
			"Max mesh output primitives",
			"Max draw mesh tasks count",
			"Max task work group invocations",
			"Max task work group size X",
			"Max task work group size Y",
			"Max task work group size Z",
			"Max task total memory size",
			"Max task output count",
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
