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
#ifndef ___C3D_UniformBufferBinding_H___
#define ___C3D_UniformBufferBinding_H___

#include "Castor3DPrerequisites.hpp"

#include <Design/OwnedBy.hpp>

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		14/12/2016
	\~english
	\brief		Represents the binding between a uniform buffer and a program.
	\~french
	\brief		Représente la liaison entre un tampon d'uniformes et un programme.
	*/
	class UniformBufferBinding
		: public Castor::OwnedBy< UniformBuffer >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_ubo		The parent uniform buffer.
		 *\param[in]	p_program	The parent program.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_ubo		le tampon d'uniformes parent.
		 *\param[in]	p_program	Le programme parent.
		 */
		C3D_API UniformBufferBinding( UniformBuffer & p_ubo, ShaderProgram const & p_program );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~UniformBufferBinding();
		/**
		 *\~english
		 *\brief		Binds the uniform buffer to the program.
		 *\param[in]	p_index	The binding point.
		 *\~french
		 *\brief		Lie le tampon d'uniformes au programme.
		 *\param[in]	p_index	Le point d'attache.
		 */
		C3D_API void Bind( uint32_t p_index )const;
		/**
		 *\~english
		 *\return		The UBO size.
		 *\~french
		 *\return		La taille de l'UBO.
		 */
		inline uint32_t GetSize()const
		{
			return m_size;
		}
		/**
		 *\~english
		 *\return		The iterator to the beginnning of the variables list.
		 *\~french
		 *\return		L'itérateur sur le début de la liste de variables.
		 */
		inline auto begin()
		{
			return m_variables.begin();
		}
		/**
		 *\~english
		 *\return		The iterator to the beginnning of the variables list.
		 *\~french
		 *\return		L'itérateur sur le début de la liste de variables.
		 */
		inline auto begin()const
		{
			return m_variables.begin();
		}
		/**
		 *\~english
		 *\return		The iterator to the end of the variables list.
		 *\~french
		 *\return		L'itérateur sur la fin de la liste de variables.
		 */
		inline auto end()
		{
			return m_variables.end();
		}
		/**
		 *\~english
		 *\return		The iterator to the end of the variables list.
		 *\~french
		 *\return		L'itérateur sur la fin de la liste de variables.
		 */
		inline auto end()const
		{
			return m_variables.end();
		}

	private:
		/**
		 *\~english
		 *\brief		Binds the uniform buffer to the program.
		 *\param[in]	p_index	The binding point.
		 *\~french
		 *\brief		Lie le tampon d'uniformes au programme.
		 *\param[in]	p_index	Le point d'attache.
		 */
		C3D_API virtual void DoBind( uint32_t p_index )const = 0;

	public:
		struct UniformInfo
		{
			//!\~english	The variable name.
			//!\~french		Le nom de la variable.
			Castor::String m_name;
			//!\~english	The variable offset into the buffer.
			//!\~french		L'offset de la variable dans le tampon.
			uint32_t m_offset;
			//!\~english	The variable stride, for array ones.
			//!\~french		Le stride de la variable, pour les tableaux.
			uint32_t m_stride;
		};
		using UniformInfoArray = std::vector< UniformInfo >;

	protected:
		//!\~english	The parent program.
		//!\~french		Le programme parent.
		ShaderProgram const & m_program;
		//!\~english	The UBO size.
		//!\~french		La taille de l'UBO.
		uint32_t m_size{ 0u };
		//!\~english	The uniform variables informations.
		//!\~french		Les informations des variables uniformes.
		std::vector< UniformInfo > m_variables;
	};
}

#endif
