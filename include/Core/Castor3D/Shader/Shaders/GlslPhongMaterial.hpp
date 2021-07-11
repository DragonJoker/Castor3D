/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslPhongMaterialModel_H___
#define ___C3D_GlslPhongMaterialModel_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

namespace castor3d::shader
{
	struct PhongLightMaterial
		: public sdw::StructInstance
	{
		template< MaterialType MaterialT >
		struct CreatorT;

		template<>
		struct CreatorT< MaterialType::ePhong >
		{
			static void create( PhongLightMaterial & material
				, sdw::Vec3 const & albedo
				, sdw::Vec3 const & specular
				, sdw::Float const & shininess )
			{
				material.specular = specular;
				material.shininess = shininess;
			}

			static void create( PhongLightMaterial & material
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
			static void create( PhongLightMaterial & material
				, sdw::Vec3 const & albedo
				, sdw::Float const & metalness
				, sdw::Float const & roughness )
			{
				material.specular = LightingModel::computeF0( albedo, metalness );
				material.shininess = LightingModel::computeShininess( LightingModel::computeRoughness( roughness ) );
			}

			static void create( PhongLightMaterial & material
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
			static void create( PhongLightMaterial & material
				, sdw::Vec3 const & albedo
				, sdw::Vec3 const & specular
				, sdw::Float const & glossiness )
			{
				material.specular = specular;
				material.shininess = LightingModel::computeShininess( glossiness );
			}

			static void create( PhongLightMaterial & material
				, sdw::Vec3 const & albedo
				, sdw::Vec4 const & data3
				, sdw::Vec4 const & data2 )
			{
				create( material, albedo, data3.rgb(), data2.a() );
			}
		};

		C3D_API PhongLightMaterial( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );
		C3D_API PhongLightMaterial & operator=( PhongLightMaterial const & rhs );

		template< MaterialType MaterialT
			, typename ... ParamsT >
			void create( ParamsT const & ... params )
		{
			CreatorT< MaterialT >::create( *this, params... );
		}

		C3D_API static ast::type::StructPtr makeType( ast::type::TypesCache & cache );

		sdw::Vec3 specular;
		sdw::Float shininess;

	private:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;
	};

	struct PhongMaterial
		: public BaseMaterial
	{
		friend class PhongMaterials;

		C3D_API PhongMaterial( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );

		C3D_API static ast::type::StructPtr makeType( ast::type::TypesCache & cache );
		C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

		C3D_API sdw::Vec3 colour()const override;

	private:
		sdw::Vec4 m_diffAmb;
		sdw::Vec4 m_specShin;

	public:
		sdw::Vec3 diffuse;
		sdw::Float ambient;
		sdw::Vec3 specular;
		sdw::Float shininess;
	};

	class PhongMaterials
		: public Materials
	{
	public:
		C3D_API explicit PhongMaterials( sdw::ShaderWriter & writer );
		C3D_API void declare( bool hasSsbo
			, uint32_t binding
			, uint32_t set )override;
		C3D_API PhongMaterial getMaterial( sdw::UInt const & index )const;
		C3D_API BaseMaterialUPtr getBaseMaterial( sdw::UInt const & index )const override;

	private:
		std::unique_ptr< sdw::ArraySsboT< PhongMaterial > > m_ssbo;
		sdw::Function< PhongMaterial, sdw::InUInt > m_getMaterial;
	};
}

#endif
