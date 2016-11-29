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
#ifndef ___C3D_TransformFeedbackParticleSystem_H___
#define ___C3D_TransformFeedbackParticleSystem_H___

#include "ParticleSystem.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/11/2016
	\~english
	\brief		Transform feedback Particle system implementation.
	\~french
	\brief		Implémentation d'un système de particules utilisant le transform feedback.
	*/
	class TransformFeedbackParticleSystem
		: public ParticleSystemImpl
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_scene			The parent scene.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_scene			La scene parente.
		 */
		C3D_API TransformFeedbackParticleSystem( ParticleSystem & p_parent );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~TransformFeedbackParticleSystem();
		/**
		 *\copydoc		Castor3D::CpuParticleSystem::Initialise
		 */
		C3D_API bool Initialise()override;
		/**
		 *\copydoc		Castor3D::CpuParticleSystem::Cleanup
		 */
		C3D_API void Cleanup()override;
		/**
		 *\copydoc		Castor3D::CpuParticleSystem::Update
		 */
		C3D_API uint32_t Update( float p_time, float p_total )override;
		/**
		 *\copydoc		Castor3D::CpuParticleSystem::AddParticleVariable
		 */
		C3D_API void AddParticleVariable( Castor::String const & p_name, ElementType p_type, Castor::String const & p_defaultValue )override;
		/**
		 *\~english
		 *\brief		Defines the program used to update the particles.
		 *\param[in]	p_program	The program.
		 *\~french
		 *\brief		Définit le programme utilisé pour mettre à jour les particules.
		 *\param[in]	p_program	Le programme.
		 */
		C3D_API void SetUpdateProgram( ShaderProgramSPtr p_program );

	private:
		/**
		 *\~english
		 *\brief		Creates the pipeline used to update the particles.
		 *\return		\p false on failure.
		 *\~french
		 *\brief		Crée le pipeline utilisé pour mettre à jour les particules.
		 *\return		\p false en cas d'échec.
		 */
		bool DoCreateUpdatePipeline();

	private:
		//!\~english	The computed elements description.
		//!\~french		La description des éléments calculés.
		BufferDeclaration m_computed;
		//!\~english	The vertex buffer elements description.
		//!\~french		La description des éléments des tampons de sommets.
		BufferDeclaration m_inputs;
		//!\~english	The program used to update the transform buffer.
		//!\~french		Le programme utilisé pour mettre à jour le tampon de transformation.
		ShaderProgramSPtr m_updateProgram;
		//!\~english	The pipeline used to update the transform buffer.
		//!\~french		Le pipeline utilisé pour mettre à jour le tampon de transformation.
		RenderPipelineUPtr m_updatePipeline;
		//!\~english	The geometry buffers used to update the transform buffer.
		//!\~french		Les tampons de géométrie utilisé pour mettre à jour le tampon de transformation.
		std::array< GeometryBuffersSPtr, 2 > m_updateGeometryBuffers;
		//!\~english	The vertex buffers used to update the transform buffer.
		//!\~french		Le tampon de sommets utilisé pour mettre à jour le tampon de transformation.
		std::array< VertexBufferSPtr, 2 > m_updateVertexBuffers;
		//!\~english	The transform feedback used to update the particles.
		//!\~french		Le transform feedback utilisé pour mettre à jour les particules.
		std::array< TransformFeedbackUPtr, 2 > m_transformFeedbacks;
		//!\~english	The frame variable buffer holding particle system related variables.
		//!\~french		Le tampon de variables contenant les variables relatives au système de particules.
		FrameVariableBufferSPtr m_ubo;
		//!\~english	The frame variable holding time since last update.
		//!\~french		La variable de frame contenant le temps écoulé depuis la dernière mise à jour.
		OneFloatFrameVariableSPtr m_deltaTime;
		//!\~english	The frame variable holding total elapsed time.
		//!\~french		La variable de frame contenant le temps total écoulé.
		OneFloatFrameVariableSPtr m_time;
		//!\~english	The frame variable holding the launches lifetime.
		//!\~french		La variable de frame contenant la durée de vie des lanceurs.
		OneFloatFrameVariableSPtr m_launcherLifetime;
		//!\~english	The frame variable holding the shells lifetime.
		//!\~french		La variable de frame contenant la durée de vie des particules.
		OneFloatFrameVariableSPtr m_shellLifetime;
		//!\~english	The frame variable holding the secondary shells lifetime.
		//!\~french		La variable de frame contenant la durée de vie des particules secondaires.
		OneFloatFrameVariableSPtr m_secondaryShellLifetime;
		//!\~english	The texture containing random directions.
		//!\~french		La texture contenant des directions aléatoires.
		TextureUnit m_randomTexture;
		//!\~english	The current render buffer index.
		//!\~french		L'indice du tampon de rendu actuel.
		uint32_t m_vtx{ 0u };
		//!\~english	The current update buffer index.
		//!\~french		L'indice du tampon de mise à jour actuel.
		uint32_t m_tfb{ 1u };
	};
}

#endif
