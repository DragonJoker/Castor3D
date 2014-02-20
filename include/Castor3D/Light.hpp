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
#ifndef ___C3D_Light___
#define ___C3D_Light___

#include "MovableObject.hpp"
#include "Renderable.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		14/02/2010
	\~english
	\brief		Holds light components
	\remark		Derived into DirectionalLight, PointLight and SpotLight
	\~french
	\brief		Contient les composantes d'une lumière
	\remark		Dérivée en DirectionalLight, PointLight et SpotLight
	*/
	class C3D_API LightCategory
	{
	public:
		/*!
		\author 	Sylvain DOREMUS
		\date 		14/02/2010
		\~english
		\brief		LightCategory loader
		\~french
		\brief		Loader de LightCategory
		*/
		class C3D_API TextLoader : public Castor::Loader< LightCategory, Castor::eFILE_TYPE_TEXT, Castor::TextFile >, CuNonCopyable
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			TextLoader( Castor::File::eENCODING_MODE p_eEncodingMode=Castor::File::eENCODING_MODE_ASCII );
			/**
			 *\~english
			 *\brief		Writes a light into a text file
			 *\param[in]	p_file	The file to save the cameras in
			 *\param[in]	p_light	The light to save
			 *\~french
			 *\brief		Ecrit une lumière dans un fichier texte
			 *\param[in]	p_file	Le fichier
			 *\param[in]	p_light	La lumière
			 */
			virtual bool operator ()( LightCategory const & p_light, Castor::TextFile & p_file);
		};

	private:
		//!\~english The light type	\~french Le type de lumière
		eLIGHT_TYPE m_eLightType;
		//!\~english The parent light	\~french La lumière parente
		Light * m_pLight;
		//!\~english The ambient colour values	\~french La couleur ambiante
		Castor::Colour m_ambient;
		//!\~english The diffuse colour values	\~french La couleur diffuse
		Castor::Colour m_diffuse;
		//!\~english The specular colour values	\~french La couleur spéculaire
		Castor::Colour m_specular;
		//!\~english The position and type of the light (type is in 4th coordinate)	\~french La position et le type de la lumière (le type est dans la 4ème coordonnée)
		Castor::Point4f m_ptPositionType;

	public:
		/**
		 *\~english
		 *\brief		The constructor, used by clone function
		 *\param[in]	p_eLightType	The light category type
		 *\~french
		 *\brief		Le constructeur utilisé par la fonction de clonage
		 *\param[in]	p_eLightType	Le type de catégorie de lumière
		 */
		LightCategory( eLIGHT_TYPE p_eLightType );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~LightCategory();
		/**
		 *\~english
		 *\brief		Renders the light => Applies it's position
		 *\~french
		 *\brief		Rend cette lumière => Applique sa position
		 */
		virtual void Render( LightRendererSPtr p_pRenderer )=0;
		/**
		 *\~english
		 *\brief		Renders the light => Applies it's position
		 *\~french
		 *\brief		Rend cette lumière => Applique sa position
		 */
		virtual void Render( LightRendererSPtr p_pRenderer, ShaderProgramBase * p_pProgram )=0;
		/**
		 *\~english
		 *\brief		Sets the light's ambient colour from
		 *\param[in]	p_ambient	The new value
		 *\~french
		 *\brief		Définit la couleur ambiante
		 *\param[in]	p_ambient	La nouvelle valeur
		 */
		void SetAmbient( float * p_ambient);
		/**
		 *\~english
		 *\brief		Sets the light's ambient colour
		 *\param[in]	 r	The red value
		 *\param[in]	 g	The green value
		 *\param[in]	 b	The blue value
		 *\~french
		 *\brief		Définit la couleur ambiante
		 *\param[in]	 r	La composante rouge
		 *\param[in]	 g	La composante verte
		 *\param[in]	 b	La composante bleue
		 */
		void SetAmbient( float r, float g, float b);
		/**
		 *\~english
		 *\brief		Sets the light's ambient colour
		 *\param[in]	p_ambient	The new value
		 *\~french
		 *\brief		Définit la couleur ambiante
		 *\param[in]	p_ambient	La nouvelle valeur
		 */
		void SetAmbient( Castor::Colour const & p_ambient);
		/**
		 *\~english
		 *\brief		Sets the light's diffuse colour
		 *\param[in]	p_diffuse	The new value
		 *\~french
		 *\brief		Définit la couleur diffuse
		 *\param[in]	p_diffuse	La nouvelle valeur
		 */
		void SetDiffuse( float * p_diffuse);
		/**
		 *\~english
		 *\brief		Sets the light's diffuse colour
		 *\param[in]	r	The red value
		 *\param[in]	g	The green value
		 *\param[in]	b	The blue value
		 *\~french
		 *\brief		Définit la couleur diffuse
		 *\param[in]	 r	La composante rouge
		 *\param[in]	 g	La composante verte
		 *\param[in]	 b	La composante bleue
		 */
		void SetDiffuse( float r, float g, float b);
		/**
		 *\~english
		 *\brief		Sets the light's diffuse colour
		 *\param[in]	p_diffuse	The new value
		 *\~french
		 *\brief		Définit la couleur diffuse
		 *\param[in]	p_diffuse	La nouvelle valeur
		 */
		void SetDiffuse( Castor::Colour const & p_diffuse);
		/**
		 *\~english
		 *\brief		Sets the light's specular colour
		 *\param[in]	p_specular	The new value
		 *\~french
		 *\brief		Définit la couleur spéculaire
		 *\param[in]	p_specular	La nouvelle valeur
		 */
		void SetSpecular( float * p_specular);
		/**
		 *\~english
		 *\brief		Sets the light's specular colour
		 *\param[in]	r	The red value
		 *\param[in]	g	The green value
		 *\param[in]	b	The blue value
		 *\~french
		 *\brief		Définit la couleur spéculaire
		 *\param[in]	 r	La composante rouge
		 *\param[in]	 g	La composante verte
		 *\param[in]	 b	La composante bleue
		 */
		void SetSpecular( float r, float g, float b);
		/**
		 *\~english
		 *\brief		Sets the light's specular colour
		 *\param[in]	p_specular	The new value
		 *\~french
		 *\brief		Définit la couleur spéculaire
		 *\param[in]	p_specular	La nouvelle valeur
		 */
		void SetSpecular( Castor::Colour const & p_specular);
		/**
		 *\~english
		 *\brief		Retrieves the light type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type de lumière
		 *\return		La valeur
		 */
		inline eLIGHT_TYPE GetLightType	()const { return m_eLightType; }
		/**
		 *\~english
		 *\brief		Retrieves the ambient colour
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur ambiante
		 *\return		La valeur
		 */
		inline Castor::Colour const & GetAmbient()const { return m_ambient; }
		/**
		 *\~english
		 *\brief		Retrieves the diffuse colour
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur diffuse
		 *\return		La valeur
		 */
		inline Castor::Colour const & GetDiffuse()const { return m_diffuse; }
		/**
		 *\~english
		 *\brief		Retrieves the specular colour
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur spéculaire
		 *\return		La valeur
		 */
		inline Castor::Colour const & GetSpecular()const { return m_specular; }
		/**
		 *\~english
		 *\brief		Retrieves the ambient colour
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur ambiante
		 *\return		La valeur
		 */
		inline Castor::Colour & GetAmbient() { return m_ambient; }
		/**
		 *\~english
		 *\brief		Retrieves the diffuse colour
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur diffuse
		 *\return		La valeur
		 */
		inline Castor::Colour & GetDiffuse() { return m_diffuse; }
		/**
		 *\~english
		 *\brief		Retrieves the specular colour
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur spéculaire
		 *\return		La valeur
		 */
		inline Castor::Colour & GetSpecular() { return m_specular; }
		/**
		 *\~english
		 *\brief		Retrieves the light position and type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type et la position de la lumière
		 *\return		La valeur
		 */
		inline Castor::Point4f const & GetPositionType()const { return m_ptPositionType; }
		/**
		 *\~english
		 *\brief		Retrieves the parent light
		 *\return		The value
		 *\~french
		 *\brief		Récupère la lumière parente
		 *\return		La valeur
		 */
		inline Light * GetLight()const { return m_pLight; }
		/**
		 *\~english
		 *\brief		Sets the parent light
		 *\return		The value
		 *\~french
		 *\brief		Définit la lumière parente
		 *\return		La valeur
		 */
		inline void SetLight( Light * val) { m_pLight = val; }
		/**
		 *\~english
		 *\brief		Sets the light type
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit le type de lumière
		 *\param[in]	La nouvelle valeur
		 */
		inline void SetLightType( eLIGHT_TYPE val) { m_eLightType = val; }
		/**
		 *\~english
		 *\brief		Sets the light's type and position
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit le type de la lumière et sa position
		 *\param[in]	La nouvelle valeur
		 */
		virtual void SetPositionType( Castor::Point4f const & p_ptPosition) { m_ptPositionType = p_ptPosition; }
		/**
		 *\~english
		 *\brief		Sets the light's type and position
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit le type de la lumière et sa position
		 *\param[in]	La nouvelle valeur
		 */
		virtual void SetPositionType( Castor::Point4d const & p_ptPosition) { m_ptPositionType[0] = float( p_ptPosition[0] );m_ptPositionType[1] = float( p_ptPosition[1] );m_ptPositionType[2] = float( p_ptPosition[2] );m_ptPositionType[3] = float( p_ptPosition[3] ); }
		/**
		 *\~english
		 *\brief		Sets the light position
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit la position de la lumière
		 *\param[in]	La nouvelle valeur
		 */
		virtual void SetPosition( Castor::Point3f const & p_ptPosition) { m_ptPositionType[0] = p_ptPosition[0];m_ptPositionType[1] = p_ptPosition[1];m_ptPositionType[2] = p_ptPosition[2]; }
		/**
		 *\~english
		 *\brief		Sets the light position
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit la position de la lumière
		 *\param[in]	La nouvelle valeur
		 */
		virtual void SetPosition( Castor::Point3d const & p_ptPosition) { m_ptPositionType[0] = float( p_ptPosition[0] );m_ptPositionType[1] = float( p_ptPosition[1] );m_ptPositionType[2] = float( p_ptPosition[2] ); }
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Light source implementation
	\~french
	\brief		Implémentation d'une source lumineuse
	\remark		
	*/
	class C3D_API Light : public MovableObject, public Renderable< Light, LightRenderer >
	{
	protected:
		//!\~english Tells the light is enabled	\~french	Dit si la lumière est active ou pas
		bool m_enabled;
		//!\~english The Light category that effectively holds light data	\~french la light category contenant les données de la lumière
		LightCategorySPtr m_pCategory;
		//!\~english	The light index	\~french	L'indice de la lumière
		int m_iIndex;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pScene		Parent scene
		 *\param[in]	p_pFactory		Factory used to create the LightCategory
		 *\param[in]	p_eLightType	The light type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pScene		La scène parente
		 *\param[in]	p_pFactory		La fabrique de LightCategory
		 *\param[in]	p_eLightType	Le type de lumière
		 */
		Light( Scene * p_pScene, LightFactory & p_factory, eLIGHT_TYPE p_eLightType );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pScene		Parent scene
		 *\param[in]	p_pNode			The parent scene node
		 *\param[in]	p_pFactory		Factory used to create the LightCategory
		 *\param[in]	p_eLightType	The light type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pScene		La scène parente
		 *\param[in]	p_pNode			Le scene node parent
		 *\param[in]	p_pFactory		La fabrique de LightCategory
		 *\param[in]	p_eLightType	Le type de lumière
		 */
		Light( LightFactory & p_factory, Scene * p_pScene, SceneNodeSPtr p_pNode, Castor::String const & p_name, eLIGHT_TYPE p_eLightType);
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~Light();
		/**
		 *\~english
		 *\brief		Registers each light type in the given factory
		 *\param[out]	p_factory	Registers the light types
		 *\~french
		 *\brief		Enregistre chaque type de lumière dans la fabrique donnée
		 *\param[out]	p_factory	Enregistre les types de lumières
		 */
		static void Register( LightFactory & p_factory );
		/**
		 *\~english
		 *\brief		Enables the light
		 *\~french
		 *\brief		Active la lumière
		 */
		void Enable();
		/**
		 *\~english
		 *\brief		Disables the light
		 *\~french
		 *\brief		Désactive la lumière
		 */
		void Disable();
		/**
		 *\~english
		 *\brief		Enables the light
		 *\param[in]	p_pProgram	The program receiving light's data
		 *\~french
		 *\brief		Active la lumière
		 *\param[in]	p_pProgram	Le programme recevant les données de la lumière
		 */
		void Enable( ShaderProgramBase * p_pProgram );
		/**
		 *\~english
		 *\brief		Disables the light
		 *\param[in]	p_pProgram	The program receiving light's data
		 *\~french
		 *\brief		Désactive la lumière
		 *\param[in]	p_pProgram	Le programme recevant les données de la lumière
		 */
		void Disable( ShaderProgramBase * p_pProgram );
		/**
		 *\~english
		 *\brief		Renders the light
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Rend la lumière
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		void Render();
		/**
		 *\~english
		 *\brief		Ends light rendering
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Fin du rendu de la lumière
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		void EndRender();
		/**
		 *\~english
		 *\brief		Renders the light
		 *\remark		Shader implementation
		 *\~french
		 *\brief		Rend la lumière
		 *\remark		Implémentation passant par les shaders
		 */
		void Render( ShaderProgramBase * p_pProgram );
		/**
		 *\~english
		 *\brief		Ends light rendering
		 *\remark		Shader implementation
		 *\~french
		 *\brief		Fin du rendu de la lumière
		 *\remark		Implémentation passant par les shaders
		 */
		void EndRender( ShaderProgramBase * p_pProgram );
		/**
		 *\~english
		 *\brief		Attaches this light to a Node
		 *\param[in]	p_pNode	The new light's parent node
		 *\~french
		 *\brief		Attache cette lumière au node donné
		 *\param[in]	p_pNode	Le nouveau node parent de cette lumière
		 */
		virtual void AttachTo( SceneNode * p_pNode);
		/**
		 *\~english
		 *\brief		Retrieves the light type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type de lumière
		 *\return		La valeur
		 */
		inline eLIGHT_TYPE GetLightType()const { return m_pCategory->GetLightType(); }
		/**
		 *\~english
		 *\brief		Retrieves the light enabled status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut d'activation de la lumière
		 *\return		La valeur
		 */
		inline bool IsEnabled()const { return m_enabled; }
		/**
		 *\~english
		 *\brief		Retrieves the ambient colour
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur ambiante
		 *\return		La valeur
		 */
		inline Castor::Colour const & GetAmbient()const { return m_pCategory->GetAmbient(); }
		/**
		 *\~english
		 *\brief		Retrieves the diffuse colour
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur diffuse
		 *\return		La valeur
		 */
		inline Castor::Colour const & GetDiffuse()const { return m_pCategory->GetDiffuse(); }
		/**
		 *\~english
		 *\brief		Retrieves the specular colour
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur spéculaire
		 *\return		La valeur
		 */
		inline Castor::Colour const & GetSpecular()const { return m_pCategory->GetSpecular(); }
		/**
		 *\~english
		 *\brief		Retrieves the ambient colour
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur ambiante
		 *\return		La valeur
		 */
		inline Castor::Colour & GetAmbient() { return m_pCategory->GetAmbient(); }
		/**
		 *\~english
		 *\brief		Retrieves the diffuse colour
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur diffuse
		 *\return		La valeur
		 */
		inline Castor::Colour & GetDiffuse() { return m_pCategory->GetDiffuse(); }
		/**
		 *\~english
		 *\brief		Retrieves the specular colour
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur spéculaire
		 *\return		La valeur
		 */
		inline Castor::Colour & GetSpecular() { return m_pCategory->GetSpecular(); }
		/**
		 *\~english
		 *\brief		Retrieves the light position and type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type et la position de la lumière
		 *\return		La valeur
		 */
		inline Castor::Point4f const & GetPositionType()const { return m_pCategory->GetPositionType(); }
		/**
		 *\~english
		 *\brief		Retrieves the light category
		 *\return		The value
		 *\~french
		 *\brief		Récupère la LightCategory
		 *\return		La valeur
		 */
		inline LightCategorySPtr GetLightCategory()const { return m_pCategory; }
		/**
		 *\~english
		 *\brief		Retrieves the light index
		 *\return		The value
		 *\~french
		 *\brief		Récupère l(indice de la lumière
		 *\return		La valeur
		 */
		inline int GetIndex()const { return m_iIndex; }
		/**
		 *\~english
		 *\brief		Sets the light index
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit l'indice de la lumière
		 *\param[in]	La nouvelle valeur
		 */
		inline void SetIndex( int val ) { m_iIndex = val; }
		/**
		 *\~english
		 *\brief		Sets the ambient colour
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit la couleur ambiante
		 *\param[in]	La nouvelle valeur
		 */
		inline void SetAmbient( float * p_ambient) { m_pCategory->SetAmbient( p_ambient); }
		/**
		 *\~english
		 *\brief		Sets the ambient colour
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit la couleur ambiante
		 *\param[in]	La nouvelle valeur
		 */
		inline void SetAmbient( float r, float g, float b) { m_pCategory->SetAmbient( r, g, b); }
		/**
		 *\~english
		 *\brief		Sets the ambient colour
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit la couleur ambiante
		 *\param[in]	La nouvelle valeur
		 */
		inline void SetAmbient( Castor::Colour const & p_ambient) { m_pCategory->SetAmbient( p_ambient); }
		/**
		 *\~english
		 *\brief		Sets the diffuse colour
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit la couleur diffuse
		 *\param[in]	La nouvelle valeur
		 */
		inline void SetDiffuse( float * p_diffuse) { m_pCategory->SetDiffuse( p_diffuse); }
		/**
		 *\~english
		 *\brief		Sets the diffuse colour
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit la couleur diffuse
		 *\param[in]	La nouvelle valeur
		 */
		inline void SetDiffuse( float r, float g, float b) { m_pCategory->SetDiffuse( r, g, b); }
		/**
		 *\~english
		 *\brief		Sets the diffuse colour
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit la couleur diffuse
		 *\param[in]	La nouvelle valeur
		 */
		inline void SetDiffuse( Castor::Colour const & p_diffuse) { m_pCategory->SetDiffuse( p_diffuse); }
		/**
		 *\~english
		 *\brief		Sets the specular colour
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit la couleur spéculaire
		 *\param[in]	La nouvelle valeur
		 */
		inline void SetSpecular( float * p_specular) { m_pCategory->SetSpecular( p_specular); }
		/**
		 *\~english
		 *\brief		Sets the specular colour
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit la couleur spéculaire
		 *\param[in]	La nouvelle valeur
		 */
		inline void SetSpecular( float r, float g, float b) { m_pCategory->SetSpecular( r, g, b); }
		/**
		 *\~english
		 *\brief		Sets the specular colour
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit la couleur spéculaire
		 *\param[in]	La nouvelle valeur
		 */
		inline void SetSpecular( Castor::Colour const & p_specular) { m_pCategory->SetSpecular( p_specular); }
		/**
		 *\~english
		 *\brief		Sets the light enabled status
		 *\param[in]	The new value
		 *\~french
		 *\brief		Définit le statut d'activation de la lumère
		 *\param[in]	La nouvelle valeur
		 */
		inline void SetEnabled( bool p_enabled) { m_enabled = p_enabled; }
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Renders a light, applies it's colours values...
	\~french
	\brief		Rend une lumière, ses couleurs, positions, ...
	*/
	class C3D_API LightRenderer : public Renderer<Light, LightRenderer>
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pRenderSystem	The render system
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pRenderSystem	Le render system
		 */
		LightRenderer( RenderSystem * p_pRenderSystem );

	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~LightRenderer();
		/**
		 *\~english
		 *\brief		Initialises the renderer
		 *\~french
		 *\brief		Initialise le renderer
		 */
		virtual void Initialise();
		/**
		 *\~english
		 *\brief		Cleans up the renderer
		 *\~french
		 *\brief		Nettoie le renderer
		 */
		virtual void Cleanup();
		/**
		 *\~english
		 *\brief		Enables the light
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Active la lumière
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		virtual void Enable() = 0;
		/**
		 *\~english
		 *\brief		Disables the light
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Désactive la lumière
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		virtual void Disable() = 0;
		/**
		 *\~english
		 *\brief		Enables the light
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Active la lumière
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		virtual void Bind(){}
		/**
		 *\~english
		 *\brief		Disables the light
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Désactive la lumière
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		virtual void Unbind(){}
		/**
		 *\~english
		 *\brief		Applies ambient colour
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Applique la couleur ambiante
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		virtual void ApplyAmbient() = 0;
		/**
		 *\~english
		 *\brief		Applies diffuse colour
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Applique la couleur diffuse
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		virtual void ApplyDiffuse() = 0;
		/**
		 *\~english
		 *\brief		Applies specular colour
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Applique la couleur speculaire
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		virtual void ApplySpecular() = 0;
		/**
		 *\~english
		 *\brief		Applies light position
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Applique la position de la source
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		virtual void ApplyPosition() = 0;
		/**
		 *\~english
		 *\brief		Applies light orientation
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Applique l'orientation de la lumière
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		virtual void ApplyOrientation() = 0;
		/**
		 *\~english
		 *\brief		Applies light constant attenuation
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Applique l'attenuation constante de la source
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		virtual void ApplyConstantAtt( float p_fConstant) = 0;
		/**
		 *\~english
		 *\brief		Applies light linear attenuation
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Applique l'attenuation linéaire de la source
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		virtual void ApplyLinearAtt( float p_fLinear) = 0;
		/**
		 *\~english
		 *\brief		Applies light quadratic attenuation
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Applique l'attenuation quadratique de la source
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		virtual void ApplyQuadraticAtt( float p_fQuadratic) = 0;
		/**
		 *\~english
		 *\brief		Applies light exponent
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Applique l'exposant de la source
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		virtual void ApplyExponent( float p_fExponent) = 0;
		/**
		 *\~english
		 *\brief		Applies light cut off
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Applique l'angle du cône de la source
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		virtual void ApplyCutOff( float p_fCutOff) = 0;
		/**
		 *\~english
		 *\brief		Enables the light
		 *\remark		Shader implementation
		 *\~french
		 *\brief		Active la lumière
		 *\remark		Implémentation passant par les shaders
		 */
		virtual void EnableShader( ShaderProgramBase * p_pProgram ) = 0;
		/**
		 *\~english
		 *\brief		Disables the light
		 *\remark		Shader implementation
		 *\~french
		 *\brief		désactive la lumière
		 *\remark		Implémentation passant par les shaders
		 */
		virtual void DisableShader( ShaderProgramBase * p_pProgram ) = 0;
		/**
		 *\~english
		 *\brief		Applies ambient colour
		 *\remark		Shader implementation
		 *\~french
		 *\brief		Applique la couleur ambiante
		 *\remark		Implémentation passant par les shaders
		 */
		virtual void ApplyAmbientShader() = 0;
		/**
		 *\~english
		 *\brief		Applies diffuse colour
		 *\remark		Shader implementation
		 *\~french
		 *\brief		Applique la couleur diffuse
		 *\remark		Implémentation passant par les shaders
		 */
		virtual void ApplyDiffuseShader() = 0;
		/**
		 *\~english
		 *\brief		Applies specular colour
		 *\remark		Shader implementation
		 *\~french
		 *\brief		Applique la couleur speculaire
		 *\remark		Implémentation passant par les shaders
		 */
		virtual void ApplySpecularShader() = 0;
		/**
		 *\~english
		 *\brief		Applies light position
		 *\remark		Shader implementation
		 *\~french
		 *\brief		Applique la position de la source
		 *\remark		Implémentation passant par les shaders
		 */
		virtual void ApplyPositionShader() = 0;
		/**
		 *\~english
		 *\brief		Applies light orientation
		 *\remark		Shader implementation
		 *\~french
		 *\brief		Applique l'orientation de la lumière
		 *\remark		Implémentation passant par les shaders
		 */
		virtual void ApplyOrientationShader() = 0;
		/**
		 *\~english
		 *\brief		Applies light constant attenuation
		 *\remark		Shader implementation
		 *\~french
		 *\brief		Applique l'attenuation constante de la source
		 *\remark		Implémentation passant par les shaders
		 */
		virtual void ApplyConstantAttShader( float p_fConstant) = 0;
		/**
		 *\~english
		 *\brief		Applies light linear attenuation
		 *\remark		Shader implementation
		 *\~french
		 *\brief		Applique l'attenuation linéaire de la source
		 *\remark		Implémentation passant par les shaders
		 */
		virtual void ApplyLinearAttShader( float p_fLinear) = 0;
		/**
		 *\~english
		 *\brief		Applies light quadratic attenuation
		 *\remark		Shader implementation
		 *\~french
		 *\brief		Applique l'attenuation quadratique de la source
		 *\remark		Implémentation passant par les shaders
		 */
		virtual void ApplyQuadraticAttShader( float p_fQuadratic) = 0;
		/**
		 *\~english
		 *\brief		Applies light exponent
		 *\remark		Shader implementation
		 *\~french
		 *\brief		Applique l'exposant de la source
		 *\remark		Implémentation passant par les shaders
		 */
		virtual void ApplyExponentShader( float p_fExponent) = 0;
		/**
		 *\~english
		 *\brief		Applies light cut off
		 *\remark		Shader implementation
		 *\~french
		 *\brief		Applique l'angle du cône de la source
		 *\remark		Implémentation passant par les shaders
		 */
		virtual void ApplyCutOffShader( float p_fCutOff) = 0;
	};
}

#pragma warning( pop )

#endif
