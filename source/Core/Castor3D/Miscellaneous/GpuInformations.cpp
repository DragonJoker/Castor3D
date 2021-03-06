#include "Castor3D/Miscellaneous/GpuInformations.hpp"

namespace castor3d
{
	std::ostream & operator<<( std::ostream & stream, GpuInformations const & object )
	{
		auto support = []( bool supported )
		{
			return ( supported ? "supported" : "not supported" );
		};
		std::array< std::string, size_t( GpuMax::eCount ) > const names
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
		};
		stream << "GPU informations:" << "\n";
		stream << "    Vendor: " << object.getVendor() << "\n";
		stream << "    API name: " << object.getRenderer() << "\n";
		stream << "    API version: " << object.getVersion() << "\n";
		stream << "    Storage Buffers: " << support( object.hasFeature( GpuFeature::eShaderStorageBuffers ) ) << "\n";
		stream << "    Stereo Rendering: " << support( object.hasFeature( GpuFeature::eStereoRendering ) ) << "\n";
		stream << "    Vertex shaders: " << object.hasShaderType( VK_SHADER_STAGE_VERTEX_BIT ) << "\n";
		stream << "    Tessellation Control shaders: " << object.hasShaderType( VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT ) << "\n";
		stream << "    Tessellation Evaluation shaders: " << object.hasShaderType( VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT ) << "\n";
		stream << "    Geometry shaders: " << object.hasShaderType( VK_SHADER_STAGE_GEOMETRY_BIT ) << "\n";
		stream << "    Fragment shaders: " << object.hasShaderType( VK_SHADER_STAGE_FRAGMENT_BIT ) << "\n";
		stream << "    Compute shaders: " << object.hasShaderType( VK_SHADER_STAGE_COMPUTE_BIT ) << "\n";

		for ( size_t i = 0; i < size_t( GpuMax::eCount ); ++i )
		{
			stream << "    " << names[i] << ": " << object.getMaxValue( GpuMax( i ) ) << "\n";
		}

		stream << std::flush;
		return stream;
	}
}
