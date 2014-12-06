/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#include "PyCastor3DPch.hpp"

#include "PyCastor3DPrerequisites.hpp"

#include <Logger.hpp>
#include <Angle.hpp>
#include <Glyph.hpp>
#include <Font.hpp>
#include <Quaternion.hpp>
#include <Position.hpp>
#include <Size.hpp>
#include <Rectangle.hpp>
#include <Image.hpp>
#include <PixelBufferBase.hpp>

#include <Castor3DPrerequisites.hpp>

using namespace Castor;
using namespace Castor3D;

namespace
{
	struct VectorNormaliser
	{
		void operator()( Point3r * p_arg )
		{
			point::normalise( *p_arg );
		}
	};
	struct VectorNegater
	{
		void operator()( Point3r * p_arg )
		{
			point::negate( *p_arg );
		}
	};
	struct VectorLengther
	{
		void operator()( Point3r * p_arg )
		{
			point::distance( *p_arg );
		}
	};
	struct VectorDotter
	{
		real operator()( Point3r const & p_1, Point3r const & p_2 )
		{
			return point::dot( p_1, p_2 );
		}
	};
	struct VectorCrosser
	{
		Point3r operator()( Point3r const & p_1, Point3r const & p_2 )
		{
			return p_1 ^ p_2;
		}
	};
	struct PxBufferCreator
	{
		PxBufferBaseSPtr operator()( Size const & p_size, ePIXEL_FORMAT p_ePixelFormat )
		{
			return PxBufferBase::create( p_size, p_ePixelFormat );
		}
	};
}

namespace boost
{
	namespace python
	{
		namespace detail
		{
			inline boost::mpl::vector< void, Point3r * >
			get_signature( VectorNormaliser, void * = 0 )
			{
				return boost::mpl::vector< void, Point3r * >();
			}
			inline boost::mpl::vector< void, Point3r * >
			get_signature( VectorNegater, void * = 0 )
			{
				return boost::mpl::vector< void, Point3r * >();
			}
			inline boost::mpl::vector< void, Point3r * >
			get_signature( VectorLengther, void * = 0 )
			{
				return boost::mpl::vector< void, Point3r * >();
			}
			inline boost::mpl::vector< real, Point3r const &, Point3r const & >
			get_signature( VectorDotter, void * = 0 )
			{
				return boost::mpl::vector< real, Point3r const &, Point3r const & >();
			}
			inline boost::mpl::vector< Point3r, Point3r const &, Point3r const & >
			get_signature( VectorCrosser, void * = 0 )
			{
				return boost::mpl::vector< Point3r, Point3r const &, Point3r const & >();
			}
			inline boost::mpl::vector< PxBufferBaseSPtr, Size const &, ePIXEL_FORMAT >
			get_signature( PxBufferCreator, void * = 0 )
			{
				return boost::mpl::vector< PxBufferBaseSPtr, Size const &, ePIXEL_FORMAT >();
			}
		}
	}
}

BOOST_PYTHON_MODULE( Castor )
{
	/**@group_name ePIXEL_FORMAT	*/
	//@{
	py::enum_< ePIXEL_FORMAT >( "PixelFormat" )
	.value( "L8", ePIXEL_FORMAT_L8 )
	.value( "L16F32F", ePIXEL_FORMAT_L16F32F )
	.value( "L32F", ePIXEL_FORMAT_L32F )
	.value( "A8L8", ePIXEL_FORMAT_A8L8 )
	.value( "AL16F32F", ePIXEL_FORMAT_AL16F32F )
	.value( "AL32F", ePIXEL_FORMAT_AL32F )
	.value( "A1R5G5B5", ePIXEL_FORMAT_A1R5G5B5 )
	.value( "A4R4G4B4", ePIXEL_FORMAT_A4R4G4B4 )
	.value( "R5G6B5", ePIXEL_FORMAT_R5G6B5 )
	.value( "R8G8B8", ePIXEL_FORMAT_R8G8B8 )
	.value( "A8R8G8B8", ePIXEL_FORMAT_A8R8G8B8 )
	.value( "RGB16F32F", ePIXEL_FORMAT_RGB16F32F )
	.value( "ARGB16F32F", ePIXEL_FORMAT_ARGB16F32F )
	.value( "RGB32F", ePIXEL_FORMAT_RGB32F )
	.value( "ARGB32F", ePIXEL_FORMAT_ARGB32F )
	.value( "DXTC1", ePIXEL_FORMAT_DXTC1 )
	.value( "DXTC3", ePIXEL_FORMAT_DXTC3 )
	.value( "DXTC5", ePIXEL_FORMAT_DXTC5 )
	.value( "YUY2", ePIXEL_FORMAT_YUY2 )
	.value( "DEPTH16", ePIXEL_FORMAT_DEPTH16 )
	.value( "DEPTH24", ePIXEL_FORMAT_DEPTH24 )
	.value( "DEPTH24S8", ePIXEL_FORMAT_DEPTH24S8 )
	.value( "DEPTH32", ePIXEL_FORMAT_DEPTH32 )
	.value( "STENCIL1", ePIXEL_FORMAT_STENCIL1 )
	.value( "STENCIL8", ePIXEL_FORMAT_STENCIL8 );
	//@}
	/**@group_name eINTERSECTION	*/
	//@{
	py::enum_< eINTERSECTION >( "Intersection" )
	.value( "IN", eINTERSECTION_IN )
	.value( "OUT", eINTERSECTION_OUT )
	.value( "INTERSECT", eINTERSECTION_INTERSECT );
	//@}
	/**@group_name eLOG_TYPE	*/
	//@{
	py::enum_< eLOG_TYPE >( "LogType" )
	.value( "DEBUG", eLOG_TYPE_DEBUG )
	.value( "MESSAGE", eLOG_TYPE_MESSAGE )
	.value( "WARNING", eLOG_TYPE_WARNING )
	.value( "ERROR", eLOG_TYPE_ERROR )
	.value( "ALL", eLOG_TYPE_COUNT );
	//@}
	/**@group_name Colour	*/
	//@{
	py::implicitly_convertible< ColourComponent, float >();
	py::class_< Colour >( "Colour" )
	.add_property( "r", cpy::make_getter( &Colour::operator[], Colour::eCOMPONENT_RED ), cpy::make_setter( &Colour::operator[], Colour::eCOMPONENT_RED ), "The red value" )
	.add_property( "g", cpy::make_getter( &Colour::operator[], Colour::eCOMPONENT_GREEN ), cpy::make_setter( &Colour::operator[], Colour::eCOMPONENT_GREEN ), "The green value" )
	.add_property( "b", cpy::make_getter( &Colour::operator[], Colour::eCOMPONENT_BLUE ), cpy::make_setter( &Colour::operator[], Colour::eCOMPONENT_BLUE ), "The blue value" )
	.add_property( "a", cpy::make_getter( &Colour::operator[], Colour::eCOMPONENT_ALPHA ), cpy::make_setter( &Colour::operator[], Colour::eCOMPONENT_ALPHA ), "The alpha value" )
	.def( py::self += py::other< Colour >() )
	.def( py::self -= py::other< Colour >() )
	.def( py::self == py::other< Colour >() )
	.def( py::self != py::other< Colour >() )
	.def( py::self += py::other< real >() )
	.def( py::self -= py::other< real >() )
	.def( py::self *= py::other< real >() )
	.def( py::self /= py::other< real >() );
	//@}
	/**@group_name Vector2D	*/
	//@{
	py::class_< Point2r >( "Vector2D", py::init< real, real >() )
	.add_property( "x", cpy::make_getter( &Point2r::operator[], 0u ), cpy::make_setter( &Point2r::operator[], 0u ), "The X coordinate" )
	.add_property( "y", cpy::make_getter( &Point2r::operator[], 1u ), cpy::make_setter( &Point2r::operator[], 1u ), "The Y coordinate" )
	.def( "normalise", VectorNormaliser() )
	.def( "negate", VectorNegater() )
	.def( py::self += py::other< Point2r >() )
	.def( py::self -= py::other< Point2r >() )
	.def( py::self *= py::other< Point2r >() )
	.def( py::self /= py::other< Point2r >() )
	.def( py::self == py::other< Point2r >() )
	.def( py::self != py::other< Point2r >() )
	.def( py::self *= py::other< real >() )
	.def( py::self /= py::other< real >() )
	.def( "length", VectorLengther() );
	py::def( "dot", VectorCrosser() );
	//@}
	/**@group_name Vector3D	*/
	//@{
	py::class_< Point3r >( "Vector3D", py::init< real, real, real >() )
	.add_property( "x", cpy::make_getter( &Point3r::operator[], 0u ), cpy::make_setter( &Point3r::operator[], 0u ), "The X coordinate" )
	.add_property( "y", cpy::make_getter( &Point3r::operator[], 1u ), cpy::make_setter( &Point3r::operator[], 1u ), "The Y coordinate" )
	.add_property( "z", cpy::make_getter( &Point3r::operator[], 2u ), cpy::make_setter( &Point3r::operator[], 2u ), "The Z coordinate" )
	.def( "normalise", VectorNormaliser() )
	.def( "negate", VectorNegater() )
	.def( py::self += py::other< Point3r >() )
	.def( py::self -= py::other< Point3r >() )
	.def( py::self *= py::other< Point3r >() )
	.def( py::self /= py::other< Point3r >() )
	.def( py::self == py::other< Point3r >() )
	.def( py::self != py::other< Point3r >() )
	.def( py::self *= py::other< real >() )
	.def( py::self /= py::other< real >() )
	.def( "length", VectorLengther() );
	py::def( "cross", VectorDotter() );
	py::def( "dot", VectorCrosser() );
	//@}
	/**@group_name Angle	*/
	//@{
	py::class_< Angle >( "Angle" )
	.add_property( "degrees", cpy::make_getter( &Angle::Degrees ), cpy::make_setter( &Angle::Degrees ), "The degrees value for the angle" )
	.add_property( "radians", cpy::make_getter( &Angle::Radians ), cpy::make_setter( &Angle::Radians ), "The radians value for the angle" )
	.add_property( "grads", cpy::make_getter( &Angle::Grads ), cpy::make_setter( &Angle::Grads ), "The grads value for the angle" )
	.def( "cos", &Angle::Cos )
	.def( "sin", &Angle::Sin )
	.def( "tan", &Angle::Tan )
	.def( "cos", &Angle::ACos )
	.def( "sin", &Angle::ASin )
	.def( "tan", &Angle::ATan )
	.def( "cosh", &Angle::Cosh )
	.def( "sinh", &Angle::Sinh )
	.def( "tanh", &Angle::Tanh )
	.def( py::self += py::other< Angle >() )
	.def( py::self -= py::other< Angle >() )
	.def( py::self *= py::other< Angle >() )
	.def( py::self /= py::other< Angle >() )
	.def( py::self == py::other< Angle >() )
	.def( py::self != py::other< Angle >() )
	.def( py::self *= py::other< real >() )
	.def( py::self /= py::other< real >() );
	//@}
	/**@group_name Quaternion	*/
	//@{
	void ( Quaternion::*quaternionToMatrix )( Matrix4x4r & )const = &Quaternion::ToRotationMatrix;
	void ( Quaternion::*quaternionFromMatrix )( Matrix4x4r const & ) = &Quaternion::FromRotationMatrix;
	py::class_< Quaternion >( "Quaternion", py::init< Point3r const &, Angle const & >() )
	.add_property( "rotation_matrix", quaternionToMatrix, quaternionFromMatrix, "The quaternion's rotation matrix" )
	.def( "transform", py::make_function( &Quaternion::Transform, py::return_value_policy< py::reference_existing_object >() ) )
	.def( "to_axis_angle", &Quaternion::ToAxisAngle )
	.def( "from_axis_angle", &Quaternion::FromAxisAngle )
	.def( "to_axes", &Quaternion::ToAxes )
	.def( "from_axes", &Quaternion::FromAxes )
	.def( "yaw", &Quaternion::GetYaw )
	.def( "pitch", &Quaternion::GetPitch )
	.def( "roll", &Quaternion::GetYaw )
	.def( "magnitude", &Quaternion::GetMagnitude )
	.def( "conjugate", &Quaternion::Conjugate )
	.def( "slerp", &Quaternion::Slerp )
	.def( py::self += py::other< Quaternion >() )
	.def( py::self -= py::other< Quaternion >() )
	.def( py::self *= py::other< Quaternion >() )
	.def( py::self == py::other< Quaternion >() )
	.def( py::self != py::other< Quaternion >() );
	//@}
	/**@group_name Matrix	*/
	//@{
	py::class_< Matrix4x4r >( "Matrix", py::init<>() )
	.def( "transpose", py::make_function( &Matrix4x4r::transpose, py::return_value_policy< py::reference_existing_object >() ) )
	.def( "invert", py::make_function( &Matrix4x4r::invert, py::return_value_policy< py::reference_existing_object >() ) );
	//@}
	/**@group_name Position	*/
	//@{
	py::class_< Position >( "Position", py::init< int32_t, int32_t >() )
	.add_property( "x", cpy::make_getter( &Position::x ), cpy::make_setter( &Position::x ), "The X coordinate" )
	.add_property( "y", cpy::make_getter( &Position::y ), cpy::make_setter( &Position::y ), "The Y coordinate" )
	.def( "set", &Position::set )
	.def( "offset", &Position::offset )
	.def( py::self == py::other< Position >() )
	.def( py::self != py::other< Position >() );
	//@}
	/**@group_name Size	*/
	//@{
	py::class_< Size >( "Size", py::init< uint32_t, uint32_t >() )
	.add_property( "width", cpy::make_getter( &Size::width ), cpy::make_setter( &Size::width ), "The width" )
	.add_property( "height", cpy::make_getter( &Size::height ), cpy::make_setter( &Size::height ), "The height" )
	.def( "set", &Size::set )
	.def( "grow", &Size::grow )
	.def( py::self == py::other< Size >() )
	.def( py::self != py::other< Size >() );
	//@}
	/**@group_name Rectangle	*/
	//@{
	eINTERSECTION( Rectangle::*IntPoint )( Position const & )const = &Rectangle::intersects;
	eINTERSECTION( Rectangle::*IntRect )( Rectangle const & )const = &Rectangle::intersects;
	py::class_< Rectangle >( "Rectangle", py::init< int32_t, int32_t, uint32_t, uint32_t >() )
	.add_property( "left", cpy::make_getter( &Rectangle::left ), cpy::make_setter( &Rectangle::left ), "The left value for the rectangle" )
	.add_property( "right", cpy::make_getter( &Rectangle::right ), cpy::make_setter( &Rectangle::right ), "The right value for the rectangle" )
	.add_property( "top", cpy::make_getter( &Rectangle::top ), cpy::make_setter( &Rectangle::top ), "The top value for the rectangle" )
	.add_property( "bottom", cpy::make_getter( &Rectangle::bottom ), cpy::make_setter( &Rectangle::bottom ), "The bottom value for the rectangle" )
	.add_property( "width", &Rectangle::width, "The rectangle width" )
	.add_property( "height", &Rectangle::height, "The rectangle height" )
	.def( "set", &Rectangle::set )
	.def( "intersects", IntPoint )
	.def( "intersects", IntRect );
	//@}
	/**@group_name Glyph	*/
	//@{
	py::class_< Glyph, boost::noncopyable >( "Glyph", py::no_init )
	.add_property( "size", py::make_function( &Glyph::GetSize, py::return_value_policy< py::copy_const_reference >() ), "The glyph height" )
	.add_property( "position", py::make_function( &Glyph::GetPosition, py::return_value_policy< py::copy_const_reference >() ), "The glyph position" )
	.add_property( "advance", py::make_function( &Glyph::GetAdvance, py::return_value_policy< py::copy_const_reference >() ), "The offset after the glyph" )
	.def( "adjust_position", &Glyph::AdjustPosition );
	//@}
	/**@group_name Font	*/
	//@{
	typedef Font::GlyphArrayIt( Font::*GlyphsItFunc )();
	py::class_< Font >( "Font", py::init< Path const &, String const &, uint32_t >() )
	.add_property( "height", &Font::GetHeight, "The font height" )
	.add_property( "max_height", &Font::GetMaxHeight, "The glyphs maximum height" )
	.add_property( "max_width", &Font::GetMaxWidth, "The glyphs maximum width" )
	.add_property( "glyphs", py::range< GlyphsItFunc, GlyphsItFunc >( &Font::Begin, &Font::End ), "The glyphs" );
	//@}
	/**@group_name PixelBuffer	*/
	//@{
	py::class_< PxBufferBase, boost::noncopyable >( "PixelBuffer", py::no_init )
	.add_property( "dimensions", py::make_function( &PxBufferBase::dimensions, py::return_value_policy< py::copy_const_reference >() ), "The buffer dimensions" )
	.add_property( "width", &PxBufferBase::width, "The buffer width" )
	.add_property( "height", &PxBufferBase::height, "The buffer height" )
	.add_property( "pixel_format", &PxBufferBase::format, "The buffer pixel format" )
	.def( "flip", &PxBufferBase::flip, py::return_value_policy< py::reference_existing_object >() )
	.def( "mirror", &PxBufferBase::mirror, py::return_value_policy< py::reference_existing_object >() )
	.def( "create", PxBufferCreator() )
	.staticmethod( "create" );
	//@}
	/**@group_name Image	*/
	//@{
	PxBufferBaseSPtr( Image::*ImageBufferGetter )()const = &Image::GetPixels;
	py::class_< Image, boost::noncopyable >( "Image", py::no_init )
	.add_property( "buffer", ImageBufferGetter, "The image pixels buffer" )
	.def( "resample", &Image::Resample, py::return_value_policy< py::reference_existing_object >() )
	.def( "fill", &Image::Fill, py::return_value_policy< py::reference_existing_object >() )
	.def( "copy_image", &Image::CopyImage, py::return_value_policy< py::reference_existing_object >() )
	.def( "sub_image", &Image::SubImage );
	//@}
	/**@group_name Logger	*/
	//@{
	void( *LoggerInitialiser )( eLOG_TYPE ) = &Logger::Initialise;
	void( *LoggerCleaner )() = &Logger::Cleanup;
	void( *LoggerFileNameSetter )( String const &, eLOG_TYPE ) = &Logger::SetFileName;
	void( *DebugLogger )( String const & ) = &Logger::LogDebug;
	void( *MessageLogger )( String const & ) = &Logger::LogMessage;
	void( *WarningLogger )( String const & ) = &Logger::LogWarning;
	void( *ErrorLogger )( String const & ) = &Logger::LogError;
	py::class_< Logger, boost::noncopyable >( "Logger", py::no_init )
	.def( "initialise", LoggerInitialiser )
	.def( "cleanup", LoggerCleaner )
	.def( "set_file_name", LoggerFileNameSetter )
	.def( "log_debug", DebugLogger )
	.def( "log_message", MessageLogger )
	.def( "low_warning", WarningLogger )
	.def( "log_error", ErrorLogger )
	.staticmethod( "initialise" )
	.staticmethod( "cleanup" )
	.staticmethod( "set_file_name" )
	.staticmethod( "log_debug" )
	.staticmethod( "log_message" )
	.staticmethod( "low_warning" )
	.staticmethod( "log_error" );
	//@}
}
