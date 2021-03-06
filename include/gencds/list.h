/*
  Copyright 2006 Mattias Holm <mattias.holm(at)openorbit.org>

  This file is part of Open Orbit. Open Orbit is free software: you can
  redistribute it and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  You should have received a copy of the GNU General Public License
  along with Open Orbit.  If not, see <http://www.gnu.org/licenses/>.

  Some files of Open Orbit have relaxed licensing conditions. This file is
  licenced under the 2-clause BSD licence.

  Redistribution and use of this file in source and binary forms, with or
  without modification, are permitted provided that the following conditions are
  met:
 
    - Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    - Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
 
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
 
/*!
 * \file list.h
 * \brief List data structures and functions.
 * 
 * This file contain the public inteface to the list data structure.
 * */

#ifndef LIST_H_
#define LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

/*!
 * \brief A node in the list.
 * 
 * The list_entry_t can only be used as with a pointer interface. The real
 * implementation is hidden in order to ensure that the data structures can be
 * updated.
 * 
 * In the first implementation these are simple nodes containing next, previous
 * and data pointers. The internals might change, non the less. The
 * list_entry_t will continue to exist, even if it is just in the form of an
 * iterator.
 * */
typedef struct _list_entry_t list_entry_t;

/*!
 * \brief A list object.
 * 
 * The list_t type represent a list ADT, the first version was implemented as a
 * linked list of list_entry_t nodes. Note that the internals are subject to
 * change, so do not use the internal structure in any way. Use only the public
 * functions.
 * */
typedef struct _list_t list_t;

list_t *list_new(void);
void list_delete(list_t *list);

size_t list_length(const list_t *list);
list_entry_t *list_first(const list_t *list);
list_entry_t *list_last(const list_t *list);
list_entry_t *list_rank(const list_t *list, unsigned int rank);

list_entry_t *list_entry_next(const list_entry_t *entry);
list_entry_t *list_entry_previous(const list_entry_t *entry);
void *list_entry_data(const list_entry_t *entry);

list_entry_t* list_find_entry(list_t *list, void *obj);


void list_append(list_t *list, void *obj);
void list_insert(list_t *list, void *obj);

void* list_remove_first(list_t *list);
void* list_remove_last(list_t *list);
void* list_remove_entry(list_t *list, list_entry_t *entry);

/*!
 * \brief Applies any function that take the list object as first parameter to
 *      all list objects.
 * 
 * \param lst A list_t object.
 * \param fn The function to apply to the list objects.
 * \param ... An arbitrary number of arguments (zero or more) to pass into the
 *      function after the object argument.
 * */
#define LIST_APPLY(lst, fn, ...)                        \
do {                                                    \
    list_entry_t *entry = list_first(lst);              \
                                                        \
    while (entry) {                                     \
        (fn)(list_entry_data(entry), ## __VA_ARGS__);   \
        entry = list_entry_next(entry);                 \
    }                                                   \
} while (0)


#define LIST_HEAD(TYPE) struct { struct TYPE *head; struct TYPE *tail; }
#define LIST_FIRST(LIST) ((LIST).head)
#define LIST_LAST(LIST) ((LIST).tail)

#define LIST_ENTRY(TYPE) struct { struct TYPE *next; struct TYPE *prev; }

#define LIST_CONCAT(LISTA, LISTB)               \
do {                                            \
  (LIST_LAST(LISTA))->next = LIST_FIRST(LISTB); \
  LIST_FIRST(LISTB)->prev = LIST_LAST(LISTA);   \
  LIST_LAST(LISTA) = LIST_LAST(LISTB);          \
} while (0)

#define LIST_APPEND(LIST,ELEM,ENTRY)    \
do {                                    \
  if (LIST_LAST(LIST)) {                \
    LIST_LAST(LIST)->ENTRY.next = ELEM; \
    ELEM->ENTRY.prev = LIST_LAST(LIST); \
    LIST_LAST(LIST) = ELEM;             \
  } else {                              \
    LIST_FIRST(LIST) = ELEM;            \
    LIST_LAST(LIST) = ELEM;             \
    ELEM->ENTRY.next = NULL;            \
    ELEM->ENTRY.prev = NULL;            \
  }                                     \
} while (0)
#define LIST_EMPTY(head) ((head).head == NULL)
#define LIST_NEXT(elem, field) ((elem)->field.next)

#define LIST_FOREACH(typ, val, lhead, field) \
for (struct typ* val = (lhead).head ; val != NULL ; val = LIST_NEXT(val, field))

#ifdef __cplusplus
}
#endif

#endif /*LIST_H_*/
