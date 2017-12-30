/*
See LICENSE file in root folder
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
		 *\param[in]	initialise	Tells if we want the program to be initialised.
		 *\return		The newly created program.
		 *\~french
		 *\brief		Crée un nouveau programme.
		 *\param[in]	initialise	Dit si on veut que le programme soit initialisé.
		 *\return		Le programme créé.
		 */
		C3D_API ShaderProgramSPtr getNewProgram( bool initialise );
		/**
		 *\~english
		 *\brief		Looks for an automatically generated program corresponding to given flags.
		 *\remarks		If none exists it is created.
		 *\param[in]	renderPass		The pass from which the program code is retrieved.
		 *\param[in]	passFlags		Bitwise ORed PassFlag.
		 *\param[in]	textureFlags	Bitwise ORed TextureChannel.
		 *\param[in]	programFlags	Bitwise ORed ProgramFlag.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\param[in]	alphaFunc		The alpha test function.
		 *\param[in]	invertNormals	Tells if the normals must be inverted, in the program.
		 *\return		The found or created program.
		 *\~french
		 *\brief		Cherche un programme automatiquement généré correspondant aux flags donnés.
		 *\param[in]	renderPass		La passe a partir de laquelle est récupéré le code du programme.
		 *\param[in]	passFlags		Une combinaison de PassFlag.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 *\param[in]	alphaFunc		La fonction de test alpha.
		 *\param[in]	invertNormals	Dit si les normales doivent être inversées, dans le programme.
		 *\return		Le programme trouvé ou créé.
		 */
		C3D_API ShaderProgramSPtr getAutomaticProgram( RenderPass const & renderPass
			, PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc
			, bool invertNormals );
		/**
		 *\~english
		 *\brief		Creates the textures related frame variables.
		 *\param[in]	program			The program to which the buffer is bound.
		 *\param[in]	passFlags		Bitwise ORed PassFlag.
		 *\param[in]	textureFlags	TextureChannel combination.
		 *\param[in]	programFlags	Bitwise ORed ProgramFlag.
		 *\~french
		 *\brief		Crée les frame variables relatives aux textures.
		 *\param[in]	program			Le programme auquel le buffer est lié.
		 *\param[in]	passFlags		Une combinaison de PassFlag.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 */
		C3D_API void createTextureVariables( ShaderProgram & program
			, PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags )const;
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
		 *\param[in]	initialise	Tells if we want the program to be initialised.
		 *\param[in]	program		The program to add.
		 *\~french
		 *\brief		Crée un nouveau programme.
		 *\param[in]	initialise	Dit si on veut que le programme soit initialisé.
		 *\param[in]	program		Le programme à ajouter.
		 */
		C3D_API void doAddProgram( ShaderProgramSPtr program, bool initialise );
		/**
		 *\~english
		 *\brief		Looks for an automatically generated program corresponding to given flags.
		 *\remarks		If none exists it is created.
		 *\param[in]	renderPass		The pass from which the program code is retrieved.
		 *\param[in]	passFlags		Bitwise ORed PassFlag.
		 *\param[in]	textureFlags	TextureChannel combination.
		 *\param[in]	programFlags	ProgramFlag combination.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\param[in]	alphaFunc		The alpha test function.
		 *\param[in]	invertNormals	Tells if the normals must be inverted, in the program.
		 *\return		The found or created program.
		 *\~french
		 *\brief		Cherche un programme automatiquement généré correspondant aux flags donnés.
		 *\param[in]	renderPass		La passe a partir de laquelle est récupéré le code du programme.
		 *\param[in]	passFlags		Une combinaison de PassFlag.
		 *\param[in]	textureFlags	TextureChannel combination.
		 *\param[in]	programFlags	ProgramFlag combination.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 *\param[in]	alphaFunc		La fonction de test alpha.
		 *\param[in]	invertNormals	Dit si les normales doivent être inversées, dans le programme.
		 *\return		Le programme trouvé ou créé.
		 */
		C3D_API ShaderProgramSPtr doCreateAutomaticProgram( RenderPass const & renderPass
			, PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc
			, bool invertNormals )const;
		/**
		 *\~english
		 *\brief		adds an automatically generated shader program corresponding to given flags.
		 *\param[in]	program			The program to add.
		 *\param[in]	passFlags		Bitwise ORed PassFlag.
		 *\param[in]	textureFlags	TextureChannel combination.
		 *\param[in]	programFlags	ProgramFlag combination.
		 *\param[in]	sceneFlags		The scene flags (fog, ...).
		 *\param[in]	alphaFunc		The alpha test function.
		 *\param[in]	invertNormals	Tells if the normals must be inverted, in the program.
		 *\return		The found program.
		 *\~french
		 *\brief		Ajoute un programme automatiquement généré correspondant aux flags donnés.
		 *\param[in]	program			Le programme à ajouter.
		 *\param[in]	passFlags		Une combinaison de PassFlag.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs de la scène (brouillard, ...).
		 *\param[in]	alphaFunc		La fonction de test alpha.
		 *\param[in]	invertNormals	Dit si les normales doivent être inversées, dans le programme.
		 *\return		Le programme trouvé.
		 */
		C3D_API void doAddAutomaticProgram( ShaderProgramSPtr program
			, PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc
			, bool invertNormals );
		/**
		 *\~english
		 *\brief		Creates a shader program for billboards rendering use.
		 *\param[in]	renderPass		The pass from which the program code is retrieved.
		 *\param[in]	passFlags		Bitwise ORed PassFlag.
		 *\param[in]	textureFlags	TextureChannel combination.
		 *\param[in]	programFlags	ProgramFlag combination.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\param[in]	alphaFunc		The alpha test function.
		 *\return		The created program.
		 *\~french
		 *\brief		Crée un programme shader pour les rendu de billboards.
		 *\param[in]	renderPass		La passe a partir de laquelle est récupéré le code du programme.
		 *\param[in]	passFlags		Une combinaison de PassFlag.
		 *\param[in]	textureFlags	Combinaison de TextureChannel.
		 *\param[in]	programFlags	Combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 *\param[in]	alphaFunc		La fonction de test alpha.
		 *\return		Le programme créé.
		 */
		C3D_API ShaderProgramSPtr doCreateBillboardProgram( RenderPass const & renderPass
			, PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc )const;
		/**
		 *\~english
		 *\brief		adds a billboards shader program corresponding to given flags.
		 *\param[in]	program			The program to add.
		 *\param[in]	passFlags		Bitwise ORed PassFlag.
		 *\param[in]	textureFlags	TextureChannel combination.
		 *\param[in]	programFlags	ProgramFlag combination.
		 *\param[in]	alphaFunc		The alpha test function.
		 *\return		The found program.
		 *\~french
		 *\brief		Ajoute un programme de billboards correspondant aux flags donnés.
		 *\param[in]	program			Le programme à ajouter.
		 *\param[in]	passFlags		Une combinaison de PassFlag.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	alphaFunc		La fonction de test alpha.
		 *\return		Le programme trouvé.
		 */
		C3D_API void doAddBillboardProgram( ShaderProgramSPtr program
			, PassFlags const & passFlags
			,TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc );

	private:
		DECLARE_MAP( uint64_t, ShaderProgramWPtr, ShaderProgramWPtrUInt64 );
		mutable std::recursive_mutex m_mutex;
		//!\~english	The loaded shader programs.
		//!\~french		Les programmes chargés.
		ShaderProgramPtrArray m_arrayPrograms;
		//!\~english	Automatically generated shader programs, sorted by texture flags.
		//!\~french		Programmes auto-générés, triés par flags de texture.
		ShaderProgramWPtrUInt64Map m_mapAutogenerated;
		//!\~english	Billboards shader programs, sorted by texture flags.
		//!\~french		Programmes shader pour billboards, triés par flags de texture.
		ShaderProgramWPtrUInt64Map m_mapBillboards;
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
