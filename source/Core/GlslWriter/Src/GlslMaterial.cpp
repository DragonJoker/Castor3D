#include "GlslMaterial.hpp"

#include "GlslSource.hpp"

using namespace Castor;

namespace GLSL
{
#if !GLSL_MATERIALS_STRUCT_OF_ARRAY

	struct BaseMaterial
		: public Type
	{
		BaseMaterial( Castor::String const & type )
			: Type( type )
		{
		}

		BaseMaterial( GlslWriter * writer, Castor::String const & name = Castor::String() )
			: BaseMaterial( cuT( "BaseMaterial " ), writer, name )
		{
		}

		BaseMaterial( Castor::String const & type
			, GlslWriter * writer
			, Castor::String const & name = Castor::String() )
			: Type( type, writer, name )
		{
		}

		Float m_opacity()
		{
			return Vec4( m_writer, m_name + cuT( ".m_common" ) ).x();
		}

		Float m_emissive()
		{
			return Vec4( m_writer, m_name + cuT( ".m_common" ) ).y();
		}

		Float m_alphaRef()
		{
			return Vec4( m_writer, m_name + cuT( ".m_common" ) ).z();
		}

		Float m_gamma()
		{
			return Vec4( m_writer, m_name + cuT( ".m_common" ) ).w();
		}

		Float m_refractionRatio()
		{
			return Vec4( m_writer, m_name + cuT( ".m_reflRefr" ) ).x();
		}

		Int m_hasRefraction()
		{
			return m_writer->Cast< Int >( Vec4( m_writer, m_name + cuT( ".m_reflRefr" ) ).y() );
		}

		Int m_hasReflection()
		{
			return m_writer->Cast< Int >( Vec4( m_writer, m_name + cuT( ".m_reflRefr" ) ).z() );
		}

		Float m_exposure()
		{
			return Vec4( m_writer, m_name + cuT( ".m_reflRefr" ) ).w();
		}
	};

	template<>
	struct name_of< BaseMaterial >
	{
		static TypeName const value = TypeName::eMaterial;
	};

	struct LegacyMaterial
		: public BaseMaterial
	{
		LegacyMaterial()
			: BaseMaterial( cuT( "LegacyMaterial " ) )
		{
		}

		LegacyMaterial( GlslWriter * writer, Castor::String const & name = Castor::String() )
			: BaseMaterial( cuT( "LegacyMaterial " ), writer, name )
		{
		}

		LegacyMaterial & operator=( LegacyMaterial const & rhs )
		{
			if ( m_writer )
			{
				m_writer->WriteAssign( *this, rhs );
			}
			else
			{
				Type::operator=( rhs );
				m_writer = rhs.m_writer;
			}

			return *this;
		}

		template< typename T >
		LegacyMaterial & operator=( T const & rhs )
		{
			UpdateWriter( rhs );
			m_writer->WriteAssign( *this, rhs );
			return *this;
		}

		static void Declare( GlslWriter & writer )
		{
			Struct material = writer.GetStruct( cuT( "LegacyMaterial" ) );
			material.DeclMember< Vec4 >( cuT( "m_diffAmb" ) );
			material.DeclMember< Vec4 >( cuT( "m_specShin" ) );
			material.DeclMember< Vec4 >( cuT( "m_common" ) );
			material.DeclMember< Vec4 >( cuT( "m_reflRefr" ) );
			material.End();
		}

		Vec3 m_diffuse()
		{
			return Vec4( m_writer, m_name + cuT( ".m_diffAmb" ) ).xyz();
		}

		Float m_ambient()
		{
			return Vec4( m_writer, m_name + cuT( ".m_diffAmb" ) ).w();
		}

		Vec3 m_specular()
		{
			return Vec4( m_writer, m_name + cuT( ".m_specShin" ) ).xyz();
		}

		Float m_shininess()
		{
			return Vec4( m_writer, m_name + cuT( ".m_specShin" ) ).w();
		}
	};

	template<>
	struct name_of< LegacyMaterial >
	{
		static TypeName const value = TypeName::eLegacyMaterial;
	};

	struct MetallicRoughnessMaterial
		: public BaseMaterial
	{
		MetallicRoughnessMaterial()
			: BaseMaterial( cuT( "MetallicRoughnessMaterial " ) )
		{
		}

		MetallicRoughnessMaterial( GlslWriter * writer, Castor::String const & name = Castor::String() )
			: BaseMaterial( cuT( "MetallicRoughnessMaterial " ), writer, name )
		{
		}

		MetallicRoughnessMaterial & operator=( MetallicRoughnessMaterial const & rhs )
		{
			if ( m_writer )
			{
				m_writer->WriteAssign( *this, rhs );
			}
			else
			{
				Type::operator=( rhs );
				m_writer = rhs.m_writer;
			}

			return *this;
		}

		template< typename T >
		MetallicRoughnessMaterial & operator=( T const & rhs )
		{
			UpdateWriter( rhs );
			m_writer->WriteAssign( *this, rhs );
			return *this;
		}

		static void Declare( GlslWriter & writer )
		{
			Struct material = writer.GetStruct( cuT( "MetallicRoughnessMaterial" ) );
			material.DeclMember< Vec4 >( cuT( "m_albRough" ) );
			material.DeclMember< Vec4 >( cuT( "m_metDiv" ) );
			material.DeclMember< Vec4 >( cuT( "m_common" ) );
			material.DeclMember< Vec4 >( cuT( "m_reflRefr" ) );
			material.End();
		}

		Vec3 m_albedo()
		{
			return Vec4( m_writer, m_name + cuT( ".m_albRough" ) ).xyz();
		}

		Float m_roughness()
		{
			return Vec4( m_writer, m_name + cuT( ".m_albRough" ) ).w();
		}

		Float m_metallic()
		{
			return Vec4( m_writer, m_name + cuT( ".m_metDiv" ) ).x();
		}
	};

	template<>
	struct name_of< MetallicRoughnessMaterial >
	{
		static TypeName const value = TypeName::eMetallicRoughnessMaterial;
	};

	struct SpecularGlossinessMaterial
		: public BaseMaterial
	{
		SpecularGlossinessMaterial()
			: BaseMaterial( cuT( "SpecularGlossinessMaterial " ) )
		{
		}

		SpecularGlossinessMaterial( GlslWriter * writer, Castor::String const & name = Castor::String() )
			: BaseMaterial( cuT( "SpecularGlossinessMaterial " ), writer, name )
		{
		}

		SpecularGlossinessMaterial & operator=( SpecularGlossinessMaterial const & rhs )
		{
			if ( m_writer )
			{
				m_writer->WriteAssign( *this, rhs );
			}
			else
			{
				Type::operator=( rhs );
				m_writer = rhs.m_writer;
			}

			return *this;
		}

		template< typename T >
		SpecularGlossinessMaterial & operator=( T const & rhs )
		{
			UpdateWriter( rhs );
			m_writer->WriteAssign( *this, rhs );
			return *this;
		}

		static void Declare( GlslWriter & writer )
		{
			Struct material = writer.GetStruct( cuT( "SpecularGlossinessMaterial" ) );
			material.DeclMember< Vec4 >( cuT( "m_diffDiv" ) );
			material.DeclMember< Vec4 >( cuT( "m_specGloss" ) );
			material.DeclMember< Vec4 >( cuT( "m_common" ) );
			material.DeclMember< Vec4 >( cuT( "m_reflRefr" ) );
			material.End();
		}

		Vec3 m_diffuse()
		{
			return Vec4( m_writer, m_name + cuT( ".m_diffDiv" ) ).xyz();
		}

		Vec3 m_specular()
		{
			return Vec4( m_writer, m_name + cuT( ".m_specGloss" ) ).xyz();
		}

		Float m_glossiness()
		{
			return Vec4( m_writer, m_name + cuT( ".m_specGloss" ) ).w();
		}
	};

	template<>
	struct name_of< SpecularGlossinessMaterial >
	{
		static TypeName const value = TypeName::eSpecularGlossinessMaterial;
	};

	//*********************************************************************************************

	Materials::Materials( GlslWriter & writer )
		: m_writer{ writer }
	{
	}

	Float Materials::GetRefractionRatio( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltinArray< BaseMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_refractionRatio();
	}

	Int Materials::GetRefraction( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltinArray< BaseMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_hasRefraction();
	}

	Int Materials::GetReflection( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltinArray< BaseMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_hasReflection();
	}

	Float Materials::GetOpacity( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltinArray< BaseMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_opacity();
	}

	Float Materials::GetGamma( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltinArray< BaseMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_gamma();
	}

	Float Materials::GetExposure( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltinArray< BaseMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_exposure();
	}

	Float Materials::GetAlphaRef( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltinArray< BaseMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_alphaRef();
	}

	Float Materials::GetEmissive( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltinArray< BaseMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_emissive();
	}

	//*********************************************************************************************

	LegacyMaterials::LegacyMaterials( GlslWriter & writer )
		: Materials{ writer }
	{
	}

	void LegacyMaterials::Declare()
	{
		LegacyMaterial::Declare( m_writer );
		Ssbo materials{ m_writer, PassBufferName, 0u };
		materials.DeclMemberArray< LegacyMaterial >( cuT( "c3d_materials" ) );
		materials.End();
	}

	Vec3 LegacyMaterials::GetDiffuse( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltinArray< LegacyMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_diffuse();
	}

	Vec3 LegacyMaterials::GetSpecular( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltinArray< LegacyMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_specular();
	}

	Float LegacyMaterials::GetAmbient( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltinArray< LegacyMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_ambient();
	}

	Float LegacyMaterials::GetShininess( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltinArray< LegacyMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_shininess();
	}

	//*********************************************************************************************

	PbrMRMaterials::PbrMRMaterials( GlslWriter & writer )
		: Materials{ writer }
	{
	}

	void PbrMRMaterials::Declare()
	{
		MetallicRoughnessMaterial::Declare( m_writer );
		Ssbo materials{ m_writer, PassBufferName, 0u };
		materials.DeclMemberArray< MetallicRoughnessMaterial >( cuT( "c3d_materials" ) );
		materials.End();
	}

	Vec3 PbrMRMaterials::GetDiffuse( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltinArray< MetallicRoughnessMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_albedo();
	}

	Vec3 PbrMRMaterials::GetAlbedo( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltinArray< MetallicRoughnessMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_albedo();
	}

	Float PbrMRMaterials::GetRoughness( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltinArray< MetallicRoughnessMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_roughness();
	}

	Float PbrMRMaterials::GetMetallic( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltinArray< MetallicRoughnessMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_metallic();
	}

	//*********************************************************************************************

	PbrSGMaterials::PbrSGMaterials( GlslWriter & writer )
		: Materials{ writer }
	{
	}

	void PbrSGMaterials::Declare()
	{
		SpecularGlossinessMaterial::Declare( m_writer );
		Ssbo materials{ m_writer, PassBufferName, 0u };
		materials.DeclMemberArray< SpecularGlossinessMaterial >( cuT( "c3d_materials" ) );
		materials.End();
	}

	Vec3 PbrSGMaterials::GetDiffuse( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltinArray< SpecularGlossinessMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_diffuse();
	}

	Vec3 PbrSGMaterials::GetSpecular( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltinArray< SpecularGlossinessMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_specular();
	}

	Float PbrSGMaterials::GetGlossiness( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltinArray< SpecularGlossinessMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_glossiness();
	}

	//*********************************************************************************************

#else

	//*********************************************************************************************

	Materials::Materials( GlslWriter & writer )
		: m_writer{ writer }
	{
	}

	Float Materials::GetOpacity( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Vec4 >( cuT( "c3d_matCommon" ), MaxMaterialsCount );
		return tmp[index].x();
	}

	Float Materials::GetEmissive( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Vec4 >( cuT( "c3d_matCommon" ), MaxMaterialsCount );
		return tmp[index].y();
	}

	Float Materials::GetAlphaRef( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Vec4 >( cuT( "c3d_matCommon" ), MaxMaterialsCount );
		return tmp[index].z();
	}

	Float Materials::GetGamma( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Vec4 >( cuT( "c3d_matCommon" ), MaxMaterialsCount );
		return tmp[index].w();
	}

	Float Materials::GetRefractionRatio( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Vec4 >( cuT( "c3d_matReflRer" ), MaxMaterialsCount );
		return tmp[index].x();
	}

	Int Materials::GetRefraction( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Vec4 >( cuT( "c3d_matReflRer" ), MaxMaterialsCount );
		return m_writer.Cast< Int >( tmp[index].y() );
	}

	Int Materials::GetReflection( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Vec4 >( cuT( "c3d_matReflRer" ), MaxMaterialsCount );
		return m_writer.Cast< Int >( tmp[index].z() );
	}

	Float Materials::GetExposure( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Vec4 >( cuT( "c3d_matExposure" ), MaxMaterialsCount );
		return tmp[index].w();
	}

	//*********************************************************************************************

	LegacyMaterials::LegacyMaterials( GlslWriter & writer )
		: Materials{ writer }
	{
	}

	void LegacyMaterials::Declare()
	{
		Ssbo materials{ m_writer, PassBufferName, 0u };
		materials.DeclMember< Vec4 >( cuT( "c3d_matDiffAmb" ), MaxMaterialsCount );
		materials.DeclMember< Vec4 >( cuT( "c3d_matSpecShin" ), MaxMaterialsCount );
		materials.DeclMember< Vec4 >( cuT( "c3d_matCommon" ), MaxMaterialsCount );
		materials.DeclMember< Vec4 >( cuT( "c3d_matReflRer" ), MaxMaterialsCount );
		materials.End();
	}

	Vec3 LegacyMaterials::GetDiffuse( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Vec4 >( cuT( "c3d_matDiffAmb" ), MaxMaterialsCount );
		return tmp[index].xyz();
	}

	Vec3 LegacyMaterials::GetSpecular( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Vec4 >( cuT( "c3d_matSpecShin" ), MaxMaterialsCount );
		return tmp[index].xyz();
	}

	Float LegacyMaterials::GetAmbient( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Vec4 >( cuT( "c3d_matDiffAmb" ), MaxMaterialsCount );
		return tmp[index].w();
	}

	Float LegacyMaterials::GetShininess( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Vec4 >( cuT( "c3d_matSpecShin" ), MaxMaterialsCount );
		return tmp[index].w();
	}

	//*********************************************************************************************

	PbrMRMaterials::PbrMRMaterials( GlslWriter & writer )
		: Materials{ writer }
	{
	}

	void PbrMRMaterials::Declare()
	{
		Ssbo materials{ m_writer, PassBufferName, 0u };
		materials.DeclMember< Vec4 >( cuT( "c3d_matAlbRough" ), MaxMaterialsCount );
		materials.DeclMember< Vec4 >( cuT( "c3d_matMetDiv" ), MaxMaterialsCount );
		materials.DeclMember< Vec4 >( cuT( "c3d_matCommon" ), MaxMaterialsCount );
		materials.DeclMember< Vec4 >( cuT( "c3d_matReflRer" ), MaxMaterialsCount );
		materials.End();
	}

	Vec3 PbrMRMaterials::GetDiffuse( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Vec4 >( cuT( "c3d_matAlbRough" ), MaxMaterialsCount );
		return tmp[index].xyz();
	}

	Vec3 PbrMRMaterials::GetAlbedo( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Vec4 >( cuT( "c3d_matAlbRough" ), MaxMaterialsCount );
		return tmp[index].xyz();
	}

	Float PbrMRMaterials::GetRoughness( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Vec4 >( cuT( "c3d_matAlbRough" ), MaxMaterialsCount );
		return tmp[index].w();
	}

	Float PbrMRMaterials::GetMetallic( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Vec4 >( cuT( "c3d_matMetDiv" ), MaxMaterialsCount );
		return tmp[index].x();
	}

	//*********************************************************************************************

	PbrSGMaterials::PbrSGMaterials( GlslWriter & writer )
		: Materials{ writer }
	{
	}

	void PbrSGMaterials::Declare()
	{
		Ssbo materials{ m_writer, PassBufferName, 0u };
		materials.DeclMember< Vec4 >( cuT( "c3d_matDiffDiv" ), MaxMaterialsCount );
		materials.DeclMember< Vec4 >( cuT( "c3d_matSpecGloss" ), MaxMaterialsCount );
		materials.DeclMember< Vec4 >( cuT( "c3d_matCommon" ), MaxMaterialsCount );
		materials.DeclMember< Vec4 >( cuT( "c3d_matReflRer" ), MaxMaterialsCount );
		materials.End();
	}

	Vec3 PbrSGMaterials::GetDiffuse( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Vec4 >( cuT( "c3d_matDiffDiv" ), MaxMaterialsCount );
		return tmp[index].xyz();
	}

	Vec3 PbrSGMaterials::GetSpecular( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Vec4 >( cuT( "c3d_matSpecGloss" ), MaxMaterialsCount );
		return tmp[index].xyz();
	}

	Float PbrSGMaterials::GetGlossiness( Int const & index )const
	{
		auto tmp = m_writer.GetBuiltin< Vec4 >( cuT( "c3d_matSpecGloss" ), MaxMaterialsCount );
		return tmp[index].w();
	}

	//*********************************************************************************************

#endif
}
