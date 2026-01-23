#include "HejlBurgessDawsonToneMapping/HejlBurgessDawsonToneMapping.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>
#include <Castor3D/Shader/Ubos/RenderUbo.hpp>
#include <Castor3D/Shader/Ubos/ColourGradingUbo.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

namespace HejlBurgessDawson
{
	namespace c3ds = c3d::shader;
	c3d::String ToneMapping::Type = cuT( "hejl" );
	c3d::MbString ToneMapping::Name = "Hejl Burgess Dawson Tone Mapping";

	ToneMapping::~ToneMapping()noexcept = default;

	ToneMapping::ToneMapping( c3d::ToneMapping const & parent
		, c3d::RenderDevice const &
		, c3d::Parameters )
		: c3d::ToneMappingImpl{ parent }
	{
	}

	void ToneMapping::getFragmentProgram( ast::ShaderBuilder & builder )
	{
		sdw::TraditionalGraphicsWriter writer{ builder };

		C3D_Render( writer, 0u, 0u );
		C3D_ColourGrading( writer, 1u, 0u );
		auto c3d_mapHdr = writer.declCombinedImg< FImg2DRgba16 >( "c3d_mapHdr", 2u, 0u );

		writer.implementEntryPointT< c3ds::Uv2FT, c3ds::Colour4FT >( [&writer, &c3d_colourGrading, &c3d_renderData, &c3d_mapHdr]( sdw::FragmentInT< c3ds::Uv2FT > const & in
			, sdw::FragmentOutT< c3ds::Colour4FT > const & out )
			{
				auto hdrColor = writer.declLocale( "hdrColor"
					, c3d_colourGrading.colourGrade( c3d_mapHdr.sample( in.uv() ).rgb() ) );
				hdrColor *= vec3( c3d_renderData.exposure() );
				auto x = writer.declLocale( "x"
					, max( hdrColor - 0.004_f, vec3( 0.0_f ) ) );
				out.colour() = vec4( ( x * ( 6.2f * x + 0.5f ) )
					/ ( x * ( 6.2f * x + 1.7f ) + 0.06f ), 1.0_f );
			} );
	}

	void ToneMapping::accept( c3d::ToneMappingVisitor & visitor )
	{
	}

	c3d::ToneMappingImplUPtr ToneMapping::create( c3d::ToneMapping const & parent, c3d::RenderDevice const & device, c3d::Parameters parameters )
	{
		return c3d::ToneMappingImplUPtr( c3d::makeRawUnique< ToneMapping >( parent, device, c3d::move( parameters ) ).release() );
	}
}
