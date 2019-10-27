#include "Castor3D/Castor3DPrerequisites.hpp"

namespace castor3d
{
	castor::String getName( TargetType value )
	{
		switch ( value )
		{
		case TargetType::eWindow:
			return cuT( "window" );
		case TargetType::eTexture:
			return cuT( "texture" );
		default:
			CU_Failure( "Unsupported TargetType" );
			return castor::cuEmptyString;
		}
	}

	castor::String getName( FrustumCorner value )
	{
		switch ( value )
		{
		case FrustumCorner::eFarLeftBottom:
			return cuT( "far_left_bottom" );
		case FrustumCorner::eFarLeftTop:
			return cuT( "far_left_top" );
		case FrustumCorner::eFarRightTop:
			return cuT( "far_right_top" );
		case FrustumCorner::eFarRightBottom:
			return cuT( "far_right_bottom" );
		case FrustumCorner::eNearLeftBottom:
			return cuT( "near_left_bottom" );
		case FrustumCorner::eNearLeftTop:
			return cuT( "near_left_top" );
		case FrustumCorner::eNearRightTop:
			return cuT( "near_right_top" );
		case FrustumCorner::eNearRightBottom:
			return cuT( "near_right_bottom" );
		default:
			CU_Failure( "Unsupported FrustumCorner" );
			return castor::cuEmptyString;
		}
	}

	castor::String getName( FrustumPlane value )
	{
		switch ( value )
		{
		case FrustumPlane::eNear:
			return cuT( "near" );
		case FrustumPlane::eFar:
			return cuT( "far" );
		case FrustumPlane::eLeft:
			return cuT( "left" );
		case FrustumPlane::eRight:
			return cuT( "right" );
		case FrustumPlane::eTop:
			return cuT( "top" );
		case FrustumPlane::eBottom:
			return cuT( "bottom" );
		default:
			CU_Failure( "Unsupported FrustumPlane" );
			return castor::cuEmptyString;
		}
	}

	castor::String getName( ViewportType value )
	{
		switch ( value )
		{
		case ViewportType::eOrtho:
			return cuT( "ortho" );
		case ViewportType::ePerspective:
			return cuT( "perspective" );
		case ViewportType::eFrustum:
			return cuT( "frustum" );
		default:
			CU_Failure( "Unsupported ViewportType" );
			return castor::cuEmptyString;
		}
	}

	castor::String getName( PickNodeType value )
	{
		switch ( value )
		{
		case PickNodeType::eNone:
			return cuT( "none" );
		case PickNodeType::eStatic:
			return cuT( "static" );
		case PickNodeType::eInstantiatedStatic:
			return cuT( "instantiated_static" );
		case PickNodeType::eSkinning:
			return cuT( "skinning" );
		case PickNodeType::eInstantiatedSkinning:
			return cuT( "instantiated_skinning" );
		case PickNodeType::eMorphing:
			return cuT( "morphing" );
		case PickNodeType::eBillboard:
			return cuT( "billboard" );
		default:
			CU_Failure( "Unsupported PickNodeType" );
			return castor::cuEmptyString;
		}
	}

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

	castor::String getName( TextureFlag value
		, MaterialType material )
	{
		switch ( value )
		{
		case TextureFlag::eNone:
			return cuT( "none" );
		case TextureFlag::eDiffuse:
			return ( material == MaterialType::eSpecularGlossiness
				? castor::String{ cuT( "albedo" ) }
				: ( material == MaterialType::eMetallicRoughness
					? castor::String{ cuT( "albedo" ) }
					: castor::String{ cuT( "diffuse" ) } ) );
		case TextureFlag::eNormal:
			return cuT( "normal" );
		case TextureFlag::eOpacity:
			return cuT( "opacity" );
		case TextureFlag::eSpecular:
			return ( material == MaterialType::eMetallicRoughness
				? castor::String{ cuT( "metallic" ) }
				: castor::String{ cuT( "specular" ) } );
		case TextureFlag::eHeight:
			return cuT( "height" );
		case TextureFlag::eGlossiness:
			return ( material == MaterialType::eSpecularGlossiness
				? castor::String{ cuT( "glossiness" ) }
				: ( material == MaterialType::eMetallicRoughness
					? castor::String{ cuT( "roughness" ) }
					: castor::String{ cuT( "shininess" ) } ) );
		case TextureFlag::eEmissive:
			return cuT( "emissive" );
		case TextureFlag::eReflection:
			return cuT( "reflection" );
		case TextureFlag::eRefraction:
			return cuT( "refraction" );
		case TextureFlag::eOcclusion:
			return cuT( "occlusion" );
		case TextureFlag::eTransmittance:
			return cuT( "transmittance" );
		default:
			CU_Failure( "Unsupported TextureFlag" );
			return castor::cuEmptyString;
		}
	}

	bool operator<( PipelineFlags const & lhs, PipelineFlags const & rhs )
	{
		return lhs.alphaFunc < rhs.alphaFunc
			|| ( lhs.alphaFunc == rhs.alphaFunc
				&& ( lhs.topology < rhs.topology
					|| ( lhs.topology == rhs.topology
						&& ( lhs.colourBlendMode < rhs.colourBlendMode
							|| ( lhs.colourBlendMode == rhs.colourBlendMode
								&& ( lhs.alphaBlendMode < rhs.alphaBlendMode
									|| ( lhs.alphaBlendMode == rhs.alphaBlendMode
										&& ( lhs.textures < rhs.textures
											|| ( lhs.textures == rhs.textures
												&& ( lhs.texturesCount < rhs.texturesCount
													|| ( lhs.texturesCount == rhs.texturesCount
														&& ( lhs.heightMapIndex < rhs.heightMapIndex
															|| ( lhs.heightMapIndex == rhs.heightMapIndex
																&& ( lhs.programFlags < rhs.programFlags
																	|| ( lhs.programFlags == rhs.programFlags
																		&& ( lhs.passFlags < rhs.passFlags
																			|| ( lhs.passFlags == rhs.passFlags
																				&& lhs.sceneFlags < rhs.sceneFlags )
																			)
																		)
																	)
																)
															)
														)
													)
												)
											)
										)
									)
								)
							)
						)
					)
				);
	}
}
