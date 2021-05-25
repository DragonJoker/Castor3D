#include "ReinhardToneMapping/ReinhardToneMapping.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>
#include <Castor3D/Shader/Ubos/HdrConfigUbo.hpp>

#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace sdw;
using namespace castor3d;

namespace Reinhard
{
	String ToneMapping::Type = cuT( "reinhard" );
	String ToneMapping::Name = cuT( "Reinhard Tone Mapping" );

	castor3d::ShaderPtr ToneMapping::create()
	{
		using namespace sdw;
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
					, c3d_mapHdr.sample( vtx_texture ).rgb() );
				// Exposure tone mapping
				auto mapped = writer.declLocale( "mapped"
					, vec3( Float( 1.0f ) ) - exp( -hdrColor * c3d_hdrConfigData.getExposure() ) );
				// Gamma correction
				pxl_rgb = vec4( c3d_hdrConfigData.applyGamma( mapped ), 1.0_f );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
