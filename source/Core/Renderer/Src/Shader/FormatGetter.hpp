/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_FormatGetter_HPP___
#define ___Renderer_FormatGetter_HPP___
#pragma once

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
			static uint32_t constexpr divisorMultiplier = 1u;
		};

		template<>
		struct FormatGetter< Vec2 >
		{
			static AttributeFormat constexpr value = AttributeFormat::eVec2f;
			static uint32_t constexpr divisorMultiplier = 1u;
		};

		template<>
		struct FormatGetter< Vec3 >
		{
			static AttributeFormat constexpr value = AttributeFormat::eVec3f;
			static uint32_t constexpr divisorMultiplier = 1u;
		};

		template<>
		struct FormatGetter< Vec4 >
		{
			static AttributeFormat constexpr value = AttributeFormat::eVec4f;
			static uint32_t constexpr divisorMultiplier = 1u;
		};

		template<>
		struct FormatGetter< Mat4 >
		{
			static AttributeFormat constexpr value = AttributeFormat::eMat4f;
			static uint32_t constexpr divisorMultiplier = 4u;
		};

		template<>
		struct FormatGetter< int >
		{
			static AttributeFormat constexpr value = AttributeFormat::eInt;
			static uint32_t constexpr divisorMultiplier = 1u;
		};

		template<>
		struct FormatGetter< IVec2 >
		{
			static AttributeFormat constexpr value = AttributeFormat::eVec2i;
			static uint32_t constexpr divisorMultiplier = 1u;
		};

		template<>
		struct FormatGetter< IVec3 >
		{
			static AttributeFormat constexpr value = AttributeFormat::eVec3i;
			static uint32_t constexpr divisorMultiplier = 1u;
		};

		template<>
		struct FormatGetter< IVec4 >
		{
			static AttributeFormat constexpr value = AttributeFormat::eVec4i;
			static uint32_t constexpr divisorMultiplier = 1u;
		};

		template<>
		struct FormatGetter< uint32_t >
		{
			static AttributeFormat constexpr value = AttributeFormat::eUInt;
			static uint32_t constexpr divisorMultiplier = 1u;
		};

		template<>
		struct FormatGetter< UIVec2 >
		{
			static AttributeFormat constexpr value = AttributeFormat::eVec2ui;
			static uint32_t constexpr divisorMultiplier = 1u;
		};

		template<>
		struct FormatGetter< UIVec3 >
		{
			static AttributeFormat constexpr value = AttributeFormat::eVec3ui;
			static uint32_t constexpr divisorMultiplier = 1u;
		};

		template<>
		struct FormatGetter< UIVec4 >
		{
			static AttributeFormat constexpr value = AttributeFormat::eVec4ui;
			static uint32_t constexpr divisorMultiplier = 1u;
		};
	}
}

#endif
