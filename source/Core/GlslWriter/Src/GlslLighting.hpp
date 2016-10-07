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
#ifndef ___GLSL_LIGHTING_H___
#define ___GLSL_LIGHTING_H___

#include "GlslIntrinsics.hpp"

namespace GLSL
{
	struct FragmentInput
	{
		GlslWriter_API FragmentInput( GlslWriter & p_writer );
		GlslWriter_API FragmentInput( InParam< Vec3 > const & p_v3Vertex, InParam< Vec3 > const & p_v3Normal, InParam< Vec3 > const & p_v3Tangent, InParam< Vec3 > const & p_v3Bitangent );
		InParam< Vec3 > m_v3Vertex;
		InParam< Vec3 > m_v3Normal;
		InParam< Vec3 > m_v3Tangent;
		InParam< Vec3 > m_v3Bitangent;
	};

	struct OutputComponents
	{
		GlslWriter_API OutputComponents( GlslWriter & p_writer );
		GlslWriter_API OutputComponents( InOutParam< Vec3 > const & p_v3Ambient, InOutParam< Vec3 > const & p_v3Diffuse, InOutParam< Vec3 > const & p_v3Specular );
		InOutParam< Vec3 > m_v3Ambient;
		InOutParam< Vec3 > m_v3Diffuse;
		InOutParam< Vec3 > m_v3Specular;
	};

	GlslWriter_API Castor::String ParamToString( Castor::String & p_sep, FragmentInput const & p_value );
	GlslWriter_API Castor::String ParamToString( Castor::String & p_sep, OutputComponents const & p_value );

	GlslWriter_API Castor::String ToString( FragmentInput const & p_value );
	GlslWriter_API Castor::String ToString( OutputComponents const & p_value );

	class LightingModel
	{
	public:
		GlslWriter_API LightingModel( uint32_t p_flags, GlslWriter & p_writer );
		GlslWriter_API void DeclareModel();
		// Calls
		GlslWriter_API void ComputeCombinedLighting( Vec3 const & p_worldEye
													 , Float const & p_shininess
													 , FragmentInput const & p_fragmentIn
													 , OutputComponents & p_output );
		GlslWriter_API Light GetDirectionalLight( Type const & p_value );
		GlslWriter_API Light GetPointLight( Type const & p_value );
		GlslWriter_API Light GetSpotLight( Type const & p_value );
		GlslWriter_API void ComputeDirectionalLight( Light const & p_light
													   , Vec3 const & p_worldEye
													   , Float const & p_shininess
													   , FragmentInput const & p_fragmentIn
													   , OutputComponents & p_output );
		GlslWriter_API void ComputePointLight( Light const & p_light
												 , Vec3 const & p_worldEye
												 , Float const & p_shininess
												 , FragmentInput const & p_fragmentIn
												 , OutputComponents & p_output );
		GlslWriter_API void ComputeSpotLight( Light const & p_light
												, Vec3 const & p_worldEye
												, Float const & p_shininess
												, FragmentInput const & p_fragmentIn
												, OutputComponents & p_output );

	protected:

	protected:
		GlslWriter_API Light GetLightColourAndPosition( Type const & p_value );
		GlslWriter_API void Declare_Light();
		GlslWriter_API void Declare_GetLightColourAndPosition();
		GlslWriter_API void Declare_GetDirectionalLight();
		GlslWriter_API void Declare_GetPointLight();
		GlslWriter_API void Declare_GetSpotLight();

		virtual void DoDeclareModel() = 0;
		virtual void Declare_ComputeDirectionalLight() = 0;
		virtual void Declare_ComputePointLight() = 0;
		virtual void Declare_ComputeSpotLight() = 0;

	protected:
		uint32_t m_flags;
		GlslWriter & m_writer;
	};

	class PhongLightingModel
		: public LightingModel
	{
	public:
		GlslWriter_API PhongLightingModel( uint32_t p_flags, GlslWriter & p_writer );
		GlslWriter_API static std::unique_ptr< LightingModel > Create( uint32_t p_flags, GlslWriter & p_writer );

	protected:
		virtual void DoDeclareModel();
		virtual void Declare_ComputeDirectionalLight();
		virtual void Declare_ComputePointLight();
		virtual void Declare_ComputeSpotLight();

		void DoComputeLight( Light const & p_light
							 , Vec3 const & p_worldEye
							 , Vec3 const & p_direction
							 , Float const & p_shininess
							 , FragmentInput const & p_fragmentIn
							 , OutputComponents & p_output );
		void DoDeclare_ComputeLight();

	public:
		GlslWriter_API static const Castor::String Name;
	};
}

#endif
