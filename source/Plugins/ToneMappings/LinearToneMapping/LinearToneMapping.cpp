#include "LinearToneMapping/LinearToneMapping.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/Ubos/HdrConfigUbo.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace sdw;
using namespace castor3d;

namespace Linear
{
	String ToneMapping::Type = cuT( "linear" );
	String ToneMapping::Name = cuT( "Linear Tone Mapping" );

	ToneMapping::ToneMapping( Engine & engine
		, RenderDevice const & device
		, HdrConfigUbo & hdrConfigUbo
		, Parameters const & parameters )
		: castor3d::ToneMapping{ Type, Name, engine, device, hdrConfigUbo, parameters }
	{
	}

	ToneMapping::~ToneMapping()
	{
	}

	ToneMappingSPtr ToneMapping::create( Engine & engine
		, castor3d::RenderDevice const & device
		, HdrConfigUbo & hdrConfigUbo
		, Parameters const & parameters )
	{
		return std::make_shared< ToneMapping >( engine
			, device
			, hdrConfigUbo
			, parameters );
	}

	castor3d::ShaderPtr ToneMapping::doCreate()
	{
		FragmentWriter writer;

		// Shader inputs
		UBO_HDR_CONFIG( writer, 0u, 0u );
		auto c3d_mapHdr = writer.declSampledImage< FImg2DRgba32 >( "c3d_mapHdr", 1u, 0u );
		auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

		// Shader outputs
		auto pxl_rgb = writer.declOutput< Vec4 >( "pxl_rgb", 0 );

		shader::Utils utils{ writer };
		utils.declareApplyGamma();

		writer.implementFunction< sdw::Void >( "main"
			, [&]()
			{
				auto hdrColor = writer.declLocale( "hdrColor"
					, texture( c3d_mapHdr, vtx_texture ).rgb() );
				hdrColor *= vec3( c3d_exposure );
				pxl_rgb = vec4( utils.applyGamma( c3d_gamma, hdrColor ), 1.0_f );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}

	void ToneMapping::doDestroy()
	{
	}

	void ToneMapping::doCpuUpdate()
	{
	}
}
