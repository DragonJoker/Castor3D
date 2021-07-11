/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslCookTorranceBRDF_H___
#define ___C3D_GlslCookTorranceBRDF_H___

#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

namespace castor3d::shader
{
	struct PbrLightMaterial
		: public sdw::StructInstance
	{
		template< MaterialType MaterialT >
		struct CreatorT;

		template<>
		struct CreatorT< MaterialType::ePhong >
		{
			static void create( PbrLightMaterial & material
				, sdw::Vec3 const & albedo
				, sdw::Vec3 const & specular
				, sdw::Float const & shininess )
			{
				material.albedo = albedo;
				material.specular = specular.rgb();
				material.metalness = LightingModel::computeMetalness( albedo, specular );
				material.roughness = LightingModel::computeRoughness( LightingModel::computeGlossiness( shininess ) );
			}
			
			static void create( PbrLightMaterial & material
				, sdw::Vec3 const & diffuse
				, sdw::Float const & gamma
				, sdw::Vec3 const & specular
				, sdw::Float const & shininess )
			{
				create( material
					, pow( max( diffuse, vec3( 0.0_f, 0.0_f, 0.0_f ) ), vec3( gamma ) )
					, specular
					, shininess );
			}

			static void create( PbrLightMaterial & material
				, sdw::Vec3 const & albedo
				, sdw::Vec4 const & data3
				, sdw::Vec4 const & data2 )
			{
				create( material, albedo, data3.rgb(), data2.a() );
			}
		};

		template<>
		struct CreatorT< MaterialType::eMetallicRoughness >
		{
			static void create( PbrLightMaterial & material
				, sdw::Vec3 const & albedo
				, sdw::Float const & metalness
				, sdw::Float const & roughness )
			{
				material.albedo = albedo;
				material.specular = LightingModel::computeF0( albedo, metalness );
				material.metalness = metalness;
				material.roughness = roughness;
			}

			static void create( PbrLightMaterial & material
				, sdw::Vec3 const & albedo
				, sdw::Vec4 const & data3
				, sdw::Vec4 const & data2 )
			{
				create( material, albedo, data3.r(), data2.a() );
			}
		};

		template<>
		struct CreatorT< MaterialType::eSpecularGlossiness >
		{
			static void create( PbrLightMaterial & material
				, sdw::Vec3 const & albedo
				, sdw::Vec3 const & specular
				, sdw::Float const & glossiness )
			{
				material.albedo = albedo;
				material.specular = specular;
				material.metalness = LightingModel::computeMetalness( albedo, specular );
				material.roughness = LightingModel::computeRoughness( glossiness );
			}

			static void create( PbrLightMaterial & material
				, sdw::Vec3 const & albedo
				, sdw::Vec4 const & data3
				, sdw::Vec4 const & data2 )
			{
				create( material, albedo, data3.rgb(), data2.a() );
			}
		};

		C3D_API PbrLightMaterial( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );
		C3D_API PbrLightMaterial & operator=( PbrLightMaterial const & rhs );

		template< MaterialType MaterialT
			, typename ... ParamsT >
		void create( ParamsT const & ... params )
		{
			CreatorT< MaterialT >::create( *this, params... );
		}

		C3D_API static ast::type::StructPtr makeType( ast::type::TypesCache & cache );

		sdw::Vec3 albedo;
		sdw::Vec3 specular;
		sdw::Float metalness;
		sdw::Float roughness;
	};

	class CookTorranceBRDF
	{
	public:
		C3D_API explicit CookTorranceBRDF( sdw::ShaderWriter & writer
			, Utils & utils );
		C3D_API void declare();
		C3D_API void declareDiffuse();
		C3D_API void compute( Light const & light
			, sdw::Vec3 const & worldEye
			, sdw::Vec3 const & direction
			, PbrLightMaterial const & material
			, Surface surface
			, OutputComponents & output )const;
		C3D_API sdw::Vec3 computeDiffuse( sdw::Vec3 const & colour
			, sdw::Vec3 const & worldEye
			, sdw::Vec3 const & direction
			, PbrLightMaterial const & material
			, Surface surface )const;
		C3D_API sdw::Vec3 computeDiffuse( Light const & light
			, sdw::Vec3 const & worldEye
			, sdw::Vec3 const & direction
			, PbrLightMaterial const & material
			, Surface surface )const;

	protected:
		void doDeclareDistribution();
		void doDeclareGeometry();
		void doDeclareComputeCookTorrance();
		void doDeclareComputeCookTorranceDiffuse();

	public:
		sdw::ShaderWriter & m_writer;
		Utils & m_utils;
		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat > m_distributionGGX;
		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat > m_geometrySchlickGGX;
		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat > m_geometrySmith;
		sdw::Function< sdw::Float
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat > m_smith;
		sdw::Function< sdw::Void
			, InLight
			, sdw::InVec3
			, sdw::InVec3
			, InPbrLightMaterial
			, InSurface
			, OutputComponents & > m_computeCookTorrance;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InVec3
			, InPbrLightMaterial
			, InSurface > m_computeCookTorranceDiffuse;
	};
}

#endif
