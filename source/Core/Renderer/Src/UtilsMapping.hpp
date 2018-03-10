/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
#ifndef ___Renderer_UtilsMapping_HPP___
#define ___Renderer_UtilsMapping_HPP___
#pragma once

#include <Design/FlagCombination.hpp>
#include <Design/Signal.hpp>
#include <Graphics/RgbaColour.hpp>
#include <Math/Angle.hpp>
#include <Math/SquareMatrix.hpp>
#include <Math/Point.hpp>
#include <Miscellaneous/DynamicLibrary.hpp>

namespace renderer
{
	/**
	*\name Mappings avec la bibliothèque utilitaire.
	*/
	/**\{*/
	template< typename Function >
	using Signal = castor::Signal< Function >;
	template< typename Signal >
	using SignalConnection = castor::Connection< Signal >;

	using Vec2 = castor::Point2f;
	using Vec3 = castor::Point3f;
	using Vec4 = castor::Point4f;
	using Mat4 = castor::Matrix4x4f;
	using RgbaColour = castor::RgbaColour;
	using Angle = castor::Angle;
	using DynamicLibrary = castor::DynamicLibrary;

	using castor::checkFlag;
	/**\}*/
}

#define Utils_ImplementFlag IMPLEMENT_FLAGS
#define Utils_EnumBounds CASTOR_SCOPED_ENUM_BOUNDS

#endif
