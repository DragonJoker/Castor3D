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

namespace castor3d
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
		: public castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	engine	The engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	engine	Le moteur
		 */
		C3D_API explicit ShaderProgramCache( Engine & engine );
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
		C3D_API void clear();
		/**
		 *\~english
		 *\brief		Cleans up all the shaders.
		 *\~french
		 *\brief		Nettoie tous les shaders.
		 */
		C3D_API void cleanup();
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
		C3D_API ShaderProgramSPtr getNewProgram( bool p_initialise );
		/**
		 *\~english
		 *\brief		Looks for an automatically generated program corresponding to given flags.
		 *\remarks		If none exists it is created.
		 *\param[in]	p_renderPass	The pass from which the program code is retrieved.
		 *\param[in]	textureFlags	Bitwise ORed TextureChannel.
		 *\param[in]	programFlags	Bitwise ORed ProgramFlag.
		 *\param[in]	sceneFlags	Scene related flags.
		 *\param[in]	alphaFunc		The alpha test function.
		 *\param[in]	invertNormals	Tells if the normals must be inverted, in the program.
		 *\return		The found or created program.
		 *\~french
		 *\brief		Cherche un programme automatiquement généré correspondant aux flags donnés.
		 *\param[in]	p_renderPass	La passe a partir de laquelle est récupéré le code du programme.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags	Les indicateurs relatifs à la scène.
		 *\param[in]	alphaFunc		La fonction de test alpha.
		 *\param[in]	invertNormals	Dit si les normales doivent être inversées, dans le programme.
		 *\return		Le programme trouvé ou créé.
		 */
		C3D_API ShaderProgramSPtr getAutomaticProgram( RenderPass const & p_renderPass
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc
			, bool invertNormals );
		/**
		 *\~english
		 *\brief		Creates the textures related frame variables.
		 *\param[in]	p_program		The program to which the buffer is bound.
		 *\param[in]	textureFlags	TextureChannel combination.
		 *\param[in]	programFlags	Bitwise ORed ProgramFlag.
		 *\~french
		 *\brief		Crée les frame variables relatives aux textures.
		 *\param[in]	p_program		Le programme auquel le buffer est lié.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 */
		C3D_API void createTextureVariables(
			ShaderProgram & p_program,
			TextureChannels const & textureFlags,
			ProgramFlags const & programFlags )const;
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
		 *\brief		adds a program to the list.
		 *\param[in]	p_initialise	Tells if we want the program to be initialised.
		 *\param[in]	p_program		The program to add.
		 *\~french
		 *\brief		Crée un nouveau programme.
		 *\param[in]	p_initialise	Dit si on veut que le programme soit initialisé.
		 *\param[in]	p_program		Le programme à ajouter.
		 */
		C3D_API void doAddProgram( ShaderProgramSPtr p_program, bool p_initialise );
		/**
		 *\~english
		 *\brief		Looks for an automatically generated program corresponding to given flags.
		 *\remarks		If none exists it is created.
		 *\param[in]	p_renderPass	The pass from which the program code is retrieved.
		 *\param[in]	textureFlags	TextureChannel combination.
		 *\param[in]	programFlags	ProgramFlag combination.
		 *\param[in]	sceneFlags	Scene related flags.
		 *\param[in]	alphaFunc		The alpha test function.
		 *\param[in]	invertNormals	Tells if the normals must be inverted, in the program.
		 *\return		The found or created program.
		 *\~french
		 *\brief		Cherche un programme automatiquement généré correspondant aux flags donnés.
		 *\param[in]	p_renderPass	La passe a partir de laquelle est récupéré le code du programme.
		 *\param[in]	textureFlags	TextureChannel combination.
		 *\param[in]	programFlags	ProgramFlag combination.
		 *\param[in]	sceneFlags	Les indicateurs relatifs à la scène.
		 *\param[in]	alphaFunc		La fonction de test alpha.
		 *\param[in]	invertNormals	Dit si les normales doivent être inversées, dans le programme.
		 *\return		Le programme trouvé ou créé.
		 */
		C3D_API ShaderProgramSPtr doCreateAutomaticProgram( RenderPass const & p_renderPass
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc
			, bool invertNormals )const;
		/**
		 *\~english
		 *\brief		adds an automatically generated shader program corresponding to given flags.
		 *\param[in]	p_program		The program to add.
		 *\param[in]	textureFlags	TextureChannel combination.
		 *\param[in]	programFlags	ProgramFlag combination.
		 *\param[in]	sceneFlags	The scene flags (fog, ...).
		 *\param[in]	alphaFunc		The alpha test function.
		 *\param[in]	invertNormals	Tells if the normals must be inverted, in the program.
		 *\return		The found program.
		 *\~french
		 *\brief		Ajoute un programme automatiquement généré correspondant aux flags donnés.
		 *\param[in]	p_program		Le programme à ajouter.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags	Les indicateurs de la scène (brouillard, ...).
		 *\param[in]	alphaFunc		La fonction de test alpha.
		 *\param[in]	invertNormals	Dit si les normales doivent être inversées, dans le programme.
		 *\return		Le programme trouvé.
		 */
		C3D_API void doAddAutomaticProgram( ShaderProgramSPtr p_program
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc
			, bool invertNormals );
		/**
		 *\~english
		 *\brief		Creates a shader program for billboards rendering use.
		 *\param[in]	p_renderPass	The pass from which the program code is retrieved.
		 *\param[in]	textureFlags	TextureChannel combination.
		 *\param[in]	programFlags	ProgramFlag combination.
		 *\param[in]	sceneFlags	Scene related flags.
		 *\param[in]	alphaFunc		The alpha test function.
		 *\return		The created program.
		 *\~french
		 *\brief		Crée un programme shader pour les rendu de billboards.
		 *\param[in]	p_renderPass	La passe a partir de laquelle est récupéré le code du programme.
		 *\param[in]	textureFlags	Combinaison de TextureChannel.
		 *\param[in]	programFlags	Combinaison de ProgramFlag.
		 *\param[in]	sceneFlags	Les indicateurs relatifs à la scène.
		 *\param[in]	alphaFunc		La fonction de test alpha.
		 *\return		Le programme créé.
		 */
		C3D_API ShaderProgramSPtr doCreateBillboardProgram( RenderPass const & p_renderPass
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc )const;
		/**
		 *\~english
		 *\brief		adds a billboards shader program corresponding to given flags.
		 *\param[in]	p_program		The program to add.
		 *\param[in]	textureFlags	TextureChannel combination.
		 *\param[in]	programFlags	ProgramFlag combination.
		 *\param[in]	alphaFunc		The alpha test function.
		 *\return		The found program.
		 *\~french
		 *\brief		Ajoute un programme de billboards correspondant aux flags donnés.
		 *\param[in]	p_program		Le programme à ajouter.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	alphaFunc		La fonction de test alpha.
		 *\return		Le programme trouvé.
		 */
		C3D_API void doAddBillboardProgram( ShaderProgramSPtr p_program
			 ,TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc );

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
	 *\param[in]	engine	The engine.
	 *\~french
	 *\brief		Crée un cache de ShaderProgram.
	 *\param[in]	engine	Le moteur.
	 */
	inline std::unique_ptr< ShaderProgramCache >
	makeCache( Engine & engine )
	{
		return std::make_unique< ShaderProgramCache >( engine );
	}
}

#endif
