/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Particle_H___
#define ___C3D_Particle_H___

#include <Pipeline/VertexLayout.hpp>

namespace castor3d
{
	template< renderer::AttributeFormat Type >
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
		 *\param[in]	p_description	The particle's elements description.
		 *\param[in]	p_defaultValues	The default values for the particle's elements.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_description	La description des éléments de la particule.
		 *\param[in]	p_defaultValues	Les valeurs par défaut des éléments de la particule.
		 */
		C3D_API Particle( BufferDeclaration const & p_description, castor::StrStrMap const & p_defaultValues );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_description	The particle's elements description.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_description	La description des éléments de la particule.
		 */
		C3D_API explicit Particle( BufferDeclaration const & p_description );
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\param[in]	p_rhs	The object to copy.
		 *\~french
		 *\brief		Constructeur par copie.
		 *\param[in]	p_rhs	L'objet à copier.
		 */
		C3D_API Particle( Particle const & p_rhs );
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\param[in]	p_rhs	The object to move.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 *\param[in]	p_rhs	L'objet à déplacer.
		 */
		C3D_API Particle( Particle && p_rhs );
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\param[in]	p_rhs	The object to copy.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 *\param[in]	p_rhs	L'objet à copier.
		 */
		C3D_API Particle & operator=( Particle const & p_rhs );
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\param[in]	p_rhs	The object to move.
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement.
		 *\param[in]	p_rhs	L'objet à déplacer.
		 */
		C3D_API Particle & operator=( Particle && p_rhs );
		/**
		 *\~english
		 *\brief		Sets the particle variable's value at given index.
		 *\param[in]	p_index	The variable index.
		 *\param[in]	p_value	The variable value.
		 *\~french
		 *\brief		Définit la valeur de la variable de particule à l'index donné.
		 *\param[in]	p_index	L'index de la variable.
		 *\param[in]	p_value	La valeur de la variable.
		 */
		template< renderer::AttributeFormat Type >
		inline void setValue( uint32_t p_index, typename ElementTyper< Type >::Type const & p_value );
		/**
		 *\~english
		 *\brief		Sets the particle variable's value at given index.
		 *\param[in]	p_index	The variable index.
		 *\return		The variable value.
		 *\~french
		 *\brief		Définit la valeur de la variable de particule à l'index donné.
		 *\param[in]	p_index	L'index de la variable.
		 *\return		La valeur de la variable.
		 */
		template< renderer::AttributeFormat Type >
		inline typename ElementTyper< Type >::Type getValue( uint32_t p_index )const;
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
		BufferDeclaration const & m_description;
		//!\~english	The particle's data.
		//!\~french		Les données de la particule.
		std::vector< uint8_t > m_data;
	};
}

#include "Particle.inl"

#endif
