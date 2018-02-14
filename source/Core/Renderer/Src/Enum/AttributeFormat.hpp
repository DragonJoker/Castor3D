/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_AttributeFormat_HPP___
#define ___Renderer_AttributeFormat_HPP___
#pragma once

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
	*\brief
	*	Gets the size of the given element type.
	*\param[in] value
	*	The element type.
	*\return
	*	The size.
	*\~french
	*\brief
	*	Récupère la taille du type d'élément donné.
	*\param[in] value
	*	Le type d'élément.
	*\return
	*	La taille.
	*/
	inline uint32_t getSize( AttributeFormat value )
	{
		switch ( value )
		{
		case AttributeFormat::eFloat:
			return uint32_t( 1u * sizeof( float ) );

		case AttributeFormat::eVec2f:
			return uint32_t( 2u * sizeof( float ) );

		case AttributeFormat::eVec3f:
			return uint32_t( 3u * sizeof( float ) );

		case AttributeFormat::eVec4f:
			return uint32_t( 4u * sizeof( float ) );

		case AttributeFormat::eMat2f:
			return uint32_t( 2u * 2u * sizeof( float ) );

		case AttributeFormat::eMat3f:
			return uint32_t( 3u * 3u * sizeof( float ) );

		case AttributeFormat::eMat4f:
			return uint32_t( 4u * 4u * sizeof( float ) );

		case AttributeFormat::eColour:
			return uint32_t( 4 * sizeof( uint8_t ) );

		case AttributeFormat::eInt:
			return uint32_t( 1u * sizeof( int32_t ) );

		case AttributeFormat::eVec2i:
			return uint32_t( 2u * sizeof( int32_t ) );

		case AttributeFormat::eVec3i:
			return uint32_t( 3u * sizeof( int32_t ) );

		case AttributeFormat::eVec4i:
			return uint32_t( 4u * sizeof( int32_t ) );

		case AttributeFormat::eUInt:
			return uint32_t( 1u * sizeof( uint32_t ) );

		case AttributeFormat::eVec2ui:
			return uint32_t( 2u * sizeof( uint32_t ) );

		case AttributeFormat::eVec3ui:
			return uint32_t( 3u * sizeof( uint32_t ) );

		case AttributeFormat::eVec4ui:
			return uint32_t( 4u * sizeof( uint32_t ) );

		default:
			assert( false && "Unsupported vertex buffer attribute type." );
			throw std::runtime_error{ "Unsupported vertex buffer attribute type" };
		}

		return 0;
	}
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
	inline std::string getName( AttributeFormat p_type )
	{
		switch ( p_type )
		{
		case AttributeFormat::eFloat:
			return "float";

		case AttributeFormat::eVec2f:
			return "vec2f";

		case AttributeFormat::eVec3f:
			return "vec3f";

		case AttributeFormat::eVec4f:
			return "vec4f";

		case AttributeFormat::eMat2f:
			return "mat2f";

		case AttributeFormat::eMat3f:
			return "mat3f";

		case AttributeFormat::eMat4f:
			return "mat4f";

		case AttributeFormat::eInt:
			return "int";

		case AttributeFormat::eVec2i:
			return "vec2i";

		case AttributeFormat::eVec3i:
			return "vec3i";

		case AttributeFormat::eVec4i:
			return "vec4i";

		case AttributeFormat::eUInt:
			return "uint";

		case AttributeFormat::eVec2ui:
			return "vec2ui";

		case AttributeFormat::eVec3ui:
			return "vec3ui";

		case AttributeFormat::eVec4ui:
			return "vec4ui";

		case AttributeFormat::eColour:
			return "colour";

		default:
			assert( false && "Unsupported AttributeFormat." );
			throw std::runtime_error{ "Unsupported AttributeFormat" };
		}

		return 0;
	}
}

#endif
