/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VoxelSceneData_H___
#define ___C3D_VoxelSceneData_H___

#include "VoxelizeModule.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

namespace castor3d
{
	class VoxelSceneData
	{
	public:
		bool enabled{ false };
		bool enableConservativeRasterization{ false };
		bool enableOcclusion{ false };
		bool enableSecondaryBounce{ false };
		bool enableTemporalSmoothing{ false };
		castor::RangedValue< uint32_t > gridSize{ 2u, castor::makeRange( 2u, 512u ) };
		//!\~english	The maximum distance for cones.
		//!\~french		La distance maximale pour les cones.
		float maxDistance{ 20.0f };
		//!\~english	The number of radiance cones used in voxel cone tracing.
		//!\~french		Le nombre de cones de radiance utilisés dans le voxel cone tracing.
		castor::RangedValue< uint32_t > numCones{ 2u, castor::makeRange( 1u, 16u ) };
		//!\~english	The size of a ray step in voxel cone tracint.
		//!\~french		La taille d'un pas de rayon dans le voxel cone tracing.
		float rayStepSize{ 0.75f };
		float voxelSizeFactor{ 1.0f };
	};
}

#endif
