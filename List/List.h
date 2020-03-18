/******************************************************************************/
/*!
    @file     List.h
    @author   FireSourcery / Kelly Controls Inc

    @section LICENSE

    Copyright (C) 2020 Kelly Controls Inc

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    @brief Header file for List.c
*/
/******************************************************************************/
#ifndef _LIST_H_
#define _LIST_H_

/*!
 * @addtogroup List
 * @{
 */

/******************************************************************************/
/*!
 *  Include
 */
/******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/
/*!
 *  Public macro definitions
 */
/******************************************************************************/
#define GET_CONTAINER_STRUCT_POINTER(p_node, CONTAINER_STRUCT_TYPE, NodeVarName) ((CONTAINER_STRUCT_TYPE *)(((uintptr_t)p_node) - offsetof(CONTAINER_STRUCT_TYPE, NodeVarName)))

/******************************************************************************/
/*!
 *  Public type definitions
 */
/******************************************************************************/
/*! @brief The list status */
typedef enum
{
	LIST_STATUS_OK = 0, 		/*!< Success */
	LIST_STATUS_DUPLICATE, 		/*!< Duplicate Error */
	LIST_STATUS_FULL, 			/*!< Full */
	LIST_STATUS_EMPTY, 			/*!< Empty */
	LIST_STATUS_ORPHAN_NODE, 	/*!< Orphan Node */
} LIST_STATUS_T;

/*! @brief The list structure */
typedef struct List
{
	struct ListNode * Head; 	/*!< list head */
	struct ListNode * Tail; 	/*!< list tail */
//	struct ListIterator * Iterator; /*!< list iterator */
	uint16_t Size;             /*!< list size */
	uint16_t Max;              /*!< list max number of nodes */
} LIST_T, *LIST_HANDLE_T;

/*! @brief The list node */
typedef struct ListNode
{
	struct ListNode 	* Next;	/*!< next list node */
	struct ListNode 	* Prev;	/*!< previous list node */
	struct List 		* List;	/*!< pointer to the list */
//	void 				* Data; /*!< pointer to the payload data */
} LIST_NODE_T, *LIST_NODE_HANDLE_T;

/******************************************************************************/
/*!
 *  Public prototypes
 */
/******************************************************************************/
/******************************************************************************/
// API
/******************************************************************************/

/*!
 * @brief Initialize the list.
 *
 * This function initialize the list.
 *
 * @param[in] list - List handle to initialize.
 * @param[in] max - Maximum number of nodes in list. 0 for unlimited.
 */
void List_Init(LIST_HANDLE_T list, uint32_t max);

/*!
 * @brief Gets head node handle.
 *
 * @param[in] list - Handle of the list.
 * @retval NULL if list is empty, handle of of head node(pointer) if list is not empty.
 */
LIST_NODE_HANDLE_T List_GetHead(LIST_HANDLE_T list);

/*!
 * @brief Gets the current size of a list.
 *
 * @param[in] list - Handle of the list.
 * @retval Current size of the list.
 */
uint32_t List_GetSize(LIST_HANDLE_T list);

/*!
 * @brief Gets the number of free places in the list.
 *
 * @param[in] list - Handle of the list.
 * @retval Available size of the list.
 */
uint32_t List_GetAvailableSize(LIST_HANDLE_T list);

/*!
 * @brief Links node to the head of the list.
 *
 * @param[in] list - Handle of the list.
 * @param[in] node - Handle of the node.
 * @retval LIST_STATUS_FULL if list is full, LIST_STATUS_OK if insertion was successful.
 */
LIST_STATUS_T List_AddHead(LIST_HANDLE_T list, LIST_NODE_HANDLE_T node);

/*!
 * @brief Links node to the tail of the list.
 *
 * @param[in] list - Handle of the list.
 * @param[in] node - Handle of the node.
 * @retval LIST_STATUS_FULL if list is full, LIST_STATUS_OK if insertion was successful.
 */
LIST_STATUS_T List_AddTail(LIST_HANDLE_T list, LIST_NODE_HANDLE_T node);

/*!
 * @brief Unlinks node from the head of the list.
 *
 * @param[in] list - Handle of the list.
 * @retval NULL if list is empty, handle of removed node(pointer) if removal was successful.
 */
LIST_NODE_HANDLE_T List_RemoveHead(LIST_HANDLE_T list);

/*!
 * @brief Gets the list that contains the given node.
 *
 * @param[in] node - Handle of the node.
 * @retval NULL if node is orphan, Handle of the list the node is inserted into.
 */
LIST_HANDLE_T List_GetList(LIST_NODE_HANDLE_T node);

/*!
 * @brief Gets next node handle for given node handle.
 *
 * @param[in] node - Handle of the node.
 * @retval NULL if list is empty, handle of of next node(pointer) if list is not empty.
 */
LIST_NODE_HANDLE_T List_GetNext(LIST_NODE_HANDLE_T node);

/*!
 * @brief Gets previous node handle for given node handle.
 *
 * @param[in] node - Handle of the node.
 * @retval NULL if list is empty, handle of of prev node(pointer) if list is not empty.
 */
LIST_NODE_HANDLE_T List_GetPrev(LIST_NODE_HANDLE_T node);

/*!
 * @brief Unlinks an node from its list.
 *
 * @param[in] node - Handle of the node.
 * @retval LIST_STATUS_ORPHAN_NODE if node is not part of any list.
 * @retval LIST_STATUS_OK if removal was successful.
 */
LIST_STATUS_T List_RemoveNode(LIST_NODE_HANDLE_T node);

/*!
 * @brief Links an node in the previous position relative to a given member of a list.
 *
 * @param[in] node - Handle of the node.
 * @param[in] newNode - New node to insert before the given member.
 * @retval LIST_STATUS_ORPHAN_NODE if node is not part of any list.
 * @retval LIST_STATUS_OK if removal was successful.
 */
LIST_STATUS_T List_InsertNodePrev(LIST_NODE_HANDLE_T node, LIST_NODE_HANDLE_T newNode);

/*! @}*/
#endif
