/*
    lmap -- recursive mutex implementation

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

    $Header$
*/

#include <lmap.h>
#include <lmap_recursive_mutex.h>

#include <pthread.h>


struct recursive_mutex_list {
    pthread_mutex_t *mx;
    pthread_t       id;
    unsigned long   count;
                    LIST_ENTRY(recursive_mutex_list) next;
};


pthread_mutex_t recursive_mutex_list_mx = PTHREAD_MUTEX_INITIALIZER;
LIST_HEAD(, recursive_mutex_list) recursive_mutex_list_head =
LIST_HEAD_INITIALIZER(head);


int
recursive_mutex_lock(pthread_mutex_t * mutex)
{
    struct recursive_mutex_list *current,
                   *newelem;
    pthread_t       me;

    pthread_mutex_lock(&recursive_mutex_list_mx);

    me = pthread_self();

    LIST_FOREACH(current, &recursive_mutex_list_head, next)
	if (mutex == current->mx && pthread_equal(me, current->id) != 0) {
	current->count++;
	pthread_mutex_unlock(&recursive_mutex_list_mx);
	return 0;
    }

    newelem = malloc(sizeof(struct recursive_mutex_list));
    if (!newelem)
	return -1;

    newelem->mx = mutex;
    newelem->id = me;
    newelem->count = 1;

    LIST_INSERT_HEAD(&recursive_mutex_list_head, newelem, next);

    pthread_mutex_unlock(&recursive_mutex_list_mx);

    pthread_mutex_lock(mutex);

    return 0;
}


int
recursive_mutex_unlock(pthread_mutex_t * mutex)
{
    struct recursive_mutex_list *current;
    pthread_t       me;

    pthread_mutex_lock(&recursive_mutex_list_mx);

    me = pthread_self();

    LIST_FOREACH(current, &recursive_mutex_list_head, next)
	if (mutex == current->mx && pthread_equal(me, current->id) != 0)
	if (--current->count == 0) {
	    LIST_REMOVE(current, next);
	    free(current);
	    pthread_mutex_unlock(mutex);
	    break;
	}

    pthread_mutex_unlock(&recursive_mutex_list_mx);
    return 0;
}
