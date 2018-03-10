/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_ImageCreateFlag_HPP___
#define ___Renderer_ImageCreateFlag_HPP___
#pragma once

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Additional parameters to image creation.
	*\~french
	*\brief
	*	Paramètres additionnels à la création d'une image.
	*/
	enum class ImageCreateFlag
		: uint32_t
	{
		//!\~english	The image will be backed using sparse memory binding.
		eSparseBinding = 0x00000001,
		//!\~english	The image can be partially backed using sparse memory binding.
		eSparseResidency = 0x00000002,
		//!\~english	The image will be backed using sparse memory binding with memory ranges that might also simultaneously be backing another image (or another portion of the same image).
		eSparseAliased = 0x00000004,
		//!\~english	The image can be used to create an image view with a different format from the image.
		eMutableFormat = 0x00000008,
		//!\~english	The image can be used to create an image view of type renderer::TextureType::eCube or renderer::TextureType::eCubeArray.
		eCubeCompatible = 0x00000010,
	};
	Utils_ImplementFlag( ImageCreateFlag )
}

#endif
