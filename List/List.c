/******************************************************************************/
/*!
    @file     List.c
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

    @brief Linked lists
*/
/******************************************************************************/

/******************************************************************************/
/*!
 * Include
 */
/******************************************************************************/
#include "List.h"
#include <stddef.h>

/******************************************************************************/
/*!
 * Private module data
 */
/******************************************************************************/
static void (*DisableIRQ)(void);	/*!< set to disable/enable interrupts during list operations */
static void (*EnableIRQ)(void);		/*!< set to disable/enable interrupts during list operations */

/******************************************************************************/
/*!
 * Private functions
 */
/******************************************************************************/
static LIST_STATUS_T Scan(LIST_HANDLE_T list, LIST_NODE_HANDLE_T newNode)
{
    LIST_NODE_HANDLE_T node = list->Head;

    while (node != NULL)
    {
        if (node == newNode) return LIST_STATUS_DUPLICATE;
        node = node->Next;
    }
    return LIST_STATUS_OK;
}

/******************************************************************************/
/*!
 * Public functions
 */
/******************************************************************************/

/******************************************************************************/
/*!
 * @brief
 *
 * @param[in] disableIRQ
 * @param[in] enableIRQ
 * @return void
 */
/******************************************************************************/
void List_InitModule(void (*disableIRQ)(void), void (*enableIRQ)(void))
{
	DisableIRQ = disableIRQ;
	EnableIRQ = enableIRQ;
}

/******************************************************************************/
/*!
 * @brief  Initializes the list descriptor.
 *
 * @param[in] list - LIST_T * to init.
 * @param[in] max - Maximum number of nodes in list. 0 for unlimited.
 * @return void
 */
/******************************************************************************/
void List_Init(LIST_HANDLE_T list, uint32_t max)
{
    list->Head = NULL;
    list->Tail = NULL;
    list->Max  = (uint16_t)max;
    list->Size = 0;
}
/******************************************************************************/
/*!
 * @brief  Gets the list that contains the given node.
 *
 * @param[in] node - Handle of the node.
 * @return NULL if node is orphan.
 *         Handle of the list the node is inserted into.
 */
/******************************************************************************/
LIST_HANDLE_T List_GetList(LIST_NODE_HANDLE_T node)
{
    return node->List;
}

/******************************************************************************/
/*!
 * @brief  Gets the current size of a list.
 *
 * @param[in] list - handle of the list.
 * @return Current size of the list.
 */
/******************************************************************************/
uint32_t List_GetSize(LIST_HANDLE_T list)
{
    return list->Size;
}

/******************************************************************************/
/*!
 * @brief  Gets the number of free places in the list.
 *
 * @param[in] list - handle of the list.
 * @return Available size of the list.
 */
/******************************************************************************/
uint32_t List_GetAvailableSize(LIST_HANDLE_T list)
{
    return ((uint32_t)list->Max - (uint32_t)list->Size);
}

/******************************************************************************/
/*!
 * @brief  Links node to the tail of the list.
 *
 * @param[in] list - handle of list to insert into.
 * @param[in] node - node to add
 * @return LIST_STATUS_FULL if list is full.
 *         LIST_STATUS_OK if insertion was successful.
 */
/******************************************************************************/
LIST_STATUS_T List_AddTail(LIST_HANDLE_T list, LIST_NODE_HANDLE_T node)
{
	if (DisableIRQ) DisableIRQ();

    if ((list->Max != 0U) && (list->Max == list->Size))
    {
        if (EnableIRQ) EnableIRQ();
        return LIST_STATUS_FULL;
    }

    if (LIST_STATUS_DUPLICATE == Scan(list, node))
    {
        if (EnableIRQ) EnableIRQ();
        return LIST_STATUS_DUPLICATE;
    }

    if (list->Size == 0U)	list->Head = node;
    else					list->Tail->Next = node;

    node->Prev = list->Tail;
    node->Next = NULL;
    node->List = list;
    list->Tail    = node;
    list->Size++;

    if (EnableIRQ) EnableIRQ();
    return LIST_STATUS_OK;
}

/******************************************************************************/
/*!
 * @brief  Links node to the head of the list.
 *
 * @param[in] list - handle of list to insert into.
 * @param[in] node - node to add
 * @return LIST_STATUS_FULL if list is full.
 *         LIST_STATUS_OK if insertion was successful.
 */
/******************************************************************************/
LIST_STATUS_T List_AddHead(LIST_HANDLE_T list, LIST_NODE_HANDLE_T node)
{
	if (DisableIRQ) DisableIRQ();

    if ((list->Max != 0U) && (list->Max == list->Size))
    {
        if (EnableIRQ) EnableIRQ();
        return LIST_STATUS_FULL;
    }

    if (LIST_STATUS_DUPLICATE == Scan(list, node))
    {
        if (EnableIRQ) EnableIRQ();
        return LIST_STATUS_DUPLICATE;
    }

    if (list->Size == 0U)	list->Tail = node;
    else					list->Head->Prev = node;

    node->Next = list->Head;
    node->Prev = NULL;
    node->List = list;
    list->Head    = node;
    list->Size++;

    if (EnableIRQ) EnableIRQ();
    return LIST_STATUS_OK;
}

/******************************************************************************/
/*!
 * @brief  Unlinks node from the head of the list.
 *
 * @param[in] list - handle of list to remove from.
 * @return NULL if list is empty.
 *         handle of removed node(pointer) if removal was successful.
 */
/******************************************************************************/
LIST_NODE_HANDLE_T List_RemoveHead(LIST_HANDLE_T list)
{
    LIST_NODE_HANDLE_T node;

    if (DisableIRQ) DisableIRQ();

    if ((NULL == list) || (list->Size == 0U))
    {
        if (EnableIRQ) EnableIRQ();
        return NULL; /*List_ is empty*/
    }

    node = list->Head;
    list->Size--;
    if (list->Size == 0U)	list->Tail = NULL;
    else					node->Next->Prev = NULL;

    list->Head = node->Next; /*Is NULL if node is head*/
    node->List = NULL;

    if (EnableIRQ) EnableIRQ();
    return node;
}

/******************************************************************************/
/*!
 * @brief  Gets head node handle.
 *
 * @param[in] list - handle of list.
 * @return NULL if list is empty.
 *         handle of head node if list is not empty.
 */
/******************************************************************************/
LIST_NODE_HANDLE_T List_GetHead(LIST_HANDLE_T list)
{
    return list->Head;
}

/******************************************************************************/
/*!
 * @brief  Gets next node handle.
 *
 * @param[in] node - handle of the node.
 * @return NULL if node is tail.
 *         handle of next node if exists.
 */
/******************************************************************************/
LIST_NODE_HANDLE_T List_GetNext(LIST_NODE_HANDLE_T node)
{
    return node->Next;
}

/******************************************************************************/
/*!
 * @brief  Gets previous node handle.
 *
 * @param[in] node - handle of the node.
 * @return NULL if node is head.
 *         handle of previous node if exists.
 */
/******************************************************************************/
LIST_NODE_HANDLE_T List_GetPrev(LIST_NODE_HANDLE_T node)
{
    return node->Prev;
}

/******************************************************************************/
/*!
 * @brief  Unlinks an node from its list.
 *
 * @param[in] node - handle of the node to remove.
 * @return LIST_STATUS_ORPHAN_NODE if node is not part of any list.
 *         LIST_STATUS_OK if removal was successful.
 */
/******************************************************************************/
LIST_STATUS_T List_RemoveNode(LIST_NODE_HANDLE_T node)
{
    if (node->List == NULL)
        return LIST_STATUS_ORPHAN_NODE; /*Node was previously removed or never added*/

    if (DisableIRQ) DisableIRQ();

    if (node->Prev == NULL) /*Node is head or solo*/
        node->List->Head = node->Next; /*is null if solo*/
    if (node->Next == NULL) /*Node is tail or solo*/
        node->List->Tail = node->Prev; /*is null if solo*/
    if (node->Prev != NULL) /*Node is not head*/
        node->Prev->Next = node->Next;
    if (node->Next != NULL) /*Node is not tail*/
        node->Next->Prev = node->Prev;

    node->List->Size--;
    node->List = NULL;

    if (EnableIRQ) EnableIRQ();
    return LIST_STATUS_OK;
}

/******************************************************************************/
/*!
 * @brief  Links an node in the previous position relative to a given member
 *         of a list.
 *
 * @param[in] node - handle of a member of a list.
 * @param[in] newNode - new node to insert before the given member.
 * @return LIST_STATUS_ORPHAN_NODE if node is not part of any list.
 *         LIST_STATUS_FULL if list is full.
 *         LIST_STATUS_OK if insertion was successful.
 */
/******************************************************************************/
LIST_STATUS_T List_InsertNodePrev(LIST_NODE_HANDLE_T node, LIST_NODE_HANDLE_T newNode)
{
    if (node->List == NULL)	return LIST_STATUS_ORPHAN_NODE; /*Node was previusly removed or never added*/

    if (DisableIRQ) DisableIRQ();

    if ((node->List->Max != 0U) && (node->List->Max == node->List->Size))
    {
        if (EnableIRQ) EnableIRQ();
        return LIST_STATUS_FULL;
    }

    if (LIST_STATUS_DUPLICATE == Scan(node->List, newNode))
    {
        if (EnableIRQ) EnableIRQ();
        return LIST_STATUS_DUPLICATE;
    }

    if (node->Prev == NULL) /*Node is list head*/
        node->List->Head = newNode;
    else
        node->Prev->Next = newNode;

    newNode->List = node->List;
    node->List->Size++;
    newNode->Next = node;
    newNode->Prev = node->Prev;
    node->Prev    = newNode;

    if (EnableIRQ) EnableIRQ();
    return LIST_STATUS_OK;
}
