#include "Castor3D/Scene/SceneModule.hpp"

#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"

CU_ImplementExportedOwnedBy( Scene, Scene )
CU_ImplementSmartPtr( c3d, CameraCache )
CU_ImplementSmartPtr( c3d, SceneCache )
CU_ImplementSmartPtr( c3d, MaterialCacheView )
CU_ImplementSmartPtr( c3d, SamplerCacheView )
CU_ImplementSmartPtr( c3d, FontCacheView )
CU_ImplementSmartPtr( c3d, ImageCacheView )


namespace c3d
{
	const String PtrCacheTraitsT< Scene, String >::Name = cuT( "Scene" );
	const String ObjectCacheTraitsT< Camera, String >::Name = cuT( "Camera" );

	String getName( MovableType value )
	{
		switch ( value )
		{
		case MovableType::eCamera:
			return cuT( "camera" );
		case MovableType::eGeometry:
			return cuT( "object" );
		case MovableType::eLight:
			return cuT( "light" );
		case MovableType::eBillboard:
			return cuT( "billboard" );
		case MovableType::eParticleEmitter:
			return cuT( "particle_emitter" );
		default:
			CU_Failure( "Unsupported MovableType" );
			return cuEmptyString;
		}
	}

	String getName( ShadowType value )
	{
		switch ( value )
		{
		case ShadowType::eNone:
			return cuT( "none" );
		case ShadowType::eRaw:
			return cuT( "raw" );
		case ShadowType::ePCF:
			return cuT( "pcf" );
		case ShadowType::eVariance:
			return cuT( "variance" );
		default:
			CU_Failure( "Unsupported ShadowType" );
			return cuEmptyString;
		}
	}

	String getName( FogType value )
	{
		switch ( value )
		{
		case FogType::eDisabled:
			return cuT( "none" );
		case FogType::eLinear:
			return cuT( "linear" );
		case FogType::eExponential:
			return cuT( "exponential" );
		case FogType::eSquaredExponential:
			return cuT( "squared_exponential" );
		default:
			CU_Failure( "Unsupported FogType" );
			return cuEmptyString;
		}
	}

	String getName( BillboardType value )
	{
		switch ( value )
		{
		case BillboardType::eSpherical:
			return cuT( "spherical" );
		case BillboardType::eCylindrical:
			return cuT( "cylindrical" );
		default:
			CU_Failure( "Unsupported BillboardType" );
			return cuEmptyString;
		}
	}

	String getName( BillboardSize value )
	{
		switch ( value )
		{
		case BillboardSize::eDynamic:
			return cuT( "dynamic" );
		case BillboardSize::eFixed:
			return cuT( "fixed" );
		default:
			CU_Failure( "Unsupported BillboardSize" );
			return cuEmptyString;
		}
	}

	FogType getFogType( SceneFlags const & flags )
	{
		FogType result = FogType::eDisabled;

		if ( checkFlag( flags, SceneFlag::eFogLinear ) )
		{
			result = FogType::eLinear;
		}
		else if ( checkFlag( flags, SceneFlag::eFogExponential ) )
		{
			result = FogType::eExponential;
		}
		else if ( checkFlag( flags, SceneFlag::eFogSquaredExponential ) )
		{
			result = FogType::eSquaredExponential;
		}

		return result;
	}

	LoggerInstance & getLogger( Scene const & scene )
	{
		return getLogger( *scene.getEngine() );
	}

	Engine & getEngine( Scene const & scene )
	{
		return *scene.getEngine();
	}
}
