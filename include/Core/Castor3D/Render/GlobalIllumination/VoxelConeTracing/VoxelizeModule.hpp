/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VoxelizeModule_H___
#define ___C3D_VoxelizeModule_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Buffer/BufferModule.hpp"

namespace castor3d
{
	/**@name Render */
	//@{
	/**@name Technique */
	//@{
	/**@name Voxelize */
	//@{

	/*
	*\~english
	*\brief
	*	Copies the voxels buffer into a 3D texture.
	*\~french
	*\brief
	*	Copie le buffer de voxels dans une texture 3D.
	*/
	class VoxelBufferToTexture;
	/*
	*\~english
	*\brief
	*	Handles the voxelization of a scene.
	*\~french
	*\brief
	*	Gère la voxellisation d'une scène.
	*/
	class VoxelizePass;
	/**
	*\~english
	*\brief
	*	Handles secondary bounce.
	*\~french
	*\brief
	*	Gère le second rebond.
	*/
	class VoxelSecondaryBounce;
	/**
	*\~english
	*\brief
	*	Holds all data needed to voxelize a scene.
	*\~french
	*\brief
	*	Contient toutes les données pour voxelliser une scène.
	*/
	class Voxelizer;
	/**
	*\~english
	*\brief
	*	Voxel configuration.
	*\~french
	*\brief
	*	Configuration des voxels.
	*/
	struct VctConfig;

	CU_DeclareSmartPtr( castor3d, VoxelBufferToTexture, C3D_API );
	CU_DeclareSmartPtr( castor3d, VoxelizePass, C3D_API );
	CU_DeclareSmartPtr( castor3d, VoxelSecondaryBounce, C3D_API );
	CU_DeclareSmartPtr( castor3d, Voxelizer, C3D_API );

	struct Voxel
	{
		uint32_t colorMask;
		uint32_t normalMask;
	};

	//@}
	//@}
	//@}
}

#endif
