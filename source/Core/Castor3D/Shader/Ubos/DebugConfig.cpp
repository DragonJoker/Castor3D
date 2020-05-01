#include "Castor3D/Shader/Ubos/DebugConfig.hpp"

#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"

namespace castor3d
{
	void DebugConfig::accept( PipelineVisitor & visitor )
	{
		visitor.visit( cuT( "Debug" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "Deferred" )
			, cuT( "Eye position" )
			, debugLightEye );
		visitor.visit( cuT( "Debug" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "Deferred" )
			, cuT( "VS Position" )
			, debugLightVSPosition );
		visitor.visit( cuT( "Debug" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "Deferred" )
			, cuT( "WS Position" )
			, debugLightWSPosition );
		visitor.visit( cuT( "Debug" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "Deferred" )
			, cuT( "WS Normal" )
			, debugLightWSNormal );
		visitor.visit( cuT( "Debug" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "Deferred" )
			, cuT( "Tex. Coord." )
			, debugLightTexCoord );
		visitor.visit( cuT( "Debug" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "Deferred" )
			, cuT( "Diffuse Lighting" )
			, debugDeferredDiffuseLighting );
		visitor.visit( cuT( "Debug" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "Deferred" )
			, cuT( "Specular Lighting" )
			, debugDeferredSpecularLighting );
		visitor.visit( cuT( "Debug" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "Deferred" )
			, cuT( "Deferred Occlusion" )
			, debugDeferredOcclusion );
		visitor.visit( cuT( "Debug" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "Deferred" )
			, cuT( "SSS Transmittance" )
			, debugDeferredSSSTransmittance );
		visitor.visit( cuT( "Debug" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "Deferred" )
			, cuT( "IBL" )
			, debugDeferredIBL );
		visitor.visit( cuT( "Debug" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "Deferred" )
			, cuT( "Normals" )
			, debugDeferredNormals );
		visitor.visit( cuT( "Debug" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "Deferred" )
			, cuT( "World Pos" )
			, debugDeferredWorldPos );
		visitor.visit( cuT( "Debug" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "Deferred" )
			, cuT( "View Pos" )
			, debugDeferredViewPos );
		visitor.visit( cuT( "Debug" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "Deferred" )
			, cuT( "Depth" )
			, debugDeferredDepth );
		visitor.visit( cuT( "Debug" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "Deferred" )
			, cuT( "Data1" )
			, debugDeferredData1 );
		visitor.visit( cuT( "Debug" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "Deferred" )
			, cuT( "Data2" )
			, debugDeferredData2 );
		visitor.visit( cuT( "Debug" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "Deferred" )
			, cuT( "Data3" )
			, debugDeferredData3 );
		visitor.visit( cuT( "Debug" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "Deferred" )
			, cuT( "Data4" )
			, debugDeferredData4 );
		visitor.visit( cuT( "Debug" )
			, VK_SHADER_STAGE_FRAGMENT_BIT
			, cuT( "Deferred" )
			, cuT( "Data5" )
			, debugDeferredData5 );
	}
}
