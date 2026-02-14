#include "ACESToneMapping/ACESToneMapping.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/Shaders/GlslBaseIO.hpp>
#include <Castor3D/Shader/Ubos/RenderUbo.hpp>
#include <Castor3D/Shader/Ubos/ColourGradingUbo.hpp>

#include <ShaderWriter/Source.hpp>
#include <ShaderWriter/TraditionalGraphicsWriter.hpp>

namespace aces
{
	namespace c3ds = c3d::shader;
	c3d::String ToneMapping::Type = cuT( "aces" );
	c3d::MbString ToneMapping::Name = "ACES Tone Mapping";

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

		writer.implementEntryPointT< c3ds::Uv2FT, c3ds::Colour4FT >( [&writer, &c3d_colourGrading, &c3d_renderData, &c3d_mapHdr, acesToneMap]( sdw::FragmentInT< c3ds::Uv2FT > const & in
			, sdw::FragmentOutT< c3ds::Colour4FT > const & out )
			{
				auto hdrColor = writer.declLocale( "hdrColor"
					, c3d_colourGrading.colourGrade( c3d_mapHdr.sample( in.uv() ).rgb() ) );
				auto current = writer.declLocale( "current"
					, acesToneMap( hdrColor * c3d_renderData.exposure() ) );

				out.colour() = vec4( c3d_renderData.applyGamma( current ), 1.0_f );
			} );
	}

	void ToneMapping::accept( c3d::ConfigurationVisitor & visitor )
	{
	}

	c3d::ToneMappingImplUPtr ToneMapping::create( c3d::ToneMapping const & parent, c3d::RenderDevice const & device, c3d::Parameters parameters )
	{
		return c3d::ToneMappingImplUPtr( c3d::makeRawUnique< ToneMapping >( parent, device, c3d::move( parameters ) ).release() );
	}
}
