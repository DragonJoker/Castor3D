#include "ACESToneMapping/ACESToneMapping.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>
#include <Castor3D/Shader/Ubos/HdrConfigUbo.hpp>

#include <ShaderWriter/Source.hpp>

namespace aces
{
	castor::String ToneMapping::Type = cuT( "aces" );
	castor::String ToneMapping::Name = cuT( "ACES Tone Mapping" );

	castor3d::ShaderPtr ToneMapping::create( castor3d::Engine & engine )
	{
		using namespace sdw;
		FragmentWriter writer{ &engine.getShaderAllocator() };

		// Shader inputs
		C3D_HdrConfig( writer, 0u, 0u );
		auto c3d_mapHdr = writer.declCombinedImg< FImg2DRgba32 >( "c3d_mapHdr", 1u, 0u );
		auto vtx_texture = writer.declInput< Vec2 >( "vtx_texture", 0u );

		// Shader outputs
		auto outColour = writer.declOutput< Vec4 >( "outColour", 0 );

		auto acesToneMap = writer.implementFunction< Vec3 >( "acesToneMap"
			, [&]( Vec3 const & x )
			{
				const float a = 2.51f;
				const float b = 0.03f;
				const float c = 2.43f;
				const float d = 0.59f;
				const float e = 0.14f;
				writer.returnStmt( ( x * ( a * x + b ) ) / ( x * ( c * x + d ) + e ) );
			}
			, InVec3{ writer, "x" } );

		writer.implementMainT< VoidT, VoidT >( [&]( FragmentIn in
			, FragmentOut out )
			{
				auto hdrColor = writer.declLocale( "hdrColor"
					, c3d_mapHdr.sample( vtx_texture ).rgb() );
				auto current = writer.declLocale( "current"
					, acesToneMap( hdrColor * c3d_hdrConfigData.getExposure() ) );

				outColour = vec4( c3d_hdrConfigData.applyGamma( current ), 1.0_f );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
