/******************************************************************************
 *
 *
 *
 *****************************************************************************/

/**
 * \defgroup lista Lista dwukierunkowa
 * @{
 *  Implementacja listy dwukierunkowej. Na listach przechowywane sa zadania w 
 *  stanie gotowym,
 *  zadania u�pione i struktury komunikacji miedzyzadaniowej z listami zadan oczekuj�cych na
 *  zas�b.
 */


#include "libos.h"

/**
 * Inicjalizuje liste dwukiernkowa. Wska�nik na pierwszy element i ostatni sa puste,
 * a liczba elementow przechowywanych na liscie 0.
 *
 * \param pCont Wska�nik do listy
 */
status_t queue_init(queue_t* pCont)
{
    if (pCont){
        pCont->pFirst = NULL;
        pCont->pLast  = NULL;
        pCont->dwLength = 0;
        return OK; 
    }else{
        return ERROR;
    }
}

/**
 * Inicjalizuje w�ze� listy ustawi�j�c wska�niki na nast�pny i poprzedni element
 * listy na NULL. Przekazywany parametr musi by� poprawnym wska�nikiem
 * \param pNode wska�nik do w�z�a
 */
void queue_init_node(node_t* pNode)
{
    pNode->pNext = NULL;
    pNode->pPrev = NULL;
}

/**
 * Wstawia element do listy za elementem o priorytecie r�wnym wstawianemu elementowi
 *
 * \param pCont Wska�nik do listy
 * \param pNode Wska�nik do wstawianego elementu
 */

status_t queue_insert(queue_t* pCont,node_prior_t* pNode)
{

    node_prior_t* current = (node_prior_t*)(pCont->pFirst);
    node_prior_t* prev    = NULL;
    UINT32 prior = pNode->dwPrior;
    
    if (!pCont->pFirst){
        pCont->pFirst = (node_t*)pNode;
        pCont->pLast  = (node_t*)pNode;
        /* kolejka byla pusta wiec na pewno bedzie miala rozmiar 1 */
	    pCont->dwLength=1; 
        pNode->pNext  = NULL;
        pNode->pPrev = NULL;
        return OK;
    }

    /* rozmiar kolejki zwiekszy sie o 1 */
    pCont->dwLength++;

    while(current){
        if (prior < current->dwPrior){
            if (!prev)
                pCont->pFirst = (node_t*)pNode;
            else
                prev->pNext = pNode;
            pNode->pNext   = current;
            pNode->pPrev   = prev;
            current->pPrev = pNode;
            return OK;
        }else{
            prev = current;
            current = current->pNext;
        }
     }
     prev->pNext  = pNode;
     pNode->pNext = NULL;
     pNode->pPrev = prev;
	 pCont->pLast = (node_t*)pNode;
     return OK;
}

/**
 * Wstawia element na pocz�tek listy
 *
 * \param pCont Wska�nik do listy
 * \param pNode Wska�nik do wstawianego elementu
 * \note Oba wska�niki musz� odnosi� si� do prawid�owych zmiennych
 */


UINT32 queue_push(queue_t* pCont,node_t* pNode)
{
    pCont->dwLength++;
    pNode->pPrev = NULL;
        
    if (pCont->pFirst){
        pNode->pNext = pCont->pFirst;
		(pCont->pFirst)->pPrev = pNode;
		pCont->pFirst = pNode;
    }else{
        pCont->pFirst = pNode;
	    pCont->pLast  = pNode;
        pNode->pNext = NULL;
    }
    return OK;

}

/**
 * Wstawia element na koniec listy
 *
 * \param pCont Wska�nik do listy
 * \param pNode Wska�nik do wstawianego elementu
 */

UINT32 queue_add(queue_t* pCont,node_t* pNode)
{
    pNode->pNext = 0;
    pCont->dwLength++;

    if (pCont->pLast){
	    (pCont->pLast)->pNext = pNode;
		pNode->pPrev = pCont->pLast;
		pCont->pLast = pNode;
    }else{
	    pCont->pFirst = pNode;
	    pCont->pLast  = pNode;
        pNode->pPrev  = NULL;
    }
    return OK;
}

/**
 * Zdejmuje element z pocz�tku listy
 *
 * \param pCont Wska�nik do listy
 * \note wska�nik musi byc odnosnikiem do prawidlowej struktury
 */

node_t* queue_pop(queue_t* pCont)
{

    node_t* tmp;
    node_t* new_first;

    if (!pCont->pFirst)
        return NULL;

    tmp =  pCont->pFirst;

    new_first = tmp->pNext;

	if (!new_first)
		pCont->pLast = NULL;
	else
		new_first->pPrev = NULL;

    pCont->pFirst = new_first;
	pCont->dwLength--;

    return tmp;
}

/**
 * Usuwa element z listy
 *
 * \param pCont Wska�nik do listy
 * \param pNode Wska�nik do usuwanego elementu
 *
 */


UINT32 queue_remove(queue_t* pCont,node_t* pNode)
{
	node_t* prev;
	node_t* next;
    
    if (pNode == pCont->pFirst){
        next = pNode->pNext;
        if (next){
            next->pPrev = NULL;
            pCont->pFirst = next;
        }else{
		    pCont->pLast = NULL;
            pCont->pFirst = NULL;
        }
    }else{
        if (pNode->pNext == NULL){
	        pCont->pLast = pNode->pPrev;
            (pCont->pLast)->pNext = NULL; /*usuniecie ostatniego elementu listy*/
        }else{
            prev = pNode->pPrev;
            next = pNode->pNext;
            prev->pNext = pNode->pNext;
            next->pPrev = pNode->pPrev;
        }

    }
/*        pNode->pNext = NULL;
        pNode->pPrev = NULL;*/
    pCont->dwLength--;
    return OK;
}

/** @} */
