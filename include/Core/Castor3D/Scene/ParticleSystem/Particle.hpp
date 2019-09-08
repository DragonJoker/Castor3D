/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Particle_H___
#define ___C3D_Particle_H___

#include "Castor3D/Scene/ParticleSystem/ParticleDeclaration.hpp"
#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>

namespace castor3d
{
	template< ParticleFormat Type >
	struct ElementTyper;
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		19/10/2016
	\~english
	\brief		Holds one particle data.
	\~french
	\brief		Contient les données d'une particule.
	*/
	class Particle
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	description		The particle's elements description.
		 *\param[in]	defaultValues	The default values for the particle's elements.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	description		La description des éléments de la particule.
		 *\param[in]	defaultValues	Les valeurs par défaut des éléments de la particule.
		 */
		C3D_API Particle( ParticleDeclaration const & description
			, castor::StrStrMap const & defaultValues );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	description	The particle's elements description.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	description	La description des éléments de la particule.
		 */
		C3D_API explicit Particle( ParticleDeclaration const & description );
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\param[in]	rhs	The object to copy.
		 *\~french
		 *\brief		Constructeur par copie.
		 *\param[in]	rhs	L'objet à copier.
		 */
		C3D_API Particle( Particle const & rhs );
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\param[in]	rhs	The object to move.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 *\param[in]	rhs	L'objet à déplacer.
		 */
		C3D_API Particle( Particle && rhs );
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\param[in]	rhs	The object to copy.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 *\param[in]	rhs	L'objet à copier.
		 */
		C3D_API Particle & operator=( Particle const & rhs );
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\param[in]	rhs	The object to move.
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement.
		 *\param[in]	rhs	L'objet à déplacer.
		 */
		C3D_API Particle & operator=( Particle && rhs );
		/**
		 *\~english
		 *\brief		Sets the particle variable's value at given index.
		 *\param[in]	index	The variable index.
		 *\param[in]	value	The variable value.
		 *\~french
		 *\brief		Définit la valeur de la variable de particule à l'index donné.
		 *\param[in]	index	L'index de la variable.
		 *\param[in]	value	La valeur de la variable.
		 */
		template< ParticleFormat Type >
		inline void setValue( uint32_t index
			, typename ElementTyper< Type >::Type const & value );
		/**
		 *\~english
		 *\brief		Sets the particle variable's value at given index.
		 *\param[in]	index	The variable index.
		 *\return		The variable value.
		 *\~french
		 *\brief		Définit la valeur de la variable de particule à l'index donné.
		 *\param[in]	index	L'index de la variable.
		 *\return		La valeur de la variable.
		 */
		template< ParticleFormat Type >
		inline typename ElementTyper< Type >::Type getValue( uint32_t index )const;
		/**
		 *\~english
		 *\return		The particle data.
		 *\~french
		 *\return		Les données de la particule.
		 */
		inline uint8_t const * getData()const
		{
			return m_data.data();
		}
		/**
		 *\~english
		 *\return		The particle data.
		 *\~french
		 *\return		Les données de la particule.
		 */
		inline uint8_t * getData()
		{
			return m_data.data();
		}

	private:
		//!\~english	The particle's elemets description.
		//!\~french		La description des éléments de la particule.
		ParticleDeclaration const & m_description;
		//!\~english	The particle's data.
		//!\~french		Les données de la particule.
		std::vector< uint8_t > m_data;
	};
}

#include "Castor3D/Scene/ParticleSystem/Particle.inl"

#endif
