/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslDirectLighting_H___
#define ___C3D_GlslDirectLighting_H___

#include "SdwModule.hpp"

#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace castor3d::shader
{
	struct DirectLighting
		: public sdw::StructInstanceHelperT< "C3D_DirectLighting"
			, sdw::type::MemoryLayout::eC
			, sdw::Vec3Field< "diffuse" >
			, sdw::Vec3Field< "dielectric" >
			, sdw::Vec3Field< "metal" >
			, sdw::Vec3Field< "scattering" >
			, sdw::Vec3Field< "coating" >
			, sdw::Vec4Field< "sheen" > >
	{
		C3D_API DirectLighting( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );
		C3D_API explicit DirectLighting( sdw::ShaderWriter & writer );

		C3D_API void attenuate( sdw::Float const attenuation
			, bool withScattering
			, bool withDiffuse );
		C3D_API void registerDebug( DebugOutputCategory const & debugOutput )const;

		C3D_API DirectLighting & operator+=( DirectLighting const & rhs );
		C3D_API DirectLighting & operator*=( sdw::Float const & rhs );

		sdw::Vec3 diffuse;
		sdw::Vec3 dielectric;
		sdw::Vec3 metal;
		sdw::Vec3 scattering;
		sdw::Vec3 coating;
		sdw::Vec4 sheen;

	private:
		static sdw::expr::ExprList makeInit();
	};

	struct IndirectLighting
		: public sdw::StructInstanceHelperT< "C3D_IndirectLighting"
			, sdw::type::MemoryLayout::eC
			, sdw::Vec3Field< "ambient" >
			, sdw::Vec4Field< "diffuse" >
			, sdw::Vec3Field< "specular" >
			, sdw::FloatField< "occlusion" > >
	{
		C3D_API IndirectLighting( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );
		C3D_API explicit IndirectLighting( sdw::ShaderWriter & writer );

		C3D_API void registerDebug( DebugOutputCategory const & debugOutput )const;

		sdw::Vec3 ambient;
		sdw::Vec4 rawDiffuse;
		sdw::Vec3 specular;
		sdw::Float occlusion;
		sdw::Vec3 diffuseColour;
		sdw::Float diffuseBlend;

	private:
		static sdw::expr::ExprList makeInit();
	};

	struct ReflectionRefraction
		: public sdw::StructInstanceHelperT< "C3D_ReflectionRefraction"
			, sdw::type::MemoryLayout::eC
			, sdw::Vec3Field< "diffuse" >
			, sdw::Vec3Field< "dielectric" >
			, sdw::Vec3Field< "metal" >
			, sdw::Vec3Field< "coating" >
			, sdw::Vec4Field< "sheen" > >
	{
		C3D_API ReflectionRefraction( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled );
		C3D_API explicit ReflectionRefraction( sdw::ShaderWriter & writer );

		C3D_API void registerDebug( DebugOutputCategory const & debugOutput )const;

		sdw::Vec3 diffuse;
		sdw::Vec3 dielectric;
		sdw::Vec3 metal;
		sdw::Vec3 coating;
		sdw::Vec4 sheen;

	private:
		static sdw::expr::ExprList makeInit();
	};
}

#endif
