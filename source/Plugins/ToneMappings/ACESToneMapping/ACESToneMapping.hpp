/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ACES_ToneMapping_H___
#define ___C3D_ACES_ToneMapping_H___

#include <Castor3D/Render/ToneMapping/ToneMapping.hpp>

namespace aces
{
	/**
	\~english
	\brief		Implements Uncharted 2 tone mapping algorithm.
	\~french
	\brief		Implémente l'algorithme de mappage de ton d'Uncharted 2.
	*/
	struct ToneMapping
	{
		static castor::String Type;
		static castor::String Name;
		static castor3d::ShaderPtr create( castor3d::Engine & engine );
	};
}

#endif

