/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_ComponentMapping_HPP___
#define ___Renderer_ComponentMapping_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Structure specifying a colour component mapping.
	*\~french
	*\brief
	*	Spécifie le mapping des composantes couleur.
	*/
	struct ComponentMapping
	{
		ComponentSwizzle r{ ComponentSwizzle::eIdentity };
		ComponentSwizzle g{ ComponentSwizzle::eIdentity };
		ComponentSwizzle b{ ComponentSwizzle::eIdentity };
		ComponentSwizzle a{ ComponentSwizzle::eIdentity };
	};
}

#endif
