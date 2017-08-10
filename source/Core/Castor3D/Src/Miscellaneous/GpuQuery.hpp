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

namespace castor3d
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
		: public castor::OwnedBy< RenderSystem >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	renderSystem	The RenderSystem.
		 *\param[in]	p_query			The query type.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le RenderSystem.
		 *\param[in]	p_query			Le type de requête.
		 */
		C3D_API GpuQuery( RenderSystem & renderSystem, QueryType p_query );
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
		C3D_API bool initialise();
		/**
		 *\~english
		 *\brief		Destroys the query on GPU.
		 *\~french
		 *\brief		Détruit la requête sur le GPU.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Begins the query.
		 *\~french
		 *\brief		Démarre la requête.
		 */
		C3D_API void begin();
		/**
		 *\~english
		 *\brief		Ends the query.
		 *\~french
		 *\brief		Termine la requête.
		 */
		C3D_API void end();
		/**
		 *\~english
		 *\brief		Retrieves query information.
		 *\param[in]	p_infos	The information to retrieve.
		 *\param[out]	p_param	Receives the result.
		 *\~french
		 *\brief		Récupère une information sur la requête.
		 *\param[in]	p_infos	L'information à récupérer.
		 *\param[out]	p_param	Reçoit le résultat.
		 */
		template< typename T >
		inline void getInfos( QueryInfo p_infos, T & p_param )const
		{
			doGetInfos( p_infos, p_param );
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
		C3D_API virtual bool doInitialise() = 0;
		/**
		 *\~english
		 *\brief		Destroys the query on GPU.
		 *\~french
		 *\brief		Détruit la requête sur le GPU.
		 */
		C3D_API virtual void doCleanup() = 0;
		/**
		 *\~english
		 *\brief		Begins the query.
		 *\~french
		 *\brief		Démarre la requête.
		 */
		C3D_API virtual void doBegin()const = 0;
		/**
		 *\~english
		 *\brief		Ends the query.
		 *\~french
		 *\brief		Termine la requête.
		 */
		C3D_API virtual void doEnd()const = 0;
		/**
		 *\~english
		 *\brief		Retrieves query information.
		 *\param[in]	p_infos	The information to retrieve.
		 *\param[out]	p_param	Receives the result.
		 *\~french
		 *\brief		Récupère une information sur la requête.
		 *\param[in]	p_infos	L'information à récupérer.
		 *\param[out]	p_param	Reçoit le résultat.
		 */
		C3D_API virtual void doGetInfos( QueryInfo p_infos, int32_t & p_param )const = 0;
		/**
		 *\~english
		 *\brief		Retrieves query information.
		 *\param[in]	p_infos	The information to retrieve.
		 *\param[out]	p_param	Receives the result.
		 *\~french
		 *\brief		Récupère une information sur la requête.
		 *\param[in]	p_infos	L'information à récupérer.
		 *\param[out]	p_param	Reçoit le résultat.
		 */
		C3D_API virtual void doGetInfos( QueryInfo p_infos, uint32_t & p_param )const = 0;
		/**
		 *\~english
		 *\brief		Retrieves query information.
		 *\param[in]	p_infos	The information to retrieve.
		 *\param[out]	p_param	Receives the result.
		 *\~french
		 *\brief		Récupère une information sur la requête.
		 *\param[in]	p_infos	L'information à récupérer.
		 *\param[out]	p_param	Reçoit le résultat.
		 */
		C3D_API virtual void doGetInfos( QueryInfo p_infos, int64_t & p_param )const = 0;
		/**
		 *\~english
		 *\brief		Retrieves query information.
		 *\param[in]	p_infos	The information to retrieve.
		 *\param[out]	p_param	Receives the result.
		 *\~french
		 *\brief		Récupère une information sur la requête.
		 *\param[in]	p_infos	L'information à récupérer.
		 *\param[out]	p_param	Reçoit le résultat.
		 */
		C3D_API virtual void doGetInfos( QueryInfo p_infos, uint64_t & p_param )const = 0;

	protected:
		//!\~english	Tells if the query is active.
		//!\~french		Dit si la requête est active.
		bool m_active{ false };
		//!\~english	The query type.
		//!\~french		Le type de requête.
		QueryType m_type;
	};
}

#endif
