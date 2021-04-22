/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ModelUbo_H___
#define ___C3D_ModelUbo_H___

#include "UbosModule.hpp"

namespace castor3d
{
	class ModelUbo
	{
	public:
		using Configuration = ModelUboConfiguration;

	public:
		C3D_API static castor::String const BufferModel;
		C3D_API static castor::String const PrvMtxModel;
		C3D_API static castor::String const CurMtxModel;
		C3D_API static castor::String const MtxNormal;
		C3D_API static castor::String const ShadowReceiver;
		C3D_API static castor::String const MaterialIndex;
		C3D_API static castor::String const EnvironmentIndex;
	};
}

#define UBO_MODEL( writer, binding, set )\
	sdw::Ubo model{ writer\
		, castor3d::ModelUbo::BufferModel\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140 };\
	auto c3d_prvMtxModel = model.declMember< sdw::Mat4 >( castor3d::ModelUbo::PrvMtxModel );\
	auto c3d_curMtxModel = model.declMember< sdw::Mat4 >( castor3d::ModelUbo::CurMtxModel );\
	auto c3d_mtxNormal = model.declMember< sdw::Mat4 >( castor3d::ModelUbo::MtxNormal );\
	auto c3d_shadowReceiver = model.declMember< sdw::Int >( castor3d::ModelUbo::ShadowReceiver );\
	auto c3d_materialIndex = model.declMember< sdw::Int >( castor3d::ModelUbo::MaterialIndex );\
	auto c3d_envMapIndex = model.declMember< sdw::Int >( castor3d::ModelUbo::EnvironmentIndex );\
	model.end()

#endif
