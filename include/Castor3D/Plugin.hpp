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
#ifndef ___C3D_Plugin___
#define ___C3D_Plugin___

#include "Prerequisites.hpp"

#ifdef WIN32
#	define WIN32_LEAN_AND_MEAN
#	if defined( _MSC_VER )
#		pragma warning( push )
#		pragma warning( disable:4311 )
#		pragma warning( disable:4312 )
#		pragma warning( disable:4251 )
#		pragma warning( disable:4275 )
#		pragma warning( disable:4290 )
#	endif
#	include <windows.h>
#endif

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		01/04/2011
	\~english
	\brief		Plugin loading exception
	\~french
	\brief		Exception de chargement de plugin
	*/
	class C3D_API PluginException : public Castor::Exception
	{
	public:
		PluginException( std::string const & p_description, char const * p_file, char const * p_function, uint32_t p_line);
		~PluginException()throw();
	};
	//! helper macro to ease the use of PluginException
#	define CASTOR_PLUGIN_EXCEPTION( x) throw PluginException( x, __FILE__, __FUNCTION__, __LINE__)
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Plugin Base class
	\remark		Manages the base plugin functions, allows plugins to check versions and to register themselves
	\~french
	\brief		Classe de base des plugins
	\remark		Gère les fonctions de base d'un plugin, permet aux plugins de faire des vérifications de version et  de s'enregistrer auprès du moteur
	*/
	class C3D_API PluginBase
	{
	private:
		//!< Signature for the plugin's type retrieval function
		typedef ePLUGIN_TYPE	GetTypeFunction();
		//!< Signature for the plugin's version checking function
		typedef void			GetRequiredVersionFunction( Version & p_version);
		//!< Signature for the plugin's name retrieval function
		typedef Castor::String	GetNameFunction();

	public:
		typedef GetTypeFunction				*	PGetTypeFunction;
		typedef GetRequiredVersionFunction	*	PGetRequiredVersionFunction;
		typedef GetNameFunction				*	PGetNameFunction;

	protected:
		//!< The plugin's version checking function
		PGetRequiredVersionFunction m_pfnGetRequiredVersion;
		//!< The plugin's name retrieval function
		PGetNameFunction m_pfnGetName;
		//!< The plugin type
		ePLUGIN_TYPE m_eType;
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_eType		The plugin type
		 *\param[in]	p_pLibrary	The shared library holding the plugin
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_eType		Le type du plugin
		 *\param[in]	p_pLibrary	La librairie partagée contenant le plugin
		 */
		PluginBase( ePLUGIN_TYPE p_eType, Castor::DynamicLibrarySPtr p_pLibrary )throw( PluginException);

	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~PluginBase() = 0;
		/**
		 *\~english
		 *\brief		Retrieves the required version for the plugin to work correctly
		 *\return		The version
		 *\~french
		 *\brief		Récupère la version nécessaire au bon fonctionnement du plugin
		 *\return		La version
		 */
		void GetRequiredVersion( Version & p_version)const;
		/**
		 *\~english
		 *\brief		Retrieves the plugin name
		 *\return		The name
		 *\~french
		 *\brief		Récupère le nom du plugin
		 *\return		Le nom
		 */
		Castor::String GetName()const;
		/**
		 *\~english
		 *\brief		Retrieves the plugin type
		 *\return		The type
		 *\~french
		 *\brief		Récupère le type du plugin
		 *\return		Le type
		 */
		inline ePLUGIN_TYPE GetType()const { return m_eType; }

	protected:
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_plugin	The Plugin object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_plugin	L'objet Plugin à copier
		 */
		PluginBase( PluginBase const & p_plugin);
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_plugin	The Plugin object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_plugin	L'objet Plugin à déplacer
		 */
		PluginBase( PluginBase && p_plugin);
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_plugin	The Plugin object to copy
		 *\return		A reference to this Plugin object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_plugin	L'objet Plugin à copier
		 *\return		Une référence sur cet objet Plugin
		 */
		PluginBase & operator =( PluginBase const & p_plugin);
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_plugin	The Plugin object to move
		 *\return		A reference to this Plugin object
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_plugin	L'objet Plugin à déplacer
		 *\return		Une référence sur cet objet Plugin
		 */
		PluginBase & operator =( PluginBase && p_plugin);
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Renderer Plugin class
	\~french
	\brief		Classe de plugin de rendu
	*/
	class C3D_API RendererPlugin : public PluginBase
	{
	private:
		friend class PluginBase;
		friend class Engine;

		typedef RenderSystem *	CreateRenderSystemFunction	( Engine * p_pEngine, Castor::Logger * p_pLogger );
		typedef void			DestroyRenderSystemFunction	( RenderSystem * p_pRenderSystem );
		typedef eRENDERER_TYPE	GetRendererTypeFunction		();

		typedef CreateRenderSystemFunction	*	PCreateRenderSystemFunction;
		typedef DestroyRenderSystemFunction	*	PDestroyRenderSystemFunction;
		typedef GetRendererTypeFunction		*	PGetRendererTypeFunction;

		PCreateRenderSystemFunction		m_pfnCreateRenderSystem;
		PDestroyRenderSystemFunction	m_pfnDestroyRenderSystem;
		PGetRendererTypeFunction		m_pfnGetRendererType;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pLibrary	The shared library holding the plugin
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pLibrary	La librairie partagée contenant le plugin
		 */
		RendererPlugin( Castor::DynamicLibrarySPtr p_pLibrary)throw( PluginException);
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~RendererPlugin();
		/**
		 *\~english
		 *\brief		Creates the RenderSystem
		 *\param[in]	p_pEngine	The core engine
		 *\param[in]	p_pLogger	The logger instance
		 *\return		The created RenderSystem instance
		 *\~french
		 *\brief		Crée le RenderSystem
		 *\param[in]	p_pEngine	Le moteur
		 *\param[in]	p_pLogger	L'instance de logger
		 *\return		L'instance de RenderSystem créée
		 */
		RenderSystem * CreateRenderSystem( Engine * p_pEngine, Castor::Logger * p_pLogger );
		/**
		 *\~english
		 *\brief		Destroys the given RenderSystem
		 *\param[in]	p_pRenderSystem	The RenderSystem
		 *\~french
		 *\brief		Détruit le RenderSystem donné
		 *\param[in]	p_pRenderSystem	Le RenderSystem
		 */
		void DestroyRenderSystem( RenderSystem * p_pRenderSystem );
		/**
		 *\~english
		 *\brief		Retrieves the renderer type
		 *\return		The type
		 *\~french
		 *\brief		Récupère le type de renderer
		 *\return		Le type
		 */
		eRENDERER_TYPE GetRendererType();

	private:
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_plugin	The Plugin object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_plugin	L'objet Plugin à copier
		 */
		RendererPlugin( RendererPlugin const & p_plugin);
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_plugin	The Plugin object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_plugin	L'objet Plugin à déplacer
		 */
		RendererPlugin( RendererPlugin && p_plugin);
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_plugin	The Plugin object to copy
		 *\return		A reference to this Plugin object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_plugin	L'objet Plugin à copier
		 *\return		Une référence sur cet objet Plugin
		 */
		RendererPlugin & operator =( RendererPlugin const & p_plugin);
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_plugin	The Plugin object to move
		 *\return		A reference to this Plugin object
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_plugin	L'objet Plugin à déplacer
		 *\return		Une référence sur cet objet Plugin
		 */
		RendererPlugin & operator =( RendererPlugin && p_plugin);
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Importer Plugin class
	\~french
	\brief		Classe de plugin d'import
	*/
	class C3D_API ImporterPlugin : public PluginBase
	{
	public:
		typedef std::pair< Castor::String, Castor::String > Extension;
		DECLARE_VECTOR( Extension, Extension );

	private:
		friend class PluginBase;
		friend class Engine;
		typedef void			CreateImporterFunction	( Engine * p_pEngine, ImporterPlugin * p_pPlugin );
		typedef void			DestroyImporterFunction	( ImporterPlugin * p_pPlugin );
		typedef ExtensionArray	GetExtensionFunction	();

		typedef CreateImporterFunction	*	PCreateImporterFunction;
		typedef DestroyImporterFunction	*	PDestroyImporterFunction;
		typedef GetExtensionFunction	*	PGetExtensionFunction;

		PCreateImporterFunction		m_pfnCreateImporter;
		PDestroyImporterFunction	m_pfnDestroyImporter;
		PGetExtensionFunction		m_pfnGetExtension;

	private:
		ImporterSPtr	m_pImporter;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pEngine		The core engine
		 *\param[in]	p_pLibrary		The shared library holding the plugin
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pEngine		Le moteur
		 *\param[in]	p_pLibrary		La librairie partagée contenant le plugin
		 */
		ImporterPlugin( Engine * p_pEngine, Castor::DynamicLibrarySPtr p_pLibrary )throw( PluginException );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~ImporterPlugin();
		/**
		 *\~english
		 *\brief		Attaches the plugin to the given Importer
		 *\param[in]	p_pImporter	The Importer
		 *\~french
		 *\brief		Attache le plugin à l'Importer donné
		 *\param[in]	p_pImporter	L'Importer
		 */
		inline void AttachImporter( ImporterSPtr p_pImporter ) { m_pImporter = p_pImporter; }
		/**
		 *\~english
		 *\brief		Detaches the plugin from the attached Importer
		 *\return		The attached Importer
		 *\~french
		 *\brief		Detache le plugin de l'Importer attaché
		 *\return		L'Importer attaché
		 */
		inline void DetachImporter() {  m_pImporter.reset(); }
		/**
		 *\~english
		 *\brief		Retrieves the attached Importer
		 *\return		The attached Importer
		 *\~french
		 *\brief		Récupère l'Importer attaché
		 *\return		L'Importer attaché
		 */
		inline ImporterSPtr GetImporter() { return m_pImporter; }
		/**
		 *\~english
		 *\brief		Retrieves the supported file extensions array
		 *\return		The supported extensions array
		 *\~french
		 *\brief		Récupère le tableau des extensions supportées
		 *\return		Le tableau d'extensions supportées
		 */
		ExtensionArray GetExtensions();

	private:
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_plugin	The Plugin object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_plugin	L'objet Plugin à copier
		 */
		ImporterPlugin( ImporterPlugin const & p_plugin);
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_plugin	The Plugin object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_plugin	L'objet Plugin à déplacer
		 */
		ImporterPlugin( ImporterPlugin && p_plugin);
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_plugin	The Plugin object to copy
		 *\return		A reference to this Plugin object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_plugin	L'objet Plugin à copier
		 *\return		Une référence sur cet objet Plugin
		 */
		ImporterPlugin & operator =( ImporterPlugin const & p_plugin);
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_plugin	The Plugin object to move
		 *\return		A reference to this Plugin object
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_plugin	L'objet Plugin à déplacer
		 *\return		Une référence sur cet objet Plugin
		 */
		ImporterPlugin & operator =( ImporterPlugin && p_plugin);
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Divider Plugin class
	\~french
	\brief		Classe de plugin de subdivision
	*/
	class C3D_API DividerPlugin : public PluginBase
	{
	private:
		friend class PluginBase;
		friend class Engine;
		typedef Subdivider *	CreateDividerFunction	();
		typedef void			DestroyDividerFunction	( Subdivider * p_pDivider );
		typedef Castor::String	GetDividerTypeFunction	();

		typedef CreateDividerFunction		*	PCreateDividerFunction;
		typedef DestroyDividerFunction		*	PDestroyDividerFunction;
		typedef GetDividerTypeFunction		*	PGetDividerTypeFunction;

		PCreateDividerFunction		m_pfnCreateDivider;
		PDestroyDividerFunction		m_pfnDestroyDivider;
		PGetDividerTypeFunction		m_pfnGetDividerType;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pLibrary	The shared library holding the plugin
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pLibrary	La librairie partagée contenant le plugin
		 */
		DividerPlugin( Castor::DynamicLibrarySPtr p_pLibrary)throw( PluginException);
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~DividerPlugin();
		/**
		 *\~english
		 *\brief		Creates the Subdivider
		 *\return		The created Subdivider instance
		 *\~french
		 *\brief		Crée le Subdivider
		 *\return		L'instance de Subdivider créée
		 */
		Subdivider * CreateDivider();
		/**
		 *\~english
		 *\brief		Destroys the given Subdivider
		 *\param[in]	p_pDivider	The Subdivider
		 *\~french
		 *\brief		Détruit le Subdivider donné
		 *\param[in]	p_pDivider	Le Subdivider
		 */
		void DestroyDivider( Subdivider * p_pDivider );
		/**
		 *\~english
		 *\brief		Tells the divider short name
		 *\~french
		 *\brief		Donne le nom court du diviseur
		 */
		Castor::String GetDividerType();

	private:
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_plugin	The Plugin object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_plugin	L'objet Plugin à copier
		 */
		DividerPlugin( DividerPlugin const & p_plugin);
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_plugin	The Plugin object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_plugin	L'objet Plugin à déplacer
		 */
		DividerPlugin( DividerPlugin && p_plugin);
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_plugin	The Plugin object to copy
		 *\return		A reference to this Plugin object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_plugin	L'objet Plugin à copier
		 *\return		Une référence sur cet objet Plugin
		 */
		DividerPlugin & operator =( DividerPlugin const & p_plugin);
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_plugin	The Plugin object to move
		 *\return		A reference to this Plugin object
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_plugin	L'objet Plugin à déplacer
		 *\return		Une référence sur cet objet Plugin
		 */
		DividerPlugin & operator =( DividerPlugin && p_plugin);
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Generator Plugin class
	\~french
	\brief		Classe de plugin de générateur procédural
	*/
	class C3D_API GeneratorPlugin : public PluginBase
	{
	private:
		friend class PluginBase;
		friend class Engine;
		typedef Subdivider * CreateGeneratorFunction	( TextureUnit * p_pTexture );
		typedef Subdivider * DestroyGeneratorFunction	( Subdivider * p_pGenerator );

		typedef CreateGeneratorFunction *	PCreateGeneratorFunction;
		typedef DestroyGeneratorFunction *	PDestroyGeneratorFunction;

		PCreateGeneratorFunction	m_pfnCreateGenerator;
		PDestroyGeneratorFunction	m_pfnDestroyGenerator;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pLibrary	The shared library holding the plugin
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pLibrary	La librairie partagée contenant le plugin
		 */
		GeneratorPlugin( Castor::DynamicLibrarySPtr p_pLibrary )throw( PluginException );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~GeneratorPlugin();
		/**
		 *\~english
		 *\brief		Creates the Importer
		 *\param[in]	p_pTexture	The texture modified by the generator
		 *\return		The created ProceduralGenerator instance
		 *\~french
		 *\brief		Crée l'Importer
		 *\param[in]	p_pTexture	La texture modifiée par le générateur
		 *\return		L'instance de ProceduralGenerator créée
		 */
		Subdivider * CreateGenerator( TextureUnit * p_pTexture );
		/**
		 *\~english
		 *\brief		Destroys the given ProceduralGenerator
		 *\param[in]	p_pGenerator	The ProceduralGenerator
		 *\~french
		 *\brief		Détruit le ProceduralGenerator donné
		 *\param[in]	p_pGenerator	Le ProceduralGenerator
		 */
		void DestroyGenerator( Subdivider * p_pGenerator );

	private:
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_plugin	The Plugin object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_plugin	L'objet Plugin à copier
		 */
		GeneratorPlugin( GeneratorPlugin const & p_plugin);
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_plugin	The Plugin object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_plugin	L'objet Plugin à déplacer
		 */
		GeneratorPlugin( GeneratorPlugin && p_plugin);
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_plugin	The Plugin object to copy
		 *\return		A reference to this Plugin object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_plugin	L'objet Plugin à copier
		 *\return		Une référence sur cet objet Plugin
		 */
		GeneratorPlugin & operator =( GeneratorPlugin const & p_plugin);
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_plugin	The Plugin object to move
		 *\return		A reference to this Plugin object
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_plugin	L'objet Plugin à déplacer
		 *\return		Une référence sur cet objet Plugin
		 */
		GeneratorPlugin & operator =( GeneratorPlugin && p_plugin);
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date		19/08/2012
	\~english
	\brief		Shader program Plugin class
	\~french
	\brief		Classe de plugin de programmes shader
	*/
	class C3D_API ShaderPlugin : public PluginBase
	{
	private:
		friend class PluginBase;
		friend class Engine;
		typedef ShaderProgramBaseSPtr	CreateShaderFunction		( RenderSystem * p_pRenderSystem );
		typedef IPipelineImpl *			CreatePipelineFunction		( Pipeline * p_pPipeline, RenderSystem * p_pRenderSystem );
		typedef void					DestroyPipelineFunction		( IPipelineImpl * p_pPipeline );
		typedef eSHADER_LANGUAGE		GetShaderLanguageFunction	();

		typedef CreateShaderFunction		*	PCreateShaderFunction;
		typedef CreatePipelineFunction		*	PCreatePipelineFunction;
		typedef DestroyPipelineFunction		*	PDestroyPipelineFunction;
		typedef GetShaderLanguageFunction	*	PGetShaderLanguageFunction;

		PCreateShaderFunction		m_pfnCreateShader;
		PCreatePipelineFunction		m_pfnCreatePipeline;
		PDestroyPipelineFunction	m_pfnDestroyPipeline;
		PGetShaderLanguageFunction	m_pfnGetShaderLanguage;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pLibrary	The shared library holding the plugin
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pLibrary	La librairie partagée contenant le plugin
		 */
		ShaderPlugin( Castor::DynamicLibrarySPtr p_pLibrary )throw( PluginException);
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~ShaderPlugin();
		/**
		 *\~english
		 *\brief		Creates a ShaderProgram object
		 *\param[in]	p_pRenderSystem	The render system
		 *\return		The created ShaderProgram instance
		 *\~french
		 *\brief		Crée un objet ShaderProgram
		 *\param[in]	p_pRenderSystem	Le render system
		 *\return		L'instance de ShaderProgram créée
		 */
		ShaderProgramBaseSPtr CreateShader( RenderSystem * p_pRenderSystem );
		/**
		 *\~english
		 *\brief		Creates a IPipelineImpl object
		 *\param[in]	p_pPipeline		The parent pipeline
		 *\param[in]	p_pRenderSystem	The render system
		 *\return		The created IPipelineImpl instance
		 *\~french
		 *\brief		Crée un objet IPipelineImpl
		 *\param[in]	p_pPipeline		Le pipeline parent
		 *\param[in]	p_pRenderSystem	Le render system
		 *\return		L'instance de IPipelineImpl créée
		 */
		IPipelineImpl * CreatePipeline( Pipeline * p_pPipeline, RenderSystem * p_pRenderSystem );
		/**
		 *\~english
		 *\brief		Destroys the given IPipelineImpl
		 *\param[in]	p_pPipeline	The IPipelineImpl
		 *\~french
		 *\brief		Détruit le IPipelineImpl donné
		 *\param[in]	p_pPipeline	Le IPipelineImpl
		 */
		void DestroyPipeline( IPipelineImpl * p_pPipeline );
		/**
		 *\~english
		 *\brief		Retrieves the shader language
		 *\return		The extension
		 *\~french
		 *\brief		Récupère le langage de shader
		 *\return		L'estenxion
		 */
		eSHADER_LANGUAGE GetShaderLanguage();

	private:
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_plugin	The Plugin object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_plugin	L'objet Plugin à copier
		 */
		ShaderPlugin( ShaderPlugin const & p_plugin);
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_plugin	The Plugin object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_plugin	L'objet Plugin à déplacer
		 */
		ShaderPlugin( ShaderPlugin && p_plugin);
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_plugin	The Plugin object to copy
		 *\return		A reference to this Plugin object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_plugin	L'objet Plugin à copier
		 *\return		Une référence sur cet objet Plugin
		 */
		ShaderPlugin & operator =( ShaderPlugin const & p_plugin);
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_plugin	The Plugin object to move
		 *\return		A reference to this Plugin object
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_plugin	L'objet Plugin à déplacer
		 *\return		Une référence sur cet objet Plugin
		 */
		ShaderPlugin & operator =( ShaderPlugin && p_plugin);
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date		19/08/2012
	\~english
	\brief		Post effect plugin class
	\~french
	\brief		Classe de plugin d'effets post rendu
	*/
	class C3D_API PostFxPlugin : public PluginBase
	{
	private:
		friend class PluginBase;
		typedef PostEffectSPtr	CreateEffectFunction		( RenderSystem * p_pRenderSystem );

		typedef CreateEffectFunction		*	PCreateEffectFunction;

		PCreateEffectFunction		m_pfnCreateEffect;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pLibrary	The shared library holding the plugin
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pLibrary	La librairie partagée contenant le plugin
		 */
		PostFxPlugin( Castor::DynamicLibrarySPtr p_pLibrary )throw( PluginException);
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~PostFxPlugin();
		/**
		 *\~english
		 *\brief		Creates a PostEffect object
		 *\param[in]	p_pRenderSystem	The render system
		 *\return		The created PostEffect instance
		 *\~french
		 *\brief		Crée un objet PostEffect
		 *\param[in]	p_pRenderSystem	Le render system
		 *\return		L'instance de PostEffect créée
		 */
		PostEffectSPtr CreateEffect( RenderSystem * p_pRenderSystem );

	private:
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_plugin	The Plugin object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_plugin	L'objet Plugin à copier
		 */
		PostFxPlugin( PostFxPlugin const & p_plugin);
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_plugin	The Plugin object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_plugin	L'objet Plugin à déplacer
		 */
		PostFxPlugin( PostFxPlugin && p_plugin);
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_plugin	The Plugin object to copy
		 *\return		A reference to this Plugin object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_plugin	L'objet Plugin à copier
		 *\return		Une référence sur cet objet Plugin
		 */
		PostFxPlugin & operator =( PostFxPlugin const & p_plugin);
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_plugin	The Plugin object to move
		 *\return		A reference to this Plugin object
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_plugin	L'objet Plugin à déplacer
		 *\return		Une référence sur cet objet Plugin
		 */
		PostFxPlugin & operator =( PostFxPlugin && p_plugin);
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date		19/08/2012
	\~english
	\brief		Render technique plugin class
	\~french
	\brief		Classe de plugin de techniques de rendu
	*/
	class C3D_API TechniquePlugin : public PluginBase
	{
	private:
		friend class PluginBase;
		typedef RenderTechniqueBaseSPtr	CreateTechniqueFunction		( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params );

		typedef CreateTechniqueFunction		*	PCreateTechniqueFunction;

		PCreateTechniqueFunction		m_pfnCreateTechnique;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pLibrary	The shared library holding the plugin
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pLibrary	La librairie partagée contenant le plugin
		 */
		TechniquePlugin( Castor::DynamicLibrarySPtr p_pLibrary )throw( PluginException);
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~TechniquePlugin();
		/**
		 *\~english
		 *\brief		Creates a RenderTechnique object
		 *\param[in]	p_renderTarget	The render target for this technique
		 *\param[in]	p_pRenderSystem	The render system
		 *\param[in]	p_params		The technique parameters
		 *\return		The created RenderTechnique instance
		 *\~french
		 *\brief		Crée un objet RenderTechnique
		 *\param[in]	p_renderTarget	La render target pour cette technique
		 *\param[in]	p_pRenderSystem	Le render system
		 *\param[in]	p_params		Les paramètres de la technique
		 *\return		L'instance de RenderTechnique créée
		 */
		RenderTechniqueBaseSPtr CreateTechnique( RenderTarget & p_renderTarget, RenderSystem * p_pRenderSystem, Parameters const & p_params );

	private:
		/**
		 *\~english
		 *\brief		Copy constructor
		 *\param[in]	p_plugin	The Plugin object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_plugin	L'objet Plugin à copier
		 */
		TechniquePlugin( TechniquePlugin const & p_plugin);
		/**
		 *\~english
		 *\brief		Move constructor
		 *\param[in]	p_plugin	The Plugin object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_plugin	L'objet Plugin à déplacer
		 */
		TechniquePlugin( TechniquePlugin && p_plugin);
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_plugin	The Plugin object to copy
		 *\return		A reference to this Plugin object
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_plugin	L'objet Plugin à copier
		 *\return		Une référence sur cet objet Plugin
		 */
		TechniquePlugin & operator =( TechniquePlugin const & p_plugin);
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_plugin	The Plugin object to move
		 *\return		A reference to this Plugin object
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement
		 *\param[in]	p_plugin	L'objet Plugin à déplacer
		 *\return		Une référence sur cet objet Plugin
		 */
		TechniquePlugin & operator =( TechniquePlugin && p_plugin);
	};
}

#if defined( _MSC_VER )
#	pragma warning( pop )
#endif

#endif

