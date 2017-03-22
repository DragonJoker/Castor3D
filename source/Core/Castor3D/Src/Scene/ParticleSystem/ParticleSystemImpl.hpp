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
#ifndef ___C3D_ParticleSystemImpl_H___
#define ___C3D_ParticleSystemImpl_H___

#include "Castor3DPrerequisites.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/11/2016
	\~english
	\brief		Particle system implementation base class.
	\~french
	\brief		Classe de base de l'implémentation d'un système de particules.
	*/
	class ParticleSystemImpl
	{
	public:
		/*!
		\author 	Sylvain DOREMUS
		\version	0.9.0
		\date		16/11/2016
		\~english
		\brief		Particle system implementation types enumeration.
		\~french
		\brief		Enumération des types d'implémentation de système de particules.
		*/
		enum class Type
		{
			//!\~english	Particles are updated on CPU.
			//!\~french		Les particules sont mises à jour sur le CPU.
			eCpu,
			//!\~english	Particles are updated using Geometry shader and Transform feedback.
			//!\~french		Les particules sont mises à jour en utilisant les geometry shaders et le Transform feedback.
			eTransformFeedback,
			//!\~english	Particles are updated using a compute shader.
			//!\~french		Les particules sont mises à jour en utilisant un compute shader.
			eComputeShader,
			CASTOR_SCOPED_ENUM_BOUNDS( eCpu )
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_type		The implementation type.
		 *\param[in]	p_parent	The parent particle system.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_type		Le type d'implémentation.
		 *\param[in]	p_parent	Le système de particules parent.
		 */
		C3D_API ParticleSystemImpl( Type p_type, ParticleSystem & p_parent );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~ParticleSystemImpl();
		/**
		 *\~english
		 *\brief		Initialises the implementation.
		 *\return		\p true if all is OK.
		 *\~french
		 *\brief		Initialise l'implémentation.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API virtual bool Initialise() = 0;
		/**
		 *\~english
		 *\brief		Cleans the implementation.
		 *\~french
		 *\brief		Nettoie l'implémentation.
		 */
		C3D_API virtual void Cleanup() = 0;
		/**
		 *\~english
		 *\brief		Adds a particle variable.
		 *\param[in]	p_name			The variable name.
		 *\param[in]	p_type			The variable type.
		 *\param[in]	p_defaultValue	The variable default value.
		 *\~french
		 *\brief		Ajoute une variable de particule.
		 *\param[in]	p_name			Le nom de la variable.
		 *\param[in]	p_type			Le type de la variable.
		 *\param[in]	p_defaultValue	La valeur par défaut de la variable.
		 */
		C3D_API virtual void AddParticleVariable( Castor::String const & p_name
			, ElementType p_type
			, Castor::String const & p_defaultValue ) = 0;
		/**
		 *\~english
		 *\brief		Updates the particles.
		 *\param[in]	p_time	The time elapsed since last update.
		 *\param[in]	p_total	The total elapsed time.
		 *\return		The particles count.
		 *\~french
		 *\brief		Met à jour les particules.
		 *\param[in]	p_time	Le temps écoulé depuis la dernière mise à jour.
		 *\param[in]	p_total	Le temps total écoulé.
		 *\return		Le nombre de particules.
		 */
		C3D_API virtual uint32_t Update( std::chrono::milliseconds const & p_time
			, std::chrono::milliseconds const & p_total ) = 0;
		/**
		 *\~english
		 *\return		The implementation type.
		 *\~french
		 *\return		Le type d'implémentation.
		 */
		inline Type GetType()
		{
			return m_type;
		}
		/**
		 *\~english
		 *\return		The parent particle system.
		 *\~french
		 *\return		Le système de particules parent.
		 */
		inline ParticleSystem const & GetParent()const
		{
			return m_parent;
		}

	protected:
		//!\~english	The parent particle system.
		//!\~french		Le système de particules parent.
		ParticleSystem & m_parent;
		//!\~english	The implementation type.
		//!\~french		Le type d'implémentation.
		Type m_type;
	};
}

#endif
