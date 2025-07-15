/*
See LICENSE file in root folder
*/
#ifndef ___C3D_HBD_TONE_MAPPING_H___
#define ___C3D_HBD_TONE_MAPPING_H___

#include <Castor3D/Render/ToneMapping/ToneMapping.hpp>

namespace HejlBurgessDawson
{
	/**
	\~english
	\brief		Implements Jim Hejl and Richard Burgess-Dawson tone mapping algorithm.
	\~french
	\brief		Implémente l'algorithme de mappage de ton de Jim Hejl et Richard Burgess-Dawson.
	*/
	struct ToneMapping
	{
		static c3d::String Type;
		static c3d::MbString Name;
		static void create( ast::ShaderBuilder & builder );
	};
}

#endif

