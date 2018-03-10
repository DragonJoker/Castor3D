/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_FormatProperties_HPP___
#define ___Renderer_FormatProperties_HPP___
#pragma once

#include "RendererPrerequisites.hpp"

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Specifies image format properties.
	*\~french
	*\brief
	*	Définit les propriétés de format d'une image.
	*/
	struct FormatProperties
	{
		FormatFeatureFlags linearTilingFeatures;
		FormatFeatureFlags optimalTilingFeatures;
		FormatFeatureFlags bufferFeatures;
	};
}

#endif
