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
		GlslWriter_API FragmentInput( InVec3 const & p_v3Vertex, InVec3 const & p_v3Normal );
		InVec3 m_v3Vertex;
		InVec3 m_v3Normal;
	};

	struct OutputComponents
	{
		GlslWriter_API OutputComponents( GlslWriter & p_writer );
		GlslWriter_API OutputComponents( InOutVec3 const & p_v3Diffuse, InOutVec3 const & p_v3Specular );
		InOutVec3 m_v3Diffuse;
		InOutVec3 m_v3Specular;
	};

	GlslWriter_API castor::String paramToString( castor::String & p_sep, FragmentInput const & p_value );
	GlslWriter_API castor::String paramToString( castor::String & p_sep, OutputComponents const & p_value );

	GlslWriter_API castor::String toString( FragmentInput const & p_value );
	GlslWriter_API castor::String toString( OutputComponents const & p_value );

	class LightingModel
	{
	public:
		GlslWriter_API LightingModel( ShadowType p_shadows, GlslWriter & p_writer );
		GlslWriter_API void declareModel();
		GlslWriter_API void declareDirectionalModel();
		GlslWriter_API void declarePointModel();
		GlslWriter_API void declareSpotModel();
		// Calls
		GlslWriter_API DirectionalLight getDirectionalLight( Type const & p_value );
		GlslWriter_API PointLight getPointLight( Type const & p_value );
		GlslWriter_API SpotLight getSpotLight( Type const & p_value );

	protected:
		GlslWriter_API Light getBaseLight( Type const & p_value );
		GlslWriter_API void doDeclareLight();
		GlslWriter_API void doDeclareDirectionalLight();
		GlslWriter_API void doDeclarePointLight();
		GlslWriter_API void doDeclareSpotLight();
		GlslWriter_API void doDeclareGetBaseLight();
		GlslWriter_API void doDeclareGetDirectionalLight();
		GlslWriter_API void doDeclareGetPointLight();
		GlslWriter_API void doDeclareGetSpotLight();

		virtual void doDeclareModel() = 0;
		virtual void doDeclareComputeDirectionalLight() = 0;
		virtual void doDeclareComputePointLight() = 0;
		virtual void doDeclareComputeSpotLight() = 0;
		virtual void doDeclareComputeOneDirectionalLight() = 0;
		virtual void doDeclareComputeOnePointLight() = 0;
		virtual void doDeclareComputeOneSpotLight() = 0;

	protected:
		ShadowType m_shadows;
		GlslWriter & m_writer;
		std::shared_ptr< Shadow > m_shadowModel;
	};
}

#endif
