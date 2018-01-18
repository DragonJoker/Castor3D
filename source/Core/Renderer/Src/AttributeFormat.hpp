/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_AttributeFormat_HPP___
#define ___Renderer_AttributeFormat_HPP___
#pragma once

#include <CastorUtilsPrerequisites.hpp>

namespace renderer
{
	/**
	*\brief
	*	Liste des formats d'attributs supportés.
	*/
	enum class AttributeFormat
	{
		eFloat,
		eVec2f,
		eVec3f,
		eVec4f,
		eMat2f,
		eMat3f,
		eMat4f,
		eInt,
		eVec2i,
		eVec3i,
		eVec4i,
		eUInt,
		eVec2ui,
		eVec3ui,
		eVec4ui,
		eColour,
	};
	/**
	 *\~english
	 *\brief		Gets the byte size of the given element type.
	 *\param[in]	type	The element type.
	 *\return		The size.
	 *\~french
	 *\brief		Récupère la taille en octets du type d'élément donné.
	 *\param[in]	type	Le type d'élément.
	 *\return		La taille.
	 */
	inline uint32_t getSize( AttributeFormat type )
	{
		switch ( type )
		{
		case renderer::AttributeFormat::eFloat:
			return uint32_t( 1u * sizeof( float ) );

		case renderer::AttributeFormat::eVec2f:
			return uint32_t( 2u * sizeof( float ) );

		case renderer::AttributeFormat::eVec3f:
			return uint32_t( 3u * sizeof( float ) );

		case renderer::AttributeFormat::eVec4f:
			return uint32_t( 4u * sizeof( float ) );

		case renderer::AttributeFormat::eMat2f:
			return uint32_t( 2u * 2u * sizeof( float ) );

		case renderer::AttributeFormat::eMat3f:
			return uint32_t( 3u * 3u * sizeof( float ) );

		case renderer::AttributeFormat::eMat4f:
			return uint32_t( 4u * 4u * sizeof( float ) );

		case renderer::AttributeFormat::eColour:
			return uint32_t( sizeof( uint32_t ) );

		case renderer::AttributeFormat::eInt:
			return uint32_t( 1u * sizeof( int32_t ) );

		case renderer::AttributeFormat::eVec2i:
			return uint32_t( 2u * sizeof( int32_t ) );

		case renderer::AttributeFormat::eVec3i:
			return uint32_t( 3u * sizeof( int32_t ) );

		case renderer::AttributeFormat::eVec4i:
			return uint32_t( 4u * sizeof( int32_t ) );

		case renderer::AttributeFormat::eUInt:
			return uint32_t( 1u * sizeof( uint32_t ) );

		case renderer::AttributeFormat::eVec2ui:
			return uint32_t( 2u * sizeof( uint32_t ) );

		case renderer::AttributeFormat::eVec3ui:
			return uint32_t( 3u * sizeof( uint32_t ) );

		case renderer::AttributeFormat::eVec4ui:
			return uint32_t( 4u * sizeof( uint32_t ) );

		default:
			assert( false && "Unsupported vertex buffer attribute type." );
			throw std::runtime_error{ "Unsupported vertex buffer attribute type" };
		}

		return 0;
	}
	/**
	 *\~english
	 *\brief		gets the name of the given element type.
	 *\param[in]	p_type	The element type.
	 *\return		The name.
	 *\~french
	 *\brief		Récupère le nom du type d'élément donné.
	 *\param[in]	p_type	Le type d'élément.
	 *\return		Le nom.
	 */
	inline std::string getName( renderer::AttributeFormat p_type )
	{
		switch ( p_type )
		{
		case renderer::AttributeFormat::eFloat:
			return "float";

		case renderer::AttributeFormat::eVec2f:
			return "vec2f";

		case renderer::AttributeFormat::eVec3f:
			return "vec3f";

		case renderer::AttributeFormat::eVec4f:
			return "vec4f";

		case renderer::AttributeFormat::eMat2f:
			return "mat2f";

		case renderer::AttributeFormat::eMat3f:
			return "mat3f";

		case renderer::AttributeFormat::eMat4f:
			return "mat4f";

		case renderer::AttributeFormat::eInt:
			return "int";

		case renderer::AttributeFormat::eVec2i:
			return "vec2i";

		case renderer::AttributeFormat::eVec3i:
			return "vec3i";

		case renderer::AttributeFormat::eVec4i:
			return "vec4i";

		case renderer::AttributeFormat::eUInt:
			return "uint";

		case renderer::AttributeFormat::eVec2ui:
			return "vec2ui";

		case renderer::AttributeFormat::eVec3ui:
			return "vec3ui";

		case renderer::AttributeFormat::eVec4ui:
			return "vec4ui";

		case renderer::AttributeFormat::eColour:
			return "colour";

		default:
			assert( false && "Unsupported vertex buffer attribute type." );
			throw std::runtime_error{ "Unsupported vertex buffer attribute type" };
		}

		return 0;
	}
}

#endif
