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
		C3D_API static uint32_t const BindingPoint;
		//!\~english	Name of the model frame variable buffer.
		//!\~french		Nom du frame variable buffer contenant les données de modèle.
		C3D_API static castor::String const BufferModel;
		//!\~english	Name of the shadow receiver status frame variable.
		//!\~french		Nom de la frame variable contenant le statut de réception d'ombres.
		C3D_API static castor::String const ShadowReceiver;
		//!\~english	Name of the material index frame variable.
		//!\~french		Nom de la frame variable contenant l'indice du matériau.
		C3D_API static castor::String const MaterialIndex;
		//!\~english	Name of the material environment index frame variable.
		//!\~french		Nom de la frame variable contenant l'indice de la map d'environnement du matériau.
		C3D_API static castor::String const EnvironmentIndex;
	};
}

#define UBO_MODEL( writer, binding, set )\
	sdw::Ubo model{ writer\
		, castor3d::ModelUbo::BufferModel\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140 };\
	auto c3d_shadowReceiver = model.declMember< sdw::Int >( castor3d::ModelUbo::ShadowReceiver );\
	auto c3d_materialIndex = model.declMember< sdw::Int >( castor3d::ModelUbo::MaterialIndex );\
	auto c3d_envMapIndex = model.declMember< sdw::Int >( castor3d::ModelUbo::EnvironmentIndex );\
	model.end()

#endif
