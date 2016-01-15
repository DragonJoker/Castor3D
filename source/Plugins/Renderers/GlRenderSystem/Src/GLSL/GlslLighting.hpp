/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___GLSL_LIGHTING_H___
#define ___GLSL_LIGHTING_H___

#include "GlslIntrinsics.hpp"

namespace GlRender
{
	namespace GLSL
	{
		struct FragmentInput
		{
			InParam< Vec3 > m_v3Vertex;
			InParam< Vec3 > m_v3Normal;
			InParam< Vec3 > m_v3Tangent;
			InParam< Vec3 > m_v3Bitangent;
		};

		struct OutputComponents
		{
			InOutParam< Vec3 > m_v3Ambient;
			InOutParam< Vec3 > m_v3Diffuse;
			InOutParam< Vec3 > m_v3Specular;
		};

		C3D_Gl_API Castor::String ParamToString( Castor::String & p_sep, FragmentInput const & p_value );
		C3D_Gl_API Castor::String ParamToString( Castor::String & p_sep, OutputComponents const & p_value );

		C3D_Gl_API Castor::String ToString( FragmentInput const & p_value );
		C3D_Gl_API Castor::String ToString( OutputComponents const & p_value );

		class LightingModel
		{
		public:
			C3D_Gl_API LightingModel( uint32_t p_flags, GlslWriter & p_writer );
			C3D_Gl_API void DeclareModel();
			// Calls
			C3D_Gl_API Light GetDirectionalLight( Type const & p_value );
			C3D_Gl_API Light GetPointLight( Type const & p_value );
			C3D_Gl_API Light GetSpotLight( Type const & p_value );
			C3D_Gl_API void ComputeDirectionalLight( Light const & p_light, Vec3 const & p_worldEye, Float const & p_shininess,
													 FragmentInput const & p_fragmentIn, OutputComponents & p_output );

			C3D_Gl_API void ComputePointLight( Light const & p_light, Vec3 const & p_worldEye, Float const & p_shininess,
											   FragmentInput const & p_fragmentIn, OutputComponents & p_output );

			C3D_Gl_API void ComputeSpotLight( Light const & p_light, Vec3 const & p_worldEye, Float const & p_shininess,
											  FragmentInput const & p_fragmentIn, OutputComponents & p_output );

		protected:
			C3D_Gl_API Light GetLightColourAndPosition( Type const & p_value );
			C3D_Gl_API void Declare_Light();
			C3D_Gl_API void Declare_GetLightColourAndPosition();
			C3D_Gl_API void Declare_GetDirectionalLight();
			C3D_Gl_API void Declare_GetPointLight();
			C3D_Gl_API void Declare_GetSpotLight();

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
			C3D_Gl_API PhongLightingModel( uint32_t p_flags, GlslWriter & p_writer );
			C3D_Gl_API static std::unique_ptr< LightingModel > Create( uint32_t p_flags, GlslWriter & p_writer );

		protected:
			virtual void DoDeclareModel();
			virtual void Declare_ComputeDirectionalLight();
			virtual void Declare_ComputePointLight();
			virtual void Declare_ComputeSpotLight();

			void DoComputeLight( Light const & p_light, Vec3 const & p_worldEye, Vec3 const & p_direction, Float const & p_shininess,
								 FragmentInput const & p_fragmentIn, OutputComponents & p_output );
			void DoDeclare_ComputeLight();

		public:
			C3D_Gl_API static const Castor::String Name;
		};
	}
}

#include "GlslLighting.inl"

#endif
