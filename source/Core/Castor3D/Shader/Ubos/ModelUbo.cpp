#include "Castor3D/Shader/Ubos/ModelUbo.hpp"

namespace castor3d
{
	uint32_t const ModelUbo::BindingPoint = 7u;
	castor::String const ModelUbo::BufferModel = cuT( "Model" );
	castor::String const ModelUbo::ShadowReceiver = cuT( "c3d_shadowReceiver" );
	castor::String const ModelUbo::MaterialIndex = cuT( "c3d_materialIndex" );
	castor::String const ModelUbo::EnvironmentIndex = cuT( "c3d_envMapIndex" );
}
