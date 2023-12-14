/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ClustersConfig_H___
#define ___C3D_ClustersConfig_H___

#include "Castor3D/Render/Clustered/ClusteredModule.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>

namespace castor3d
{
	struct ClustersConfig
	{
		C3D_API ClustersConfig();

		C3D_API void accept( ConfigurationVisitorBase & visitor );

		C3D_API static void addParsers( castor::AttributeParsers & result );

		//!\~english	The activation status.
		//!\~french		Le statut d'activation.
		bool enabled{ true };
		bool dirty{ true };
		//!\~english	Tells if lights are put in the BVH.
		//!\~french		Dit si les sources lumineuses sont mises dans le BVH.
		castor::GroupChangeTracked< bool > useLightsBVH;
		//!\~english	Tells if the lights are sorted.
		//!\~french		Dit si les sources lumineuses sont triées.
		castor::GroupChangeTracked< bool > sortLights;
		//!\~english	Tells if the depth buffer is used to reduce affected clusters.
		//!\~french		Dit si le buffer de profondeur est utlisé pour réduire le nombre de clusters affectés.
		castor::GroupChangeTracked< bool > parseDepthBuffer;
		//!\~english	Clusters grid Z will be limited to lights AABB depth boundaries.
		//!\~french		Les Z de la grille de clusters seront limités aux limites de profondeur des AABB des sources lumineuses.
		castor::GroupChangeTracked< bool > limitClustersToLightsAABB;
		//!\~english	Use spot light bounding cone when assigning lights to clusters.
		//!\~french		Utiliser le cône englobant les spot lights lors de l'affectation des sources lumineuses aux clusters.
		castor::GroupChangeTracked< bool > useSpotBoundingCone;
		//!\~english	Use spot light tight bounding box when computing lights AABB.
		//!\~french		Utiliser la bounding box la plus petite possible lors du calcul des AABB des ousrces lumineuses.
		castor::GroupChangeTracked< bool > useSpotTightBoundingBox;
		//!\~english	Enable use of warp optimisation in the reduce lights AABB pass.
		//!\~french		Autoriser l'utilisation de l'optimisation des warps dans la passe de réduction des AABB des sources lumineuses.
		castor::GroupChangeTracked< bool > enableReduceWarpOptimisation;
		//!\~english	Enable use of warp optimisation in the build BVH pass.
		//!\~french		Autoriser l'utilisation de l'optimisation des warps dans la passe de construction du BVH.
		castor::GroupChangeTracked< bool > enableBVHWarpOptimisation;
		//!\~english	Enable sorting of clusters lights.
		//!\~french		Autoriser le tri des sources lumineuses dans les clusters.
		castor::GroupChangeTracked< bool > enablePostAssignSort;
		//!\~english	The clusters Z split scheme.
		//!\~french		Le mode découpage en Z des clusters.
		castor::GroupChangeTracked < ClusterSplitScheme > splitScheme;
		//!\~english	The biased exponential split scheme bias.
		//!\~french		Le décalage en mode de découpage exponentiel décalé.
		castor::GroupChangeTracked< float > bias;
	};

	C3D_API bool operator==( ClustersConfig const & lhs, ClustersConfig const & rhs );
}

#endif
