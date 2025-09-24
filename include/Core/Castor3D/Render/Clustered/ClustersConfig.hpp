/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ClustersConfig_H___
#define ___C3D_ClustersConfig_H___

#include "Castor3D/Render/Clustered/ClusteredModule.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace c3d
{
	enum class ClusterDebugDisplay
	{
		eNone = 0,
		eClustersAABB = 1,
		eLightsAABB = 2,
		eLightsBVH = 3,
		CU_ScopedEnumBounds( eNone, eLightsBVH )
	};

	struct ClustersConfig
	{
		C3D_API ClustersConfig();

		C3D_API void accept( ConfigurationVisitorBase & visitor );

		C3D_API static void addParsers( AttributeParsers & result );

		//!\~english	The activation status.
		//!\~french		Le statut d'activation.
		bool enabled{ true };
		bool dirty{ true };
		//!\~english	Enable use of warp optimisation in the reduce lights AABB pass.
		//!\~french		Autoriser l'utilisation de l'optimisation des warps dans la passe de réduction des AABB des sources lumineuses.
		GroupChangeTracked< bool > enableReduceWarpOptimisation;
		//!\~english	Enable wave intrinsics when parsing of clusters lights.
		//!\~french		Autoriser les wave intrinsics lors de l'utilisation des sources lumineuses dans les clusters.
		GroupChangeTracked< bool > enableWaveIntrinsics;
		//!\~english	Locks clusters frustum, for debug purpose.
		//!\~french		Verrouille le frustum des clusters, pour le débogage.
		GroupChangeTracked< bool > lockClustersFrustum;
		//!\~english	Debug display mode.
		//!\~french		Mode d'affichage de debug.
		GroupChangeTracked< ClusterDebugDisplay > debugDisplay;
		//!\~english	The clusters Z split scheme.
		//!\~french		Le mode découpage en Z des clusters.
		GroupChangeTracked < ClusterSplitScheme > splitScheme;
		//!\~english	The hybrid split scheme minimal threshold distance.
		//!\~french		La distance minimale en mode de découpage hybride.
		GroupChangeTracked< float > minDistance;

	private:
		friend bool operator==( ClustersConfig const & lhs, ClustersConfig const & rhs )noexcept
		{
			return lhs.enabled == rhs.enabled
				&& lhs.enableReduceWarpOptimisation == rhs.enableReduceWarpOptimisation
				&& lhs.splitScheme == rhs.splitScheme
				&& lhs.minDistance == rhs.minDistance;
		}
	};
}

#endif
