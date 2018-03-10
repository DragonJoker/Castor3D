/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_ComponentSwizzle_HPP___
#define ___Renderer_ComponentSwizzle_HPP___
#pragma once

namespace renderer
{
	/**
	*\~english
	*\brief
	*	Specifies how an image component is swizzled.
	*\~french
	*\brief
	*	Définit comment une composante d'une image est réarrangée.
	*/
	enum class ComponentSwizzle
		: uint32_t
	{
		//!\~english	The component is set to identity swizzle.
		//!\~french		La composante est inchangée.
		eIdentity,
		//!\~english	The component is set to 0.
		//!\~french		La composante est mise à 0
		eZero,
		//!\~english	The component is set to 1 or 1.0, depending on the pixel format.
		//!\~french		La composante est mise à 1 ou 1.0, selon le format des pixels.
		eOne,
		//!\~english	The component is set to the red component.
		//!\~french		La composante est définie à la composante rouge.
		eR,
		//!\~english	The component is set to the green component.
		//!\~french		La composante est définie à la composante verte.
		eG,
		//!\~english	The component is set to the blue component.
		//!\~french		La composante est définie à la composante bleue.
		eB,
		//!\~english	The component is set to the alpha component.
		//!\~french		La composante est définie à la composante alpha.
		eA,
	};
	Utils_ImplementFlag( ComponentSwizzle )
	/**
	*\~english
	*\brief
	*	Gets the name of the given element type.
	*\param[in] value
	*	The element type.
	*\return
	*	The name.
	*\~french
	*\brief
	*	Récupère le nom du type d'élément donné.
	*\param[in] value
	*	Le type d'élément.
	*\return
	*	Le nom.
	*/
	inline std::string getName( ComponentSwizzle value )
	{
		switch ( value )
		{
		case ComponentSwizzle::eIdentity:
			return "identity";

		case ComponentSwizzle::eZero:
			return "zero";

		case ComponentSwizzle::eOne:
			return "one";

		case ComponentSwizzle::eR:
			return "r";

		case ComponentSwizzle::eG:
			return "g";

		case ComponentSwizzle::eB:
			return "b";

		case ComponentSwizzle::eA:
			return "a";

		default:
			assert( false && "Unsupported ComponentSwizzle." );
			throw std::runtime_error{ "Unsupported ComponentSwizzle" };
		}

		return 0;
	}
}

#endif
