#include "ReinhardToneMapping/ReinhardToneMapping.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>
#include <Castor3D/Shader/Ubos/HdrConfigUbo.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

namespace Reinhard
{
	namespace c3d = castor3d::shader;
	castor::String ToneMapping::Type = cuT( "reinhard" );
	castor::String ToneMapping::Name = cuT( "Reinhard Tone Mapping" );

	castor3d::ShaderPtr ToneMapping::create( castor3d::Engine & engine )
	{
		sdw::TraditionalGraphicsWriter writer{ &engine.getShaderAllocator() };

		C3D_HdrConfig( writer, 0u, 0u );
		auto c3d_mapHdr = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapHdr", 1u, 0u );

		castor3d::ToneMapping::getVertexProgram( writer );

		writer.implementEntryPointT< c3d::Uv2FT, c3d::Colour4FT >( [&]( sdw::FragmentInT< c3d::Uv2FT > in
			, sdw::FragmentOutT< c3d::Colour4FT > out )
			{
				auto hdrColor = writer.declLocale( "hdrColor"
					, c3d_mapHdr.sample( in.uv() ).rgb() );
				// Exposure tone mapping
				auto mapped = writer.declLocale( "mapped"
					, vec3( 1.0_f ) - exp( -hdrColor * c3d_hdrConfigData.getExposure() ) );
				// Gamma correction
				out.colour() = vec4( c3d_hdrConfigData.applyGamma( mapped ), 1.0_f );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
