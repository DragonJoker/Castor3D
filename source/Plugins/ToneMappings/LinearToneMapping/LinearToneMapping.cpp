#include "LinearToneMapping/LinearToneMapping.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/Ubos/HdrConfigUbo.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <ShaderWriter/Source.hpp>

namespace Linear
{
	castor::String ToneMapping::Type = cuT( "linear" );
	castor::String ToneMapping::Name = cuT( "Linear Tone Mapping" );

	castor3d::ShaderPtr ToneMapping::create( castor3d::Engine & engine )
	{
		sdw::FragmentWriter writer{ &engine.getShaderAllocator() };

		// Shader inputs
		C3D_HdrConfig( writer, 0u, 0u );
		auto c3d_mapHdr = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapHdr", 1u, 0u );
		auto vtx_texture = writer.declInput< sdw::Vec2 >( "vtx_texture", 0u );

		// Shader outputs
		auto outColour = writer.declOutput< sdw::Vec4 >( "outColour", 0 );

		writer.implementMainT< sdw::VoidT, sdw::VoidT >( [&]( sdw::FragmentIn in
			, sdw::FragmentOut out )
			{
				auto hdrColor = writer.declLocale( "hdrColor"
					, c3d_mapHdr.sample( vtx_texture ).rgb() );
				hdrColor *= vec3( c3d_hdrConfigData.getExposure() );
				outColour = vec4( c3d_hdrConfigData.applyGamma( hdrColor ), 1.0_f );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
