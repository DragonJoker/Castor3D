/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderVolumetricModule_H___
#define ___C3D_RenderVolumetricModule_H___

#include "Castor3D/Castor3DModule.hpp"

namespace c3d
{
	/**@name Render */
	//@{
	/**@name Volumetric */
	//@{
	static u32 constexpr FroxelIntegrateTileSize = 64u;
	/**
	*\~english
	*\brief
	*	Froxels configuration.
	*\~french
	*\brief
	*	Configuration des froxels.
	*/
	struct FroxelsConfig;
	/**
	*\~english
	*\brief
	*	The buffer containing the clusters.
	*\~french
	*\brief
	*	Le buffer contenant les clusters.
	*/
	class FrustumFroxels;
	/**
	*\~english
	*\brief
	*	Class in charge of registering volume component plugins.
	*\~french
	*\brief
	*	Classe en charge de l'enregistrement des plugins de composants de vo:lumes.
	*/
	class VolumeComponentRegister;
	/**
	*\~english
	*\brief
	*	Class in charge of rendering all volumetrics.
	*\~french
	*\brief
	*	Classe en charge du rendu des volumes.
	*/
	class VolumetricRendering;
	/**
	\~english
	\brief		Plugin for a volume component.
	\~french
	\brief		Plugin pour un composant de volume.
	*/
	class VolumeComponentPlugin;

	/** @cond !Doxygen */
	CU_DeclareSmartPtr( c3d, VolumeComponentPlugin, C3D_API );
	CU_DeclareSmartPtr( c3d, VolumeComponentRegister, C3D_API );
	/** @endcond */

	using CreateVolumeComponentPlugin = Function< VolumeComponentPluginUPtr( VolumeComponentRegister const & ) >;

	namespace shader
	{
		class VolumeComponentShader;

		using VolumeComponentShaderPtr = RawUniquePtr< VolumeComponentShader >;
	};

	//@}
	//@}
}

#endif
