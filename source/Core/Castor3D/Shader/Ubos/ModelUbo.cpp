#include "Castor3D/Shader/Ubos/ModelUbo.hpp"

namespace castor3d
{
	castor::String const ModelUbo::BufferModel = cuT( "Model" );
	castor::String const ModelUbo::PrvMtxModel = cuT( "c3d_prvMtxModel" );
	castor::String const ModelUbo::CurMtxModel = cuT( "c3d_curMtxModel" );
	castor::String const ModelUbo::MtxNormal = cuT( "c3d_mtxNormal" );
	castor::String const ModelUbo::ShadowReceiver = cuT( "c3d_shadowReceiver" );
	castor::String const ModelUbo::MaterialIndex = cuT( "c3d_materialIndex" );
	castor::String const ModelUbo::EnvironmentIndex = cuT( "c3d_envMapIndex" );
}
