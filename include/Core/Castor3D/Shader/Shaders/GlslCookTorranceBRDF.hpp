/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslCookTorranceBRDF_H___
#define ___C3D_GlslCookTorranceBRDF_H___

#include "Castor3D/Shader/Shaders/GlslLighting.hpp"

namespace castor3d::shader
{
	struct PbrLightMaterial
	{
		C3D_API explicit PbrLightMaterial( sdw::ShaderWriter & writer );
		C3D_API PbrLightMaterial( sdw::InOutVec3 const & specular
			, sdw::InOutFloat const & metalness
			, sdw::InOutFloat const & roughness );

		C3D_API ast::expr::Expr * getExpr()const;
		C3D_API sdw::ShaderWriter * getWriter()const;
		C3D_API void setVar( ast::var::VariableList::const_iterator & var );

		bool isEnabled()const
		{
			return true;
		}

		sdw::InOutVec3 specular;
		sdw::InOutFloat metalness;
		sdw::InOutFloat roughness;

	private:
		ast::expr::ExprPtr m_expr;
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
			, PbrLightMaterial & material
			, Surface surface
			, OutputComponents & output )const;
		C3D_API sdw::Vec3 computeDiffuse( sdw::Vec3 const & colour
			, sdw::Vec3 const & worldEye
			, sdw::Vec3 const & direction
			, PbrLightMaterial & material
			, Surface surface )const;
		C3D_API sdw::Vec3 computeDiffuse( Light const & light
			, sdw::Vec3 const & worldEye
			, sdw::Vec3 const & direction
			, PbrLightMaterial & material
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
			, PbrLightMaterial &
			, InSurface
			, OutputComponents & > m_computeCookTorrance;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InVec3
			, PbrLightMaterial &
			, InSurface > m_computeCookTorranceDiffuse;
	};
}

#endif
