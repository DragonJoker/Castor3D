/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_TextureConfiguration_H___
#define ___GLSL_TextureConfiguration_H___

#include "SdwModule.hpp"

#include <ShaderWriter/MatTypes/Mat4.hpp>
#include <ShaderWriter/Intrinsics/Intrinsics.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>

namespace castor3d
{
	namespace shader
	{
		castor::String const TextureConfigurationBufferName = cuT( "TextureConfigurations" );

		template< typename LhsT, typename RhsT >
		inline LhsT translateUV( LhsT const & translate
			, RhsT const & uv )
		{
			return translate + uv;
		}

		inline sdw::Vec2 rotateUV( sdw::Vec2 const & rotate
			, sdw::Vec2 const & uv )
		{
			auto mid = 0.5_f;
			return vec2( rotate.x() * ( uv.x() - mid ) + rotate.y() * ( uv.y() - mid ) + mid
				, rotate.x() * ( uv.y() - mid ) - rotate.y() * ( uv.x() - mid ) + mid );
		}

		inline sdw::Vec3 rotateUV( sdw::Vec3 const & rotate
			, sdw::Vec3 const & uv )
		{
			return ( ( uv - vec3( 0.5_f, 0.5f, 0.5f ) ) * rotate ) + vec3( 0.5_f, 0.5f, 0.5f );
		}

		template< typename LhsT, typename RhsT >
		inline LhsT scaleUV( LhsT const & scale
			, RhsT const & uv )
		{
			return scale * uv;
		}

		struct TextureConfigData
			: public sdw::StructInstance
		{
			friend class TextureConfigurations;

		public:
			C3D_API TextureConfigData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			SDW_DeclStructInstance( C3D_API, TextureConfigData );

			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );
			C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

			C3D_API void computeGeometryMapContribution( Utils &  utils
				, PassFlags const & passFlags
				, TextureFlags const & textureFlags
				, std::string const & name
				, shader::TextureAnimData const & anim
				, sdw::CombinedImage2DRgba32 const & map
				, sdw::Vec3 & texCoords
				, sdw::Float & opacity
				, sdw::Vec3 & tangentSpaceViewPosition
				, sdw::Vec3 & tangentSpaceFragPosition );
			C3D_API void computeGeometryMapContribution( Utils &  utils
				, PassFlags const & passFlags
				, TextureFlags const & textureFlags
				, std::string const & name
				, shader::TextureAnimData const & anim
				, sdw::CombinedImage2DRgba32 const & map
				, sdw::Vec3 & texCoords
				, sdw::Float & opacity
				, sdw::Vec3 & normal
				, sdw::Vec3 & tangent
				, sdw::Vec3 & bitangent
				, sdw::Vec3 & tangentSpaceViewPosition
				, sdw::Vec3 & tangentSpaceFragPosition );
			C3D_API sdw::Vec4 computeCommonMapContribution( Utils & utils
				, PassFlags const & passFlags
				, TextureFlags const & textureFlags
				, std::string const & name
				, shader::TextureAnimData const & anim
				, sdw::CombinedImage2DRgba32 const & map
				, sdw::Vec3 const & texCoords
				, sdw::Vec3 & emissive
				, sdw::Float & opacity
				, sdw::Float & occlusion
				, sdw::Float & transmittance
				, sdw::Vec3 & normal
				, sdw::Vec3 & tangent
				, sdw::Vec3 & bitangent
				, sdw::Vec3 & tangentSpaceViewPosition
				, sdw::Vec3 & tangentSpaceFragPosition );
			C3D_API sdw::Vec4 computeCommonMapVoxelContribution( Utils & utils
				, PassFlags const & passFlags
				, TextureFlags const & textureFlags
				, std::string const & name
				, shader::TextureAnimData const & anim
				, sdw::CombinedImage2DRgba32 const & map
				, sdw::Vec3 const & texCoords
				, sdw::Vec3 & emissive
				, sdw::Float & opacity
				, sdw::Float & occlusion );

			C3D_API void transformUV( TextureAnimData const & config
				, sdw::Vec2 & uv )const;
			C3D_API void transformUVW( TextureAnimData const & config
				, sdw::Vec3 & uvw )const;

			C3D_API sdw::Float getGlossiness( sdw::Vec4 const & sampled
				, sdw::Float const & glossiness )const;
			C3D_API sdw::Vec3 getColour( sdw::Vec4 const & sampled
				, sdw::Vec3 const & colour )const;
			C3D_API sdw::Float getOpacity( sdw::Vec4 const & sampled
				, sdw::Float const & opacity )const;

			C3D_API void applyDiffuse( TextureFlags const & textureFlags
				, sdw::Vec4 const & sampled
				, sdw::Vec3 & diffuse )const;
			C3D_API void applyAlbedo( TextureFlags const & textureFlags
				, sdw::Vec4 const & sampled
				, sdw::Vec3 & diffuse )const;
			C3D_API void applyEmissive( TextureFlags const & textureFlags
				, sdw::Vec4 const & sampled
				, sdw::Vec3 & emissive )const;
			C3D_API void applySpecular( TextureFlags const & textureFlags
				, sdw::Vec4 const & sampled
				, sdw::Vec3 & specular )const;
			C3D_API void applyMetalness( TextureFlags const & textureFlags
				, sdw::Vec4 const & sampled
				, sdw::Float & metalness )const;
			C3D_API void applyShininess( TextureFlags const & textureFlags
				, sdw::Vec4 const & sampled
				, sdw::Float & shininess )const;
			C3D_API void applyRoughness( TextureFlags const & textureFlags
				, sdw::Vec4 const & sampled
				, sdw::Float & roughness )const;
			C3D_API void applyOpacity( TextureFlags const & textureFlags
				, sdw::Vec4 const & sampled
				, sdw::Float & opacity )const;
			C3D_API void applyNormal( TextureFlags const & textureFlags
				, sdw::Vec4 const & sampled
				, sdw::Mat3 const & tbn
				, sdw::Vec3 & normal )const;
			C3D_API void applyNormal( TextureFlags const & textureFlags
				, sdw::Vec4 const & sampled
				, sdw::Vec3 const & normal
				, sdw::Vec3 const & tangent
				, sdw::Vec3 const & bitangent
				, sdw::Vec3 & result )const;
			C3D_API void applyHeight( TextureFlags const & textureFlags
				, sdw::Vec4 const & sampled
				, sdw::Float & height )const;
			C3D_API void applyOcclusion( TextureFlags const & textureFlags
				, sdw::Vec4 const & sampled
				, sdw::Float & occlusion )const;
			C3D_API void applyTransmittance( TextureFlags const & textureFlags
				, sdw::Vec4 const & sampled
				, sdw::Float & transmittance )const;

			sdw::Boolean isDiffuse()const
			{
				return colEnbl != 0.0_f;
			}

			sdw::Boolean isAlbedo()const
			{
				return colEnbl != 0.0_f;
			}

			sdw::Boolean isEmissive()const
			{
				return emsEnbl != 0.0_f;
			}

			sdw::Boolean isSpecular()const
			{
				return spcEnbl != 0.0_f;
			}

			sdw::Boolean isMetalness()const
			{
				return metEnbl != 0.0_f;
			}

			sdw::Boolean isShininess()const
			{
				return shnEnbl != 0.0_f;
			}

			sdw::Boolean isGlossiness()const
			{
				return shnEnbl != 0.0_f;
			}

			sdw::Boolean isRoughness()const
			{
				return rghEnbl != 0.0_f;
			}

			sdw::Boolean isOcclusion()const
			{
				return occEnbl != 0.0_f;
			}

			sdw::Boolean isTransmittance()const
			{
				return trsEnbl != 0.0_f;
			}

			sdw::Boolean isOpacity()const
			{
				return opaEnbl != 0.0_f;
			}

			sdw::Boolean isNormal()const
			{
				return nmlEnbl != 0.0_f;
			}

			sdw::Boolean isHeight()const
			{
				return hgtEnbl != 0.0_f;
			}

			sdw::Boolean isGeometry()const
			{
				return opaEnbl != 0.0_f
					|| nmlEnbl != 0.0_f
					|| hgtEnbl != 0.0_f;
			}

			sdw::Boolean isGeometryOnly()const
			{
				return colEnbl == 0.0_f
					&& emsEnbl == 0.0_f
					&& metEnbl == 0.0_f
					&& occEnbl == 0.0_f
					&& rghEnbl == 0.0_f
					&& spcEnbl == 0.0_f
					&& shnEnbl == 0.0_f
					&& trsEnbl == 0.0_f;
			}

		private:
			sdw::Float getFloat( sdw::Vec4 const & sampled
				, sdw::Float const & mask )const;
			sdw::Vec3 getVec3( sdw::Vec4 const & sampled
				, sdw::Float const & mask )const;
			void convertUV( sdw::Vec2 & uv )const;
			void convertUVW( sdw::Vec3 & uvw )const;
			void convertToTile( sdw::Vec2 & uv )const;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;

		public:
			sdw::Vec4 colOpa;
			sdw::Vec4 spcShn;
			sdw::Vec4 metRgh;
			sdw::Vec4 emsOcc;
			sdw::Vec4 trsDum;
			sdw::Vec4 nmlFcr;
			sdw::Vec4 hgtFcr;
			sdw::Vec4 mscVls;
			sdw::Vec4 texTrn;
			sdw::Vec4 texRot;
			sdw::Vec4 texScl;
			sdw::Vec4 tleSet;

		public:
			sdw::Float colEnbl;
			sdw::Float colMask;
			sdw::Float opaEnbl;
			sdw::Float opaMask;
			sdw::Float spcEnbl;
			sdw::Float spcMask;
			sdw::Float shnEnbl;
			sdw::Float shnMask;
			sdw::Float metEnbl;
			sdw::Float metMask;
			sdw::Float rghEnbl;
			sdw::Float rghMask;
			sdw::Float emsEnbl;
			sdw::Float emsMask;
			sdw::Float occEnbl;
			sdw::Float occMask;
			sdw::Float trsEnbl;
			sdw::Float trsMask;
			sdw::Float nmlEnbl;
			sdw::Float nmlMask;
			sdw::Float nmlFact;
			sdw::Float nmlGMul;
			sdw::Float hgtEnbl;
			sdw::Float hgtMask;
			sdw::Float hgtFact;
			sdw::Float fneedYI;
			sdw::UInt needsYI;
			sdw::Boolean isTrnfAnim;
			sdw::Boolean isTileAnim;
			sdw::UInt texSet;
		};

		class TextureConfigurations
		{
		public:
			C3D_API explicit TextureConfigurations( sdw::ShaderWriter & writer
				, bool enable = true );
			C3D_API explicit TextureConfigurations( sdw::ShaderWriter & writer
				, uint32_t binding
				, uint32_t set
				, bool enable = true );
			C3D_API void declare( uint32_t binding, uint32_t set );
			C3D_API TextureConfigData getTextureConfiguration( sdw::UInt const & index )const;
			C3D_API void computeGeometryMapContributions( Utils & utils
				, PassFlags const & passFlags
				, TextureFlagsArray const & textures
				, TextureAnimations const & textureAnims
				, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
				, shader::Material const & material
				, sdw::Vec3 & texCoords0
				, sdw::Vec3 & texCoords1
				, sdw::Vec3 & texCoords2
				, sdw::Vec3 & texCoords3
				, sdw::Float & opacity
				, sdw::Vec3 & tangentSpaceViewPosition
				, sdw::Vec3 & tangentSpaceFragPosition )const;
			C3D_API sdw::Vec3 getTexcoord( TextureConfigData const & data
				, sdw::Vec3 const & texCoords0
				, sdw::Vec3 const & texCoords1
				, sdw::Vec3 const & texCoords2
				, sdw::Vec3 const & texCoords3 )const;
			C3D_API void setTexcoord( TextureConfigData const & data
				, sdw::Vec3 const & value
				, sdw::Vec3 & texCoords0
				, sdw::Vec3 & texCoords1
				, sdw::Vec3 & texCoords2
				, sdw::Vec3 & texCoords3 )const;

			bool isEnabled()const noexcept
			{
				return m_enable;
			}

		private:
			sdw::ShaderWriter & m_writer;
			bool m_enable;
			std::unique_ptr< sdw::ArraySsboT< TextureConfigData > > m_ssbo;
			mutable sdw::Function< sdw::Vec3
				, sdw::InUInt
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InVec3 > m_getTexcoord4;
			mutable sdw::Function< sdw::Vec3
				, sdw::InUInt
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InVec3 > m_getTexcoord3;
			mutable sdw::Function< sdw::Vec3
				, sdw::InUInt
				, sdw::InVec3
				, sdw::InVec3 > m_getTexcoord2;
			mutable sdw::Function< sdw::Void
				, sdw::InUInt
				, sdw::InVec3
				, sdw::InOutVec3
				, sdw::InOutVec3
				, sdw::InOutVec3
				, sdw::InOutVec3 > m_setTexcoord4;
			mutable sdw::Function< sdw::Void
				, sdw::InUInt
				, sdw::InVec3
				, sdw::InOutVec3
				, sdw::InOutVec3
				, sdw::InOutVec3 > m_setTexcoord3;
			mutable sdw::Function< sdw::Void
				, sdw::InUInt
				, sdw::InVec3
				, sdw::InOutVec3
				, sdw::InOutVec3 > m_setTexcoord2;
		};
	}
}

#endif
