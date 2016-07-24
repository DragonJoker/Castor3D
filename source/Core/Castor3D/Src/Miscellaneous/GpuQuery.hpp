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
#ifndef ___C3D_QUERY_H___
#define ___C3D_QUERY_H___

#include "Castor3DPrerequisites.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date		16/03/2016
	\version	0.8.0
	\~english
	\brief		GPU query implementation.
	\~french
	\brief		Implémentation d'une requête GPU.
	*/
	class GpuQuery
		: public Castor::OwnedBy< RenderSystem >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_renderSystem	The RenderSystem.
		 *\param[in]	p_query			The query type.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_renderSystem	Le RenderSystem.
		 *\param[in]	p_query			Le type de requête.
		 */
		C3D_API GpuQuery( RenderSystem & p_renderSystem, eQUERY_TYPE p_query );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~GpuQuery();
		/**
		 *\~english
		 *\brief		Creates the query on GPU.
		 *\return		\p true if everything is OK.
		 *\~french
		 *\brief		Crée la requête sur le GPU.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool Create();
		/**
		 *\~english
		 *\brief		Destroys the query on GPU.
		 *\~french
		 *\brief		Détruit la requête sur le GPU.
		 */
		C3D_API void Destroy();
		/**
		 *\~english
		 *\brief		Begins the query.
		 *\return		\p true if everything is OK.
		 *\~french
		 *\brief		Démarre la requête.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool Begin();
		/**
		 *\~english
		 *\brief		Ends the query.
		 *\~french
		 *\brief		Termine la requête.
		 */
		C3D_API void End();
		/**
		 *\~english
		 *\brief		Retrieves query information.
		 *\param[in]	p_infos	The information to retrieve.
		 *\param[out]	p_param	Receives the result.
		 *\return		\p true if the information was retrieved.
		 *\~french
		 *\brief		Récupère une information sur la requête.
		 *\param[in]	p_infos	L'information à récupérer.
		 *\param[out]	p_param	Reçoit le résultat.
		 *\return		\p true si l'information a été récupérée.
		 */
		template< typename T >
		inline bool GetInfos( eQUERY_INFO p_infos, T & p_param )const
		{
			return DoGetInfos( p_infos, p_param );
		}

	protected:
		/**
		 *\~english
		 *\brief		Creates the query on GPU.
		 *\return		\p true if everything is OK.
		 *\~french
		 *\brief		Crée la requête sur le GPU.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API virtual bool DoCreate() = 0;
		/**
		 *\~english
		 *\brief		Destroys the query on GPU.
		 *\~french
		 *\brief		Détruit la requête sur le GPU.
		 */
		C3D_API virtual void DoDestroy() = 0;
		/**
		 *\~english
		 *\brief		Begins the query.
		 *\return		\p true if everything is OK.
		 *\~french
		 *\brief		Démarre la requête.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API virtual bool DoBegin()const = 0;
		/**
		 *\~english
		 *\brief		Ends the query.
		 *\~french
		 *\brief		Termine la requête.
		 */
		C3D_API virtual void DoEnd()const = 0;
		/**
		 *\~english
		 *\brief		Retrieves query information.
		 *\param[in]	p_infos	The information to retrieve.
		 *\param[out]	p_param	Receives the result.
		 *\return		\p true if the information was retrieved.
		 *\~french
		 *\brief		Récupère une information sur la requête.
		 *\param[in]	p_infos	L'information à récupérer.
		 *\param[out]	p_param	Reçoit le résultat.
		 *\return		\p true si l'information a été récupérée.
		 */
		C3D_API virtual bool DoGetInfos( eQUERY_INFO p_infos, int32_t & p_param )const = 0;
		/**
		 *\~english
		 *\brief		Retrieves query information.
		 *\param[in]	p_infos	The information to retrieve.
		 *\param[out]	p_param	Receives the result.
		 *\return		\p true if the information was retrieved.
		 *\~french
		 *\brief		Récupère une information sur la requête.
		 *\param[in]	p_infos	L'information à récupérer.
		 *\param[out]	p_param	Reçoit le résultat.
		 *\return		\p true si l'information a été récupérée.
		 */
		C3D_API virtual bool DoGetInfos( eQUERY_INFO p_infos, uint32_t & p_param )const = 0;
		/**
		 *\~english
		 *\brief		Retrieves query information.
		 *\param[in]	p_infos	The information to retrieve.
		 *\param[out]	p_param	Receives the result.
		 *\return		\p true if the information was retrieved.
		 *\~french
		 *\brief		Récupère une information sur la requête.
		 *\param[in]	p_infos	L'information à récupérer.
		 *\param[out]	p_param	Reçoit le résultat.
		 *\return		\p true si l'information a été récupérée.
		 */
		C3D_API virtual bool DoGetInfos( eQUERY_INFO p_infos, int64_t & p_param )const = 0;
		/**
		 *\~english
		 *\brief		Retrieves query information.
		 *\param[in]	p_infos	The information to retrieve.
		 *\param[out]	p_param	Receives the result.
		 *\return		\p true if the information was retrieved.
		 *\~french
		 *\brief		Récupère une information sur la requête.
		 *\param[in]	p_infos	L'information à récupérer.
		 *\param[out]	p_param	Reçoit le résultat.
		 *\return		\p true si l'information a été récupérée.
		 */
		C3D_API virtual bool DoGetInfos( eQUERY_INFO p_infos, uint64_t & p_param )const = 0;

	protected:
		//!\~english Tells if the query is active.	\~french Dit si la requête est active.
		bool m_active{ false };
		//!\~english The query type.	\~french Le type de requête.
		eQUERY_TYPE m_type;
	};
}

#endif
