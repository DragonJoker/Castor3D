/*
See LICENSE file in root folder
*/
#ifndef ___C3D_L_TONE_MAPPING_H___
#define ___C3D_L_TONE_MAPPING_H___

#include <Castor3D/Render/ToneMapping/ToneMapping.hpp>

namespace NoTM
{
	/**
	\~english
	\brief		Implements linear tone mapping algorithm.
	\~french
	\brief		Implémente l'algorithme de mappage de ton linéaire.
	*/
	struct ToneMapping
	{
		static castor::String Type;
		static castor::MbString Name;
		static void create( ast::ShaderBuilder & builder );
	};
}

#endif

