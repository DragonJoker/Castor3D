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
#ifndef ___C3D_SHADER_CACHE_H___
#define ___C3D_SHADER_CACHE_H___

#include "Castor3DPrerequisites.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		Cache used to hold the shader programs. Holds it, destroys it during a rendering loop
	\~french
	\brief		Cache utilisé pour garder les programmes de shaders. Il les garde et permet leur destruction au cours d'une boucle de rendu
	*/
	class ShaderProgramCache
		: public Castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_engine	The engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_engine	Le moteur
		 */
		C3D_API explicit ShaderProgramCache( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~ShaderProgramCache();
		/**
		 *\~english
		 *\brief		Destroys all the shaders of the array of shaders to destroy
		 *\~french
		 *\brief		Détruit tous les shaders du tableau de shaders à détruire
		 */
		C3D_API void Clear();
		/**
		 *\~english
		 *\brief		Cleans up all the shaders.
		 *\~french
		 *\brief		Nettoie tous les shaders.
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Creates a new program
		 *\param[in]	p_langage	The wanted shader language
		 *\return		The newly created program
		 *\~french
		 *\brief		Crée un nouveau programme
		 *\param[in]	p_langage	Le langage de shader souhaité
		 *\return		Le programme créé
		 */
		C3D_API ShaderProgramSPtr GetNewProgram();
		/**
		 *\~english
		 *\brief		Looks for an automatically generated program corresponding to given flags.
		 *\remarks		If none exists it is created.
		 *\param[in]	p_technique		The technique from witch the program code is retrieved.
		 *\param[in]	p_textureFlags	Bitwise ORed TextureChannel.
		 *\param[in]	p_programFlags	Bitwise ORed ePROGRAM_FLAG.
		 *\return		The found or created program.
		 *\~french
		 *\brief		Cherche un programme automatiquement généré correspondant aux flags donnés.
		 *\param[in]	p_technique		La technique a pqrtir de laquelle est recuperee le code du programme.
		 *\param[in]	p_textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	p_programFlags	Une combinaison de ePROGRAM_FLAG.
		 *\return		Le programme trouvé ou créé.
		 */
		C3D_API ShaderProgramSPtr GetAutomaticProgram( RenderTechnique const & p_technique, uint32_t p_textureFlags, uint32_t p_programFlags );
		/**
		 *\~english
		 *\brief		Looks for a billboards shader program corresponding to given flags.
		 *\param[in]	p_textureFlags	Bitwise ORed TextureChannel.
		 *\param[in]	p_programFlags	Bitwise ORed ePROGRAM_FLAG.
		 *\return		The found program.
		 *\~french
		 *\brief		Cherche un programme de billboards correspondant aux flags donnés.
		 *\param[in]	p_textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	p_programFlags	Une combinaison de ePROGRAM_FLAG.
		 *\return		Le programme trouvé.
		 */
		C3D_API ShaderProgramSPtr GetBillboardProgram( uint32_t p_textureFlags, uint32_t p_programFlags )const;
		/**
		 *\~english
		 *\brief		Adds a billboards shader program corresponding to given flags.
		 *\param[in]	p_program		The program to add.
		 *\param[in]	p_textureFlags	Bitwise ORed TextureChannel.
		 *\param[in]	p_programFlags	Bitwise ORed ProgramFlag.
		 *\return		The found program.
		 *\~french
		 *\brief		Ajoute un programme de billboards correspondant aux flags donnés.
		 *\param[in]	p_program		Le programme à ajouter.
		 *\param[in]	p_textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	p_programFlags	Une combinaison de ProgramFlag.
		 *\return		Le programme trouvé.
		 */
		C3D_API void AddBillboardProgram( ShaderProgramSPtr p_program, uint32_t p_textureFlags, uint32_t p_programFlags );
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
		C3D_API FrameVariableBufferSPtr CreateMatrixBuffer( ShaderProgram & p_program, uint32_t p_shaderMask );
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
		C3D_API FrameVariableBufferSPtr CreateSceneBuffer( ShaderProgram & p_program, uint32_t p_shaderMask );
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
		C3D_API FrameVariableBufferSPtr CreatePassBuffer( ShaderProgram & p_program, uint32_t p_shaderMask );
		/**
		 *\~english
		 *\brief		Creates an animation frame variable buffer
		 *\param[in]	p_program		The program to which the buffer is bound
		 *\param[in]	p_programFlags	Bitwise ORed ProgramFlag.
		 *\param[in]	p_shaderMask	MASK_SHADER_TYPE combination, to set at what shaders it is to be bound
		 *\return		The frame variable buffer
		 *\~french
		 *\brief		Crée un frame variable buffer pour les données d'animation
		 *\param[in]	p_program		Le programme auquel le buffer est lié
		 *\param[in]	p_programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	p_shaderMask	Combinaison de MASK_SHADER_TYPE, pour déterminer les shaders auxquels il doit être lié
		 *\return		Le frame variable buffer
		 */
		C3D_API FrameVariableBufferSPtr CreateAnimationBuffer( ShaderProgram & p_program, uint32_t p_programFlags, uint32_t p_shaderMask );
		/**
		 *\~english
		 *\brief		Creates the textures related frame variables
		 *\param[in]	p_uiTextureFlags	Bitwise ORed TextureChannel
		 *\param[in]	p_program			The program to which the buffer is bound
		 *\~french
		 *\brief		Crée les frame variables relatives aux textures
		 *\param[in]	p_uiTextureFlags	Une combinaison de TextureChannel
		 *\param[in]	p_program			Le programme auquel le buffer est lié
		 */
		C3D_API void CreateTextureVariables( ShaderProgram & p_program, uint32_t p_uiTextureFlags );
		/**
		 *\~english
		 *\brief		Locks the collection mutex
		 *\~french
		 *\brief		Locke le mutex de la collection
		 */
		inline void lock()const
		{
			m_mutex.lock();
		}
		/**
		 *\~english
		 *\brief		Unlocks the collection mutex
		 *\~french
		 *\brief		Délocke le mutex de la collection
		 */
		inline void unlock()const
		{
			m_mutex.unlock();
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
		DECLARE_MAP( uint64_t, ShaderProgramWPtr, ShaderProgramWPtrUInt64 );
		mutable std::recursive_mutex m_mutex;
		//!\~english The loaded shader programs	\~french Les programmes chargés
		ShaderProgramPtrArray m_arrayPrograms;
		//!\~english Automatically generated shader programs, sorted by texture flags	\~french Programmes auto-générés, triés par flags de texture
		ShaderProgramWPtrUInt64Map m_mapAutogenerated;
		//!\~english Billboards shader programs, sorted by texture flags.	\~french Programmes shader pour billboards, triés par flags de texture.
		ShaderProgramWPtrUInt64Map m_mapBillboards;
		//!\~english The maximal number of vertices the geometry shader can output	\~french Nombre maximale de vertex qu'un shader géométrique peut sortir
		int m_nbVerticesOut{ 0 };
	};
	/**
	 *\~english
	 *\brief		Creates a ShaderProgram cache.
	 *\param[in]	p_engine	The engine.
	 *\~french
	 *\brief		Crée un cache de ShaderProgram.
	 *\param[in]	p_engine	Le moteur.
	 */
	inline std::unique_ptr< ShaderProgramCache >
	MakeCache( Engine & p_engine )
	{
		return std::make_unique< ShaderProgramCache >( p_engine );
	}
}

#endif
