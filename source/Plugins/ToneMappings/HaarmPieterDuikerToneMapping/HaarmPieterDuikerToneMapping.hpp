/*
See LICENSE file in root folder
*/
#ifndef ___C3D_HPD_TONE_MAPPING_H___
#define ___C3D_HPD_TONE_MAPPING_H___

#include <Castor3D/Render/ToneMapping/ToneMapping.hpp>

namespace HaarmPieterDuiker
{
	/**
	\~english
	\brief		Implements Haarm-Peter Duiker tone mapping algorithm.
	\~french
	\brief		Implàmente l'algorithme de mappage de ton de Haarm-Peter Duiker.
	*/
	struct ToneMapping
	{
		static castor::String Type;
		static castor::String Name;
		static castor3d::ShaderPtr create();
	};
}

#endif

