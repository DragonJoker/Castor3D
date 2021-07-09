/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslPhongMaterialModel_H___
#define ___C3D_GlslPhongMaterialModel_H___

#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

namespace castor3d::shader
{
	struct PhongLightMaterial
	{
		C3D_API explicit PhongLightMaterial( sdw::ShaderWriter & writer );
		C3D_API PhongLightMaterial( sdw::InVec3 const & specular
			, sdw::InFloat const & shininess );

		C3D_API ast::expr::Expr * getExpr()const;
		C3D_API sdw::ShaderWriter * getWriter()const;
		C3D_API void setVar( ast::var::VariableList::const_iterator & var );

		bool isEnabled()const
		{
			return true;
		}

		sdw::InVec3 specular;
		sdw::InFloat shininess;

	private:
		ast::expr::ExprPtr m_expr;
	};

	C3D_API ast::expr::ExprList makeFnArg( sdw::ShaderWriter & shader
		, PhongLightMaterial const & value );

	struct PhongMaterial
		: public BaseMaterial
	{
		friend class PhongMaterials;

		C3D_API PhongMaterial( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );

		C3D_API PhongLightMaterial getLightMaterial()const;
		C3D_API static PhongLightMaterial getLightMaterial( sdw::Vec3 specular
			, sdw::Float shininess );
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
