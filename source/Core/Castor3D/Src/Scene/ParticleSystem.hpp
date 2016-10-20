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

#include "Mesh/Buffer/TransformBufferDeclaration.hpp"

#include <Miscellaneous/PreciseTimer.hpp>

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
	using ParticleTransformBuffer = CpuTransformBuffer< Particle >;
	DECLARE_SMART_PTR( ParticleTransformBuffer );
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
		: public MovableObject
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
		 *\param[in]	p_renderSystem	The RenderSystem.
		 *\param[in]	p_count			The particles count.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name			Le nom.
		 *\param[in]	p_scene			La scene parente.
		 *\param[in]	p_parent		Le noeud de scène parent.
		 *\param[in]	p_renderSystem	Le RenderSystem.
		 *\param[in]	p_count			Le nombre de particules.
		 */
		C3D_API ParticleSystem( Castor::String const & p_name, Scene & p_scene, SceneNodeSPtr p_parent, RenderSystem & p_renderSystem, size_t p_count );
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
		 *\brief		Retrieves the material
		 *\return		The value
		 *\~french
		 *\brief		Recupere le materiau
		 *\return		La valeur
		 */
		inline MaterialSPtr GetMaterial()const
		{
			return m_material.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the billboards dimensions
		 *\return		The value
		 *\~french
		 *\brief		Recupere les dimensions des billboards
		 *\return		La valeur
		 */
		inline Castor::Size const & GetDimensions()const
		{
			return m_dimensions;
		}

	private:
		//!\~english	The Vertex buffer's description.
		//!\~french		La description du tampon de sommets.
		TransformBufferDeclaration m_declaration;
		//!\~english	The particles count.
		//!\~french		Le nombre de particules.
		size_t m_count{ 0 };
		//!\~english	The material.
		//!\~french		Le materiau.
		MaterialWPtr m_material;
		//!\~english	The billboards dimensions.
		//!\~french		Les dimensions des billboards.
		Castor::Size m_dimensions;
		//!\~english	The particles buffer.
		//!\~french		Le tampon de particules.
		std::array< VertexBufferSPtr, 2u > m_particleBuffers;
		//!\~english	The Transform Feedback buffers.
		//!\~french		Les tampons de transform feedback.
		std::array< ParticleTransformBufferUPtr, 2u > m_transformBuffers;
		//!\~english	The timer, for the particles update.
		//!\~french		Le timer, pour la mise à jour des particules.
		Castor::PreciseTimer m_timer;
	};
}

#endif
