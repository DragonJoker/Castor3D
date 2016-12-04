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
		 *\brief		Creates a new program.
		 *\param[in]	p_initialise	Tells if we want the program to be initialised.
		 *\return		The newly created program.
		 *\~french
		 *\brief		Crée un nouveau programme.
		 *\param[in]	p_initialise	Dit si on veut que le programme soit initialisé.
		 *\return		Le programme créé.
		 */
		C3D_API ShaderProgramSPtr GetNewProgram( bool p_initialise );
		/**
		 *\~english
		 *\brief		Looks for an automatically generated program corresponding to given flags.
		 *\remarks		If none exists it is created.
		 *\param[in]	p_renderPass	The pass from which the program code is retrieved.
		 *\param[in]	p_textureFlags	Bitwise ORed TextureChannel.
		 *\param[in]	p_programFlags	Bitwise ORed ProgramFlag.
		 *\param[in]	p_sceneFlags	Scene related flags.
		 *\param[in]	p_invertNormals	Tells if the normals must be inverted, in the program.
		 *\return		The found or created program.
		 *\~french
		 *\brief		Cherche un programme automatiquement généré correspondant aux flags donnés.
		 *\param[in]	p_renderPass	La passe a partir de laquelle est récupéré le code du programme.
		 *\param[in]	p_textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	p_programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	p_sceneFlags	Les indicateurs relatifs à la scène.
		 *\param[in]	p_invertNormals	Dit si les normales doivent être inversées, dans le programme.
		 *\return		Le programme trouvé ou créé.
		 */
		C3D_API ShaderProgramSPtr GetAutomaticProgram(
			RenderPass const & p_renderPass,
			Castor::FlagCombination< TextureChannel > const & p_textureFlags,
			Castor::FlagCombination< ProgramFlag > const & p_programFlags,
			uint8_t p_sceneFlags,
			bool p_invertNormals );
		/**
		 *\~english
		 *\brief		Creates a matrix frame variable buffer.
		 *\param[in]	p_program		The programme to which the buffer is bound.
		 *\param[in]	p_programFlags	ProgramFlag combination.
		 *\param[in]	p_shaderMask	ShaderTypeFlag combination, to set at what shaders it is to be bound.
		 *\return		The frame variable buffer.
		 *\~french
		 *\brief		Crée un frame variable buffer pour les matrices.
		 *\param[in]	p_program		Le programme auquel le buffer est lié.
		 *\param[in]	p_programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	p_shaderMask	Combinaison de ShaderTypeFlag, pour déterminer les shaders auxquels il doit être lié.
		 *\return		Le frame variable buffer.
		 */
		C3D_API FrameVariableBuffer & CreateMatrixBuffer(
			ShaderProgram & p_program,
			Castor::FlagCombination< ProgramFlag > const & p_programFlags,
			Castor::FlagCombination< ShaderTypeFlag > const & p_shaderMask )const;
		/**
		 *\~english
		 *\brief		Creates a scene frame variable buffer.
		 *\param[in]	p_program		The program to which the buffer is bound.
		 *\param[in]	p_programFlags	ProgramFlag combination.
		 *\param[in]	p_shaderMask	ShaderTypeFlag combination, to set at what shaders it is to be bound.
		 *\return		The frame variable buffer.
		 *\~french
		 *\brief		Crée un frame variable buffer pour les données de scène.
		 *\param[in]	p_program		Le programme auquel le buffer est lié.
		 *\param[in]	p_programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	p_shaderMask	Combinaison de ShaderTypeFlag, pour déterminer les shaders auxquels il doit être lié.
		 *\return		Le frame variable buffer.
		 */
		C3D_API FrameVariableBuffer & CreateSceneBuffer(
			ShaderProgram & p_program,
			Castor::FlagCombination< ProgramFlag > const & p_programFlags,
			Castor::FlagCombination< ShaderTypeFlag > const & p_shaderMask )const;
		/**
		 *\~english
		 *\brief		Creates a pass frame variable buffer.
		 *\param[in]	p_program		The program to which the buffer is bound.
		 *\param[in]	p_programFlags	ProgramFlag combination.
		 *\param[in]	p_shaderMask	ShaderTypeFlag combination, to set at what shaders it is to be bound.
		 *\return		The frame variable buffer.
		 *\~french
		 *\brief		Crée un frame variable buffer pour les données de passe.
		 *\param[in]	p_program		Le programme auquel le buffer est lié.
		 *\param[in]	p_programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	p_shaderMask	Combinaison de ShaderTypeFlag, pour déterminer les shaders auxquels il doit être lié.
		 *\return		Le frame variable buffer.
		 */
		C3D_API FrameVariableBuffer & CreatePassBuffer(
			ShaderProgram & p_program,
			Castor::FlagCombination< ProgramFlag > const & p_programFlags,
			Castor::FlagCombination< ShaderTypeFlag > const & p_shaderMask )const;
		/**
		 *\~english
		 *\brief		Creates an animation frame variable buffer.
		 *\param[in]	p_program		The program to which the buffer is bound.
		 *\param[in]	p_programFlags	ProgramFlag combination.
		 *\param[in]	p_shaderMask	ShaderTypeFlag combination, to set at what shaders it is to be bound.
		 *\return		The frame variable buffer.
		 *\~french
		 *\brief		Crée un frame variable buffer pour les données d'animation.
		 *\param[in]	p_program		Le programme auquel le buffer est lié.
		 *\param[in]	p_programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	p_shaderMask	Combinaison ShaderTypeFlag, pour déterminer les shaders auxquels il doit être lié.
		 *\return		Le frame variable buffer.
		 */
		C3D_API FrameVariableBuffer & CreateAnimationBuffer(
			ShaderProgram & p_program,
			Castor::FlagCombination< ProgramFlag > const & p_programFlags,
			Castor::FlagCombination< ShaderTypeFlag > const & p_shaderMask )const;
		/**
		 *\~english
		 *\brief		Creates the textures related frame variables.
		 *\param[in]	p_textureFlags	TextureChannel combination.
		 *\param[in]	p_program		The program to which the buffer is bound.
		 *\~french
		 *\brief		Crée les frame variables relatives aux textures.
		 *\param[in]	p_textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	p_program		Le programme auquel le buffer est lié.
		 */
		C3D_API void CreateTextureVariables(
			ShaderProgram & p_program,
			Castor::FlagCombination< TextureChannel > const & p_textureFlags )const;
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
		/**
		 *\~english
		 *\brief		Adds a program to the list.
		 *\param[in]	p_initialise	Tells if we want the program to be initialised.
		 *\param[in]	p_program		The program to add.
		 *\~french
		 *\brief		Crée un nouveau programme.
		 *\param[in]	p_initialise	Dit si on veut que le programme soit initialisé.
		 *\param[in]	p_program		Le programme à ajouter.
		 */
		C3D_API void DoAddProgram( ShaderProgramSPtr p_program, bool p_initialise );
		/**
		 *\~english
		 *\brief		Looks for an automatically generated program corresponding to given flags.
		 *\remarks		If none exists it is created.
		 *\param[in]	p_renderPass	The pass from which the program code is retrieved.
		 *\param[in]	p_textureFlags	TextureChannel combination.
		 *\param[in]	p_programFlags	ProgramFlag combination.
		 *\param[in]	p_sceneFlags	Scene related flags.
		 *\param[in]	p_invertNormals	Tells if the normals must be inverted, in the program.
		 *\return		The found or created program.
		 *\~french
		 *\brief		Cherche un programme automatiquement généré correspondant aux flags donnés.
		 *\param[in]	p_renderPass	La passe a partir de laquelle est récupéré le code du programme.
		 *\param[in]	p_textureFlags	TextureChannel combination.
		 *\param[in]	p_programFlags	ProgramFlag combination.
		 *\param[in]	p_sceneFlags	Les indicateurs relatifs à la scène.
		 *\param[in]	p_invertNormals	Dit si les normales doivent être inversées, dans le programme.
		 *\return		Le programme trouvé ou créé.
		 */
		C3D_API ShaderProgramSPtr DoCreateAutomaticProgram(
			RenderPass const & p_renderPass,
			Castor::FlagCombination< TextureChannel > const & p_textureFlags,
			Castor::FlagCombination< ProgramFlag > const & p_programFlags,
			uint8_t p_sceneFlags,
			bool p_invertNormals )const;
		/**
		 *\~english
		 *\brief		Adds an automatically generated shader program corresponding to given flags.
		 *\param[in]	p_program		The program to add.
		 *\param[in]	p_textureFlags	TextureChannel combination.
		 *\param[in]	p_programFlags	ProgramFlag combination.
		 *\param[in]	p_sceneFlags	The scene flags (fog, ...).
		 *\param[in]	p_invertNormals	Tells if the normals must be inverted, in the program.
		 *\return		The found program.
		 *\~french
		 *\brief		Ajoute un programme automatiquement généré correspondant aux flags donnés.
		 *\param[in]	p_program		Le programme à ajouter.
		 *\param[in]	p_textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	p_programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	p_sceneFlags	Les indicateurs de la scène (brouillard, ...).
		 *\param[in]	p_invertNormals	Dit si les normales doivent être inversées, dans le programme.
		 *\return		Le programme trouvé.
		 */
		C3D_API void DoAddAutomaticProgram(
			ShaderProgramSPtr p_program,
			Castor::FlagCombination< TextureChannel > const & p_textureFlags,
			Castor::FlagCombination< ProgramFlag > const & p_programFlags,
			uint8_t p_sceneFlags,
			bool p_invertNormals );
		/**
		 *\~english
		 *\brief		Creates a shader program for billboards rendering use.
		 *\param[in]	p_renderPass	The pass from which the program code is retrieved.
		 *\param[in]	p_textureFlags	TextureChannel combination.
		 *\param[in]	p_programFlags	ProgramFlag combination.
		 *\param[in]	p_sceneFlags	Scene related flags.
		 *\return		The created program.
		 *\~french
		 *\brief		Crée un programme shader pour les rendu de billboards.
		 *\param[in]	p_renderPass	La passe a partir de laquelle est récupéré le code du programme.
		 *\param[in]	p_textureFlags	Combinaison de TextureChannel.
		 *\param[in]	p_programFlags	Combinaison de ProgramFlag.
		 *\param[in]	p_sceneFlags	Les indicateurs relatifs à la scène.
		 *\return		Le programme créé.
		 */
		C3D_API ShaderProgramSPtr DoCreateBillboardProgram(
			RenderPass const & p_renderPass,
			Castor::FlagCombination< TextureChannel > const & p_textureFlags,
			Castor::FlagCombination< ProgramFlag > const & p_programFlags,
			uint8_t p_sceneFlags )const;
		/**
		 *\~english
		 *\brief		Adds a billboards shader program corresponding to given flags.
		 *\param[in]	p_program		The program to add.
		 *\param[in]	p_textureFlags	TextureChannel combination.
		 *\param[in]	p_programFlags	ProgramFlag combination.
		 *\return		The found program.
		 *\~french
		 *\brief		Ajoute un programme de billboards correspondant aux flags donnés.
		 *\param[in]	p_program		Le programme à ajouter.
		 *\param[in]	p_textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	p_programFlags	Une combinaison de ProgramFlag.
		 *\return		Le programme trouvé.
		 */
		C3D_API void DoAddBillboardProgram(
			ShaderProgramSPtr p_program,
			Castor::FlagCombination< TextureChannel > const & p_textureFlags,
			Castor::FlagCombination< ProgramFlag > const & p_programFlags,
			uint8_t p_sceneFlags );

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
