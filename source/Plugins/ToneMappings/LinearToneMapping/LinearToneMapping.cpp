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

	castor3d::ShaderPtr ToneMapping::create()
	{
		FragmentWriter writer;

		// Shader inputs
		UBO_HDR_CONFIG( writer, 0u, 0u );
		auto c3d_mapHdr = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapHdr", 1u, 0u );
		auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

		// Shader outputs
		auto pxl_rgb = writer.declOutput< Vec4 >( "pxl_rgb", 0 );

		writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
			, FragmentOut out )
			{
				auto hdrColor = writer.declLocale( "hdrColor"
					, c3d_mapHdr.sample( vtx_texture ).rgb() );
				hdrColor *= vec3( c3d_hdrConfigData.getExposure() );
				pxl_rgb = vec4( c3d_hdrConfigData.applyGamma( hdrColor ), 1.0_f );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
