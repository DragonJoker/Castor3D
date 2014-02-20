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
#ifndef ___C3D_BufferElement___
#define ___C3D_BufferElement___

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Element usage enumeration
	\~french
	\brief		Enumération des utilisations d'éléments de tampon
	*/
	typedef enum eELEMENT_USAGE CASTOR_TYPE( uint8_t )
	{	eELEMENT_USAGE_POSITION		//!< Position coords
	,	eELEMENT_USAGE_NORMAL		//!< Normal coords
	,	eELEMENT_USAGE_TANGENT		//!< Tangent coords
	,	eELEMENT_USAGE_BITANGENT	//!< Bitangent coords
	,	eELEMENT_USAGE_DIFFUSE		//!< Diffuse colour
	,	eELEMENT_USAGE_TEXCOORDS0	//!< Texture coordinates 0
	,	eELEMENT_USAGE_TEXCOORDS1	//!< Texture coordinates 1
	,	eELEMENT_USAGE_TEXCOORDS2	//!< Texture coordinates 2
	,	eELEMENT_USAGE_TEXCOORDS3	//!< Texture coordinates 3
	,	eELEMENT_USAGE_BONE_IDS		//!< Bone IDs
	,	eELEMENT_USAGE_BONE_WEIGHTS	//!< Bone weights
	,	eELEMENT_USAGE_COUNT
	}	eELEMENT_USAGE;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Element type enumeration
	\~french
	\brief		Enumération des types pour les éléments de tampon
	*/
	typedef enum eELEMENT_TYPE CASTOR_TYPE( uint8_t )
	{	eELEMENT_TYPE_1FLOAT	//!< 1 float
	,	eELEMENT_TYPE_2FLOATS	//!< 2 floats
	,	eELEMENT_TYPE_3FLOATS	//!< 3 floats
	,	eELEMENT_TYPE_4FLOATS	//!< 4 floats
	,	eELEMENT_TYPE_COLOUR	//!< colour (uint32_t)
	,	eELEMENT_TYPE_1INT		//!< 1 int (4 bytes each)
	,	eELEMENT_TYPE_2INTS		//!< 2 ints (4 bytes each)
	,	eELEMENT_TYPE_3INTS		//!< 3 ints (4 bytes each)
	,	eELEMENT_TYPE_4INTS		//!< 4 ints (4 bytes each)
	,	eELEMENT_TYPE_COUNT
	}	eELEMENT_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Buffer element description
	\remark		Describes usage and type of an element in a vertex buffer
	\~french
	\brief		Description d'un élément de tampon
	\remark		Décrit l'utilisation et le type d'un élément de tampon de sommets
	*/
	struct BufferElementDeclaration
	{
		//!\~english	Stream index	\~french	Index du flux
		uint32_t m_uiStream;
		//!\~english	Element usage	\~french	Utilisation de l'élément
		eELEMENT_USAGE m_eUsage;
		//!\~english	Element type	\~french	Type de l'élément
		eELEMENT_TYPE m_eDataType;
		//!\~english	Offset in stream	\~french	Offset dans le tampon
		uint32_t m_uiOffset;
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		BufferElementDeclaration()
			:	m_uiStream	()
			,	m_eUsage	()
			,	m_eDataType	()
			,	m_uiOffset	()
		{
		}
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_uiStream	Stream index
		 *\param[in]	p_eUsage	Element usage
		 *\param[in]	p_eType		Element type
		 *\param[in]	p_uiOffset	Offset in the stream
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_uiStream	Index du flux
		 *\param[in]	p_eUsage	Utilisation de l'élément
		 *\param[in]	p_eType		Type de l'élément
		 *\param[in]	p_uiOffset	Offset dans le tampon
		 */
		BufferElementDeclaration( uint32_t p_uiStream, eELEMENT_USAGE p_eUsage, eELEMENT_TYPE p_eType, uint32_t p_uiOffset=0 )
			:	m_uiStream	( p_uiStream	)
			,	m_eUsage	( p_eUsage		)
			,	m_eDataType	( p_eType		)
			,	m_uiOffset	( p_uiOffset	)
		{
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Description of all elements in a vertex buffer
	\~french
	\brief		Description de tous les éléments dans un tampon de sommets
	*/
	class C3D_API BufferDeclaration
	{
	public:
		DECLARE_VECTOR( BufferElementDeclaration, BufferElementDeclaration );

	protected:
		friend void swap( BufferDeclaration & p_obj1, BufferDeclaration & p_obj2 );
		//!\~english	Element description array	\~french	Tableau de descriptions d'éléments
		BufferElementDeclarationArray m_arrayElements;
		//!\~english	Byte size of the element	\~french	Taille de l'élément, en octets
		uint32_t m_uiStride;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_elements	The elements array
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_elements	Tableau des éléments
		 */
		template< uint32_t N >
		BufferDeclaration( BufferElementDeclaration const ( & p_elements )[N] )
		{
			DoInitialise( p_elements, N );
		}
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pElements		The elements array
		 *\param[in]	p_uiNbElements	Elements array size
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pElements		Tableau d'éléments
		 *\param[in]	p_uiNbElements	Taille du tableau d'éléments
		 */
		BufferDeclaration( BufferElementDeclaration const * p_pElements, uint32_t p_uiNbElements );
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_declaration	The BufferDeclaration object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_declaration	L'objet BufferDeclaration à copier
		 */
		BufferDeclaration( BufferDeclaration const & p_declaration );
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	p_declaration	The BufferDeclaration object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_declaration	L'objet BufferDeclaration à déplacer
		 */
		BufferDeclaration( BufferDeclaration && p_declaration );
		/**
		 *\~english
		 *\brief		Assignment operator
		 *\param[in]	p_declaration	The BufferDeclaration object to copy
		 *\return		A reference to this6.5.0 BufferDeclaration object
		 *\~french
		 *\brief		Opérateur d'affectation
		 *\param[in]	p_declaration	L'objet BufferDeclaration à copier
		 *\return		Une référence sur cet objet BufferDeclaration
		 */
		BufferDeclaration & operator =( BufferDeclaration p_declaration );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~BufferDeclaration();
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the begin of the elements array
		 *\return		Constant iterator to the begin of the elements array
		 *\~french
		 *\brief		Récupère un itérateur sur le début du tableau d'éléments
		 *\return		Itérateur constant sur le début du tableau d'éléments
		 */
		inline BufferElementDeclarationArrayConstIt	Begin()const { return m_arrayElements.begin(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the elements array
		 *\return		Constant iterator to the end of the elements array
		 *\~french
		 *\brief		Récupère un itérateur sur la fin du tableau d'éléments
		 *\return		Itérateur constant sur la fin du tableau d'éléments
		 */
		inline BufferElementDeclarationArrayConstIt	End()const { return m_arrayElements.end(); }
		/**
		 *\~english
		 *\brief		Retrieves the elements count
		 *\return		The elements count
		 *\~french
		 *\brief		Récupère le compte des éléments
		 *\return		Le compte des éléments
		 */
		inline uint32_t Size()const { return uint32_t( m_arrayElements.size() );	}
		/**
		 *\~english
		 *\brief		Retrieves the total elements byte count
		 *\return		The byte count
		 *\~french
		 *\brief		Récupère la taille totale en octet des éléments
		 *\return		La taille en octets
		 */
		inline uint32_t GetStride()const { return m_uiStride; }
		/**
		 *\~english
		 *\brief		Array accessor
		 *\remark		That function doesn't check if index is out of bound so expect crashes if used incorrectly
		 *\param[in]	p_uiIndex	The index
		 *\return		The element at given index
		 *\~french
		 *\brief		Accesseur de type tableau
		 *\remark		Cette fonction ne vérifie pas la validité de l'index donné, donc attention aux crashes si mal utilisée
		 *\param[in]	p_uiIndex	L'index
		 *\return		L'élément à l'index donné
		 */
		inline BufferElementDeclaration const & operator []( uint32_t p_uiIndex )const { return m_arrayElements[p_uiIndex]; }
		/**
		 *\~english
		 *\brief		Array accessor
		 *\remark		That function doesn't check if index is out of bound so expect crashes if used incorrectly
		 *\param[in]	p_uiIndex	The index
		 *\return		The element at given index
		 *\~french
		 *\brief		Accesseur de type tableau
		 *\remark		Cette fonction ne vérifie pas la validité de l'index donné, donc attention aux crashes si mal utilisée
		 *\param[in]	p_uiIndex	L'index
		 *\return		L'élément à l'index donné
		 */
		inline BufferElementDeclaration & operator []( uint32_t p_uiIndex ) { return m_arrayElements[p_uiIndex]; }

	private:
		void DoInitialise( BufferElementDeclaration const * p_pElements, uint32_t p_uiNbElements );
	};
	/**
	 *\~english
	 *\brief		Swapping function
	 *\param[in]	p_obj1, p_obj2	The 2 objects to swap
	 *\~french
	 *\brief		Fonction d'échange
	 *\param[in]	p_obj1, p_obj2	Les 2 objets à échanger
	 */
	void swap( BufferDeclaration & p_obj1, BufferDeclaration & p_obj2 );
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Holds data of a vertex buffer elements group
	\~french
	\brief		Contient les données d'un groupe d'éléments de tampon de sommets
	*/
	class C3D_API BufferElementGroup
	{
	protected:
		//!\~english	The elements values	\~french Les valeurs des éléments
		uint8_t * m_pBuffer;
		//!\~english	The group index	\~french	L'indice du groupe
		uint32_t m_uiIndex;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pBuffer		The data buffer
		 *\param[in]	p_uiIndex		The group index
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pBuffer		Le tampon de données
		 *\param[in]	p_uiIndex		L'indice du groupe
		 */
		BufferElementGroup( uint8_t * p_pBuffer=NULL, uint32_t p_uiIndex=0 );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~BufferElementGroup();
		/**
		 *\~english
		 *\brief		Links the element values to the ones in parameter.
		 *\~french
		 *\brief		Lie les valeurs de l'élément à celles données en paramètre.
		 */
		void LinkCoords( uint8_t * p_pBuffer );
		/**
		 *\~english
		 *\brief		Links the element values to the ones in parameter.
		 *\remark		Replace buffer values by element values
		 *\param[in]	p_pBuffer	The values buffer
		 *\~french
		 *\brief		Lie les valeurs de l'élément à celles données en paramètre.
		 *\remark		Remplace les valeurs du tampon par celles de l'élément
		 *\param[in]	p_pBuffer	Le tampon de valeurs
		 */
		void LinkCoords( uint8_t * p_pBuffer, uint32_t p_uiSize );
		/**
		 *\~english
		 *\brief		Retrieves a pointer on the data buffer
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère un pointeur sur le tampon de données
		 *\return		Le pointeur
		 */
		inline uint8_t * ptr() { return m_pBuffer; }
		/**
		 *\~english
		 *\brief		Retrieves a constant pointer on the data buffer
		 *\return		The pointer
		 *\~french
		 *\brief		Récupère un pointeur constant sur le tampon de données
		 *\return		Le pointeur
		 */
		inline uint8_t const * const_ptr()const { return m_pBuffer; }
		/**
		 *\~english
		 *\brief		Retrieves the group index
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'indice du groupe
		 *\return		La valeur
		 */
		inline uint32_t GetIndex()const { return m_uiIndex; }
		/**
		 *\~english
		 *\brief		Sets the group index
		 *\param[in]	p_uiIndex	The new value
		 *\~french
		 *\brief		Définit l'indice du groupe
		 *\param[in]	p_uiIndex	La nouvelle valeur
		 */
		inline void SetIndex( uint32_t p_uiIndex ) { m_uiIndex = p_uiIndex; }
	};
}

#pragma warning( pop )

#endif
