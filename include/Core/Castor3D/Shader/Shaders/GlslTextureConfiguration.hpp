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

		inline sdw::Vec2 rotateUV( sdw::Float const & rotateU
			, sdw::Float const & rotateV
			, sdw::Vec2 const & uv )
		{
			auto mid = 0.5_f;
			return vec2( rotateU * ( uv.x() - mid ) + rotateV * ( uv.y() - mid ) + mid
				, rotateU * ( uv.y() - mid ) - rotateV * ( uv.x() - mid ) + mid );
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
				, sdw::U32Vec4ArrayField< "components", 4u >
				, sdw::Vec3Field< "translate" >
				, sdw::FloatField< "rotateU" >
				, sdw::Vec3Field< "scale" >
				, sdw::FloatField< "rotateV" >
				, sdw::Vec4Field< "tileSet" >
				, sdw::FloatField< "nmlGMul" >
				, sdw::FloatField< "nmlFact" >
				, sdw::UIntField< "nml2Chan" >
				, sdw::FloatField< "hgtFact" >
				, sdw::UIntField< "isTrnfAnim" >
				, sdw::UIntField< "isTileAnim" >
				, sdw::UIntField< "needsYI" >
				, sdw::UIntField< "texSet" >
				, sdw::UIntField< "componentCount" >
				, sdw::UIntArrayField< "pad", 3u > >
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

			C3D_API static sdw::Float getFloat( sdw::Vec4 const & sampled
				, sdw::UInt const & mask );
			C3D_API static sdw::Vec3 getVec3( sdw::Vec4 const & sampled
				, sdw::UInt const & mask );

			auto translate()const { return getMember< "translate" >(); }
			auto rotateU()const { return getMember< "rotateU" >(); }
			auto rotateV()const { return getMember< "rotateV" >(); }
			auto scale()const { return getMember< "scale" >(); }
			auto tileSet()const { return getMember< "tileSet" >(); }
			auto component( sdw::UInt const & index )const { return getMember< "components" >()[index].xyz(); }
			auto component( uint32_t index )const { return getMember< "components" >()[index].xyz(); }
			auto nmlFact()const { return getMember< "nmlFact" >(); }
			auto nmlGMul()const { return getMember< "nmlGMul" >(); }
			auto nml2Chan()const { return getMember< "nml2Chan" >(); }
			auto hgtFact()const { return getMember< "hgtFact" >(); }
			auto needsYI()const { return getMember< "needsYI" >(); }
			auto isTrnfAnim()const { return getMember< "isTrnfAnim" >() != 0_u; }
			auto isTileAnim()const { return getMember< "isTileAnim" >() != 0_u; }
			auto texSet()const { return getMember< "texSet" >(); }
			auto componentCount()const { return getMember< "componentCount" >(); }

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
				, TextureCombine const & texturesFlags
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
			mutable sdw::Function< sdw::Void
				, sdw::InUInt
				, InOutBlendComponents > m_applyTexture;
		};
	}
}

#include "GlslTextureConfiguration.inl"

#endif
