#include "Castor3D/Castor3DPrerequisites.hpp"

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
			return cuT( "square_exponential" );
		default:
			CU_Failure( "Unsupported FogType" );
			return castor::cuEmptyString;
		}
	}

	castor::String getName( BackgroundType value )
	{
		switch ( value )
		{
		case BackgroundType::eColour:
			return cuT( "colour" );
		case BackgroundType::eImage:
			return cuT( "image" );
		case BackgroundType::eSkybox:
			return cuT( "skybox" );
		default:
			CU_Failure( "Unsupported BackgroundType" );
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

	castor::String getName( ParticleFormat value )
	{
		switch ( value )
		{
		case ParticleFormat::eInt:
			return cuT( "int" );
		case ParticleFormat::eVec2i:
			return cuT( "vec2i" );
		case ParticleFormat::eVec3i:
			return cuT( "vec3i" );
		case ParticleFormat::eVec4i:
			return cuT( "vec4i" );
		case ParticleFormat::eUInt:
			return cuT( "uint" );
		case ParticleFormat::eVec2ui:
			return cuT( "vec2ui" );
		case ParticleFormat::eVec3ui:
			return cuT( "vec3ui" );
		case ParticleFormat::eVec4ui:
			return cuT( "vec4ui" );
		case ParticleFormat::eFloat:
			return cuT( "float" );
		case ParticleFormat::eVec2f:
			return cuT( "vec2f" );
		case ParticleFormat::eVec3f:
			return cuT( "vec3f" );
		case ParticleFormat::eVec4f:
			return cuT( "vec4f" );
		case ParticleFormat::eMat2f:
			return cuT( "mat2x2f" );
		case ParticleFormat::eMat3f:
			return cuT( "mat3x3f" );
		case ParticleFormat::eMat4f:
			return cuT( "mat4x4f" );
		default:
			CU_Failure( "Unsupported ParticleFormat" );
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
