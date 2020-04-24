#ifndef ___CU_NoiseModule_HPP___
#define ___CU_NoiseModule_HPP___

#include <cstdint>

namespace castor
{
	/**
	*\~english
	*\brief
	*	2D Perlin noise generator.
	*\~french
	*\brief
	*	Générateur de bruit 2D de Perlin.
	*/
	template< typename T >
	class PerlinNoiseT;
	/**
	*\~english
	*\brief
	*	2D Simplex noise generator.
	*\~french
	*\brief
	*	Générateur de bruit 2D Simplex.
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
}

#endif
