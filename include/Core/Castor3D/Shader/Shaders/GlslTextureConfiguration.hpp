/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_TextureConfiguration_H___
#define ___GLSL_TextureConfiguration_H___

#include "Castor3D/Castor3DPrerequisites.hpp"

#include <ShaderWriter/MatTypes/Mat4.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>

namespace castor3d
{
	namespace shader
	{
		static uint32_t constexpr MaxTextureConfigurationCount = 4000u;
		static int constexpr MaxTextureConfigurationComponentsCount = 7;
		castor::String const TextureConfigurationBufferName = cuT( "TextureConfigurations" );

		class TextureConfigurations;

		struct TextureConfigData
			: public sdw::StructInstance
		{
			friend class TextureConfigurations;

		public:
			C3D_API TextureConfigData( sdw::Shader * shader
				, ast::expr::ExprPtr expr );

			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );
			C3D_API static ast::type::StructPtr makeType( ast::type::TypesCache & cache );

			C3D_API sdw::Vec3 getDiffuse( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled
				, sdw::Vec3 const & diffuse
				, sdw::Float gamma )const;
			C3D_API sdw::Vec3 getAlbedo( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled
				, sdw::Vec3 const & diffuse
				, sdw::Float gamma )const;
			C3D_API sdw::Vec3 getEmissive( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled
				, sdw::Vec3 const & emissive
				, sdw::Float gamma )const;
			C3D_API sdw::Vec3 getSpecular( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled
				, sdw::Vec3 const & specular )const;
			C3D_API sdw::Float getMetalness( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled
				, sdw::Float const & metalness )const;
			C3D_API sdw::Float getShininess( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled
				, sdw::Float const & shininess )const;
			C3D_API sdw::Float getGlossiness( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled
				, sdw::Float const & glossiness )const;
			C3D_API sdw::Float getRoughness( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled
				, sdw::Float const & roughness )const;
			C3D_API sdw::Float getOpacity( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled
				, sdw::Float const & opacity )const;
			C3D_API sdw::Vec3 getNormal( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled
				, sdw::Vec3 const & normal
				, sdw::Vec3 const & tangent
				, sdw::Vec3 const & bitangent )const;
			C3D_API sdw::Float getHeight( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled
				, sdw::Float const & height )const;
			C3D_API sdw::Float getOcclusion( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled
				, sdw::Float const & occlusion )const;
			C3D_API sdw::Float getTransmittance( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled
				, sdw::Float const & transmittance )const;
			C3D_API sdw::Vec2 convertUV( sdw::ShaderWriter & writer
				, sdw::Vec2 const & uv )const;
			C3D_API sdw::Vec3 convertUVW( sdw::ShaderWriter & writer
				, sdw::Vec3 const & uvw )const;

		private:
			sdw::Float getMix( sdw::ShaderWriter & writer
				, sdw::Vec2 const & mask )const;
			sdw::Float getFloat( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled
				, sdw::Vec2 const & mask )const;
			sdw::Float getFloat( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled
				, sdw::Float const & factor
				, sdw::Vec2 const & mask )const;
			sdw::Vec3 getVec3( sdw::ShaderWriter & writer
				, sdw::Vec4 const & sampled
				, sdw::Vec2 const & mask )const;
			sdw::Vec3 removeGamma( sdw::ShaderWriter & writer
				, sdw::Vec3 const & srgb
				, sdw::Float const & gamma )const;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;

		public:
			sdw::Vec4 colrSpec;
			sdw::Vec4 glossOpa;
			sdw::Vec4 emisOccl;
			sdw::Vec4 trnsDumm;
			sdw::Vec4 normalFc;
			sdw::Vec4 heightFc;
			sdw::Vec4 miscVals;

		public:
			sdw::Vec2 colourMask;
			sdw::Vec2 specularMask;
			sdw::Vec2 glossinessMask;
			sdw::Vec2 opacityMask;
			sdw::Vec2 emissiveMask;
			sdw::Vec2 normalMask;
			sdw::Float normalFactor;
			sdw::Float normalGMultiplier;
			sdw::Vec2 heightMask;
			sdw::Float heightFactor;
			sdw::Vec2 occlusionMask;
			sdw::Vec2 transmittanceMask;
			sdw::UInt environment;
			sdw::UInt needsGammaCorrection;
			sdw::UInt needsYInversion;
		};

		class TextureConfigurations
		{
		public:
			C3D_API explicit TextureConfigurations( sdw::ShaderWriter & writer );
			C3D_API void declare( bool hasSsbo );
			C3D_API TextureConfigData getTextureConfiguration( sdw::UInt const & index )const;

		private:
			sdw::ShaderWriter & m_writer;
			std::unique_ptr< sdw::Struct > m_type;
			std::unique_ptr< sdw::ArraySsboT< TextureConfigData > > m_ssbo;
			sdw::Function< TextureConfigData
				, sdw::InUInt > m_getTextureConfiguration;
		};
	}
}

#endif
