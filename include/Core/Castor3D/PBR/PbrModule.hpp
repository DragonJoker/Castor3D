/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PbrModule_H___
#define ___C3D_PbrModule_H___

#include "Castor3D/Castor3DModule.hpp"

namespace castor3d
{
	/**@name PBR */
	//@{

	/**
	*\~english
	*\brief
	*	Computes the convoluted BRDF 2D texture.
	*\~french
	*\brief
	*	Calcule la texture 2D contenant le BRDF convolu.
	*/
	class BrdfPrefilter;
	/**
	*\~english
	*\brief
	*	Prefilters an environment map, by generating its mipmap levels.
	*\~french
	*\brief
	*	Préfiltre une texture d'environnement, en générant ses différents niveaux de mipmap.
	*/
	class EnvironmentPrefilter;
	/**
	*\~english
	*\brief
	*	Holds environment maps used in IBL.
	*\~french
	*\brief
	*	Contient les textures d'environnement utilisées dans l'IBL.
	*/
	class IblTextures;
	/**
	*\~english
	*\brief
	*	Computes the radiance map from an environment map.
	*\~french
	*\brief
	*	Calcule la texture de radiance pour une texture d'environnement donnée.
	*/
	class RadianceComputer;

	CU_DeclareSmartPtr( IblTextures );

	//@}
}

#endif
