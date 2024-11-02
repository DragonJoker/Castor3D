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
			, sdw::Vec3Field< "coating" >
			, sdw::Vec4Field< "sheen" > >
	{
		DirectLighting( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, castor::move( expr ), enabled }
		{
		}

		explicit DirectLighting( sdw::ShaderWriter & writer )
			: DirectLighting{ writer
				, sdw::makeAggrInit( DirectLighting::makeType( sdw::getTypesCache( writer ) ), makeInit() )
				, true }
		{
		}

		auto ambient()const { return getMember< "ambient" >(); }
		auto diffuse()const { return getMember< "diffuse" >(); }
		auto specular()const { return getMember< "specular" >(); }
		auto scattering()const { return getMember< "scattering" >(); }
		auto coating()const { return getMember< "coating" >(); }
		auto sheen()const { return getMember< "sheen" >(); }

		void registerDebug( DebugOutput & debugOutput
			, castor::String const & category )const;

	private:
		C3D_API static sdw::expr::ExprList makeInit();
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
			: StructInstanceHelperT{ writer, castor::move( expr ), enabled }
		{
		}

		explicit IndirectLighting( sdw::ShaderWriter & writer )
			: IndirectLighting{ writer
				, sdw::makeAggrInit( IndirectLighting::makeType( sdw::getTypesCache( writer ) ), makeInit() )
				, true }
		{
		}

		auto ambient()const { return getMember< "ambient" >(); }
		auto rawDiffuse()const { return getMember< "diffuse" >(); }
		auto specular()const { return getMember< "specular" >(); }
		auto occlusion()const { return getMember< "occlusion" >(); }
		auto diffuseColour()const { return rawDiffuse().rgb(); }
		auto diffuseBlend()const { return rawDiffuse().a(); }

		void registerDebug( DebugOutput & debugOutput
			, castor::String const & category )const;

	private:
		C3D_API static sdw::expr::ExprList makeInit();
	};

	struct ReflectionRefraction
		: public sdw::StructInstanceHelperT< "C3D_ReflectionRefraction"
			, sdw::type::MemoryLayout::eC
			, sdw::Vec3Field< "reflDiffuse" >
			, sdw::Vec3Field< "reflSpecular" >
			, sdw::Vec3Field< "reflCoating" >
			, sdw::Vec4Field< "reflSheen" >
			, sdw::Vec3Field< "refrColour" > >
	{
		ReflectionRefraction( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, castor::move( expr ), enabled }
			, reflDiffuse { getMember< "reflDiffuse" >() }
			, reflSpecular { getMember< "reflSpecular" >() }
			, reflCoating { getMember< "reflCoating" >() }
			, reflSheen { getMember< "reflSheen" >() }
			, refrColour { getMember< "refrColour" >() }
		{
		}

		explicit ReflectionRefraction( sdw::ShaderWriter & writer )
			: ReflectionRefraction{ writer
				, sdw::makeAggrInit( IndirectLighting::makeType( sdw::getTypesCache( writer ) ), makeInit() )
				, true }
		{
		}

		sdw::Vec3 reflDiffuse;
		sdw::Vec3 reflSpecular;
		sdw::Vec3 reflCoating;
		sdw::Vec4 reflSheen;
		sdw::Vec3 refrColour;

		void registerDebug( DebugOutput & debugOutput
			, castor::String const & category )const;

	private:
		C3D_API static sdw::expr::ExprList makeInit();
	};
}

#endif
