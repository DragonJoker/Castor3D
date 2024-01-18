/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VctConfig_H___
#define ___C3D_VctConfig_H___

#include "VoxelizeModule.hpp"

#include "Castor3D/Limits.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

namespace castor3d
{
	struct VctConfig
	{
		C3D_API void accept( ConfigurationVisitorBase & visitor );

		C3D_API static void addParsers( castor::AttributeParsers & result );

		bool enabled{ false };
		bool enableConservativeRasterization{ false };
		bool enableOcclusion{ false };
		bool enableSecondaryBounce{ false };
		bool enableTemporalSmoothing{ false };
		castor::RangedValue< uint32_t > gridSize{ 2u, castor::makeRange( 2u, VctMaxTextureSize ) };
		//!\~english	The maximum distance for cones.
		//!\~french		La distance maximale pour les cones.
		float maxDistance{ 20.0f };
		//!\~english	The number of radiance cones used in voxel cone tracing.
		//!\~french		Le nombre de cones de radiance utilisés dans le voxel cone tracing.
		castor::RangedValue< uint32_t > numCones{ 2u, castor::makeRange( 1u, VctMaxDiffuseCones ) };
		//!\~english	The size of a ray step in voxel cone tracint.
		//!\~french		La taille d'un pas de rayon dans le voxel cone tracing.
		float rayStepSize{ 0.75f };
		float voxelSizeFactor{ 1.0f };
	};

	inline bool operator==( VctConfig const & lhs, VctConfig const & rhs )noexcept
	{
		return lhs.enabled == rhs.enabled
			&& lhs.enableConservativeRasterization == rhs.enableConservativeRasterization
			&& lhs.enableOcclusion == rhs.enableOcclusion
			&& lhs.enableSecondaryBounce == rhs.enableSecondaryBounce
			&& lhs.enableTemporalSmoothing == rhs.enableTemporalSmoothing
			&& lhs.gridSize == rhs.gridSize
			&& lhs.maxDistance == rhs.maxDistance
			&& lhs.numCones == rhs.numCones
			&& lhs.rayStepSize == rhs.rayStepSize
			&& lhs.voxelSizeFactor == rhs.voxelSizeFactor;
	}
}

#endif
