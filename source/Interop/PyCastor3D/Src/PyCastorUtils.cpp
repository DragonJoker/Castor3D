#include "PyCastor3DPrerequisites.hpp"

using namespace Castor;
using namespace Castor3D;

Colour * CreateColourFromComponents( float p_r, float p_g, float p_b, float p_a )
{
	return new Colour( Colour::from_components( p_r, p_g, p_b, p_a ) );
}

void ExportCastorUtils()
{
	// Make "from castor.utils import <whatever>" work
	py::object l_module( py::handle<>( py::borrowed( PyImport_AddModule( "castor.utils" ) ) ) );
	// Make "from castor import utils" work
	py::scope().attr( "utils" ) = l_module;
	// Set the current scope to the new sub-module
	py::scope l_scope = l_module;

	/**@group_name ePIXEL_FORMAT	*/
	//@{
	py::enum_< PixelFormat >( "PixelFormat" )
	.value( "L8", PixelFormat::L8 )
	.value( "L16F32F", PixelFormat::L16F32F )
	.value( "L32F", PixelFormat::L32F )
	.value( "A8L8", PixelFormat::A8L8 )
	.value( "AL16F32F", PixelFormat::AL16F32F )
	.value( "AL32F", PixelFormat::AL32F )
	.value( "A1R5G5B5", PixelFormat::A1R5G5B5 )
	.value( "A4R4G4B4", PixelFormat::A4R4G4B4 )
	.value( "R5G6B5", PixelFormat::R5G6B5 )
	.value( "R8G8B8", PixelFormat::R8G8B8 )
	.value( "A8R8G8B8", PixelFormat::A8R8G8B8 )
	.value( "RGB16F32F", PixelFormat::RGB16F32F )
	.value( "ARGB16F32F", PixelFormat::RGBA16F32F )
	.value( "RGB32F", PixelFormat::RGB32F )
	.value( "ARGB32F", PixelFormat::RGBA32F )
	.value( "DXTC1", PixelFormat::DXTC1 )
	.value( "DXTC3", PixelFormat::DXTC3 )
	.value( "DXTC5", PixelFormat::DXTC5 )
	.value( "YUY2", PixelFormat::YUY2 )
	.value( "DEPTH16", PixelFormat::D16 )
	.value( "DEPTH24", PixelFormat::D24 )
	.value( "DEPTH24S8", PixelFormat::D24S8 )
	.value( "DEPTH32", PixelFormat::D32 )
	.value( "STENCIL1", PixelFormat::S1 )
	.value( "STENCIL8", PixelFormat::S8 )
	;
	//@}
	/**@group_name eINTERSECTION	*/
	//@{
	py::enum_< Intersection >( "Intersection" )
	.value( "IN", Intersection::In )
	.value( "OUT", Intersection::Out )
	.value( "INTERSECT", Intersection::Intersect )
	;
	//@}
	/**@group_name eLOG_TYPE	*/
	//@{
	py::enum_< ELogType >( "LogType" )
	.value( "DEBUG", ELogType_DEBUG )
	.value( "INFO", ELogType_INFO )
	.value( "WARNING", ELogType_WARNING )
	.value( "ERROR", ELogType_ERROR )
	.value( "ALL", ELogType_COUNT )
	;
	//@}
	/**@group_name Colour	*/
	//@{
	py::implicitly_convertible< ColourComponent, float >();
	py::class_< Colour >( "Colour", py::no_init )
	.def( "__init__", py::make_constructor( &CreateColourFromComponents ) )
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
	.def( py::self /= py::other< real >() )
	;
	//@}
	/**@group_name Vector2D	*/
	//@{
	py::def( "dot", cpy::VectorCrosser() );
	py::class_< Point2r, std::shared_ptr< Point2r >, boost::noncopyable >( "Vector2D", py::init< real, real >() )
	.add_property( "x", cpy::make_getter( &Point2r::operator[], 0u ), cpy::make_setter( &Point2r::operator[], 0u ), "The X coordinate" )
	.add_property( "y", cpy::make_getter( &Point2r::operator[], 1u ), cpy::make_setter( &Point2r::operator[], 1u ), "The Y coordinate" )
	.def( "normalise", cpy::VectorNormaliser() )
	.def( "negate", cpy::VectorNegater() )
	.def( py::self += py::other< Point2r >() )
	.def( py::self -= py::other< Point2r >() )
	.def( py::self *= py::other< Point2r >() )
	.def( py::self /= py::other< Point2r >() )
	.def( py::self == py::other< Point2r >() )
	.def( py::self != py::other< Point2r >() )
	.def( py::self *= py::other< real >() )
	.def( py::self /= py::other< real >() )
	.def( "length", cpy::VectorLengther() )
	;
	//@}
	/**@group_name Vector3D	*/
	//@{
	py::def( "cross", cpy::VectorDotter() );
	py::def( "dot", cpy::VectorCrosser() );
	py::class_< Point3r, std::shared_ptr< Point3r >, boost::noncopyable >( "Vector3D", py::init< real, real, real >() )
	.add_property( "x", cpy::make_getter( &Point3r::operator[], 0u ), cpy::make_setter( &Point3r::operator[], 0u ), "The X coordinate" )
	.add_property( "y", cpy::make_getter( &Point3r::operator[], 1u ), cpy::make_setter( &Point3r::operator[], 1u ), "The Y coordinate" )
	.add_property( "z", cpy::make_getter( &Point3r::operator[], 2u ), cpy::make_setter( &Point3r::operator[], 2u ), "The Z coordinate" )
	.def( "normalise", cpy::VectorNormaliser() )
	.def( "negate", cpy::VectorNegater() )
	.def( py::self += py::other< Point3r >() )
	.def( py::self -= py::other< Point3r >() )
	.def( py::self *= py::other< Point3r >() )
	.def( py::self /= py::other< Point3r >() )
	.def( py::self == py::other< Point3r >() )
	.def( py::self != py::other< Point3r >() )
	.def( py::self *= py::other< real >() )
	.def( py::self /= py::other< real >() )
	.def( "length", cpy::VectorLengther() )
	;
	//@}
	/**@group_name Angle	*/
	//@{
	py::class_< Angle >( "Angle" )
	.add_property( "degrees", cpy::make_getter( &Angle::degrees ), cpy::make_setter( &Angle::degrees ), "The degrees value for the angle" )
	.add_property( "radians", cpy::make_getter( &Angle::radians ), cpy::make_setter( &Angle::radians ), "The radians value for the angle" )
	.add_property( "grads", cpy::make_getter( &Angle::grads ), cpy::make_setter( &Angle::grads ), "The grads value for the angle" )
	.def( "cos", &Angle::cos )
	.def( "sin", &Angle::sin )
	.def( "tan", &Angle::tan )
	.def( "cos", &Angle::acos )
	.def( "sin", &Angle::asin )
	.def( "tan", &Angle::atan )
	.def( "cosh", &Angle::cosh )
	.def( "sinh", &Angle::sinh )
	.def( "tanh", &Angle::tanh )
	.def( py::self += py::other< Angle >() )
	.def( py::self -= py::other< Angle >() )
	.def( py::self *= py::other< Angle >() )
	.def( py::self /= py::other< Angle >() )
	.def( py::self == py::other< Angle >() )
	.def( py::self != py::other< Angle >() )
	.def( py::self *= py::other< real >() )
	.def( py::self /= py::other< real >() )
	;
	//@}
	/**@group_name Quaternion	*/
	//@{
	void ( Quaternion::*quaternionFromAxisAngle )( Point3f const &, Angle const & ) = &Quaternion::from_axis_angle;
	void ( Quaternion::*quaternionToAxisAngle )( Point3f &, Angle & )const = &Quaternion::to_axis_angle;
	void ( Quaternion::*quaternionFromAxes )( Point3f const &, Point3f const &, Point3f const & ) = &Quaternion::from_axes;
	void ( Quaternion::*quaternionToAxes )( Point3f &, Point3f &, Point3f & )const = &Quaternion::to_axes;
	Point3f & ( Quaternion::*quaternionTransform )( Point3f const &, Point3f & )const = &Quaternion::transform;
	void ( Quaternion::*quaternionToMatrix )( Matrix4x4r & )const = &Quaternion::to_matrix;
	void ( Quaternion::*quaternionFromMatrix )( Matrix4x4r const & ) = &Quaternion::from_matrix;
	Quaternion( Quaternion::*quaternionLerp )( Quaternion const & p_target, float p_factor )const = &Quaternion::lerp;
	Quaternion( Quaternion::*quaternionMix )( Quaternion const & p_target, float p_factor )const = &Quaternion::mix;
	Quaternion( Quaternion::*quaternionSlerp )( Quaternion const & p_target, float p_factor )const = &Quaternion::slerp;
	py::class_< Quaternion >( "Quaternion", py::init< Point3r const &, Angle const & >() )
	.add_property( "rotation_matrix", quaternionToMatrix, quaternionFromMatrix, "The quaternion's rotation matrix" )
	.def( "transform", py::make_function( quaternionTransform, py::return_value_policy< py::reference_existing_object >() ) )
	.def( "to_axis_angle", quaternionToAxisAngle )
	.def( "from_axis_angle", quaternionFromAxisAngle )
	.def( "to_axes", quaternionToAxes )
	.def( "from_axes", quaternionFromAxes )
	.def( "yaw", &Quaternion::get_yaw )
	.def( "pitch", &Quaternion::get_pitch )
	.def( "roll", &Quaternion::get_roll )
	.def( "magnitude", &Quaternion::get_magnitude )
	.def( "conjugate", &Quaternion::conjugate )
	.def( "lerp", quaternionLerp )
	.def( "slerp", quaternionMix )
	.def( "slerp", quaternionSlerp )
	.def( py::self += py::other< Quaternion >() )
	.def( py::self -= py::other< Quaternion >() )
	.def( py::self *= py::other< Quaternion >() )
	.def( py::self == py::other< Quaternion >() )
	.def( py::self != py::other< Quaternion >() )
	;
	//@}
	/**@group_name Matrix	*/
	//@{
	py::class_< Matrix4x4r >( "Matrix", py::init<>() )
	.def( "transpose", py::make_function( &Matrix4x4r::transpose, py::return_value_policy< py::reference_existing_object >() ) )
	.def( "invert", py::make_function( &Matrix4x4r::invert, py::return_value_policy< py::reference_existing_object >() ) )
	;
	//@}
	/**@group_name Position	*/
	//@{
	py::class_< Position >( "Position", py::init< int32_t, int32_t >() )
	.add_property( "x", cpy::make_getter( &Position::x ), cpy::make_setter( &Position::x ), "The X coordinate" )
	.add_property( "y", cpy::make_getter( &Position::y ), cpy::make_setter( &Position::y ), "The Y coordinate" )
	.def( "set", &Position::set )
	.def( "offset", &Position::offset )
	.def( py::self == py::other< Position >() )
	.def( py::self != py::other< Position >() )
	;
	//@}
	/**@group_name Size	*/
	//@{
	py::class_< Size >( "Size", py::init< uint32_t, uint32_t >() )
	.add_property( "width", cpy::make_getter( &Size::width ), cpy::make_setter( &Size::width ), "The width" )
	.add_property( "height", cpy::make_getter( &Size::height ), cpy::make_setter( &Size::height ), "The height" )
	.def( "set", &Size::set )
	.def( "grow", &Size::grow )
	.def( py::self == py::other< Size >() )
	.def( py::self != py::other< Size >() )
	;
	//@}
	/**@group_name Rectangle	*/
	//@{
	Intersection( Rectangle::*IntPoint )( Position const & )const = &Rectangle::intersects;
	Intersection( Rectangle::*IntRect )( Rectangle const & )const = &Rectangle::intersects;
	py::class_< Rectangle >( "Rectangle", py::init< int32_t, int32_t, uint32_t, uint32_t >() )
	.add_property( "left", cpy::make_getter( &Rectangle::left ), cpy::make_setter( &Rectangle::left ), "The left value for the rectangle" )
	.add_property( "right", cpy::make_getter( &Rectangle::right ), cpy::make_setter( &Rectangle::right ), "The right value for the rectangle" )
	.add_property( "top", cpy::make_getter( &Rectangle::top ), cpy::make_setter( &Rectangle::top ), "The top value for the rectangle" )
	.add_property( "bottom", cpy::make_getter( &Rectangle::bottom ), cpy::make_setter( &Rectangle::bottom ), "The bottom value for the rectangle" )
	.add_property( "width", &Rectangle::width, "The rectangle width" )
	.add_property( "height", &Rectangle::height, "The rectangle height" )
	.def( "set", &Rectangle::set )
	.def( "intersects", IntPoint )
	.def( "intersects", IntRect )
	;
	//@}
	/**@group_name Glyph	*/
	//@{
	py::class_< Glyph, boost::noncopyable >( "Glyph", py::no_init )
	.add_property( "size", py::make_function( &Glyph::GetSize, py::return_value_policy< py::copy_const_reference >() ), "The glyph height" )
	.add_property( "bearing", py::make_function( &Glyph::GetBearing, py::return_value_policy< py::copy_const_reference >() ), "The glyph position" )
	.add_property( "advance", &Glyph::GetAdvance, "The offset after the glyph" )
	;
	//@}
	/**@group_name Font	*/
	//@{
	typedef Font::GlyphArray::iterator( Font::*GlyphsItFunc )();
	py::class_< Font, boost::noncopyable >( "Font", py::init< String const &, uint32_t, Path const & >() )
	.add_property( "height", &Font::GetHeight, "The font height" )
	.add_property( "max_height", &Font::GetMaxHeight, "The glyphs maximum height" )
	.add_property( "max_width", &Font::GetMaxWidth, "The glyphs maximum width" )
	.add_property( "glyphs", py::range< GlyphsItFunc, GlyphsItFunc >( &Font::begin, &Font::end ), "The glyphs" )
	;
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
	.def( "create", cpy::PxBufferCreator() )
	.staticmethod( "create" )
	;
	//@}
	/**@group_name Image	*/
	//@{
	PxBufferBaseSPtr( Image::*ImageBufferGetter )()const = &Image::GetPixels;
	py::class_< Image, boost::noncopyable >( "Image", py::no_init )
	.add_property( "buffer", ImageBufferGetter, "The image pixels buffer" )
	.def( "resample", &Image::Resample, py::return_value_policy< py::reference_existing_object >() )
	.def( "fill", &Image::Fill, py::return_value_policy< py::reference_existing_object >() )
	.def( "copy_image", &Image::CopyImage, py::return_value_policy< py::reference_existing_object >() )
	.def( "sub_image", &Image::SubImage )
	;
	//@}
	/**@group_name Logger	*/
	//@{
	void( *LoggerInitialiser )( ELogType ) = &Logger::Initialise;
	void( *LoggerCleaner )() = &Logger::Cleanup;
	void( *LoggerFileNameSetter )( String const &, ELogType ) = &Logger::SetFileName;
	void( *DebugLogger )( String const & ) = &Logger::LogDebug;
	void( *InfoLogger )( String const & ) = &Logger::LogInfo;
	void( *WarningLogger )( String const & ) = &Logger::LogWarning;
	void( *ErrorLogger )( String const & ) = &Logger::LogError;
	py::class_< Logger, boost::noncopyable >( "Logger", py::no_init )
	.def( "initialise", LoggerInitialiser )
	.def( "cleanup", LoggerCleaner )
	.def( "set_file_name", LoggerFileNameSetter )
	.def( "log_debug", DebugLogger )
	.def( "log_info", InfoLogger )
	.def( "log_warning", WarningLogger )
	.def( "log_error", ErrorLogger )
	.staticmethod( "initialise" )
	.staticmethod( "cleanup" )
	.staticmethod( "set_file_name" )
	.staticmethod( "log_debug" )
	.staticmethod( "log_message" )
	.staticmethod( "log_warning" )
	.staticmethod( "log_error" )
	;
	//@}
}

