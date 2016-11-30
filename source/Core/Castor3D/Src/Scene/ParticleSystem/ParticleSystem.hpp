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
#ifndef ___C3D_ParticleSystem_H___
#define ___C3D_ParticleSystem_H___

#include "Scene/MovableObject.hpp"

#include "Mesh/Buffer/BufferDeclaration.hpp"

#include <Miscellaneous/PreciseTimer.hpp>

namespace Castor3D
{
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
		 *\brief		Updates the particles.
		 *\~french
		 *\brief		Met à jour les particules.
		 */
		C3D_API void Update();
		/**
		 *\~english
		 *\brief		Sets the material
		 *\param[in]	p_value	The new value
		 *\~french
		 *\brief		Definit le materiau
		 *\param[in]	p_value	La nouvelle valeur
		 */
		C3D_API void SetMaterial( MaterialSPtr p_value );
		/**
		 *\~english
		 *\brief		Sets the particles dimensions.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Definit les dimensions des particules.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		C3D_API void SetDimensions( Castor::Size const & p_value );
		/**
		 *\~english
		 *\brief		Sets the particles type name.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Definit le nom du type de particules.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		C3D_API void SetParticleType( Castor::String const & p_value );
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
		 *\brief		Adds a particle variable.
		 *\param[in]	p_name	The variable name.
		 *\param[in]	p_type	The variable type.
		 *\~french
		 *\brief		Ajoute une variable de particule.
		 *\param[in]	p_name	Le nom de la variable.
		 *\param[in]	p_type	Le type de la variable.
		 */
		C3D_API void AddParticleVariable( Castor::String const & p_name, ElementType p_type, Castor::String const & p_defaultValue );
		/**
		 *\~english
		 *\brief		Defines the program used to update the particles through transform feedback.
		 *\param[in]	p_program	The program.
		 *\~french
		 *\brief		Définit le programme utilisé pour mettre à jour les particules via le transform feedback.
		 *\param[in]	p_program	Le programme.
		 */
		C3D_API void SetTFUpdateProgram( ShaderProgramSPtr p_program );
		/**
		 *\~english
		 *\brief		Defines the program used to update the particles through compute shader.
		 *\param[in]	p_program	The program.
		 *\~french
		 *\brief		Définit le programme utilisé pour mettre à jour les particules via un compute shader.
		 *\param[in]	p_program	Le programme.
		 */
		C3D_API void SetCSUpdateProgram( ShaderProgramSPtr p_program );
		/**
		 *\~english
		 *\return		The particles written at last update.
		 *\~french
		 *\return		Le nombre de particules écrites lors de la dernière mise à jour.
		 */
		C3D_API uint32_t GetParticlesCount()const
		{
			return m_activeParticlesCount;
		}
		/**
		 *\~english
		 *\return		The maximum particles count.
		 *\~french
		 *\return		Le nombre de particules maximum.
		 */
		C3D_API size_t GetMaxParticlesCount()const
		{
			return m_particlesCount;
		}
		/**
		 *\~english
		 *\return		The billboards.
		 *\~french
		 *\return		Les billboards.
		 */
		inline BillboardBaseSPtr GetBillboards()const
		{
			return m_particlesBillboard;
		}
		/**
		 *\~english
		 *\return		The particle's components default values.
		 *\~french
		 *\return		Les valeurs par défaut des composantes d'une particule.
		 */
		inline Castor::StrStrMap const & GetDefaultValues()const
		{
			return m_defaultValues;
		}

	protected:
		//!\~english	The map of default value per variable name.
		//!\~french		La map de valeur par défaut pour les variables.
		Castor::StrStrMap m_defaultValues;
		//!\~english	The billboard vertex buffer position element description.
		//!\~french		La description de l'élément position du tampons de sommets des billboards.
		BufferDeclaration m_billboardInputs;
		//!\~english	The billboards containing the particles.
		//!\~french		Les billboards contenant les particules.
		BillboardBaseSPtr m_particlesBillboard;
		//!\~english	The billboards dimensions.
		//!\~french		Les dimensions des billboards.
		Castor::Size m_dimensions;
		//!\~english	The Material.
		//!\~french		Le Material.
		MaterialWPtr m_material;
		//!\~english	The particles count.
		//!\~french		Le nombre de particules.
		size_t m_particlesCount{ 0u };
		//!\~english	The active particles count.
		//!\~french		Le nombre de particules actives.
		uint32_t m_activeParticlesCount{ 0u };
		//!\~english	The offset of the center attribute in the vertex buffer.
		//!\~french		Le décalage de l'attribut du centre dans le tampon de sommets..
		uint32_t m_centerOffset{ 0u };
		//!\~english	The timer, for the particles update.
		//!\~french		Le timer, pour la mise à jour des particules.
		Castor::PreciseTimer m_timer;
		//!\~english	Tells that the next update is the first one.
		//!\~french		Dit que la prochaine mise à jour est la première.
		bool m_firstUpdate{ true };
		//!\~english	The total elapsed time.
		//!\~french		Le temps total écoulé.
		float m_totalTime{ 0.0f };
		//!\~english	The CPU implementation.
		//!\~french		L'implémentation CPU.
		CpuParticleSystemUPtr m_cpuImpl;
		//!\~english	The implementation using transform feedback.
		//!\~french		L'implémentation utilisant le transform feedback.
		TransformFeedbackParticleSystemUPtr m_tfImpl;
		//!\~english	The implementation using compute shader.
		//!\~french		L'implémentation utilisant un compute shader.
		ComputeParticleSystemUPtr m_csImpl;
		//!\~english	The implementation chosen after initialisation.
		//!\~french		L'implémentation choisie après initialisation.
		ParticleSystemImpl * m_impl{ nullptr };
	};
}

#endif
