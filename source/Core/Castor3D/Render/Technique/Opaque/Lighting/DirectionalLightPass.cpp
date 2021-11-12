#include "Castor3D/Render/Technique/Opaque/Lighting/DirectionalLightPass.hpp"

#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	ShaderPtr DirectionalLightPass::getVertexShaderSource()
	{
		using namespace sdw;
		VertexWriter writer;

		// Shader inputs
		UBO_MATRIX( writer, uint32_t( LightPassLgtIdx::eMatrix ), 1u );
		auto position = writer.declInput< Vec2 >( "position", 0u );

		writer.implementMainT< VoidT, VoidT >( [&]( VertexIn in
			, VertexOut out )
			{
				out.vtx.position = c3d_matrixData.viewToProj( vec4( position, 0.0_f, 1.0_f ) );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
