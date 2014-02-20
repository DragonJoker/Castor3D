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
#ifndef ___Castor_Memory___
#	define ___Castor_Memory___

#	include "MemoryImpl.hpp"

#	if CHECK_MEMORYLEAKS && (!defined( VLD_AVAILABLE ) || !USE_VLD)
/**
 *\~english
 *\brief		new operator
 *\remark		Uses custom allocation for memory leaks detection
 *\param[in]	p_size			The allocation size
 *\param[in]	p_szFile		The file name
 *\param[in]	p_szFunction	The function name
 *\param[in]	p_uiLine		The line number
 *\return		The allocated pointer
 *\~french
 *\brief		Opérateur new
 *\remark		Utilise l'allocation perso pour la détection des fuites de mémoire
 *\param[in]	p_size	La taille d'allocation
 *\return		Le pointeur alloué
 */
inline void * operator new( std::size_t p_size, char const * p_szFile, char const * p_szFunction, uint32_t p_uiLine )
{
	return Castor::TracedAllocate( p_size, p_szFile, p_szFunction, p_uiLine, false );
}
/**
 *\~english
 *\brief		new operator, no throw version
 *\remark		Uses custom allocation for memory leaks detection
 *\param[in]	p_size			The allocation size
 *\param[in]	p_szFile		The file name
 *\param[in]	p_szFunction	The function name
 *\param[in]	p_uiLine		The line number
 *\return		The allocated pointer
 *\~french
 *\brief		Opérateur new, version no throw
 *\remark		Utilise l'allocation perso pour la détection des fuites de mémoire
 *\param[in]	p_size			La taille d'allocation
 *\param[in]	p_szFile		Le nom du fichier
 *\param[in]	p_szFunction	Le nom de la fonction
 *\param[in]	p_uiLine		Le numéro de ligne
 *\return		Le pointeur alloué
 */
inline void * operator new( std::size_t p_size, char const * p_szFile, char const * p_szFunction, uint32_t p_uiLine, std::nothrow_t const & )throw()
{
	return Castor::TracedAllocate( p_size, p_szFile, p_szFunction, p_uiLine, false, std::nothrow );
}
/**
 *\~english
 *\brief		Array new operator
 *\remark		Uses custom allocation for memory leaks detection
 *\param[in]	p_size			The allocation size
 *\param[in]	p_szFile		The file name
 *\param[in]	p_szFunction	The function name
 *\param[in]	p_uiLine		The line number
 *\return		The allocated pointer
 *\~french
 *\brief		Opérateur new tableau
 *\remark		Utilise l'allocation perso pour la détection des fuites de mémoire
 *\param[in]	p_size			La taille d'allocation
 *\param[in]	p_szFile		Le nom du fichier
 *\param[in]	p_szFunction	Le nom de la fonction
 *\param[in]	p_uiLine		Le numéro de ligne
 *\return		Le pointeur alloué
 */
inline void * operator new[]( std::size_t p_size, char const * p_szFile, char const * p_szFunction, uint32_t p_uiLine )
{
	return Castor::TracedAllocate( p_size, p_szFile, p_szFunction, p_uiLine, true );
}
/**
 *\~english
 *\brief		Array new operator, no throw version
 *\remark		Uses custom allocation for memory leaks detection
 *\param[in]	p_size			The allocation size
 *\param[in]	p_szFile		The file name
 *\param[in]	p_szFunction	The function name
 *\param[in]	p_uiLine		The line number
 *\return		The allocated pointer
 *\~french
 *\brief		Opérateur new tableau, version no throw
 *\remark		Utilise l'allocation perso pour la détection des fuites de mémoire
 *\param[in]	p_size			La taille d'allocation
 *\param[in]	p_szFile		Le nom du fichier
 *\param[in]	p_szFunction	Le nom de la fonction
 *\param[in]	p_uiLine		Le numéro de ligne
 *\return		Le pointeur alloué
 */
inline void * operator new[]( std::size_t p_size, char const * p_szFile, char const * p_szFunction, uint32_t p_uiLine, std::nothrow_t const & )throw()
{
	return Castor::TracedAllocate( p_size, p_szFile, p_szFunction, p_uiLine, true, std::nothrow );
}
/**
 *\~english
 *\brief		new operator
 *\remark		Uses custom allocation for memory leaks detection
 *\param[in]	p_size			The allocation size
 *\return		The allocated pointer
 *\~french
 *\brief		Opérateur new
 *\remark		Utilise l'allocation perso pour la détection des fuites de mémoire
 *\param[in]	p_size	La taille d'allocation
 *\return		Le pointeur alloué
 */
inline void * operator new( std::size_t p_size )
{
	return Castor::TracedAllocate( p_size, NULL, NULL, 0, false );
}
/**
 *\~english
 *\brief		new operator, no throw version
 *\remark		Uses custom allocation for memory leaks detection
 *\param[in]	p_size			The allocation size
 *\return		The allocated pointer
 *\~french
 *\brief		Opérateur new, version no throw
 *\remark		Utilise l'allocation perso pour la détection des fuites de mémoire
 *\param[in]	p_size			La taille d'allocation
 *\return		Le pointeur alloué
 */
inline void * operator new( std::size_t p_size, std::nothrow_t const & )throw()
{
	return Castor::TracedAllocate( p_size, NULL, NULL, 0, false, std::nothrow );
}
/**
 *\~english
 *\brief		Array new operator
 *\remark		Uses custom allocation for memory leaks detection
 *\param[in]	p_size			The allocation size
 *\return		The allocated pointer
 *\~french
 *\brief		Opérateur new tableau
 *\remark		Utilise l'allocation perso pour la détection des fuites de mémoire
 *\param[in]	p_size			La taille d'allocation
 *\return		Le pointeur alloué
 */
inline void * operator new[]( std::size_t p_size )
{
	return Castor::TracedAllocate( p_size, NULL, NULL, 0, true );
}
/**
 *\~english
 *\brief		Array new operator, no throw version
 *\remark		Uses custom allocation for memory leaks detection
 *\param[in]	p_size			The allocation size
 *\return		The allocated pointer
 *\~french
 *\brief		Opérateur new tableau, version no throw
 *\remark		Utilise l'allocation perso pour la détection des fuites de mémoire
 *\param[in]	p_size			La taille d'allocation
 *\return		Le pointeur alloué
 */
inline void * operator new[]( std::size_t p_size, std::nothrow_t const & )throw()
{
	return Castor::TracedAllocate( p_size, NULL, NULL, 0, true, std::nothrow );
}
/**
 *\~english
 *\brief		delete operator
 *\param[in]	p_pointer		The pointer
 *\param[in]	p_szFile		The file name
 *\param[in]	p_szFunction	The function name
 *\param[in]	p_uiLine		The line number
 *\~french
 *\brief		Opérateur delete
 *\param[in]	p_size	La taille d'allocation
 *\param[in]	p_size			Le pointeur
 *\param[in]	p_szFile		Le nom du fichier
 *\param[in]	p_szFunction	Le nom de la fonction
 *\param[in]	p_uiLine		Le numéro de ligne
 */
inline void operator delete( void * p_pointer, char const * p_szFile, char const * p_szFunction, uint32_t p_uiLine )
{
	return Castor::TracedDeallocate( p_pointer, p_szFile, p_szFunction, p_uiLine, false );
}
/**
 *\~english
 *\brief		delete operator, no throw version
 *\param[in]	p_size			The pointer
 *\param[in]	p_szFile		The file name
 *\param[in]	p_szFunction	The function name
 *\param[in]	p_uiLine		The line number
 *\~french
 *\brief		Opérateur delete, version no throw
 *\param[in]	p_size			Le pointeur
 *\param[in]	p_szFile		Le nom du fichier
 *\param[in]	p_szFunction	Le nom de la fonction
 *\param[in]	p_uiLine		Le numéro de ligne
 */
inline void operator delete( void * p_pointer, char const * p_szFile, char const * p_szFunction, uint32_t p_uiLine, std::nothrow_t const & )throw()
{
	return Castor::TracedDeallocate( p_pointer, p_szFile, p_szFunction, p_uiLine, false, std::nothrow );
}
/**
 *\~english
 *\brief		Array delete operator
 *\param[in]	p_size			The pointer
 *\param[in]	p_szFile		The file name
 *\param[in]	p_szFunction	The function name
 *\param[in]	p_uiLine		The line number
 *\~french
 *\brief		Opérateur delete tableau
 *\param[in]	p_size			Le pointeur
 *\param[in]	p_szFile		Le nom du fichier
 *\param[in]	p_szFunction	Le nom de la fonction
 *\param[in]	p_uiLine		Le numéro de ligne
 */
inline void operator delete[]( void * p_pointer, char const * p_szFile, char const * p_szFunction, uint32_t p_uiLine )
{
	return Castor::TracedDeallocate( p_pointer, p_szFile, p_szFunction, p_uiLine, true );
}
/**
 *\~english
 *\brief		Array delete operator, no throw version
 *\param[in]	p_size			The pointer
 *\param[in]	p_szFile		The file name
 *\param[in]	p_szFunction	The function name
 *\param[in]	p_uiLine		The line number
 *\~french
 *\brief		Opérateur delete tableau, version no throw
 *\param[in]	p_size			Le pointeur
 *\param[in]	p_szFile		Le nom du fichier
 *\param[in]	p_szFunction	Le nom de la fonction
 *\param[in]	p_uiLine		Le numéro de ligne
 */
inline void operator delete[]( void * p_pointer, char const * p_szFile, char const * p_szFunction, uint32_t p_uiLine, std::nothrow_t const & )throw()
{
	return Castor::TracedDeallocate( p_pointer, p_szFile, p_szFunction, p_uiLine, true, std::nothrow );
}

#		define new			new( __FILE__, __FUNCTION__, __LINE__ )
#		define delete		Castor::MemoryManager::GetSingleton() >> 
#		define delete_array	Castor::MemoryManager::GetSingleton() - 
#	else
#		define delete_array	delete [] 
#	endif

#endif	//___Castor_Memory___
