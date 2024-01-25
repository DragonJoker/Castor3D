/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Particle_H___
#define ___C3D_Particle_H___

#include "ParticleModule.hpp"

#include "Castor3D/Scene/ParticleSystem/ParticleDeclaration.hpp"

#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>

namespace castor3d
{
	class Particle
	{
	public:
		C3D_API Particle( Particle const & rhs );
		C3D_API Particle( Particle && rhs )noexcept;
		C3D_API Particle & operator=( Particle const & rhs );
		C3D_API Particle & operator=( Particle && rhs )noexcept;
		C3D_API ~Particle()noexcept = default;
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
		 *\brief		Sets the particle variable's value at given index.
		 *\param[in]	index	The variable index.
		 *\param[in]	value	The variable value.
		 *\~french
		 *\brief		Définit la valeur de la variable de particule à l'index donné.
		 *\param[in]	index	L'index de la variable.
		 *\param[in]	value	La valeur de la variable.
		 */
		template< ParticleFormat Type >
		void setValue( uint32_t index
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
		typename ElementTyper< Type >::Type getValue( uint32_t index )const;
		/**
		 *\~english
		 *\return		The particle data.
		 *\~french
		 *\return		Les données de la particule.
		 */
		uint8_t const * getData()const
		{
			return m_data.data();
		}
		/**
		 *\~english
		 *\return		The particle data.
		 *\~french
		 *\return		Les données de la particule.
		 */
		uint8_t * getData()
		{
			return m_data.data();
		}

	private:
		ParticleDeclaration const & m_description;
		castor::Vector< uint8_t > m_data;
	};
}

#include "Castor3D/Scene/ParticleSystem/Particle.inl"

#endif
