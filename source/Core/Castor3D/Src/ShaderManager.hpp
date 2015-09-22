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
#ifndef ___C3D_SHADER_MANAGER_H___
#define ___C3D_SHADER_MANAGER_H___

#include "Castor3DPrerequisites.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		Manager used to hold the shader programs. Holds it, destroys it during a rendering loop
	\~french
	\brief		Manager utilisé pour garder les programmes de shaders. Il les garde et permet leur destruction au cours d'une boucle de rendu
	*/
	class C3D_API ShaderManager
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		ShaderManager();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~ShaderManager();
		/**
		 *\~english
		 *\brief		Destroys all the shaders of the array of shaders to destroy
		 *\~french
		 *\brief		Détruit tous les shaders du tableau de shaders à détruire
		 */
		void Update();
		/**
		 *\~english
		 *\brief		Flushes the active shaders list and adds all it's shaders to the array of shaders to destroy
		 *\remarks		Calls Update
		 *\~french
		 *\brief		Vide la liste des shaders actifs dans celle des shaders à détruire
		 *\remarks		Appelle Update
		 */
		void Clear();
		/**
		 *\~english
		 *\brief		Flushes the active shaders list and adds all it's shaders to the array of shaders to destroy
		 *\~french
		 *\brief		Vide la liste des shaders actifs dans celle des shaders à détruire
		 */
		void ClearShaders();
		/**
		 *\~english
		 *\brief		Creates a new program
		 *\param[in]	p_eLanguage	The wanted shader language
		 *\return		The newly created program
		 *\~french
		 *\brief		Crée un nouveau programme
		 *\param[in]	p_eLanguage	Le langage de shader souhaité
		 *\return		Le programme créé
		 */
		ShaderProgramBaseSPtr GetNewProgram( eSHADER_LANGUAGE p_eLanguage = eSHADER_LANGUAGE_AUTO );
		/**
		 *\~english
		 *\brief		Looks for an automatically generated program corresponding to given flags
		 *\remarks		If none exists it is created
		 *\param[in]	p_technique			The technique from witch the program code is retrieved
		 *\param[in]	p_uiTextureFlags	Bitwise ORed eTEXTURE_CHANNEL
		 *\param[in]	p_uiProgramFlags	Bitwise ORed ePROGRAM_FLAG
		 *\return		The found or created program
		 *\~french
		 *\brief		Cherche un programme automatiquement généré correspondant aux flags donnés
		 *\param[in]	p_technique			La technique a pqrtir de laquelle est recuperee le code du programme
		 *\param[in]	p_uiTextureFlags	Une combinaison de eTEXTURE_CHANNEL
		 *\param[in]	p_uiProgramFlags	Une combinaison de ePROGRAM_FLAG
		 *\return		Le programme trouvé ou créé
		 */
		ShaderProgramBaseSPtr GetAutomaticProgram( RenderTechniqueBase const & p_technique, uint32_t p_uiTextureFlags, uint32_t p_uiProgramFlags );
		/**
		 *\~english
		 *\brief		Creates a matrix frame variable buffer
		 *\param[in]	p_program		The programme to which the buffer is bound
		 *\param[in]	p_shaderMask	MASK_SHADER_TYPE combination, to set at what shaders it is to be bound
		 *\return		The frame variable buffer
		 *\~french
		 *\brief		Crée un frame variable buffer pour les matrices
		 *\param[in]	p_program		Le programme auquel le buffer est lié
		 *\param[in]	p_shaderMask	Combinaison de MASK_SHADER_TYPE, pour déterminer les shaders auxquels il doit être lié
		 *\return		Le frame variable buffer
		 */
		FrameVariableBufferSPtr CreateMatrixBuffer( ShaderProgramBase & p_program, uint32_t p_shaderMask );
		/**
		 *\~english
		 *\brief		Creates a scene frame variable buffer
		 *\param[in]	p_program		The program to which the buffer is bound
		 *\param[in]	p_shaderMask	MASK_SHADER_TYPE combination, to set at what shaders it is to be bound
		 *\return		The frame variable buffer
		 *\~french
		 *\brief		Crée un frame variable buffer pour les données de scène
		 *\param[in]	p_program		Le programme auquel le buffer est lié
		 *\param[in]	p_shaderMask	Combinaison de MASK_SHADER_TYPE, pour déterminer les shaders auxquels il doit être lié
		 *\return		Le frame variable buffer
		 */
		FrameVariableBufferSPtr CreateSceneBuffer( ShaderProgramBase & p_program, uint32_t p_shaderMask );
		/**
		 *\~english
		 *\brief		Creates a pass frame variable buffer
		 *\param[in]	p_program		The program to which the buffer is bound
		 *\param[in]	p_shaderMask	MASK_SHADER_TYPE combination, to set at what shaders it is to be bound
		 *\return		The frame variable buffer
		 *\~french
		 *\brief		Crée un frame variable buffer pour les données de passe
		 *\param[in]	p_program		Le programme auquel le buffer est lié
		 *\param[in]	p_shaderMask	Combinaison de MASK_SHADER_TYPE, pour déterminer les shaders auxquels il doit être lié
		 *\return		Le frame variable buffer
		 */
		FrameVariableBufferSPtr CreatePassBuffer( ShaderProgramBase & p_program, uint32_t p_shaderMask );
		/**
		 *\~english
		 *\brief		Creates the textures related frame variables
		 *\param[in]	p_uiTextureFlags	Bitwise ORed eTEXTURE_CHANNEL
		 *\param[in]	p_program			The program to which the buffer is bound
		 *\~french
		 *\brief		Crée les frame variables relatives aux textures
		 *\param[in]	p_uiTextureFlags	Une combinaison de eTEXTURE_CHANNEL
		 *\param[in]	p_program			Le programme auquel le buffer est lié
		 */
		void CreateTextureVariables( ShaderProgramBase & p_program, uint32_t p_uiTextureFlags );
		/**
		 *\~english
		 *\brief		Sets the RenderSystem
		 *\param[in]	p_pRenderSystem	The new value
		 *\~french
		 *\brief		Définit le RenderSystem
		 *\param[in]	p_pRenderSystem	La nouvelle valeur
		 */
		inline void SetRenderSystem( RenderSystem * const & p_pRenderSystem )
		{
			m_renderSystem = p_pRenderSystem;
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the beginning of the shaders list.
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur le début de la liste de shaders.
		 *\return		L4itérateur
		 */
		inline ShaderProgramPtrArray::iterator begin()
		{
			return m_arrayPrograms.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the beginning of the shaders list.
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur le début de la liste de shaders.
		 *\return		L4itérateur
		 */
		inline ShaderProgramPtrArray::const_iterator begin()const
		{
			return m_arrayPrograms.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the shaders list.
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur la fin de la liste de shaders.
		 *\return		L4itérateur
		 */
		inline ShaderProgramPtrArray::iterator end()
		{
			return m_arrayPrograms.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the shaders list.
		 *\return		The iterator
		 *\~french
		 *\brief		Récupère un itérateur sur la fin de la liste de shaders.
		 *\return		L4itérateur
		 */
		inline ShaderProgramPtrArray::const_iterator end()const
		{
			return m_arrayPrograms.end();
		}

	private:
		DECLARE_MAP( uint64_t, ShaderProgramBaseWPtr, ShaderProgramWPtrUInt64 );
		//!\~english The loaded shader programs	\~french Les programmes chargés
		ShaderProgramPtrArray m_arrayPrograms;
		//!\~english Automatically generated shader programs, sorted by texture flags	\~french Programmes auto-générés, triés par flags de texture
		ShaderProgramWPtrUInt64Map m_mapAutogenerated;
		//!\~english The maximal number of vertices the geometry shader can output	\~french Nombre maximale de vertex qu'un shader géométrique peut sortir
		int m_nbVerticesOut;
		//!\~english The RenderSystem used to create programs	\~french Le RenderSystem utilisé pour la création des programmes
		RenderSystem * m_renderSystem;
	};
}

#pragma warning( pop )

#endif
