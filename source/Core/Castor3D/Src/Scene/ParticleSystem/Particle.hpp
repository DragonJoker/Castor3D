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
#ifndef ___C3D_Particle_H___
#define ___C3D_Particle_H___

#include "Mesh/Buffer/BufferDeclaration.hpp"

namespace Castor3D
{
	template< ElementType Type >
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
		C3D_API Particle( BufferDeclaration const & p_description, Castor::StrStrMap const & p_defaultValues );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_description	The particle's elements description.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_description	La description des éléments de la particule.
		 */
		C3D_API Particle( BufferDeclaration const & p_description );
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
		template< ElementType Type >
		inline void SetValue( uint32_t p_index, typename ElementTyper< Type >::Type const & p_value );
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
		template< ElementType Type >
		inline typename ElementTyper< Type >::Type GetValue( uint32_t p_index )const;
		/**
		 *\~english
		 *\return		The particle data.
		 *\~french
		 *\return		Les données de la particule.
		 */
		inline uint8_t const * GetData()const
		{
			return m_data.data();
		}
		/**
		 *\~english
		 *\return		The particle data.
		 *\~french
		 *\return		Les données de la particule.
		 */
		inline uint8_t * GetData()
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
