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
			, sdw::Vec3Field< "ambient" >
			, sdw::Vec3Field< "diffuse" >
			, sdw::Vec3Field< "specular" >
			, sdw::Vec3Field< "scattering" >
			, sdw::Vec3Field< "coatingSpecular" >
			, sdw::Vec2Field< "sheen" > >
	{
		DirectLighting( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		DirectLighting( sdw::ShaderWriter & writer )
			: DirectLighting{ writer
				, sdw::makeAggrInit( DirectLighting::makeType( writer.getTypesCache() ), makeZero() )
				, true }
		{
		}

		auto ambient()const { return getMember< "ambient" >(); }
		auto diffuse()const { return getMember< "diffuse" >(); }
		auto specular()const { return getMember< "specular" >(); }
		auto scattering()const { return getMember< "scattering" >(); }
		auto coatingSpecular()const { return getMember< "coatingSpecular" >(); }
		auto sheen()const { return getMember< "sheen" >(); }

	private:
		C3D_API static sdw::expr::ExprList makeZero();
	};

	struct IndirectLighting
		: public sdw::StructInstanceHelperT< "C3D_IndirectLighting"
			, sdw::type::MemoryLayout::eC
			, sdw::Vec3Field< "ambient" >
			, sdw::Vec4Field< "diffuse" >
			, sdw::Vec3Field< "specular" >
			, sdw::FloatField< "occlusion" > >
	{
		IndirectLighting( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		IndirectLighting( sdw::ShaderWriter & writer )
			: IndirectLighting{ writer
				, sdw::makeAggrInit( IndirectLighting::makeType( writer.getTypesCache() ), makeZero() )
				, true }
		{
		}

		auto ambient()const { return getMember< "ambient" >(); }
		auto rawDiffuse()const { return getMember< "diffuse" >(); }
		auto specular()const { return getMember< "specular" >(); }
		auto occlusion()const { return getMember< "occlusion" >(); }
		auto diffuseColour()const { return rawDiffuse().rgb(); }
		auto diffuseBlend()const { return rawDiffuse().a(); }

	private:
		C3D_API static sdw::expr::ExprList makeZero();
	};
}

#endif
