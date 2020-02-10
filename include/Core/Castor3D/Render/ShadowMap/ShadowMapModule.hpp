/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShadowMapModule_H___
#define ___C3D_ShadowMapModule_H___

#include "Castor3D/Scene/Light/LightModule.hpp"

namespace castor3d
{
	/**@name ShadowMap */
	//@{

	class ShadowMap;
	class ShadowMapPass;

	CU_DeclareSmartPtr( ShadowMap );
	CU_DeclareSmartPtr( ShadowMapPass );

	using ShadowMapRefIds = std::pair< std::reference_wrapper< ShadowMap >, UInt32Array >;
	using ShadowMapRefArray = std::vector< ShadowMapRefIds >;
	using ShadowMapLightTypeArray = std::array< ShadowMapRefArray, size_t( LightType::eCount ) >;
	/**
	*\~english
	*\brief
	*	Writes the shadow maps descriptors to the given writes.
	*\~french
	*\brief
	*	Ecrit les descripteurs des shadow maps dans les writes donnés.
	*/
	void bindShadowMaps( ShadowMapRefArray const & shadowMaps
		, ashes::WriteDescriptorSetArray & writes
		, uint32_t & index );

	//@}
}

#endif
