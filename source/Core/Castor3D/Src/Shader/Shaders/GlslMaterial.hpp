/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_Materials_H___
#define ___GLSL_Materials_H___

#include "Castor3DPrerequisites.hpp"

#include <GlslMat.hpp>

namespace castor3d
{
	namespace shader
	{
		static uint32_t constexpr MaxMaterialsCount = 2000u;
		static int constexpr MaxMaterialComponentsCount = 6;
		castor::String const PassBufferName = cuT( "Materials" );

		class LegacyMaterials;
		class PbrMRMaterials;
		class PbrSGMaterials;
		
		struct BaseMaterial
			: public glsl::Type
		{
			friend class Materials;

		protected:
			C3D_API explicit BaseMaterial( castor::String const & type );
			C3D_API BaseMaterial( glsl::GlslWriter * writer
				, castor::String const & name = castor::String() );
			C3D_API BaseMaterial( castor::String const & type
				, glsl::GlslWriter * writer
				, castor::String const & name = castor::String() );

		public:
			C3D_API virtual glsl::Vec3 m_diffuse()const = 0;
			C3D_API glsl::Float m_opacity()const;
			C3D_API glsl::Float m_emissive()const;
			C3D_API glsl::Float m_alphaRef()const;
			C3D_API glsl::Float m_gamma()const;
			C3D_API glsl::Float m_refractionRatio()const;
			C3D_API glsl::Int m_hasRefraction()const;
			C3D_API glsl::Int m_hasReflection()const;
			C3D_API glsl::Float m_exposure()const;
			C3D_API glsl::Int m_subsurfaceScatteringEnabled()const;
			C3D_API glsl::Float m_gaussianWidth()const;
			C3D_API glsl::Float m_subsurfaceScatteringStrength()const;
			C3D_API glsl::Int m_transmittanceProfileSize()const;
			C3D_API glsl::Array< glsl::Vec4 > m_transmittanceProfile()const;

		protected:
			C3D_API glsl::Vec4 m_common()const;
			C3D_API glsl::Vec4 m_reflRefr()const;
			C3D_API glsl::Vec4 m_sssInfo()const;
		};

		DECLARE_SMART_PTR( BaseMaterial );

		struct LegacyMaterial
			: public BaseMaterial
		{
			friend class LegacyMaterials;

			C3D_API LegacyMaterial();
			C3D_API LegacyMaterial( glsl::GlslWriter * writer
				, castor::String const & name = castor::String() );
			C3D_API LegacyMaterial & operator=( LegacyMaterial const & rhs );
			C3D_API static void declare( glsl::GlslWriter & writer );
			C3D_API glsl::Vec3 m_diffuse()const override;
			C3D_API glsl::Float m_ambient()const;
			C3D_API glsl::Vec3 m_specular()const;
			C3D_API glsl::Float m_shininess()const;

			template< typename T >
			LegacyMaterial & operator=( T const & rhs )
			{
				updateWriter( rhs );
				m_writer->writeAssign( *this, rhs );
				return *this;
			}

		protected:
			C3D_API glsl::Vec4 m_diffAmb()const;
			C3D_API glsl::Vec4 m_specShin()const;
		};

		struct MetallicRoughnessMaterial
			: public BaseMaterial
		{
			friend class PbrMRMaterials;

			C3D_API MetallicRoughnessMaterial();
			C3D_API MetallicRoughnessMaterial( glsl::GlslWriter * writer
				, castor::String const & name = castor::String() );
			C3D_API MetallicRoughnessMaterial & operator=( MetallicRoughnessMaterial const & rhs );
			C3D_API static void declare( glsl::GlslWriter & writer );
			C3D_API glsl::Vec3 m_diffuse()const override;
			C3D_API glsl::Vec3 m_albedo()const;
			C3D_API glsl::Float m_roughness()const;
			C3D_API glsl::Float m_metallic()const;

			template< typename T >
			MetallicRoughnessMaterial & operator=( T const & rhs )
			{
				updateWriter( rhs );
				m_writer->writeAssign( *this, rhs );
				return *this;
			}

		protected:
			C3D_API glsl::Vec4 m_albRough()const;
			C3D_API glsl::Vec4 m_metDiv()const;
		};

		struct SpecularGlossinessMaterial
			: public BaseMaterial
		{
			friend class PbrSGMaterials;

			C3D_API SpecularGlossinessMaterial();
			C3D_API SpecularGlossinessMaterial( glsl::GlslWriter * writer
				, castor::String const & name = castor::String() );
			C3D_API SpecularGlossinessMaterial & operator=( SpecularGlossinessMaterial const & rhs );
			C3D_API static void declare( glsl::GlslWriter & writer );
			C3D_API glsl::Vec3 m_diffuse()const override;
			C3D_API glsl::Vec3 m_specular()const;
			C3D_API glsl::Float m_glossiness()const;

			template< typename T >
			SpecularGlossinessMaterial & operator=( T const & rhs )
			{
				updateWriter( rhs );
				m_writer->writeAssign( *this, rhs );
				return *this;
			}

		protected:
			C3D_API glsl::Vec4 m_diffDiv()const;
			C3D_API glsl::Vec4 m_specGloss()const;
		};

		class Materials
		{
		protected:
			C3D_API explicit Materials( glsl::GlslWriter & writer );

		public:
			C3D_API virtual void declare() = 0;
			C3D_API virtual BaseMaterialUPtr getBaseMaterial( glsl::Int const & index )const = 0;

		protected:
			glsl::GlslWriter & m_writer;
		};

		class LegacyMaterials
			: public Materials
		{
		public:
			C3D_API explicit LegacyMaterials( glsl::GlslWriter & writer );
			C3D_API void declare()override;
			C3D_API LegacyMaterial getMaterial( glsl::Int const & index )const;
			C3D_API BaseMaterialUPtr getBaseMaterial( glsl::Int const & index )const override;

		private:
			glsl::Function< LegacyMaterial, glsl::InInt > m_getMaterial;
		};

		class PbrMRMaterials
			: public Materials
		{
		public:
			C3D_API explicit PbrMRMaterials( glsl::GlslWriter & writer );
			C3D_API void declare()override;
			C3D_API MetallicRoughnessMaterial getMaterial( glsl::Int const & index )const;
			C3D_API BaseMaterialUPtr getBaseMaterial( glsl::Int const & index )const override;

		private:
			glsl::Function< MetallicRoughnessMaterial, glsl::InInt > m_getMaterial;
		};

		class PbrSGMaterials
			: public Materials
		{
		public:
			C3D_API explicit PbrSGMaterials( glsl::GlslWriter & writer );
			C3D_API void declare()override;
			C3D_API SpecularGlossinessMaterial getMaterial( glsl::Int const & index )const;
			C3D_API BaseMaterialUPtr getBaseMaterial( glsl::Int const & index )const override;

		private:
			glsl::Function< SpecularGlossinessMaterial, glsl::InInt > m_getMaterial;
		};
	}
}

#endif
