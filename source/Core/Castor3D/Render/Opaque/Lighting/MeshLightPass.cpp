#include "Castor3D/Render/Opaque/Lighting/MeshLightPass.hpp"

#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelDataUbo.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	ShaderPtr MeshLightPass::getVertexShaderSource()
	{
		using namespace sdw;
		VertexWriter writer;

		// Shader inputs
		C3D_Camera( writer, uint32_t( LightPassLgtIdx::eMatrix ), 1u );
		C3D_ModelData( writer, uint32_t( LightPassLgtIdx::eModelMatrix ), 1u );
		auto vertex = writer.declInput< Vec3 >( "position", 0u );

		writer.implementMainT< VoidT, VoidT >( [&]( VertexIn in
			, VertexOut out )
			{
				out.vtx.position = c3d_cameraData.worldToCurProj( c3d_modelData.modelToWorld( vec4( vertex, 1.0_f ) ) );
			} );

		return std::make_unique< ast::Shader >( std::move( writer.getShader() ) );
	}
}
