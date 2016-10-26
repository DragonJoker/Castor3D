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
#ifndef ___C3D_PARTICLE_SYSTEM_H___
#define ___C3D_PARTICLE_SYSTEM_H___

#include "MovableObject.hpp"

#include "Mesh/Buffer/BufferDeclaration.hpp"
#include "Texture/TextureUnit.hpp"

#include <Miscellaneous/PreciseTimer.hpp>
#include <Design/Named.hpp>
#include <Design/OwnedBy.hpp>

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		19/10/2016
	\~english
	\brief		Holds one particle data.
	\~french
	\brief		Contient les données d'une particule.
	*/
	struct Particle
	{
		enum class Type
		{
			Launcher,
			Shell,
			SecondaryShell
		};
		//!\~english	The particle type.
		//!\~french		Le type de particule.
		float m_type;
		//!\~english	The particle position.
		//!\~french		La position de la particule.
		Castor::Point3f m_position;
		//!\~english	The particle velocity.
		//!\~french		La vitesse de la particule.
		Castor::Point3f m_velocity;
		//!\~english	The particle life time.
		//!\~french		La durée de vie de la particule.
		float m_lifeTime;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		19/10/2016
	\~english
	\brief		Particle system implementation.
	\~french
	\brief		Implémentation d'un système de particules.
	*/
	class ParticleSystem
		: public Castor::Named
		, public Castor::OwnedBy< Scene >
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\version	0.9.0
		\date		19/10/2016
		\~english
		\brief		ParticleSystem loader
		\~french
		\brief		Loader de ParticleSystem
		*/
		class TextWriter
			: public MovableObject::TextWriter
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API explicit TextWriter( Castor::String const & p_tabs );
			/**
			 *\~english
			 *\brief		Writes a ParticleSystem into a text file
			 *\param[in]	p_file	The file to save the object in
			 *\param[in]	p_obj	The object to save
			 *\~french
			 *\brief		Ecrit un ParticleSystem dans un fichier texte
			 *\param[in]	p_file	Le fichier
			 *\param[in]	p_obj	L'objet
			 */
			C3D_API bool operator()( ParticleSystem const & p_obj, Castor::TextFile & p_file );
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name			The name.
		 *\param[in]	p_scene			The parent scene.
		 *\param[in]	p_parent		The parent scene node.
		 *\param[in]	p_count			The particles count.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name			Le nom.
		 *\param[in]	p_scene			La scene parente.
		 *\param[in]	p_parent		Le noeud de scène parent.
		 *\param[in]	p_count			Le nombre de particules.
		 */
		C3D_API ParticleSystem( Castor::String const & p_name, Scene & p_scene, SceneNodeSPtr p_parent, size_t p_count );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~ParticleSystem();
		/**
		 *\~english
		 *\brief		Initialises GPU side elements
		 *\return		\p true if all is OK
		 *\~french
		 *\brief		Initialise les elements GPU
		 *\return		\p true si tout s'est bien passe
		 */
		C3D_API bool Initialise();
		/**
		 *\~english
		 *\brief		Cleans GPU side elements up
		 *\~french
		 *\brief		Nettoie les elements GPU
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Sets the material
		 *\param[in]	p_material	The new value
		 *\~french
		 *\brief		Definit le materiau
		 *\param[in]	p_material	La nouvelle valeur
		 */
		C3D_API void SetMaterial( MaterialSPtr p_material );
		/**
		 *\~english
		 *\brief		Updates the particles.
		 *\~french
		 *\brief		Met à jour les particules.
		 */
		C3D_API void Update();
		/**
		 *\~english
		 *\brief		Sets the particles dimensions.
		 *\param[in]	p_dimensions	The new value.
		 *\~french
		 *\brief		Definit les dimensions des particules.
		 *\param[in]	p_dimensions	La nouvelle valeur.
		 */
		C3D_API void SetDimensions( Castor::Size const & p_dimensions );
		/**
		 *\~english
		 *\return		The material.
		 *\~french
		 *\return		Le materiau.
		 */
		C3D_API MaterialSPtr GetMaterial()const;
		/**
		 *\~english
		 *\return		The billboards dimensions.
		 *\~french
		 *\return		Les dimensions des billboards.
		 */
		C3D_API Castor::Size const & GetDimensions()const;
		/**
		 *\~english
		 *\brief		Detaches the movable object from it's parent
		 *\~french
		 *\brief		Détache l'objet de son parent
		 */
		C3D_API void Detach();
		/**
		 *\~english
		 *\brief		Attaches the movable object to a node
		 *\~french
		 *\brief		Attache l'object à un noeud
		 */
		C3D_API void AttachTo( SceneNodeSPtr p_node );
		/**
		 *\~english
		 *\brief		Retrieves the parent node
		 *\return		The value
		 *\~french
		 *\brief		Récupère le noeud parent
		 *\return		La valeur
		 */
		C3D_API SceneNodeSPtr GetParent()const;
		/**
		 *\~english
		 *\return		The billboards.
		 *\~french
		 *\return		Les billboards.
		 */
		inline BillboardListBaseSPtr GetBillboards()const
		{
			return m_particlesBillboard;
		}

	private:
		bool DoCreateUpdateProgram();
		bool DoCreateUpdatePipeline();
		void DoUpdate();

	private:
		//!\~english	The computed elements description.
		//!\~french		La description des éléments calculés.
		BufferDeclaration m_computed;
		//!\~english	The billboards containing the particles.
		//!\~french		Les billboards contenant les particules.
		BillboardListBaseSPtr m_particlesBillboard;
		//!\~english	The particles count.
		//!\~french		Le nombre de particules.
		size_t m_particlesCount{ 0 };
		//!\~english	The program used to update the transform buffer.
		//!\~french		Le programme utilisé pour mettre à jour le tampon de transformation.
		ShaderProgramSPtr m_updateProgram;
		//!\~english	The pipeline used to update the transform buffer.
		//!\~french		Le pipeline utilisé pour mettre à jour le tampon de transformation.
		PipelineUPtr m_updatePipeline;
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
		//!\~english	The timer, for the particles update.
		//!\~french		Le timer, pour la mise à jour des particules.
		Castor::PreciseTimer m_timer;
		//!\~english	The texture containing random directions.
		//!\~french		La texture contenant des directions aléatoires.
		TextureUnit m_randomTexture;
		//!\~english	Tells that the next update is the first one.
		//!\~french		Dit que la prochaine mise à jour est la première.
		bool m_firstUpdate{ true };
		//!\~english	The total elapsed time.
		//!\~french		Le temps total écoulé.
		float m_totalTime{ 0.0f };
		uint32_t m_vtx{ 0u };
		uint32_t m_tfb{ 1u };
	};
}

#endif
