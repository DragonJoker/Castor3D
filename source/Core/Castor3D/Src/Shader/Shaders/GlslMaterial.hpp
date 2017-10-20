/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
			C3D_API BaseMaterial( castor::String const & type );
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
			C3D_API Materials( glsl::GlslWriter & writer );

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
			C3D_API LegacyMaterials( glsl::GlslWriter & writer );
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
			C3D_API PbrMRMaterials( glsl::GlslWriter & writer );
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
			C3D_API PbrSGMaterials( glsl::GlslWriter & writer );
			C3D_API void declare()override;
			C3D_API SpecularGlossinessMaterial getMaterial( glsl::Int const & index )const;
			C3D_API BaseMaterialUPtr getBaseMaterial( glsl::Int const & index )const override;

		private:
			glsl::Function< SpecularGlossinessMaterial, glsl::InInt > m_getMaterial;
		};
	}
}

#endif
