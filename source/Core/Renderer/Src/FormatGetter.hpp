/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#pragma once
#include <Math/SquareMatrix.hpp>

namespace renderer
{
	namespace details
	{
		template< typename T >
		struct FormatGetter;

		template<>
		struct FormatGetter< float >
		{
			static AttributeFormat constexpr value = AttributeFormat::eFloat;
		};

		template<>
		struct FormatGetter< Vec2 >
		{
			static AttributeFormat constexpr value = AttributeFormat::eVec2f;
		};

		template<>
		struct FormatGetter< Vec3 >
		{
			static AttributeFormat constexpr value = AttributeFormat::eVec3f;
		};

		template<>
		struct FormatGetter< Vec4 >
		{
			static AttributeFormat constexpr value = AttributeFormat::eVec4f;
		};

		template<>
		struct FormatGetter< int >
		{
			static AttributeFormat constexpr value = AttributeFormat::eInt;
		};

		template<>
		struct FormatGetter< IVec2 >
		{
			static AttributeFormat constexpr value = AttributeFormat::eVec2i;
		};

		template<>
		struct FormatGetter< IVec3 >
		{
			static AttributeFormat constexpr value = AttributeFormat::eVec3i;
		};

		template<>
		struct FormatGetter< IVec4 >
		{
			static AttributeFormat constexpr value = AttributeFormat::eVec4i;
		};
	}
}

