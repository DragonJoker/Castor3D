#include "LinearToneMapping/LinearToneMapping.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>
#include <Castor3D/Shader/Ubos/HdrConfigUbo.hpp>
#include <Castor3D/Shader/Ubos/ColourGradingUbo.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

namespace Linear
{
	namespace c3d = castor3d::shader;
	castor::String ToneMapping::Type = cuT( "linear" );
	castor::MbString ToneMapping::Name = "Linear Tone Mapping";

	void ToneMapping::create( ast::ShaderBuilder & builder )
	{
		sdw::TraditionalGraphicsWriter writer{ builder };

		C3D_HdrConfig( writer, 0u, 0u );
		C3D_ColourGrading( writer, 1u, 0u );
		auto c3d_mapHdr = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapHdr", 2u, 0u );

		writer.implementEntryPointT< c3d::Uv2FT, c3d::Colour4FT >( [&]( sdw::FragmentInT< c3d::Uv2FT > in
			, sdw::FragmentOutT< c3d::Colour4FT > out )
			{
				auto hdrColor = writer.declLocale( "hdrColor"
					, c3d_colourGrading.colourGrade( c3d_mapHdr.sample( in.uv() ).rgb() ) );
				hdrColor *= vec3( c3d_hdrConfigData.getExposure() );
				out.colour() = vec4( c3d_hdrConfigData.applyGamma( hdrColor ), 1.0_f );
			} );
	}
}
