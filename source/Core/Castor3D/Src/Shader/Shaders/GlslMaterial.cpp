#include "GlslMaterial.hpp"

#include "GlslSource.hpp"

using namespace castor;
using namespace glsl;

namespace castor3d
{
	namespace shader
	{
		//*****************************************************************************************

		BaseMaterial::BaseMaterial( castor::String const & type )
			: Type( type )
		{
		}

		BaseMaterial::BaseMaterial( GlslWriter * writer, castor::String const & name )
			: BaseMaterial( cuT( "BaseMaterial " ), writer, name )
		{
		}

		BaseMaterial::BaseMaterial( castor::String const & type
			, GlslWriter * writer
			, castor::String const & name )
			: Type( type, writer, name )
		{
		}

		Float BaseMaterial::m_opacity()
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_common" ) ).x();
		}

		Float BaseMaterial::m_emissive()
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_common" ) ).y();
		}

		Float BaseMaterial::m_alphaRef()
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_common" ) ).z();
		}

		Float BaseMaterial::m_gamma()
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_common" ) ).w();
		}

		Float BaseMaterial::m_refractionRatio()
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_reflRefr" ) ).x();
		}

		Int BaseMaterial::m_hasRefraction()
		{
			return m_writer->cast< Int >( Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_reflRefr" ) ).y() );
		}

		Int BaseMaterial::m_hasReflection()
		{
			return m_writer->cast< Int >( Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_reflRefr" ) ).z() );
		}

		Float BaseMaterial::m_exposure()
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_reflRefr" ) ).w();
		}

		Int BaseMaterial::m_subsurfaceScatteringEnabled()
		{
			return m_writer->cast< Int >( Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_sssInfo" ) ).r() );
		}

		Int BaseMaterial::m_distanceBasedTransmission()
		{
			return m_writer->cast< Int >( Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_transmittance" ) ).a() );
		}

		Vec3 BaseMaterial::m_backLitCoefficient()
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_transmittance" ) ).rgb();
		}
		
		Vec4 BaseMaterial::m_common()
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_common" ) );
		}

		Vec4 BaseMaterial::m_reflRefr()
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_reflRefr" ) );
		}

		Vec4 BaseMaterial::m_sssInfo()
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_sssInfo" ) );
		}

		Vec4 BaseMaterial::m_transmittance()
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_transmittance" ) );
		}

		//*****************************************************************************************

		LegacyMaterial::LegacyMaterial()
			: BaseMaterial( cuT( "LegacyMaterial " ) )
		{
		}

		LegacyMaterial::LegacyMaterial( GlslWriter * writer, castor::String const & name )
			: BaseMaterial( cuT( "LegacyMaterial " ), writer, name )
		{
		}

		LegacyMaterial & LegacyMaterial::operator=( LegacyMaterial const & rhs )
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

		void LegacyMaterial::declare( GlslWriter & writer )
		{
			Struct material = writer.getStruct( cuT( "LegacyMaterial" ) );
			material.declMember< Vec4 >( cuT( "m_diffAmb" ) );
			material.declMember< Vec4 >( cuT( "m_specShin" ) );
			material.declMember< Vec4 >( cuT( "m_common" ) );
			material.declMember< Vec4 >( cuT( "m_reflRefr" ) );
			material.declMember< Vec4 >( cuT( "m_sssInfo" ) );
			material.declMember< Vec4 >( cuT( "m_transmittance" ) );
			material.end();
		}

		Vec3 LegacyMaterial::m_diffuse()
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_diffAmb" ) ).rgb();
		}

		Float LegacyMaterial::m_ambient()
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_diffAmb" ) ).a();
		}

		Vec3 LegacyMaterial::m_specular()
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_specShin" ) ).rgb();
		}

		Float LegacyMaterial::m_shininess()
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_specShin" ) ).a();
		}

		Vec4 LegacyMaterial::m_diffAmb()
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_diffAmb" ) );
		}

		Vec4 LegacyMaterial::m_specShin()
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_specShin" ) );
		}

		//*****************************************************************************************

		MetallicRoughnessMaterial::MetallicRoughnessMaterial()
			: BaseMaterial( cuT( "MetallicRoughnessMaterial " ) )
		{
		}

		MetallicRoughnessMaterial::MetallicRoughnessMaterial( GlslWriter * writer, castor::String const & name )
			: BaseMaterial( cuT( "MetallicRoughnessMaterial " ), writer, name )
		{
		}

		MetallicRoughnessMaterial & MetallicRoughnessMaterial::operator=( MetallicRoughnessMaterial const & rhs )
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

		void MetallicRoughnessMaterial::declare( GlslWriter & writer )
		{
			Struct material = writer.getStruct( cuT( "MetallicRoughnessMaterial" ) );
			material.declMember< Vec4 >( cuT( "m_albRough" ) );
			material.declMember< Vec4 >( cuT( "m_metDiv" ) );
			material.declMember< Vec4 >( cuT( "m_common" ) );
			material.declMember< Vec4 >( cuT( "m_reflRefr" ) );
			material.declMember< Vec4 >( cuT( "m_sssInfo" ) );
			material.declMember< Vec4 >( cuT( "m_transmittance" ) );
			material.end();
		}
		
		Vec3 MetallicRoughnessMaterial::m_diffuse()
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_albRough" ) ).rgb();
		}

		Vec3 MetallicRoughnessMaterial::m_albedo()
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_albRough" ) ).rgb();
		}

		Float MetallicRoughnessMaterial::m_roughness()
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_albRough" ) ).a();
		}

		Float MetallicRoughnessMaterial::m_metallic()
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_metDiv" ) ).r();
		}

		Vec4 MetallicRoughnessMaterial::m_albRough()
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_albRough" ) );
		}

		Vec4 MetallicRoughnessMaterial::m_metDiv()
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_metDiv" ) );
		}

		//*****************************************************************************************

		SpecularGlossinessMaterial::SpecularGlossinessMaterial()
			: BaseMaterial( cuT( "SpecularGlossinessMaterial " ) )
		{
		}

		SpecularGlossinessMaterial::SpecularGlossinessMaterial( GlslWriter * writer, castor::String const & name )
			: BaseMaterial( cuT( "SpecularGlossinessMaterial " ), writer, name )
		{
		}

		SpecularGlossinessMaterial & SpecularGlossinessMaterial::operator=( SpecularGlossinessMaterial const & rhs )
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

		void SpecularGlossinessMaterial::declare( GlslWriter & writer )
		{
			Struct material = writer.getStruct( cuT( "SpecularGlossinessMaterial" ) );
			material.declMember< Vec4 >( cuT( "m_diffDiv" ) );
			material.declMember< Vec4 >( cuT( "m_specGloss" ) );
			material.declMember< Vec4 >( cuT( "m_common" ) );
			material.declMember< Vec4 >( cuT( "m_reflRefr" ) );
			material.declMember< Vec4 >( cuT( "m_sssInfo" ) );
			material.declMember< Vec4 >( cuT( "m_transmittance" ) );
			material.end();
		}

		Vec3 SpecularGlossinessMaterial::m_diffuse()
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_diffDiv" ) ).rgb();
		}

		Vec3 SpecularGlossinessMaterial::m_specular()
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_specGloss" ) ).rgb();
		}

		Float SpecularGlossinessMaterial::m_glossiness()
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_specGloss" ) ).a();
		}

		Vec4 SpecularGlossinessMaterial::m_diffDiv()
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_diffDiv" ) );
		}

		Vec4 SpecularGlossinessMaterial::m_specGloss()
		{
			return Vec4( m_writer, ( m_name.empty() ? m_value.str() : m_name ) + cuT( ".m_specGloss" ) );
		}

#if !GLSL_MATERIALS_STRUCT_OF_ARRAY

		//*********************************************************************************************

		Materials::Materials( GlslWriter & writer )
			: m_writer{ writer }
		{
		}

		//*********************************************************************************************

		LegacyMaterials::LegacyMaterials( GlslWriter & writer )
			: Materials{ writer }
		{
		}

		void LegacyMaterials::declare()
		{
			LegacyMaterial::declare( m_writer );

			if ( m_writer.hasShaderStorageBuffers() )
			{
				Ssbo materials{ m_writer, PassBufferName, 0u };
				materials.declMemberArray< LegacyMaterial >( cuT( "c3d_materials" ) );
				materials.end();
			}
			else
			{
				auto c3d_materials = m_writer.declUniform< SamplerBuffer >( cuT( "c3d_materials" ) );
				m_getMaterial = m_writer.implementFunction< LegacyMaterial >( cuT( "getMaterial" )
					, [this, &c3d_materials]( Int const & index )
					{
						auto result = m_writer.declLocale< LegacyMaterial >( cuT( "result" ) );
						auto offset = m_writer.declLocale( cuT( "offset" ), index * Int( MaxMaterialComponentsCount ) );
						result.m_diffAmb() = texelFetch( c3d_materials, offset++ );
						result.m_specShin () = texelFetch( c3d_materials, offset++ );
						result.m_common () = texelFetch( c3d_materials, offset++ );
						result.m_reflRefr () = texelFetch( c3d_materials, offset++ );
						result.m_sssInfo () = texelFetch( c3d_materials, offset++ );
						result.m_transmittance () = texelFetch( c3d_materials, offset++ );
						m_writer.returnStmt( result );
					}
					, InInt{ &m_writer, cuT( "index" ) } );
			}
		}

		LegacyMaterial LegacyMaterials::getMaterial( Int const & index )const
		{
			if ( m_writer.hasShaderStorageBuffers() )
			{
				auto tmp = m_writer.getBuiltinArray< LegacyMaterial >( cuT( "c3d_materials" ) );
				return tmp[index];
			}
			else
			{
				return m_getMaterial( index );
			}
		}

		BaseMaterialUPtr LegacyMaterials::getBaseMaterial( Int const & index )const
		{
			if ( m_writer.hasShaderStorageBuffers() )
			{
				auto tmp = m_writer.getBuiltinArray< LegacyMaterial >( cuT( "c3d_materials" ) );
				return std::make_unique< LegacyMaterial >( tmp[index] );
			}
			else
			{
				return std::make_unique< LegacyMaterial > (m_getMaterial( index ) );
			}
		}

		//*********************************************************************************************

		PbrMRMaterials::PbrMRMaterials( GlslWriter & writer )
			: Materials{ writer }
		{
		}

		void PbrMRMaterials::declare()
		{
			MetallicRoughnessMaterial::declare( m_writer );

			if ( m_writer.hasShaderStorageBuffers() )
			{
				Ssbo materials{ m_writer, PassBufferName, 0u };
				auto c3d_materials = materials.declMemberArray< MetallicRoughnessMaterial >( cuT( "c3d_materials" ) );
				materials.end();
			}
			else
			{
				auto c3d_materials = m_writer.declUniform< SamplerBuffer >( cuT( "c3d_materials" ) );
				m_getMaterial = m_writer.implementFunction< MetallicRoughnessMaterial >( cuT( "getMaterial" )
					, [this, &c3d_materials]( Int const & index )
					{
						auto result = m_writer.declLocale< MetallicRoughnessMaterial >( cuT( "result" ) );
						auto offset = m_writer.declLocale( cuT( "offset" ), index * Int( MaxMaterialComponentsCount ) );
						result.m_albRough() = texelFetch( c3d_materials, offset++ );
						result.m_metDiv() = texelFetch( c3d_materials, offset++ );
						result.m_common() = texelFetch( c3d_materials, offset++ );
						result.m_reflRefr() = texelFetch( c3d_materials, offset++ );
						result.m_sssInfo() = texelFetch( c3d_materials, offset++ );
						result.m_transmittance() = texelFetch( c3d_materials, offset++ );
						m_writer.returnStmt( result );
					}
					, InInt{ &m_writer, cuT( "index" ) } );
			}
		}
		
		MetallicRoughnessMaterial PbrMRMaterials::getMaterial( Int const & index )const
		{
			if ( m_writer.hasShaderStorageBuffers() )
			{
				auto tmp = m_writer.getBuiltinArray< MetallicRoughnessMaterial >( cuT( "c3d_materials" ) );
				return tmp[index];
			}
			else
			{
				return m_getMaterial( index );
			}
		}

		BaseMaterialUPtr PbrMRMaterials::getBaseMaterial( Int const & index )const
		{
			auto material = m_writer.declLocale( cuT( "material" )
				, getMaterial( index ) );
			return std::make_unique< MetallicRoughnessMaterial >( material );
		}

		//*********************************************************************************************

		PbrSGMaterials::PbrSGMaterials( GlslWriter & writer )
			: Materials{ writer }
		{
		}

		void PbrSGMaterials::declare()
		{
			SpecularGlossinessMaterial::declare( m_writer );

			if ( m_writer.hasShaderStorageBuffers() )
			{
				Ssbo materials{ m_writer, PassBufferName, 0u };
				materials.declMemberArray< SpecularGlossinessMaterial >( cuT( "c3d_materials" ) );
				materials.end();
			}
			else
			{
				auto c3d_materials = m_writer.declUniform< SamplerBuffer >( cuT( "c3d_materials" ) );
				m_getMaterial = m_writer.implementFunction< SpecularGlossinessMaterial >( cuT( "getMaterial" )
					, [this, &c3d_materials]( Int const & index )
					{
						auto result = m_writer.declLocale< SpecularGlossinessMaterial >( cuT( "result" ) );
						auto offset = m_writer.declLocale( cuT( "offset" ), index * Int( MaxMaterialComponentsCount ) );
						result.m_diffDiv() = texelFetch( c3d_materials, offset++ );
						result.m_specGloss() = texelFetch( c3d_materials, offset++ );
						result.m_common() = texelFetch( c3d_materials, offset++ );
						result.m_reflRefr() = texelFetch( c3d_materials, offset++ );
						result.m_sssInfo() = texelFetch( c3d_materials, offset++ );
						result.m_transmittance() = texelFetch( c3d_materials, offset++ );
						m_writer.returnStmt( result );
					}
					, InInt{ &m_writer, cuT( "index" ) } );
			}
		}

		SpecularGlossinessMaterial PbrSGMaterials::getMaterial( Int const & index )const
		{
			if ( m_writer.hasShaderStorageBuffers() )
			{
				auto tmp = m_writer.getBuiltinArray< SpecularGlossinessMaterial >( cuT( "c3d_materials" ) );
				return tmp[index];
			}
			else
			{
				return m_getMaterial( index );
			}
		}

		BaseMaterialUPtr PbrSGMaterials::getBaseMaterial( Int const & index )const
		{
			if ( m_writer.hasShaderStorageBuffers() )
			{
				auto tmp = m_writer.getBuiltinArray< SpecularGlossinessMaterial >( cuT( "c3d_materials" ) );
				return std::make_unique< SpecularGlossinessMaterial >( tmp[index] );
			}
			else
			{
				return std::make_unique< SpecularGlossinessMaterial >( m_getMaterial( index ) );
			}
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

		shader::LegacyMaterials::shader::LegacyMaterials( GlslWriter & writer )
			: Materials{ writer }
		{
		}

		void shader::LegacyMaterials::declare()
		{
			Ssbo materials{ m_writer, PassBufferName, 0u };
			materials.declMember< Vec4 >( cuT( "c3d_matDiffAmb" ), MaxMaterialsCount );
			materials.declMember< Vec4 >( cuT( "c3d_matSpecShin" ), MaxMaterialsCount );
			materials.declMember< Vec4 >( cuT( "c3d_matCommon" ), MaxMaterialsCount );
			materials.declMember< Vec4 >( cuT( "c3d_matReflRer" ), MaxMaterialsCount );
			materials.declMember< Vec4 >( cuT( "m_sssInfo" ), MaxMaterialsCount );
			materials.declMember< Vec4 >( cuT( "m_transmittance" ), MaxMaterialsCount );
			materials.end();
		}

		Vec3 shader::LegacyMaterials::getDiffuse( Int const & index )const
		{
			auto tmp = m_writer.getBuiltin< Vec4 >( cuT( "c3d_matDiffAmb" ), MaxMaterialsCount );
			return tmp[index].xyz();
		}

		Vec3 shader::LegacyMaterials::getSpecular( Int const & index )const
		{
			auto tmp = m_writer.getBuiltin< Vec4 >( cuT( "c3d_matSpecShin" ), MaxMaterialsCount );
			return tmp[index].xyz();
		}

		Float shader::LegacyMaterials::getAmbient( Int const & index )const
		{
			auto tmp = m_writer.getBuiltin< Vec4 >( cuT( "c3d_matDiffAmb" ), MaxMaterialsCount );
			return tmp[index].w();
		}

		Float shader::LegacyMaterials::getShininess( Int const & index )const
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
			materials.declMember< Vec4 >( cuT( "m_sssInfo" ), MaxMaterialsCount );
			materials.declMember< Vec4 >( cuT( "m_transmittance" ), MaxMaterialsCount );
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
			materials.declMember< Vec4 >( cuT( "m_sssInfo" ), MaxMaterialsCount );
			materials.declMember< Vec4 >( cuT( "m_transmittance" ), MaxMaterialsCount );
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
}
