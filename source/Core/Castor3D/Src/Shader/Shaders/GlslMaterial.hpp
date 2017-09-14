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
			C3D_API virtual glsl::Vec3 m_diffuse() = 0;
			C3D_API glsl::Float m_opacity();
			C3D_API glsl::Float m_emissive();
			C3D_API glsl::Float m_alphaRef();
			C3D_API glsl::Float m_gamma();
			C3D_API glsl::Float m_refractionRatio();
			C3D_API glsl::Int m_hasRefraction();
			C3D_API glsl::Int m_hasReflection();
			C3D_API glsl::Float m_exposure();
			C3D_API glsl::Int m_subsurfaceScatteringEnabled();
			C3D_API glsl::Int m_distanceBasedTransmission();
			C3D_API glsl::Int m_backLitCoefficient();

		protected:
			C3D_API glsl::Vec4 m_common();
			C3D_API glsl::Vec4 m_reflRefr();
			C3D_API glsl::Vec4 m_sssInfo();
			C3D_API glsl::Vec4 m_transmittance();
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
			C3D_API glsl::Vec3 m_diffuse()override;
			C3D_API glsl::Float m_ambient();
			C3D_API glsl::Vec3 m_specular();
			C3D_API glsl::Float m_shininess();

			template< typename T >
			LegacyMaterial & operator=( T const & rhs )
			{
				updateWriter( rhs );
				m_writer->writeAssign( *this, rhs );
				return *this;
			}

		protected:
			C3D_API glsl::Vec4 m_diffAmb();
			C3D_API glsl::Vec4 m_specShin();
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
			C3D_API glsl::Vec3 m_diffuse()override;
			C3D_API glsl::Vec3 m_albedo();
			C3D_API glsl::Float m_roughness();
			C3D_API glsl::Float m_metallic();

			template< typename T >
			MetallicRoughnessMaterial & operator=( T const & rhs )
			{
				updateWriter( rhs );
				m_writer->writeAssign( *this, rhs );
				return *this;
			}

		protected:
			C3D_API glsl::Vec4 m_albRough();
			C3D_API glsl::Vec4 m_metDiv();
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
			C3D_API glsl::Vec3 m_diffuse()override;
			C3D_API glsl::Vec3 m_specular();
			C3D_API glsl::Float m_glossiness();

			template< typename T >
			SpecularGlossinessMaterial & operator=( T const & rhs )
			{
				updateWriter( rhs );
				m_writer->writeAssign( *this, rhs );
				return *this;
			}

		protected:
			C3D_API glsl::Vec4 m_diffDiv();
			C3D_API glsl::Vec4 m_specGloss();
		};

		class Materials
		{
		protected:
			C3D_API Materials( glsl::GlslWriter & writer );

		public:
			C3D_API virtual void declare() = 0;
			C3D_API virtual BaseMaterialUPtr getBaseMaterial( glsl::Int const & index )const = 0;
			//C3D_API virtual glsl::Vec3 getDiffuse( glsl::Int const & index )const = 0;
			//C3D_API virtual glsl::Float getAlphaRef( glsl::Int const & index )const = 0;
			//C3D_API virtual glsl::Float getOpacity( glsl::Int const & index )const = 0;

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
			//C3D_API glsl::Vec3 getDiffuse( glsl::Int const & index )const override;
			//C3D_API glsl::Float getAlphaRef( glsl::Int const & index )const override;
			//C3D_API glsl::Float getOpacity( glsl::Int const & index )const override;

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
			//C3D_API glsl::Vec3 getDiffuse( glsl::Int const & index )const override;
			//C3D_API glsl::Float getAlphaRef( glsl::Int const & index )const override;
			//C3D_API glsl::Float getOpacity( glsl::Int const & index )const override;

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
			//C3D_API glsl::Vec3 getDiffuse( glsl::Int const & index )const override;
			//C3D_API glsl::Float getAlphaRef( glsl::Int const & index )const override;
			//C3D_API glsl::Float getOpacity( glsl::Int const & index )const override;

		private:
			glsl::Function< SpecularGlossinessMaterial, glsl::InInt > m_getMaterial;
		};
	}
}

#endif
