#include "PyCastor3D/PyCastor3DPrerequisites.hpp"

using namespace castor;
using namespace castor3d;

RgbaColour * CreateRgbaColourFromComponents( float p_r, float p_g, float p_b, float p_a )
{
	return new RgbaColour( RgbaColour::fromComponents( p_r, p_g, p_b, p_a ) );
}

HdrRgbaColour * CreateHdrRgbaColourFromComponents( float p_r, float p_g, float p_b, float p_a )
{
	return new HdrRgbaColour( HdrRgbaColour::fromComponents( p_r, p_g, p_b, p_a ) );
}

RgbColour * CreateRgbColourFromComponents( float p_r, float p_g, float p_b )
{
	return new RgbColour( RgbColour::fromComponents( p_r, p_g, p_b ) );
}

HdrRgbColour * CreateHdrRgbColourFromComponents( float p_r, float p_g, float p_b )
{
	return new HdrRgbColour( HdrRgbColour::fromComponents( p_r, p_g, p_b ) );
}

void ExportCastorUtils()
{
	// Make "from castor.utils import <whatever>" work
	py::object l_module( py::handle<>( py::borrowed( PyImport_AddModule( "castor.utils" ) ) ) );
	// Make "from castor import utils" work
	py::scope().attr( "utils" ) = l_module;
	// set the current scope to the new sub-module
	py::scope l_scope = l_module;

	/**@group_name ePIXEL_FORMAT	*/
	//@{
	py::enum_< PixelFormat >( "PixelFormat" )
		.value( "R8_UNORM", PixelFormat::eR8_UNORM )
		.value( "R16_SFLOAT", PixelFormat::eR16_SFLOAT )
		.value( "R32_SFLOAT", PixelFormat::eR32_SFLOAT )
		.value( "R8A8_UNORM", PixelFormat::eR8A8_UNORM )
		.value( "R16A16_SFLOAT", PixelFormat::eR16A16_SFLOAT )
		.value( "R32A32_SFLOAT", PixelFormat::eR32A32_SFLOAT )
		.value( "R5G5B5A1_UNORM", PixelFormat::eR5G5B5A1_UNORM )
		.value( "R5G6B5_UNORM", PixelFormat::eR5G6B5_UNORM )
		.value( "R8G8B8_UNORM", PixelFormat::eR8G8B8_UNORM )
		.value( "B8G8R8_UNORM", PixelFormat::eB8G8R8_UNORM )
		.value( "R8G8B8_SRGB", PixelFormat::eR8G8B8_SRGB )
		.value( "B8G8R8_SRGB", PixelFormat::eB8G8R8_SRGB )
		.value( "R8G8B8A8_UNORM", PixelFormat::eR8G8B8A8_UNORM )
		.value( "A8B8G8R8_UNORM", PixelFormat::eA8B8G8R8_UNORM )
		.value( "R8G8B8A8_SRGB", PixelFormat::eR8G8B8A8_SRGB )
		.value( "A8B8G8R8_SRGB", PixelFormat::eA8B8G8R8_SRGB )
		.value( "R16G16B16_SFLOAT", PixelFormat::eR16G16B16_SFLOAT )
		.value( "R16G16B16A16_SFLOAT", PixelFormat::eR16G16B16A16_SFLOAT )
		.value( "R32G32B32_SFLOAT", PixelFormat::eR32G32B32_SFLOAT )
		.value( "R32G32B32A32_SFLOAT", PixelFormat::eR32G32B32A32_SFLOAT )
		.value( "BC1_RGB_UNORM_BLOCK", PixelFormat::eBC1_RGB_UNORM_BLOCK )
		.value( "BC3_UNORM_BLOCK", PixelFormat::eBC3_UNORM_BLOCK )
		.value( "BC5_UNORM_BLOCK", PixelFormat::eBC5_UNORM_BLOCK )
		.value( "D16_UNORM", PixelFormat::eD16_UNORM )
		.value( "D24_UNORM_S8_UINT", PixelFormat::eD24_UNORM_S8_UINT )
		.value( "D32_UNORM", PixelFormat::eD32_UNORM )
		.value( "D32_SFLOAT", PixelFormat::eD32_SFLOAT )
		.value( "D32_SFLOAT_S8_UINT", PixelFormat::eD32_SFLOAT_S8_UINT )
		.value( "S8_UINT", PixelFormat::eS8_UINT )
		;
	//@}
	/**@group_name eINTERSECTION	*/
	//@{
	py::enum_< Intersection >( "Intersection" )
		.value( "IN", Intersection::eIn )
		.value( "OUT", Intersection::eOut )
		.value( "INTERSECT", Intersection::eIntersect )
		;
	//@}
	/**@group_name eLOG_TYPE	*/
	//@{
	py::enum_< LogType >( "LogType" )
		.value( "TRACE", LogType::eTrace )
		.value( "DEBUG", LogType::eDebug )
		.value( "INFO", LogType::eInfo )
		.value( "WARNING", LogType::eWarning )
		.value( "ERROR", LogType::eError )
		.value( "ALL", LogType::eCount )
		;
	//@}
	/**@group_name RgbColour	*/
	//@{
	py::implicitly_convertible< ColourComponent, float >();
	py::class_< RgbColour >( "RgbColour", py::no_init )
		.def( "__init__", py::make_constructor( &CreateRgbColourFromComponents ) )
		.add_property( "r", cpy::make_getter< ColourComponent, RgbColour, size_t >( &RgbColour::operator[], 0u ), cpy::make_setter< ColourComponent, RgbColour, size_t >( &RgbColour::operator[], 0u ), "The red value" )
		.add_property( "g", cpy::make_getter< ColourComponent, RgbColour, size_t >( &RgbColour::operator[], 1u ), cpy::make_setter< ColourComponent, RgbColour, size_t >( &RgbColour::operator[], 1u ), "The green value" )
		.add_property( "b", cpy::make_getter< ColourComponent, RgbColour, size_t >( &RgbColour::operator[], 2u ), cpy::make_setter< ColourComponent, RgbColour, size_t >( &RgbColour::operator[], 2u ), "The blue value" )
		.def( py::self += py::other< RgbColour >() )
		.def( py::self -= py::other< RgbColour >() )
		.def( py::self == py::other< RgbColour >() )
		.def( py::self != py::other< RgbColour >() )
		.def( py::self += py::other< float >() )
		.def( py::self -= py::other< float >() )
		.def( py::self *= py::other< float >() )
		.def( py::self /= py::other< float >() )
		;
	//@}
	/**@group_name RgbaColour	*/
	//@{
	py::class_< RgbaColour >( "RgbaColour", py::no_init )
		.def( "__init__", py::make_constructor( &CreateRgbColourFromComponents ) )
		.add_property( "r", cpy::make_getter< ColourComponent, RgbaColour, size_t >( &RgbaColour::operator[], 0u ), cpy::make_setter< ColourComponent, RgbaColour, size_t >( &RgbaColour::operator[], 0u ), "The red value" )
		.add_property( "g", cpy::make_getter< ColourComponent, RgbaColour, size_t >( &RgbaColour::operator[], 1u ), cpy::make_setter< ColourComponent, RgbaColour, size_t >( &RgbaColour::operator[], 1u ), "The green value" )
		.add_property( "b", cpy::make_getter< ColourComponent, RgbaColour, size_t >( &RgbaColour::operator[], 2u ), cpy::make_setter< ColourComponent, RgbaColour, size_t >( &RgbaColour::operator[], 2u ), "The blue value" )
		.add_property( "a", cpy::make_getter< ColourComponent, RgbaColour, size_t >( &RgbaColour::operator[], 3u ), cpy::make_setter< ColourComponent, RgbaColour, size_t >( &RgbaColour::operator[], 3u ), "The alpha value" )
		.def( py::self += py::other< RgbaColour >() )
		.def( py::self -= py::other< RgbaColour >() )
		.def( py::self == py::other< RgbaColour >() )
		.def( py::self != py::other< RgbaColour >() )
		.def( py::self += py::other< float >() )
		.def( py::self -= py::other< float >() )
		.def( py::self *= py::other< float >() )
		.def( py::self /= py::other< float >() )
		;
	//@}
	/**@group_name Vector2D	*/
	//@{
	py::def( "dot", cpy::VectorCrosser() );
	py::class_< Point2f, std::shared_ptr< Point2f >, boost::noncopyable >( "Vector2D", py::init< float, float >() )
		.add_property( "x", cpy::make_getter( &Point2f::operator[], 0u ), cpy::make_setter( &Point2f::operator[], 0u ), "The X coordinate" )
		.add_property( "y", cpy::make_getter( &Point2f::operator[], 1u ), cpy::make_setter( &Point2f::operator[], 1u ), "The Y coordinate" )
		.def( "normalise", cpy::VectorNormaliser() )
		.def( "negate", cpy::VectorNegater() )
		.def( py::self += py::other< Point2f >() )
		.def( py::self -= py::other< Point2f >() )
		.def( py::self *= py::other< Point2f >() )
		.def( py::self /= py::other< Point2f >() )
		.def( py::self == py::other< Point2f >() )
		.def( py::self != py::other< Point2f >() )
		.def( py::self *= py::other< float >() )
		.def( py::self /= py::other< float >() )
		.def( "length", cpy::VectorLengther() )
		;
	//@}
	/**@group_name Vector3D	*/
	//@{
	py::def( "cross", cpy::Vectordotter() );
	py::def( "dot", cpy::VectorCrosser() );
	py::class_< Point3f, std::shared_ptr< Point3f >, boost::noncopyable >( "Vector3D", py::init< float, float, float >() )
		.add_property( "x", cpy::make_getter( &Point3f::operator[], 0u ), cpy::make_setter( &Point3f::operator[], 0u ), "The X coordinate" )
		.add_property( "y", cpy::make_getter( &Point3f::operator[], 1u ), cpy::make_setter( &Point3f::operator[], 1u ), "The Y coordinate" )
		.add_property( "z", cpy::make_getter( &Point3f::operator[], 2u ), cpy::make_setter( &Point3f::operator[], 2u ), "The Z coordinate" )
		.def( "normalise", cpy::VectorNormaliser() )
		.def( "negate", cpy::VectorNegater() )
		.def( py::self += py::other< Point3f >() )
		.def( py::self -= py::other< Point3f >() )
		.def( py::self *= py::other< Point3f >() )
		.def( py::self /= py::other< Point3f >() )
		.def( py::self == py::other< Point3f >() )
		.def( py::self != py::other< Point3f >() )
		.def( py::self *= py::other< float >() )
		.def( py::self /= py::other< float >() )
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
		.def( py::self *= py::other< float >() )
		.def( py::self /= py::other< float >() )
		;
	//@}
	/**@group_name Quaternion	*/
	//@{
	void ( Quaternion::*quaternionToAxisAngle )( Point3f &, Angle & )const = &Quaternion::toAxisAngle;
	void ( Quaternion::*quaternionToAxes )( Point3f &, Point3f &, Point3f & )const = &Quaternion::toAxes;
	Point3f & ( Quaternion::*quaternionTransform )( Point3f const &, Point3f & )const = &Quaternion::transform;
	void ( Quaternion::*quaternionToMatrix )( Matrix4x4f & )const = &Quaternion::toMatrix;
	Quaternion( Quaternion::*quaternionLerp )( Quaternion const & p_target, float p_factor )const = &Quaternion::lerp;
	Quaternion( Quaternion::*quaternionMix )( Quaternion const & p_target, float p_factor )const = &Quaternion::mix;
	Quaternion( Quaternion::*quaternionSlerp )( Quaternion const & p_target, float p_factor )const = &Quaternion::slerp;
	py::class_< Quaternion >( "Quaternion" )
		.add_property( "rotation_matrix", quaternionToMatrix, py::make_function( cpy::QuaternionFromMatrix(), py::return_value_policy< py::return_by_value >() ), "The quaternion's rotation matrix" )
		.def( "transform", py::make_function( quaternionTransform, py::return_value_policy< py::reference_existing_object >() ) )
		.def( "toAxisAngle", quaternionToAxisAngle )
		.def( "fromAxisAngle", py::make_function( cpy::QuaternionFromAxisAngle(), py::return_value_policy< py::return_by_value >() ) )
		.def( "toAxes", quaternionToAxes )
		.def( "fromAxes", py::make_function( cpy::QuaternionFromAxes(), py::return_value_policy< py::return_by_value >() ) )
		.def( "yaw", &Quaternion::getYaw )
		.def( "pitch", &Quaternion::getPitch )
		.def( "roll", &Quaternion::getRoll )
		.def( "magnitude", &Quaternion::getMagnitude )
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
	py::class_< Matrix4x4f >( "Matrix", py::init<>() )
		.def( "transpose", py::make_function( &Matrix4x4f::transpose, py::return_value_policy< py::reference_existing_object >() ) )
		.def( "invert", py::make_function( &Matrix4x4f::invert, py::return_value_policy< py::reference_existing_object >() ) )
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
		.add_property( "width", cpy::make_getter( &Size::getWidth ), "The width" )
		.add_property( "height", cpy::make_getter( &Size::getHeight ), "The height" )
		.def( "set", &Size::set )
		.def( "grow", &Size::grow )
		.def( py::self == py::other< Size >() )
		.def( py::self != py::other< Size >() )
		;
	//@}
	/**@group_name Rectangle	*/
	//@{
	Intersection( Rectangle::*IntPoint )( Position const & )const = &Rectangle::intersects;
	Intersection( Rectangle::*IntRect )( castor::Rectangle const & )const = &Rectangle::intersects;
	py::class_< castor::Rectangle >( "Rectangle", py::init< int32_t, int32_t, uint32_t, uint32_t >() )
		.add_property( "left", cpy::make_getter( &Rectangle::left ), cpy::make_setter( &Rectangle::left ), "The left value for the rectangle" )
		.add_property( "right", cpy::make_getter( &Rectangle::right ), cpy::make_setter( &Rectangle::right ), "The right value for the rectangle" )
		.add_property( "top", cpy::make_getter( &Rectangle::top ), cpy::make_setter( &Rectangle::top ), "The top value for the rectangle" )
		.add_property( "bottom", cpy::make_getter( &Rectangle::bottom ), cpy::make_setter( &Rectangle::bottom ), "The bottom value for the rectangle" )
		.add_property( "width", &Rectangle::getWidth, "The rectangle width" )
		.add_property( "height", &Rectangle::getHeight, "The rectangle height" )
		.def( "set", &Rectangle::set )
		.def( "intersects", IntPoint )
		.def( "intersects", IntRect )
		;
	//@}
	/**@group_name Glyph	*/
	//@{
	py::class_< Glyph, boost::noncopyable >( "Glyph", py::no_init )
		.add_property( "size", py::make_function( &Glyph::getSize, py::return_value_policy< py::copy_const_reference >() ), "The glyph height" )
		.add_property( "bearing", py::make_function( &Glyph::getBearing, py::return_value_policy< py::copy_const_reference >() ), "The glyph position" )
		.add_property( "advance", &Glyph::getAdvance, "The offset after the glyph" )
		;
	//@}
	/**@group_name Font	*/
	//@{
	typedef Font::GlyphArray::iterator( Font::*GlyphsItFunc )( );
	py::class_< Font, boost::noncopyable >( "Font", py::init< String const &, uint32_t, Path const & >() )
		.add_property( "height", &Font::getHeight, "The font height" )
		.add_property( "max_height", &Font::getMaxHeight, "The glyphs maximum height" )
		.add_property( "max_width", &Font::getMaxWidth, "The glyphs maximum width" )
		.add_property( "glyphs", py::range< GlyphsItFunc, GlyphsItFunc >( &Font::begin, &Font::end ), "The glyphs" )
		;
	//@}
	/**@group_name PixelBuffer	*/
	//@{
	py::class_< PxBufferBase, boost::noncopyable >( "PixelBuffer", py::no_init )
		.add_property( "dimensions", py::make_function( &PxBufferBase::getDimensions, py::return_value_policy< py::copy_const_reference >() ), "The buffer dimensions" )
		.add_property( "width", &PxBufferBase::getWidth, "The buffer width" )
		.add_property( "height", &PxBufferBase::getHeight, "The buffer height" )
		.add_property( "pixel_format", &PxBufferBase::getFormat, "The buffer pixel format" )
		.def( "flip", &PxBufferBase::flip, py::return_value_policy< py::reference_existing_object >() )
		.def( "create", cpy::PxBufferCreator() )
		.staticmethod( "create" )
		;
	//@}
	/**@group_name Image	*/
	//@{
	PxBufferBaseSPtr( Image::*ImageBuffergetter )( )const = &Image::getPixels;
	py::class_< Image, boost::noncopyable >( "Image", py::no_init )
		.add_property( "buffer", ImageBuffergetter, "The image pixels buffer" )
		.def( "resample", &Image::resample, py::return_value_policy< py::reference_existing_object >() )
		.def( "fillRgb", cpy::FillRgb(), py::return_value_policy< py::reference_existing_object >() )
		.def( "fillRgba", cpy::FillRgba(), py::return_value_policy< py::reference_existing_object >() )
		;
	//@}
	/**@group_name Logger	*/
	//@{
	LoggerInstance * ( *LoggerInitialiser )( LogType ) = &Logger::initialise;
	void( *LoggerCleaner )( ) = &Logger::cleanup;
	void( *LoggerFileNamesetter )( Path const &, LogType ) = &Logger::setFileName;
	void( *DebugLogger )( String const & ) = &Logger::logDebug;
	void( *InfoLogger )( String const & ) = &Logger::logInfo;
	void( *WarningLogger )( String const & ) = &Logger::logWarning;
	void( *ErrorLogger )( String const & ) = &Logger::logError;
	py::class_< Logger, boost::noncopyable >( "Logger", py::no_init )
		.def( "initialise", LoggerInitialiser )
		.def( "cleanup", LoggerCleaner )
		.def( "set_file_name", LoggerFileNamesetter )
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

