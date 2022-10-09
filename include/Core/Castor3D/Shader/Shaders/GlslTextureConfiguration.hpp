/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_TextureConfiguration_H___
#define ___GLSL_TextureConfiguration_H___

#include "GlslBuffer.hpp"

#include <ShaderWriter/MatTypes/Mat4.hpp>
#include <ShaderWriter/Intrinsics/Intrinsics.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>

namespace castor3d
{
	namespace shader
	{
		class PassShaders;

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
			: public sdw::StructInstanceHelperT< "C3D_TextureConfigData"
				, ast::type::MemoryLayout::eStd430
				, sdw::FloatField< "colEnbl" >
				, sdw::FloatField< "colMask" >
				, sdw::FloatField< "opaEnbl" >
				, sdw::FloatField< "opaMask" >
				, sdw::FloatField< "spcEnbl" >
				, sdw::FloatField< "spcMask" >
				, sdw::FloatField< "glsEnbl" >
				, sdw::FloatField< "glsMask" >
				, sdw::FloatField< "metEnbl" >
				, sdw::FloatField< "metMask" >
				, sdw::FloatField< "rghEnbl" >
				, sdw::FloatField< "rghMask" >
				, sdw::FloatField< "emsEnbl" >
				, sdw::FloatField< "emsMask" >
				, sdw::FloatField< "occEnbl" >
				, sdw::FloatField< "occMask" >
				, sdw::FloatField< "trsEnbl" >
				, sdw::FloatField< "trsMask" >
				, sdw::FloatField< "nmlEnbl" >
				, sdw::FloatField< "nmlMask" >
				, sdw::FloatField< "nmlFact" >
				, sdw::FloatField< "nmlGMul" >
				, sdw::FloatField< "hgtEnbl" >
				, sdw::FloatField< "hgtMask" >
				, sdw::FloatField< "hgtFact" >
				, sdw::FloatField< "pad0" >
				, sdw::UIntField< "needsYI" >
				, sdw::UIntField< "isTrnfAnim" >
				, sdw::UIntField< "isTileAnim" >
				, sdw::UIntField< "texSet" >
				, sdw::UIntField< "pad1" >
				, sdw::UIntField< "pad2" >
				, sdw::Vec4Field< "translate" >
				, sdw::Vec4Field< "rotate" >
				, sdw::Vec4Field< "scale" >
				, sdw::Vec4Field< "tileSet" > >
		{
			friend class TextureConfigurations;

		public:
			C3D_API TextureConfigData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );

			C3D_API void transformUV( Utils & utils
				, TextureAnimData const & anim
				, sdw::Vec2 & uv )const;
			C3D_API void transformUVW( Utils & utils
				, TextureAnimData const & anim
				, sdw::Vec3 & uvw )const;
			C3D_API void transformUV( Utils & utils
				, TextureAnimData const & anim
				, DerivTex & uv )const;

			C3D_API sdw::Float getFloat( sdw::Vec4 const & sampled
				, sdw::Float const & mask )const;
			C3D_API sdw::Vec3 getVec3( sdw::Vec4 const & sampled
				, sdw::Float const & mask )const;

			auto colEnbl()const { return getMember< "colEnbl" >(); }
			auto colMask()const { return getMember< "colMask" >(); }
			auto opaEnbl()const { return getMember< "opaEnbl" >(); }
			auto opaMask()const { return getMember< "opaMask" >(); }
			auto spcEnbl()const { return getMember< "spcEnbl" >(); }
			auto spcMask()const { return getMember< "spcMask" >(); }
			auto glsEnbl()const { return getMember< "glsEnbl" >(); }
			auto glsMask()const { return getMember< "glsMask" >(); }
			auto metEnbl()const { return getMember< "metEnbl" >(); }
			auto metMask()const { return getMember< "metMask" >(); }
			auto rghEnbl()const { return getMember< "rghEnbl" >(); }
			auto rghMask()const { return getMember< "rghMask" >(); }
			auto emsEnbl()const { return getMember< "emsEnbl" >(); }
			auto emsMask()const { return getMember< "emsMask" >(); }
			auto occEnbl()const { return getMember< "occEnbl" >(); }
			auto occMask()const { return getMember< "occMask" >(); }
			auto trsEnbl()const { return getMember< "trsEnbl" >(); }
			auto trsMask()const { return getMember< "trsMask" >(); }
			auto nmlEnbl()const { return getMember< "nmlEnbl" >(); }
			auto nmlMask()const { return getMember< "nmlMask" >(); }
			auto nmlFact()const { return getMember< "nmlFact" >(); }
			auto nmlGMul()const { return getMember< "nmlGMul" >(); }
			auto hgtEnbl()const { return getMember< "hgtEnbl" >(); }
			auto hgtMask()const { return getMember< "hgtMask" >(); }
			auto hgtFact()const { return getMember< "hgtFact" >(); }
			auto needsYI()const { return getMember< "needsYI" >(); }
			auto isTrnfAnim()const { return getMember< "isTrnfAnim" >() != 0_u; }
			auto isTileAnim()const { return getMember< "isTileAnim" >() != 0_u; }
			auto texSet()const { return getMember< "texSet" >(); }
			auto translate()const { return getMember< "translate" >(); }
			auto rotate()const { return getMember< "rotate" >(); }
			auto scale()const { return getMember< "scale" >(); }
			auto tileSet()const { return getMember< "tileSet" >(); }

			sdw::Boolean isGeometry()const
			{
				return opaEnbl() != 0.0_f
					|| nmlEnbl() != 0.0_f
					|| hgtEnbl() != 0.0_f;
			}

			sdw::Boolean isGeometryOnly()const
			{
				return colEnbl() == 0.0_f
					&& spcEnbl() == 0.0_f
					&& glsEnbl() == 0.0_f
					&& metEnbl() == 0.0_f
					&& emsEnbl() == 0.0_f
					&& occEnbl() == 0.0_f
					&& rghEnbl() == 0.0_f
					&& trsEnbl() == 0.0_f;
			}

			sdw::Vec2 getUv( sdw::Vec3 const & uvw )const
			{
				return uvw.xy();
			}

			sdw::Vec2 toUv( sdw::Vec3 const & uvw )const
			{
				return uvw.xy();
			}

			void setUv( sdw::Vec3 & lhs
				, sdw::Vec2 const & rhs )const
			{
				lhs.xy() = rhs;
			}

			sdw::Vec2 getUv( DerivTex const & uvw )const
			{
				return uvw.uv();
			}

			DerivTex toUv( DerivTex const & uvw )const
			{
				return uvw;
			}

			void setUv( DerivTex & lhs
				, DerivTex const & rhs )const
			{
				lhs.uv() = rhs.uv();
			}

			sdw::Float fneedYI;
		};

		class TextureConfigurations
			: public BufferT< TextureConfigData >
		{
		public:
			C3D_API TextureConfigurations( sdw::ShaderWriter & writer
				, uint32_t binding
				, uint32_t set
				, bool enable = true );
			C3D_API void computeMapsContributions( PassShaders const & passShaders
				, PipelineFlags const & flags
				, TextureAnimations const & textureAnims
				, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
				, Material const & material
				, BlendComponents & components )const;
			C3D_API void computeMapsContributions( PassShaders const & passShaders
				, TextureFlags const & texturesFlags
				, TextureAnimations const & textureAnims
				, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
				, Material const & material
				, BlendComponents & components )const;
			template< typename TexcoordT, typename FlagsT >
			void computeMapsContributionsT( PassShaders const & passShaders
				, FlagsT const & flags
				, TextureAnimations const & textureAnims
				, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
				, Material const & material
				, BlendComponents & components )const;
			template< typename TexcoordT >
			static TexcoordT getTexcoord( TextureConfigData const & data
				, TexcoordT const & texCoords0
				, TexcoordT const & texCoords1
				, TexcoordT const & texCoords2
				, TexcoordT const & texCoords3 );
			template< typename TexcoordT >
			static void setTexcoord( TextureConfigData const & data
				, TexcoordT const & value
				, TexcoordT & texCoords0
				, TexcoordT & texCoords1
				, TexcoordT & texCoords2
				, TexcoordT & texCoords3 );

			TextureConfigData getTextureConfiguration( sdw::UInt const & index )const
			{
				return BufferT< TextureConfigData >::getData( index - 1u );
			}

		private:
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

#include "GlslTextureConfiguration.inl"

#endif
