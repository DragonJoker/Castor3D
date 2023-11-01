#include "LinearToneMapping/LinearToneMapping.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>
#include <Castor3D/Shader/Ubos/HdrConfigUbo.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

namespace Linear
{
	namespace c3d = castor3d::shader;
	castor::String ToneMapping::Type = cuT( "linear" );
	castor::String ToneMapping::Name = cuT( "Linear Tone Mapping" );

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
				hdrColor *= vec3( c3d_hdrConfigData.getExposure() );
				out.colour() = vec4( c3d_hdrConfigData.applyGamma( hdrColor ), 1.0_f );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
