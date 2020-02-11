#include "Castor3D/Scene/ParticleSystem/ParticleModule.hpp"

namespace castor3d
{
	castor::String getName( ElementUsage value )
	{
		switch ( value )
		{
		case ElementUsage::eUnknown:
			return cuT( "unknown" );
		case ElementUsage::ePosition:
			return cuT( "position" );
		case ElementUsage::eNormal:
			return cuT( "normal" );
		case ElementUsage::eTangent:
			return cuT( "tangent" );
		case ElementUsage::eBitangent:
			return cuT( "bitangent" );
		case ElementUsage::eDiffuse:
			return cuT( "diffuse" );
		case ElementUsage::eTexCoords:
			return cuT( "texcoords" );
		case ElementUsage::eBoneIds0:
			return cuT( "bone_ids0" );
		case ElementUsage::eBoneIds1:
			return cuT( "bone_ids1" );
		case ElementUsage::eBoneWeights0:
			return cuT( "bone_weights0" );
		case ElementUsage::eBoneWeights1:
			return cuT( "bone_weights1" );
		case ElementUsage::eTransform:
			return cuT( "transform" );
		case ElementUsage::eMatIndex:
			return cuT( "mat_index" );
		default:
			CU_Failure( "Unsupported ElementUsage" );
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
}
