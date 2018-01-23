/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_UtilsMapping_HPP___
#define ___Renderer_UtilsMapping_HPP___
#pragma once

#include <Design/FlagCombination.hpp>
#include <Design/Signal.hpp>
#include <Graphics/PixelFormat.hpp>
#include <Graphics/RgbaColour.hpp>
#include <Math/Angle.hpp>
#include <Math/PlaneEquation.hpp>
#include <Math/SquareMatrix.hpp>
#include <Math/Quaternion.hpp>
#include <Math/Point.hpp>
#include <Miscellaneous/DynamicLibrary.hpp>

namespace renderer
{
	/**
	*\name Mappings avec la bibliothèque utilitaire.
	*/
	/**\{*/
	template< typename T >
	using Vec2T = castor::Point2< T >;
	template< typename T >
	using Vec3T = castor::Point3< T >;
	template< typename T >
	using Vec4T = castor::Point4< T >;
	template< typename T >
	using Mat4T = castor::Matrix4x4< T >;
	template< typename Function >
	using Signal = castor::Signal< Function >;
	template< typename Signal >
	using SignalConnection = castor::Connection< Signal >;

	using Vec2 = castor::Point2f;
	using Vec3 = castor::Point3f;
	using Vec4 = castor::Point4f;
	using Mat4 = castor::Matrix4x4f;
	using IVec2 = castor::Point2i;
	using IVec3 = castor::Point3i;
	using IVec4 = castor::Point4i;
	using UIVec2 = castor::Point2ui;
	using UIVec3 = castor::Point3ui;
	using UIVec4 = castor::Point4ui;
	using Quaternion = castor::Quaternion;
	using RgbaColour = castor::RgbaColour;
	using RgbColour = castor::RgbColour;
	using Angle = castor::Angle;
	using PixelFormat = castor::PixelFormat;
	using Clock = std::chrono::high_resolution_clock;
	using Milliseconds = castor::Milliseconds;
	using DynamicLibrary = castor::DynamicLibrary;
	using ByteArray = castor::ByteArray;
	using UInt16Array = std::vector< uint16_t >;
	using UInt32Array = std::vector< uint32_t >;
	using UInt64Array = std::vector< uint64_t >;
	using FloatArray = std::vector< float >;
	using Vec3Array = castor::Point3fArray;
	using Vec2Array = castor::Point2fArray;
	using StringArray = castor::StringArray;

	using castor::checkFlag;
	/**\}*/
}

#define Utils_ImplementFlag IMPLEMENT_FLAGS
#define Utils_EnumBounds CASTOR_SCOPED_ENUM_BOUNDS

#include "PixelFormat.hpp"

#endif
