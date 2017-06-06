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
#ifndef ___C3D_DeferredSpotLightPass_H___
#define ___C3D_DeferredSpotLightPass_H___

#include "MeshLightPass.hpp"

namespace Castor3D
{
	class SpotLightPass
		: public MeshLightPass
	{
	protected:
		struct Program
			: public MeshLightPass::Program
		{
		public:
			Program( Engine & p_engine
				, Castor::String const & p_vtx
				, Castor::String const & p_pxl );
			virtual ~Program();

		private:
			void DoBind( Light const & p_light )override;

		private:
			//!\~english	The variable containing the light position.
			//!\~french		La variable contenant la position de la lumière.
			PushUniform3fSPtr m_lightPosition;
			//!\~english	The variable containing the light attenuation.
			//!\~french		La variable contenant l'atténuation de la lumière.
			PushUniform3fSPtr m_lightAttenuation;
			//!\~english	The variable containing the light direction.
			//!\~french		La variable contenant la direction de la lumière.
			PushUniform3fSPtr m_lightDirection;
			//!\~english	The variable containing the light exponent.
			//!\~french		La variable contenant l'exposant de la lumière.
			PushUniform1fSPtr m_lightExponent;
			//!\~english	The variable containing the light cut off.
			//!\~french		La variable contenant l'angle du cône de la lumière.
			PushUniform1fSPtr m_lightCutOff;
			//!\~english	The variable containing the light space transformation matrix.
			//!\~french		La variable contenant la matrice de transformation de la lumière.
			PushUniform4x4fSPtr m_lightTransform;
		};

	public:
		SpotLightPass( Engine & p_engine
			, FrameBuffer & p_frameBuffer
			, FrameBufferAttachment & p_depthAttach
			, bool p_shadows );
		~SpotLightPass();

	private:
		LightPass::ProgramPtr DoCreateProgram( Castor::String const & p_vtx
			, Castor::String const & p_pxl )const override;
		Castor::Point3fArray DoGenerateVertices()const override;
		UIntArray DoGenerateFaces()const override;
		Castor::Matrix4x4r DoComputeModelMatrix( Light const & p_light
			, Camera const & p_camera )const override;
	};
}

#endif
