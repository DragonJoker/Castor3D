/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_GPU_BUFFER_H___
#define ___C3D_GPU_BUFFER_H___

#include "Castor3DPrerequisites.hpp"

#include <cstddef>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.5.0
	\date		22/10/2011
	\~english
	\brief		Base class for renderer dependant buffers
	\~french
	\brief		Classe de base pour les tampons d�pendants du renderer
	\remark
	*/
	template< typename T >
	class GpuBuffer
	{
	protected:
		typedef Castor3D::CpuBuffer< T > * HardwareBufferPtr;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		GpuBuffer() {}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~GpuBuffer() {}
		/**
		 *\~english
		 *\brief		Creation function
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction de cr�ation
		 *\return		\p true si tout s'est bien pass�
		 */
		virtual bool Create()
		{
			return false;
		}
		/**
		 *\~english
		 *\brief		Destruction function
		 *\~french
		 *\brief		Fonction de destruction
		 */
		virtual void Destroy() {}
		/**
		 *\~english
		 *\brief		Initialisation function, used by VBOs
		 *\param[in]	p_eType		Buffer access type
		 *\param[in]	p_eNature	Buffer access nature
		 *\param[in]	p_pProgram	The shader program
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction d'initialisation, utilis�e par les VBOs
		 *\param[in]	p_eType		Type d'acc�s du tampon
		 *\param[in]	p_eNature	Nature d'acc�s du tampon
		 *\param[in]	p_pProgram	Le programme de shader
		 *\return		\p true si tout s'est bien pass�
		 */
		virtual bool Initialise( eBUFFER_ACCESS_TYPE p_eType, eBUFFER_ACCESS_NATURE p_eNature, Castor3D::ShaderProgramBaseSPtr p_pProgram = nullptr ) = 0;
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		virtual void Cleanup() {}
		/**
		 *\~english
		 *\brief		Locks the buffer, id est maps it into memory so we can modify it
		 *\remark		Maps from buffer[p_uiOffset*sizeof( T )] to buffer[(p_uiOffset+p_uiSize-1)*sizeof( T )]
		 *\param[in]	p_uiOffset	The start offset in the buffer
		 *\param[in]	p_uiCount	The mapped elements count
		 *\param[in]	p_uiFlags	The lock flags
		 *\return		The mapped buffer address
		 *\~french
		 *\brief		Locke le tampon, c�d le mappe en m�moire ram afin d'y autoriser des modifications
		 *\remark		Mappe de tampon[p_uiOffset*sizeof( T )] � tampon[(p_uiOffset+p_uiSize-1) * sizeof( T )]
		 *\param[in]	p_uiOffset	L'offset de d�part
		 *\param[in]	p_uiCount	Le nombre d'�l�ments � mapper
		 *\param[in]	p_uiFlags	Les flags de lock
		 *\return		L'adresse du tampon mapp�
		 */
		virtual T * Lock( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags ) = 0;
		/**
		 *\~english
		 *\brief		Unlocks the buffer, id est unmaps it from memory so no modification can be made after that
		 *\remark		All modifications made in the mapped buffer are put into GPU memory
		 *\~french
		 *\brief		Un locke le tampon, c�d l'unmappe de la m�moire ram afin de ne plus autoriser de modifications dessus
		 *\remark		Toutes les modifications qui avaient �t� effectu�es sur le tampon mapp� sont rapatri�es dans la m�moire GPU
		 */
		virtual void Unlock() = 0;
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active
		 *\return		\p true if successful
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activ�
		 *\return		\p true si tout s'est bien pass�
		 */
		virtual bool Bind() = 0;
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active
		 *\remark		Used for instanciation
		 *\param[in]	p_uiCount	Instances count
		 *\return		\p true if successful
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activ�
		 *\remark		Utilis� pour l'instanciation
		 *\param[in]	p_uiCount	Nombre d'instances
		 *\return		\p true si tout s'est bien pass�
		 */
		virtual bool Bind( uint32_t p_uiCount )
		{
			return false;
		}
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive
		 *\~french
		 *\brief		Fonction de d�sactivation, pour dire au GPU qu'il est d�sactiv�
		 */
		virtual void Unbind() = 0;
		/**
		 *\~english
		 *\brief		Transmits data to the GPU buffer from RAM
		 *\param[in]	p_pBuffer	The data
		 *\param[in]	p_iSize		Data buffer size
		 *\param[in]	p_eType		Transfer type
		 *\param[in]	p_eNature	Transfer nature
		 *\return		\p true if successful
		 *\~french
		 *\brief		Transf�re des donn�es au tampon GPU � partir de la ram
		 *\param[in]	p_pBuffer	Les donn�es
		 *\param[in]	p_iSize		Taille du tampon de donn�es
		 *\param[in]	p_eType		Type de transfert
		 *\param[in]	p_eNature	Nature du transfert
		 *\return		\p true si tout s'est bien pass�
		 */
		virtual bool Fill( T const * p_pBuffer, ptrdiff_t p_iSize, Castor3D::eBUFFER_ACCESS_TYPE p_eType, Castor3D::eBUFFER_ACCESS_NATURE p_eNature ) = 0;
	};
}

#endif
