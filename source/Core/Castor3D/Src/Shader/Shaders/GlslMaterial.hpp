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
		castor::String const PassBufferName = cuT( "Materials" );

		class Materials
		{
		protected:
			C3D_API Materials( glsl::GlslWriter & writer );

		public:
			C3D_API virtual void declare() = 0;
			C3D_API virtual glsl::Vec3 getDiffuse( glsl::Int const & index )const = 0;
			C3D_API glsl::Float getRefractionRatio( glsl::Int const & index )const;
			C3D_API glsl::Float getOpacity( glsl::Int const & index )const;
			C3D_API glsl::Float getEmissive( glsl::Int const & index )const;
			C3D_API glsl::Int getRefraction( glsl::Int const & index )const;
			C3D_API glsl::Int getReflection( glsl::Int const & index )const;
			C3D_API glsl::Float getGamma( glsl::Int const & index )const;
			C3D_API glsl::Float getExposure( glsl::Int const & index )const;
			C3D_API glsl::Float getAlphaRef( glsl::Int const & index )const;

		protected:
			glsl::GlslWriter & m_writer;
		};

		class LegacyMaterials
			: public Materials
		{
		public:
			C3D_API shader::LegacyMaterials( glsl::GlslWriter & writer );
			C3D_API void declare()override;
			C3D_API glsl::Vec3 getDiffuse( glsl::Int const & index )const override;
			C3D_API glsl::Vec3 getSpecular( glsl::Int const & index )const;
			C3D_API glsl::Float getAmbient( glsl::Int const & index )const;
			C3D_API glsl::Float getShininess( glsl::Int const & index )const;
		};

		class PbrMRMaterials
			: public Materials
		{
		public:
			C3D_API PbrMRMaterials( glsl::GlslWriter & writer );
			C3D_API void declare()override;
			C3D_API glsl::Vec3 getDiffuse( glsl::Int const & index )const override;
			C3D_API glsl::Vec3 getAlbedo( glsl::Int const & index )const;
			C3D_API glsl::Float getRoughness( glsl::Int const & index )const;
			C3D_API glsl::Float getMetallic( glsl::Int const & index )const;
		};

		class PbrSGMaterials
			: public Materials
		{
		public:
			C3D_API PbrSGMaterials( glsl::GlslWriter & writer );
			C3D_API void declare()override;
			C3D_API glsl::Vec3 getDiffuse( glsl::Int const & index )const override;
			C3D_API glsl::Vec3 getSpecular( glsl::Int const & index )const;
			C3D_API glsl::Float getGlossiness( glsl::Int const & index )const;
		};
	}
}

#endif
