#include "GlslMaterial.hpp"

#include "GlslSource.hpp"

using namespace Castor;

namespace GLSL
{
	//*********************************************************************************************

	Materials::Materials( GlslWriter & writer )
		: m_writer{ writer }
	{
	}

	Float Materials::GetRefractionRatio( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Float >( cuT( "c3d_matRefractionRatio" ), MaxMaterialsCount );
		return tmp[index];
	}

	Int Materials::GetRefraction( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Int >( cuT( "c3d_matHasRefraction" ), MaxMaterialsCount );
		return tmp[index];
	}

	Int Materials::GetReflection( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Int >( cuT( "c3d_matHasReflection" ), MaxMaterialsCount );
		return tmp[index];
	}

	Float Materials::GetOpacity( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Float >( cuT( "c3d_matOpacity" ), MaxMaterialsCount );
		return tmp[index];
	}

	Float Materials::GetGamma( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Float >( cuT( "c3d_matGamma" ), MaxMaterialsCount );
		return tmp[index];
	}

	Float Materials::GetExposure( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Float >( cuT( "c3d_matExposure" ), MaxMaterialsCount );
		return tmp[index];
	}

	Float Materials::GetAlphaRef( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Float >( cuT( "c3d_matAlphaRef" ), MaxMaterialsCount );
		return tmp[index];
	}

	Float Materials::GetEmissive( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Float >( cuT( "c3d_matEmissive" ), MaxMaterialsCount );
		return tmp[index];
	}

	//*********************************************************************************************

	LegacyMaterials::LegacyMaterials( GlslWriter & writer )
		: Materials{ writer }
	{
	}

	void LegacyMaterials::Declare()
	{
		Ssbo materials{ m_writer, PassBufferName, 0u };
		materials.DeclMember< Vec3 >( cuT( "c3d_matDiffuse" ), MaxMaterialsCount );
		materials.DeclMember< Vec3 >( cuT( "c3d_matSpecular" ), MaxMaterialsCount );
		materials.DeclMember< Float >( cuT( "c3d_matShininess" ), MaxMaterialsCount );
		materials.DeclMember< Float >( cuT( "c3d_matAmbient" ), MaxMaterialsCount );
		materials.DeclMember< Float >( cuT( "c3d_matOpacity" ), MaxMaterialsCount );
		materials.DeclMember< Float >( cuT( "c3d_matEmissive" ), MaxMaterialsCount );
		materials.DeclMember< Float >( cuT( "c3d_matGamma" ), MaxMaterialsCount );
		materials.DeclMember< Float >( cuT( "c3d_matExposure" ), MaxMaterialsCount );
		materials.DeclMember< Float >( cuT( "c3d_matAlphaRef" ), MaxMaterialsCount );
		materials.DeclMember< Float >( cuT( "c3d_matRefractionRatio" ), MaxMaterialsCount );
		materials.DeclMember< Int >( cuT( "c3d_matHasRefraction" ), MaxMaterialsCount );
		materials.DeclMember< Int >( cuT( "c3d_matHasReflection" ), MaxMaterialsCount );
		materials.End();
	}

	Vec3 LegacyMaterials::GetDiffuse( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Vec3 >( cuT( "c3d_matDiffuse" ), MaxMaterialsCount );
		return tmp[index];
	}

	Vec3 LegacyMaterials::GetSpecular( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Vec3 >( cuT( "c3d_matSpecular" ), MaxMaterialsCount );
		return tmp[index];
	}

	Float LegacyMaterials::GetAmbient( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Float >( cuT( "c3d_matAmbient" ), MaxMaterialsCount );
		return tmp[index];
	}

	Float LegacyMaterials::GetShininess( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Float >( cuT( "c3d_matShininess" ), MaxMaterialsCount );
		return tmp[index];
	}

	//*********************************************************************************************

	PbrMRMaterials::PbrMRMaterials( GlslWriter & writer )
		: Materials{ writer }
	{
	}

	void PbrMRMaterials::Declare()
	{
		Ssbo materials{ m_writer, PassBufferName, 0u };
		materials.DeclMember< Vec3 >( cuT( "c3d_matAlbedo" ), MaxMaterialsCount );
		materials.DeclMember< Float >( cuT( "c3d_matRoughness" ), MaxMaterialsCount );
		materials.DeclMember< Float >( cuT( "c3d_matMetallic" ), MaxMaterialsCount );
		materials.DeclMember< Float >( cuT( "c3d_matOpacity" ), MaxMaterialsCount );
		materials.DeclMember< Float >( cuT( "c3d_matEmissive" ), MaxMaterialsCount );
		materials.DeclMember< Float >( cuT( "c3d_matGamma" ), MaxMaterialsCount );
		materials.DeclMember< Float >( cuT( "c3d_matExposure" ), MaxMaterialsCount );
		materials.DeclMember< Float >( cuT( "c3d_matAlphaRef" ), MaxMaterialsCount );
		materials.DeclMember< Float >( cuT( "c3d_matRefractionRatio" ), MaxMaterialsCount );
		materials.DeclMember< Int >( cuT( "c3d_matHasRefraction" ), MaxMaterialsCount );
		materials.DeclMember< Int >( cuT( "c3d_matHasReflection" ), MaxMaterialsCount );
		materials.End();
	}

	Vec3 PbrMRMaterials::GetDiffuse( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Vec3 >( cuT( "c3d_matAlbedo" ), MaxMaterialsCount );
		return tmp[index];
	}

	Vec3 PbrMRMaterials::GetAlbedo( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Vec3 >( cuT( "c3d_matAlbedo" ), MaxMaterialsCount );
		return tmp[index];
	}

	Float PbrMRMaterials::GetRoughness( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Float >( cuT( "c3d_matRoughness" ), MaxMaterialsCount );
		return tmp[index];
	}

	Float PbrMRMaterials::GetMetallic( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Float >( cuT( "c3d_matMetallic" ), MaxMaterialsCount );
		return tmp[index];
	}

	//*********************************************************************************************

	PbrSGMaterials::PbrSGMaterials( GlslWriter & writer )
		: Materials{ writer }
	{
	}

	void PbrSGMaterials::Declare()
	{
		Ssbo materials{ m_writer, PassBufferName, 0u };
		materials.DeclMember< Vec3 >( cuT( "c3d_matDiffuse" ), MaxMaterialsCount );
		materials.DeclMember< Vec3 >( cuT( "c3d_matSpecular" ), MaxMaterialsCount );
		materials.DeclMember< Float >( cuT( "c3d_matGlossiness" ), MaxMaterialsCount );
		materials.DeclMember< Float >( cuT( "c3d_matOpacity" ), MaxMaterialsCount );
		materials.DeclMember< Float >( cuT( "c3d_matEmissive" ), MaxMaterialsCount );
		materials.DeclMember< Float >( cuT( "c3d_matGamma" ), MaxMaterialsCount );
		materials.DeclMember< Float >( cuT( "c3d_matExposure" ), MaxMaterialsCount );
		materials.DeclMember< Float >( cuT( "c3d_matAlphaRef" ), MaxMaterialsCount );
		materials.DeclMember< Float >( cuT( "c3d_matRefractionRatio" ), MaxMaterialsCount );
		materials.DeclMember< Int >( cuT( "c3d_matHasRefraction" ), MaxMaterialsCount );
		materials.DeclMember< Int >( cuT( "c3d_matHasReflection" ), MaxMaterialsCount );
		materials.End();
	}

	Vec3 PbrSGMaterials::GetDiffuse( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Vec3 >( cuT( "c3d_matDiffuse" ), MaxMaterialsCount );
		return tmp[index];
	}

	Vec3 PbrSGMaterials::GetSpecular( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Vec3 >( cuT( "c3d_matSpecular" ), MaxMaterialsCount );
		return tmp[index];
	}

	Float PbrSGMaterials::GetGlossiness( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Float >( cuT( "c3d_matGlossiness" ), MaxMaterialsCount );
		return tmp[index];
	}

	//*********************************************************************************************
}
