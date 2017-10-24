/*
See LICENSE file in root folder
*/
#ifndef ___C3D_UniformBufferBinding_H___
#define ___C3D_UniformBufferBinding_H___

#include "Castor3DPrerequisites.hpp"

#include <Design/OwnedBy.hpp>

namespace castor3d
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
		: public castor::OwnedBy< UniformBuffer >
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
		C3D_API void bind( uint32_t p_index )const;
		/**
		 *\~english
		 *\return		The UBO size.
		 *\~french
		 *\return		La taille de l'UBO.
		 */
		inline uint32_t getSize()const
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
		C3D_API virtual void doBind( uint32_t p_index )const = 0;

	public:
		struct UniformInfo
		{
			//!\~english	The variable name.
			//!\~french		Le nom de la variable.
			castor::String m_name;
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
