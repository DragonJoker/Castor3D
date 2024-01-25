#include "ACESToneMapping/ACESToneMapping.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>
#include <Castor3D/Shader/Ubos/HdrConfigUbo.hpp>
#include <Castor3D/Shader/Ubos/ColourGradingUbo.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

namespace aces
{
	namespace c3d = castor3d::shader;
	castor::String ToneMapping::Type = cuT( "aces" );
	castor::MbString ToneMapping::Name = "ACES Tone Mapping";

	void ToneMapping::create( ast::ShaderBuilder & builder )
	{
		sdw::TraditionalGraphicsWriter writer{ builder };

		C3D_HdrConfig( writer, 0u, 0u );
		C3D_ColourGrading( writer, 1u, 0u );
		auto c3d_mapHdr = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapHdr", 2u, 0u );

		auto acesToneMap = writer.implementFunction< sdw::Vec3 >( "acesToneMap"
			, [&]( sdw::Vec3 const & x )
			{
				const float a = 2.51f;
				const float b = 0.03f;
				const float c = 2.43f;
				const float d = 0.59f;
				const float e = 0.14f;
				writer.returnStmt( ( x * ( a * x + b ) ) / ( x * ( c * x + d ) + e ) );
			}
			, sdw::InVec3{ writer, "x" } );

		writer.implementEntryPointT< c3d::Uv2FT, c3d::Colour4FT >( [&]( sdw::FragmentInT< c3d::Uv2FT > in
			, sdw::FragmentOutT< c3d::Colour4FT > out )
			{
				auto hdrColor = writer.declLocale( "hdrColor"
					, c3d_colourGrading.colourGrade( c3d_mapHdr.sample( in.uv() ).rgb() ) );
				auto current = writer.declLocale( "current"
					, acesToneMap( hdrColor * c3d_hdrConfigData.getExposure() ) );

				out.colour() = vec4( c3d_hdrConfigData.applyGamma( current ), 1.0_f );
			} );
	}
}
