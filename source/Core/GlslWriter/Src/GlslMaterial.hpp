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
	Castor::String const PassBufferName = cuT( "c3d_materials" );

	class Materials
	{
	protected:
		GlslWriter_API Materials( GlslWriter & p_writer );
		GlslWriter_API virtual void Declare() = 0;

	public:
		GlslWriter_API Float GetRefractionRatio( Int const & p_index )const;
		GlslWriter_API Float GetOpacity( Int const & p_index )const;
		GlslWriter_API Float GetEmissive( Int const & p_index )const;
		GlslWriter_API Int GetRefraction( Int const & p_index )const;
		GlslWriter_API Int GetReflection( Int const & p_index )const;
		GlslWriter_API Float GetGamma( Int const & p_index )const;
		GlslWriter_API Float GetExposure( Int const & p_index )const;
		GlslWriter_API Float GetAlphaRef( Int const & p_index )const;

	protected:
		GlslWriter_API void DoDeclare( int p_offsetRR, int p_indexRR
			, int p_offsetRF, int p_indexRF
			, int p_offsetRL, int p_indexRL
			, int p_offsetOP, int p_indexOP
			, int p_offsetGM, int p_indexGM
			, int p_offsetEX, int p_indexEX
			, int p_offsetAR, int p_indexAR
			, int p_offsetEM, int p_indexEM );

	private:
		// Materials are aligned on vec4, so the size of a material
		// is the number of vec4 needed to contain it.
		GlslWriter_API virtual int DoGetMaterialSize()const = 0;

	protected:
		GlslWriter & m_writer;
		Function< Int, InInt, InInt, InInt > m_int;
		Function< Float, InInt, InInt, InInt > m_float;
		Function< Vec3, InInt, InInt, InInt > m_vec3;

	private:
		Function< Float, InInt > m_refractionRatio;
		Function< Int, InInt > m_refraction;
		Function< Int, InInt > m_reflection;
		Function< Float, InInt > m_opacity;
		Function< Float, InInt > m_gamma;
		Function< Float, InInt > m_exposure;
		Function< Float, InInt > m_alphaRef;
		Function< Float, InInt > m_emissive;
	};

	class LegacyMaterials
		: public Materials
	{
	public:
		GlslWriter_API LegacyMaterials( GlslWriter & p_writer );
		GlslWriter_API void Declare()override;
		GlslWriter_API Vec3 GetDiffuse( Int const & p_index )const;
		GlslWriter_API Vec3 GetSpecular( Int const & p_index )const;
		GlslWriter_API Float GetAmbient( Int const & p_index )const;
		GlslWriter_API Float GetShininess( Int const & p_index )const;

		static uint32_t constexpr Size = 4u;

	private:
		inline int DoGetMaterialSize()const override
		{
			return int( Size );
		}

	private:
		Function< Vec3, InInt > m_diffuse;
		Function< Float, InInt > m_ambient;
		Function< Vec3, InInt > m_specular;
		Function< Float, InInt > m_shininess;
	};

	class PbrMaterials
		: public Materials
	{
	public:
		GlslWriter_API PbrMaterials( GlslWriter & p_writer );
		GlslWriter_API void Declare()override;
		GlslWriter_API Vec3 GetAlbedo( Int const & p_index )const;
		GlslWriter_API Float GetRoughness( Int const & p_index )const;
		GlslWriter_API Float GetMetallic( Int const & p_index )const;

		static uint32_t constexpr Size = 4u;

	private:
		inline int DoGetMaterialSize()const override
		{
			return int( Size );
		}

	private:
		Function< Vec3, InInt > m_albedo;
		Function< Float, InInt > m_roughness;
		Function< Float, InInt > m_metallic;
	};
}

#endif
