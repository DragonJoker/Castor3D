/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShaderBuffer_H___
#define ___C3D_ShaderBuffer_H___

#include "Castor3DPrerequisites.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Wrapper class to select between SSBO or TBO.
	\remarks	Allows to user either one or the other in the same way.
	\~french
	\brief		Classe permettant de choisir entre SSBO et TBO.
	\remarks	Permet d'utiliser l'un comme l'autre de la même manière.
	*/
	class ShaderBuffer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	size	The buffer size.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	size	La taille du tampon.
		 */
		C3D_API ShaderBuffer( Engine & engine
			, uint32_t size );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ShaderBuffer();
		/**
		 *\~english
		 *\brief		Updates the buffer.
		 *\~french
		 *\brief		Met à jour le tampon.
		 */
		C3D_API void update();
		/**
		 *\~english
		 *\brief		Creates the descriptor set layout binding at given point.
		 *\param[in]	index	The binding point index.
		 *\~french
		 *\brief		Crée une attache de layout de set de descripteurs au point donné.
		 *\param[in]	index	L'indice du point d'attache.
		 */
		C3D_API renderer::DescriptorSetLayoutBinding createBinding( uint32_t index = 0u )const;
		/**
		 *\~english
		 *\return		The pointer to the buffer.
		 *\~french
		 *\brief		Le pointeur sur le tampon.
		 */
		inline uint8_t * getPtr()
		{
			return m_data.data();
		}
		/**
		 *\~english
		 *\return		The buffer size.
		 *\~french
		 *\brief		La taille du tampon.
		 */
		inline uint32_t getSize()
		{
			return uint32_t( m_data.size() );
		}

	protected:
		//!\~english	The SSBO, or TBO.
		//!\~french		Le SSBO, ou TBO.
		renderer::BufferBasePtr m_buffer;
		//!\~english	The storage data.
		//!\~french		Les données du stockage.
		renderer::ByteArray m_data;
	};
}

#endif
