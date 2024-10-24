/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_TextureConfiguration_H___
#define ___GLSL_TextureConfiguration_H___

#include "GlslBuffer.hpp"
#include "GlslDerivativeValue.hpp"
#include "GlslTextureTransform.hpp"

#include <ShaderWriter/MatTypes/Mat4.hpp>
#include <ShaderWriter/Intrinsics/Intrinsics.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>

namespace castor3d::shader
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
			, sdw::StructFieldT< TextureTransformData, "transform" >
			, sdw::FloatField< "nmlGMul" >
			, sdw::FloatField< "nmlFact" >
			, sdw::UIntField< "nml2Chan" >
			, sdw::FloatField< "hgtFact" >
			, sdw::UIntField< "isTrnfAnim" >
			, sdw::UIntField< "isTileAnim" >
			, sdw::UIntField< "needsYI" >
			, sdw::UIntField< "texSet" > >
	{
		friend class TextureConfigurations;

	public:
		C3D_API TextureConfigData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );

		C3D_API void transformUV( Utils & utils
			, TextureTransformData const & anim
			, sdw::Vec2 & uv )const;
		C3D_API void transformUVW( Utils & utils
			, TextureTransformData const & anim
			, sdw::Vec3 & uvw )const;
		C3D_API void transformUV( Utils & utils
			, TextureTransformData const & anim
			, DerivTex & uv )const;

		C3D_API static sdw::Float getFloat( sdw::Vec4 const & sampled
			, sdw::UInt const & mask );
		C3D_API static sdw::Vec3 getVec3( sdw::Vec4 const & sampled
			, sdw::UInt const & mask );

		auto transform()const { return getMember< "transform" >(); }
		auto translate()const { return transform().translate(); }
		auto rotateU()const { return transform().rotateU(); }
		auto rotateV()const { return transform().rotateV(); }
		auto scale()const { return transform().scale(); }
		auto tileSet()const { return transform().tileSet(); }
		auto nmlFact()const { return getMember< "nmlFact" >(); }
		auto nmlGMul()const { return getMember< "nmlGMul" >(); }
		auto nml2Chan()const { return getMember< "nml2Chan" >(); }
		auto hgtFact()const { return getMember< "hgtFact" >(); }
		auto needsYI()const { return getMember< "needsYI" >(); }
		auto isTrnfAnim()const { return getMember< "isTrnfAnim" >() != 0_u; }
		auto isTileAnim()const { return getMember< "isTileAnim" >() != 0_u; }
		auto texSet()const { return getMember< "texSet" >(); }

		C3D_API sdw::Vec2 getUv( DerivTex const & uvw )const;
		C3D_API void setUv( DerivTex & lhs
			, DerivTex const & rhs )const;
		C3D_API DerivTex toUv( DerivTex const & uvw )const;

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
		template< typename TexcoordT >
		TexcoordT computeTexcoordsT( PassShaders const & passShaders
			, TextureConfigData const & config
			, TextureTransformData const & anim
			, BlendComponents const & components )const;
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
		C3D_API RetDerivTex computeTexcoordsDerivTex( PassShaders const & passShaders
			, TextureConfigData const & config
			, TextureTransformData const & anim
			, BlendComponents const & components )const;
		C3D_API sdw::RetVec3 computeTexcoordsVec3( PassShaders const & passShaders
			, TextureConfigData const & config
			, TextureTransformData const & anim
			, BlendComponents const & components )const;
		C3D_API sdw::RetVec2 computeTexcoordsVec2( PassShaders const & passShaders
			, TextureConfigData const & config
			, TextureTransformData const & anim
			, BlendComponents const & components )const;

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
		mutable sdw::Function< sdw::Vec2
			, InTextureConfigData
			, InTextureTransformData
			, InBlendComponents > m_computeTexcoordsVec2;
		mutable sdw::Function< sdw::Vec3
			, InTextureConfigData
			, InTextureTransformData
			, InBlendComponents > m_computeTexcoordsVec3;
		mutable sdw::Function< DerivTex
			, InTextureConfigData
			, InTextureTransformData
			, InBlendComponents > m_computeTexcoordsDerivTex;
		mutable sdw::Function< sdw::Vec4
			, sdw::InUInt
			, InTextureConfigData
			, InOutBlendComponents > m_sampleTexture;
	};
}

#include "GlslTextureConfiguration.inl"

#endif
