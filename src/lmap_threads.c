/*
    lmap -- thread handling

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
#include <lmap_threads.h>

#include <pthread.h>

struct thread_list {
   struct lmap_thread t;
   LIST_ENTRY (thread_list) next;
};

/* global data */

static LIST_HEAD(, thread_list) thread_list_head;

int tl = 0;

pthread_mutex_t threads_mutex = PTHREAD_MUTEX_INITIALIZER;
#define THREADS_LOCK     do{ pthread_mutex_lock(&threads_mutex); } while(0)
#define THREADS_UNLOCK   do{ pthread_mutex_unlock(&threads_mutex); } while(0)
   
/* protos... */

char * lmap_thread_getname(pthread_t id);
char * lmap_thread_getdesc(pthread_t id);
void lmap_thread_register(pthread_t id, char *name, char *desc);
pthread_t lmap_thread_new(char *name, char *desc, void *(*function)(void *), void *args);
void lmap_thread_destroy(pthread_t id);
void lmap_thread_init(void);
void lmap_thread_testcancel(void);
void lmap_thread_kill_all(void);

/*******************************************/

/* returns the name of a thread */

char * lmap_thread_getname(pthread_t id)
{
   struct thread_list *current;

   if (id == LMAP_SELF)
      id = pthread_self();

   THREADS_LOCK;

   LIST_FOREACH(current, &thread_list_head, next) {
      if (current->t.id == id) {
         THREADS_UNLOCK;
         return current->t.name;
      }
   }

   THREADS_UNLOCK;

   return "NR_THREAD";
}

/* returns the description of a thread */

char * lmap_thread_getdesc(pthread_t id)
{
   struct thread_list *current;

   if (id == LMAP_SELF)
      id = pthread_self();
  
   THREADS_LOCK;
   
   LIST_FOREACH(current, &thread_list_head, next) {
      if (current->t.id == id) {
         THREADS_UNLOCK;
         return current->t.description;
      }
   }
   
   THREADS_UNLOCK;
   
   return "";
}


/* add a thread in the thread list */

void lmap_thread_register(pthread_t id, char *name, char *desc)
{
   struct thread_list *current, *newelem;

   if (id == LMAP_SELF)
      id = pthread_self();
   
   DEBUG_MSG("lmap_thread_register -- [%d] %s", id, name);

   newelem = (struct thread_list *) calloc(1, sizeof(struct thread_list));
   ON_ERROR(newelem, NULL, "cant allocate memory");
              
   newelem->t.id = id;
   newelem->t.name = strdup(name);
   newelem->t.description = strdup(desc);

   THREADS_LOCK;
   
   LIST_FOREACH(current, &thread_list_head, next) {
      if (current->t.id == id) {
         LIST_REPLACE(current, newelem, next);
         SAFE_FREE(current);
         THREADS_UNLOCK;
         return;
      }
   }

   LIST_INSERT_HEAD(&thread_list_head, newelem, next);
   
   THREADS_UNLOCK;
   
}

/*
 * creates a new thread on the given function
 */

pthread_t lmap_thread_new(char *name, char *desc, void *(*function)(void *), void *args)
{
   pthread_t id;

   DEBUG_MSG("lmap_thread_new -- %s", name);

   if (pthread_create(&id, NULL, function, args) < 0)
      ERROR_MSG("not enough system resources to create a new thread");

   lmap_thread_register(id, name, desc);

   DEBUG_MSG("lmap_thread_new -- %d created ", (u_int32)id);
   
   return id;
}

/*
 * destroy a thread in the list
 */

void lmap_thread_destroy(pthread_t id)
{
   struct thread_list *current;

   DEBUG_MSG("lmap_thread_destroy -- terminating %lu [%s]", id, lmap_thread_getname(id));

   pthread_cancel((pthread_t)id);

   pthread_join((pthread_t)id, NULL);

   THREADS_LOCK;
   
   LIST_FOREACH(current, &thread_list_head, next) {
      if (current->t.id == id) {
         SAFE_FREE(current->t.name);
         SAFE_FREE(current->t.description);
         LIST_REMOVE(current, next);
         SAFE_FREE(current);
         THREADS_UNLOCK;
         return;
      }
   }

   THREADS_UNLOCK;

}

/* 
 * set the state of a thread 
 * all the new thread should call this on startup
 */

void lmap_thread_init(void)
{
   DEBUG_MSG("lmap_thread_init -- %d", pthread_self());
   
   /* 
    * allow a thread to be cancelled as soon as the
    * cancellation  request  is received
    */
        
   pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

}

/*
 * kill all the registerd thread but
 * the calling one
 */

void lmap_thread_kill_all(void)
{
   struct thread_list *current;
   pthread_t id = pthread_self();

   DEBUG_MSG("lmap_thread_kill_all -- caller %lu [%s]", id, lmap_thread_getname(id));

   LIST_FOREACH(current, &thread_list_head, next) {
      if (current->t.id != id) {
         lmap_thread_destroy(current->t.id);      
      }
   }

}

/*
 * set a cancellation point
 */

void lmap_thread_testcancel(void)
{
   pthread_testcancel();
}


/* EOF */

// vim:ts=3:expandtab

