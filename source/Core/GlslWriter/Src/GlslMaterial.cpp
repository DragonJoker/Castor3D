#include "GlslMaterial.hpp"

#include "GlslSource.hpp"

using namespace castor;

namespace glsl
{
#if !GLSL_MATERIALS_STRUCT_OF_ARRAY

	struct BaseMaterial
		: public Type
	{
		BaseMaterial( castor::String const & type )
			: Type( type )
		{
		}

		BaseMaterial( GlslWriter * writer, castor::String const & name = castor::String() )
			: BaseMaterial( cuT( "BaseMaterial " ), writer, name )
		{
		}

		BaseMaterial( castor::String const & type
			, GlslWriter * writer
			, castor::String const & name = castor::String() )
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
			return m_writer->cast< Int >( Vec4( m_writer, m_name + cuT( ".m_reflRefr" ) ).y() );
		}

		Int m_hasReflection()
		{
			return m_writer->cast< Int >( Vec4( m_writer, m_name + cuT( ".m_reflRefr" ) ).z() );
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

		LegacyMaterial( GlslWriter * writer, castor::String const & name = castor::String() )
			: BaseMaterial( cuT( "LegacyMaterial " ), writer, name )
		{
		}

		LegacyMaterial & operator=( LegacyMaterial const & rhs )
		{
			if ( m_writer )
			{
				m_writer->writeAssign( *this, rhs );
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
			updateWriter( rhs );
			m_writer->writeAssign( *this, rhs );
			return *this;
		}

		static void declare( GlslWriter & writer )
		{
			Struct material = writer.getStruct( cuT( "LegacyMaterial" ) );
			material.declMember< Vec4 >( cuT( "m_diffAmb" ) );
			material.declMember< Vec4 >( cuT( "m_specShin" ) );
			material.declMember< Vec4 >( cuT( "m_common" ) );
			material.declMember< Vec4 >( cuT( "m_reflRefr" ) );
			material.end();
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

		MetallicRoughnessMaterial( GlslWriter * writer, castor::String const & name = castor::String() )
			: BaseMaterial( cuT( "MetallicRoughnessMaterial " ), writer, name )
		{
		}

		MetallicRoughnessMaterial & operator=( MetallicRoughnessMaterial const & rhs )
		{
			if ( m_writer )
			{
				m_writer->writeAssign( *this, rhs );
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
			updateWriter( rhs );
			m_writer->writeAssign( *this, rhs );
			return *this;
		}

		static void declare( GlslWriter & writer )
		{
			Struct material = writer.getStruct( cuT( "MetallicRoughnessMaterial" ) );
			material.declMember< Vec4 >( cuT( "m_albRough" ) );
			material.declMember< Vec4 >( cuT( "m_metDiv" ) );
			material.declMember< Vec4 >( cuT( "m_common" ) );
			material.declMember< Vec4 >( cuT( "m_reflRefr" ) );
			material.end();
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

		SpecularGlossinessMaterial( GlslWriter * writer, castor::String const & name = castor::String() )
			: BaseMaterial( cuT( "SpecularGlossinessMaterial " ), writer, name )
		{
		}

		SpecularGlossinessMaterial & operator=( SpecularGlossinessMaterial const & rhs )
		{
			if ( m_writer )
			{
				m_writer->writeAssign( *this, rhs );
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
			updateWriter( rhs );
			m_writer->writeAssign( *this, rhs );
			return *this;
		}

		static void declare( GlslWriter & writer )
		{
			Struct material = writer.getStruct( cuT( "SpecularGlossinessMaterial" ) );
			material.declMember< Vec4 >( cuT( "m_diffDiv" ) );
			material.declMember< Vec4 >( cuT( "m_specGloss" ) );
			material.declMember< Vec4 >( cuT( "m_common" ) );
			material.declMember< Vec4 >( cuT( "m_reflRefr" ) );
			material.end();
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

	Float Materials::getRefractionRatio( Int const & index )const
	{
		auto tmp = m_writer.getBuiltinArray< BaseMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_refractionRatio();
	}

	Int Materials::getRefraction( Int const & index )const
	{
		auto tmp = m_writer.getBuiltinArray< BaseMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_hasRefraction();
	}

	Int Materials::getReflection( Int const & index )const
	{
		auto tmp = m_writer.getBuiltinArray< BaseMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_hasReflection();
	}

	Float Materials::getOpacity( Int const & index )const
	{
		auto tmp = m_writer.getBuiltinArray< BaseMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_opacity();
	}

	Float Materials::getGamma( Int const & index )const
	{
		auto tmp = m_writer.getBuiltinArray< BaseMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_gamma();
	}

	Float Materials::getExposure( Int const & index )const
	{
		auto tmp = m_writer.getBuiltinArray< BaseMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_exposure();
	}

	Float Materials::getAlphaRef( Int const & index )const
	{
		auto tmp = m_writer.getBuiltinArray< BaseMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_alphaRef();
	}

	Float Materials::getEmissive( Int const & index )const
	{
		auto tmp = m_writer.getBuiltinArray< BaseMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_emissive();
	}

	//*********************************************************************************************

	LegacyMaterials::LegacyMaterials( GlslWriter & writer )
		: Materials{ writer }
	{
	}

	void LegacyMaterials::declare()
	{
		LegacyMaterial::declare( m_writer );
		Ssbo materials{ m_writer, PassBufferName, 0u };
		materials.declMemberArray< LegacyMaterial >( cuT( "c3d_materials" ) );
		materials.end();
	}

	Vec3 LegacyMaterials::getDiffuse( Int const & index )const
	{
		auto tmp = m_writer.getBuiltinArray< LegacyMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_diffuse();
	}

	Vec3 LegacyMaterials::getSpecular( Int const & index )const
	{
		auto tmp = m_writer.getBuiltinArray< LegacyMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_specular();
	}

	Float LegacyMaterials::getAmbient( Int const & index )const
	{
		auto tmp = m_writer.getBuiltinArray< LegacyMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_ambient();
	}

	Float LegacyMaterials::getShininess( Int const & index )const
	{
		auto tmp = m_writer.getBuiltinArray< LegacyMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_shininess();
	}

	//*********************************************************************************************

	PbrMRMaterials::PbrMRMaterials( GlslWriter & writer )
		: Materials{ writer }
	{
	}

	void PbrMRMaterials::declare()
	{
		MetallicRoughnessMaterial::declare( m_writer );
		Ssbo materials{ m_writer, PassBufferName, 0u };
		materials.declMemberArray< MetallicRoughnessMaterial >( cuT( "c3d_materials" ) );
		materials.end();
	}

	Vec3 PbrMRMaterials::getDiffuse( Int const & index )const
	{
		auto tmp = m_writer.getBuiltinArray< MetallicRoughnessMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_albedo();
	}

	Vec3 PbrMRMaterials::getAlbedo( Int const & index )const
	{
		auto tmp = m_writer.getBuiltinArray< MetallicRoughnessMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_albedo();
	}

	Float PbrMRMaterials::getRoughness( Int const & index )const
	{
		auto tmp = m_writer.getBuiltinArray< MetallicRoughnessMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_roughness();
	}

	Float PbrMRMaterials::getMetallic( Int const & index )const
	{
		auto tmp = m_writer.getBuiltinArray< MetallicRoughnessMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_metallic();
	}

	//*********************************************************************************************

	PbrSGMaterials::PbrSGMaterials( GlslWriter & writer )
		: Materials{ writer }
	{
	}

	void PbrSGMaterials::declare()
	{
		SpecularGlossinessMaterial::declare( m_writer );
		Ssbo materials{ m_writer, PassBufferName, 0u };
		materials.declMemberArray< SpecularGlossinessMaterial >( cuT( "c3d_materials" ) );
		materials.end();
	}

	Vec3 PbrSGMaterials::getDiffuse( Int const & index )const
	{
		auto tmp = m_writer.getBuiltinArray< SpecularGlossinessMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_diffuse();
	}

	Vec3 PbrSGMaterials::getSpecular( Int const & index )const
	{
		auto tmp = m_writer.getBuiltinArray< SpecularGlossinessMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_specular();
	}

	Float PbrSGMaterials::getGlossiness( Int const & index )const
	{
		auto tmp = m_writer.getBuiltinArray< SpecularGlossinessMaterial >( cuT( "c3d_materials" ) );
		return tmp[index].m_glossiness();
	}

	//*********************************************************************************************

#else

	//*********************************************************************************************

	Materials::Materials( GlslWriter & writer )
		: m_writer{ writer }
	{
	}

	Float Materials::getOpacity( Int const & index )const
	{
		auto tmp = m_writer.getBuiltin< Vec4 >( cuT( "c3d_matCommon" ), MaxMaterialsCount );
		return tmp[index].x();
	}

	Float Materials::getEmissive( Int const & index )const
	{
		auto tmp = m_writer.getBuiltin< Vec4 >( cuT( "c3d_matCommon" ), MaxMaterialsCount );
		return tmp[index].y();
	}

	Float Materials::getAlphaRef( Int const & index )const
	{
		auto tmp = m_writer.getBuiltin< Vec4 >( cuT( "c3d_matCommon" ), MaxMaterialsCount );
		return tmp[index].z();
	}

	Float Materials::getGamma( Int const & index )const
	{
		auto tmp = m_writer.getBuiltin< Vec4 >( cuT( "c3d_matCommon" ), MaxMaterialsCount );
		return tmp[index].w();
	}

	Float Materials::getRefractionRatio( Int const & index )const
	{
		auto tmp = m_writer.getBuiltin< Vec4 >( cuT( "c3d_matReflRer" ), MaxMaterialsCount );
		return tmp[index].x();
	}

	Int Materials::getRefraction( Int const & index )const
	{
		auto tmp = m_writer.getBuiltin< Vec4 >( cuT( "c3d_matReflRer" ), MaxMaterialsCount );
		return m_writer.cast< Int >( tmp[index].y() );
	}

	Int Materials::getReflection( Int const & index )const
	{
		auto tmp = m_writer.getBuiltin< Vec4 >( cuT( "c3d_matReflRer" ), MaxMaterialsCount );
		return m_writer.cast< Int >( tmp[index].z() );
	}

	Float Materials::getExposure( Int const & index )const
	{
		auto tmp = m_writer.getBuiltin< Vec4 >( cuT( "c3d_matExposure" ), MaxMaterialsCount );
		return tmp[index].w();
	}

	//*********************************************************************************************

	LegacyMaterials::LegacyMaterials( GlslWriter & writer )
		: Materials{ writer }
	{
	}

	void LegacyMaterials::declare()
	{
		Ssbo materials{ m_writer, PassBufferName, 0u };
		materials.declMember< Vec4 >( cuT( "c3d_matDiffAmb" ), MaxMaterialsCount );
		materials.declMember< Vec4 >( cuT( "c3d_matSpecShin" ), MaxMaterialsCount );
		materials.declMember< Vec4 >( cuT( "c3d_matCommon" ), MaxMaterialsCount );
		materials.declMember< Vec4 >( cuT( "c3d_matReflRer" ), MaxMaterialsCount );
		materials.end();
	}

	Vec3 LegacyMaterials::getDiffuse( Int const & index )const
	{
		auto tmp = m_writer.getBuiltin< Vec4 >( cuT( "c3d_matDiffAmb" ), MaxMaterialsCount );
		return tmp[index].xyz();
	}

	Vec3 LegacyMaterials::getSpecular( Int const & index )const
	{
		auto tmp = m_writer.getBuiltin< Vec4 >( cuT( "c3d_matSpecShin" ), MaxMaterialsCount );
		return tmp[index].xyz();
	}

	Float LegacyMaterials::getAmbient( Int const & index )const
	{
		auto tmp = m_writer.getBuiltin< Vec4 >( cuT( "c3d_matDiffAmb" ), MaxMaterialsCount );
		return tmp[index].w();
	}

	Float LegacyMaterials::getShininess( Int const & index )const
	{
		auto tmp = m_writer.getBuiltin< Vec4 >( cuT( "c3d_matSpecShin" ), MaxMaterialsCount );
		return tmp[index].w();
	}

	//*********************************************************************************************

	PbrMRMaterials::PbrMRMaterials( GlslWriter & writer )
		: Materials{ writer }
	{
	}

	void PbrMRMaterials::declare()
	{
		Ssbo materials{ m_writer, PassBufferName, 0u };
		materials.declMember< Vec4 >( cuT( "c3d_matAlbRough" ), MaxMaterialsCount );
		materials.declMember< Vec4 >( cuT( "c3d_matMetDiv" ), MaxMaterialsCount );
		materials.declMember< Vec4 >( cuT( "c3d_matCommon" ), MaxMaterialsCount );
		materials.declMember< Vec4 >( cuT( "c3d_matReflRer" ), MaxMaterialsCount );
		materials.end();
	}

	Vec3 PbrMRMaterials::getDiffuse( Int const & index )const
	{
		auto tmp = m_writer.getBuiltin< Vec4 >( cuT( "c3d_matAlbRough" ), MaxMaterialsCount );
		return tmp[index].xyz();
	}

	Vec3 PbrMRMaterials::getAlbedo( Int const & index )const
	{
		auto tmp = m_writer.getBuiltin< Vec4 >( cuT( "c3d_matAlbRough" ), MaxMaterialsCount );
		return tmp[index].xyz();
	}

	Float PbrMRMaterials::getRoughness( Int const & index )const
	{
		auto tmp = m_writer.getBuiltin< Vec4 >( cuT( "c3d_matAlbRough" ), MaxMaterialsCount );
		return tmp[index].w();
	}

	Float PbrMRMaterials::getMetallic( Int const & index )const
	{
		auto tmp = m_writer.getBuiltin< Vec4 >( cuT( "c3d_matMetDiv" ), MaxMaterialsCount );
		return tmp[index].x();
	}

	//*********************************************************************************************

	PbrSGMaterials::PbrSGMaterials( GlslWriter & writer )
		: Materials{ writer }
	{
	}

	void PbrSGMaterials::declare()
	{
		Ssbo materials{ m_writer, PassBufferName, 0u };
		materials.declMember< Vec4 >( cuT( "c3d_matDiffDiv" ), MaxMaterialsCount );
		materials.declMember< Vec4 >( cuT( "c3d_matSpecGloss" ), MaxMaterialsCount );
		materials.declMember< Vec4 >( cuT( "c3d_matCommon" ), MaxMaterialsCount );
		materials.declMember< Vec4 >( cuT( "c3d_matReflRer" ), MaxMaterialsCount );
		materials.end();
	}

	Vec3 PbrSGMaterials::getDiffuse( Int const & index )const
	{
		auto tmp = m_writer.getBuiltin< Vec4 >( cuT( "c3d_matDiffDiv" ), MaxMaterialsCount );
		return tmp[index].xyz();
	}

	Vec3 PbrSGMaterials::getSpecular( Int const & index )const
	{
		auto tmp = m_writer.getBuiltin< Vec4 >( cuT( "c3d_matSpecGloss" ), MaxMaterialsCount );
		return tmp[index].xyz();
	}

	Float PbrSGMaterials::getGlossiness( Int const & index )const
	{
		auto tmp = m_writer.getBuiltin< Vec4 >( cuT( "c3d_matSpecGloss" ), MaxMaterialsCount );
		return tmp[index].w();
	}

	//*********************************************************************************************

#endif
}
