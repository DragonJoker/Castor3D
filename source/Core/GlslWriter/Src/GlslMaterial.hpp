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

#include "GlslMat.hpp"

namespace GLSL
{
	static uint32_t constexpr MaxMaterialsCount = 2000u;
	castor::String const PassBufferName = cuT( "Materials" );

	class Materials
	{
	protected:
		GlslWriter_API Materials( GlslWriter & writer );

	public:
		GlslWriter_API virtual void declare() = 0;
		GlslWriter_API virtual Vec3 getDiffuse( Int const & index )const = 0;
		GlslWriter_API Float getRefractionRatio( Int const & index )const;
		GlslWriter_API Float getOpacity( Int const & index )const;
		GlslWriter_API Float getEmissive( Int const & index )const;
		GlslWriter_API Int getRefraction( Int const & index )const;
		GlslWriter_API Int getReflection( Int const & index )const;
		GlslWriter_API Float getGamma( Int const & index )const;
		GlslWriter_API Float getExposure( Int const & index )const;
		GlslWriter_API Float getAlphaRef( Int const & index )const;

	protected:
		GlslWriter & m_writer;
	};

	class LegacyMaterials
		: public Materials
	{
	public:
		GlslWriter_API LegacyMaterials( GlslWriter & writer );
		GlslWriter_API void declare()override;
		GlslWriter_API Vec3 getDiffuse( Int const & index )const override;
		GlslWriter_API Vec3 getSpecular( Int const & index )const;
		GlslWriter_API Float getAmbient( Int const & index )const;
		GlslWriter_API Float getShininess( Int const & index )const;
	};

	class PbrMRMaterials
		: public Materials
	{
	public:
		GlslWriter_API PbrMRMaterials( GlslWriter & writer );
		GlslWriter_API void declare()override;
		GlslWriter_API Vec3 getDiffuse( Int const & index )const override;
		GlslWriter_API Vec3 getAlbedo( Int const & index )const;
		GlslWriter_API Float getRoughness( Int const & index )const;
		GlslWriter_API Float getMetallic( Int const & index )const;
	};

	class PbrSGMaterials
		: public Materials
	{
	public:
		GlslWriter_API PbrSGMaterials( GlslWriter & writer );
		GlslWriter_API void declare()override;
		GlslWriter_API Vec3 getDiffuse( Int const & index )const override;
		GlslWriter_API Vec3 getSpecular( Int const & index )const;
		GlslWriter_API Float getGlossiness( Int const & index )const;
	};
}

#endif
