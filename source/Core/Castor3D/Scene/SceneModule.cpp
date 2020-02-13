#include "Castor3D/Scene/SceneModule.hpp"

#include "Castor3D/Scene/Scene.hpp"

CU_ImplementExportedOwnedBy( castor3d::Scene, Scene )

namespace castor3d
{
	castor::String getName( MovableType value )
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
			return castor::cuEmptyString;
		}
	}

	castor::String getName( ShadowType value )
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
			return castor::cuEmptyString;
		}
	}

	castor::String getName( FogType value )
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
			return castor::cuEmptyString;
		}
	}

	castor::String getName( BillboardType value )
	{
		switch ( value )
		{
		case BillboardType::eSpherical:
			return cuT( "spherical" );
		case BillboardType::eCylindrical:
			return cuT( "cylindrical" );
		default:
			CU_Failure( "Unsupported BillboardType" );
			return castor::cuEmptyString;
		}
	}

	castor::String getName( BillboardSize value )
	{
		switch ( value )
		{
		case BillboardSize::eDynamic:
			return cuT( "dynamic" );
		case BillboardSize::eFixed:
			return cuT( "fixed" );
		default:
			CU_Failure( "Unsupported BillboardSize" );
			return castor::cuEmptyString;
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
}
