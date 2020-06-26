/*
See LICENSE file in root folder
*/
#ifndef ___CU_NoiseModule_HPP___
#define ___CU_NoiseModule_HPP___
#pragma once

#include "CastorUtils/CastorUtils.hpp"

namespace castor
{
	/**@name Noise */
	//@{
	/**
	*\~english
	*\brief
	*	3D Perlin noise generator.
	*\~french
	*\brief
	*	Générateur de bruit 3D de Perlin.
	*/
	template< typename T >
	class PerlinNoiseT;
	/**
	*\~english
	*\brief
	*	3D Simplex noise generator.
	*\~french
	*\brief
	*	Générateur de bruit 3D Simplex.
	*/
	template< typename T >
	class SimplexNoiseT;
	/**
	*\~english
	*\brief
	*	Fractal noise generator for PerlinNoise or SimplexNoise.
	*\~french
	*\brief
	*	Générateur de bruit fractal pour PerlinNoise et SimplexNoise.
	*/
	template< typename NoiseT >
	class FractalNoiseT;
	/**
	*\~english
	*\brief
	*	Blue noise texture generator.
	*\~french
	*\brief
	*	Générateur texture de blue noise.
	*/
	class BlueNoise;
	/**
	*\~english
	*\brief
	*	White noise texture generator.
	*\~french
	*\brief
	*	Générateur texture de white noise.
	*/
	class WhiteNoise;
	//@}
}

#endif
