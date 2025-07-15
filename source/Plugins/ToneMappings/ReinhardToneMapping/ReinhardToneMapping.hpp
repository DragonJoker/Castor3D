/*
See LICENSE file in root folder
*/
#ifndef ___C3D_REINHARD_TONE_MAPPING_H___
#define ___C3D_REINHARD_TONE_MAPPING_H___

#include <Castor3D/Render/ToneMapping/ToneMapping.hpp>

namespace Reinhard
{
	/**
	\~english
	\brief		Implements Reinhard tone mapping algorithm.
	\~french
	\brief		Implémente l'algorithme de mappage de ton de Reinhard
	*/
	struct ToneMapping
	{
		static c3d::String Type;
		static c3d::MbString Name;
		static void create( ast::ShaderBuilder & builder );
	};
}

#endif

