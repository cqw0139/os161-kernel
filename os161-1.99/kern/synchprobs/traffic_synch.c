#include <types.h>
#include <lib.h>
#include <synchprobs.h>
#include <synch.h>
#include <opt-A1.h>
#include <array.h>

/* 
 * This simple default synchronization mechanism allows only vehicle at a time
 * into the intersection.   The intersectionSem is used as a a lock.
 * We use a semaphore rather than a lock so that this code will work even
 * before locks are implemented.
 */

/* 
 * Replace this default synchronization mechanism with your own (better) mechanism
 * needed for your solution.   Your mechanism may use any of the available synchronzation
 * primitives, e.g., semaphores, locks, condition variables.   You are also free to 
 * declare other global variables if your solution requires them.
 */

/*
 * replace this with declarations of any synchronization and other variables you need here
 */
//static struct semaphore *intersectionSem;

int check(Direction origin, Direction destination);

static struct cv *oeast;
static struct cv *osouth;
static struct cv *owest;
static struct cv *onorth;
static struct lock *aaa;
static int oeds = 0;
static int oedw = 0;
static int oedn = 0;
static int osde = 0;
static int osdw = 0;
static int osdn = 0;
static int owde = 0;
static int owds = 0;
static int owdn = 0;
static int onde = 0;
static int onds = 0;
static int ondw = 0;

struct v{
  unsigned int o,d;
};

/*struct v * vcre(int ori, int des);

struct v * vcre(int ori, int des){
  struct v * out = kmalloc(sizeof(struct v));
  if (out == NULL) {
                return NULL;
        }
  out->o = ori;
  out->d = des;
  return out;
}*/


//struct array * inter;



/* 
 * The simulation driver will call this function once before starting
 * the simulation
 *
 * You can use it to initialize synchronization and other variables.
 * 
 */
void
intersection_sync_init(void)
{
  /* replace this default implementation with your own implementation */
  //inter = array_create();
  //array_init(inter);
  //array_setsize(inter,1000);
  aaa = lock_create("aaa");
  oeast = cv_create("east");
  osouth = cv_create("south");
  owest = cv_create("west");
  onorth = cv_create("north");
  //intersectionSem = sem_create("intersectionSem",1);
  /*if (intersectionSem == NULL) {
    panic("could not create intersection semaphore");
  }*/
  return;
}

/* 
 * The simulation driver will call this function once after
 * the simulation has finished
 *
 * You can use it to clean up any synchronization and other variables.
 *
 */
void
intersection_sync_cleanup(void)
{
  /* replace this default implementation with your own implementation */
  //array_destroy(inter);
  //KASSERT(intersectionSem != NULL);
  //sem_destroy(intersectionSem);
  lock_destroy(aaa);
  cv_destroy(oeast);
  cv_destroy(osouth);
  cv_destroy(owest);
  cv_destroy(onorth);
}


/*
 * The simulation driver will call this function each time a vehicle
 * tries to enter the intersection, before it enters.
 * This function should cause the calling simulation thread 
 * to block until it is OK for the vehicle to enter the intersection.
 *
 * parameters:
 *    * origin: the Direction from which the vehicle is arriving
 *    * destination: the Direction in which the vehicle is trying to go
 *
 * return value: none
 */

int check(Direction origin, Direction destination){
  /*int num = array_num(inter);
  struct v *x;
  for(int i=0;i<num;i++){
    x = array_get(inter,i);
    if(origin != x->o ){
      if((x->o != destination)||(x->d != origin)){
        if((x->d == destination)||
           (((x->d - x->o) != 1)&&
            ((x->o - x->d) != 3)&&
            ((destination - origin) != 1)&&
            ((origin - destination) != 3))){
          return 1;
        }
      }
    }
  }*/
  if(origin == 0){
    if(destination == 1){
      if((osde > 0)||(owde > 0)||(oedw > 0)||(oeds > 0)||(osdn > 0)||(osdw > 0)||(owdn > 0)){
        return 1;
      }
    }else if(destination == 2){
      if((owds > 0)||(oeds > 0)||(osdw > 0)||(owde > 0)||(oedw > 0)||(owdn > 0)){
        return 1;
      }
    }else if(destination == 3){
      if((osdw > 0)||(oedw > 0)){
        return 1;
      }
    }
  }else if(origin == 1){
    if(destination == 0){
      if((osdn > 0)||(owdn > 0)){
        return 1;
      }
    }else if(destination == 2){
      if((owds > 0)||(onds > 0)||(osdn > 0)||(osdw > 0)||(owde > 0)||(owdn > 0)||(onde > 0)){
        return 1;
      }
    }else if(destination == 3){
      if((ondw > 0)||(osdw > 0)||(owdn > 0)||(onde > 0)||(osdn > 0)||(onds > 0)){
        return 1;
      }
    }
  }else if(origin == 2){
    if(destination == 1){
      if((owde > 0)||(onde > 0)){
        return 1;
      }
    }else if(destination == 0){
      if((oedn > 0)||(owdn > 0)||(onde > 0)||(oedw > 0)||(owde >0)||(oeds > 0)){
        return 1;
      }
    }else if(destination == 3){
      if((ondw > 0)||(oedw > 0)||(owde > 0)||(owdn > 0)||(onds > 0)||(onde > 0)||(oeds > 0)){
        return 1;
      }
    }
  }else if(origin == 3){
    if(destination == 1){
      if((osde > 0)||(onde > 0)||(oeds > 0)||(osdn > 0)||(onds > 0)||(osdw > 0)){
        return 1;
      }
    }else if(destination == 2){
      if((onds > 0)||(oeds > 0)){
        return 1;
      }
    }else if(destination == 0){
      if((oedn > 0)||(osdn > 0)||(onds > 0)||(onde > 0)||(oedw > 0)||(oeds > 0)||(osdw > 0)){
        return 1;
      }
    }
  }
  return 0;
}

void
intersection_before_entry(Direction origin, Direction destination) 
{
  /* replace this default implementation with your own implementation */
  //(void)origin;  /* avoid compiler complaint about unused parameter */
  //(void)destination; /* avoid compiler complaint about unused parameter */
  lock_acquire(aaa);
  while(check(origin, destination) == 1){
    if(origin == 0){
      cv_wait(onorth,aaa);
    }else if(origin == 1){
      cv_wait(oeast,aaa);
    }else if(origin == 2){
      cv_wait(osouth,aaa);
    }else if(origin == 3){
      cv_wait(owest,aaa);
    }
  }
  if(origin == 0){
    if(destination == 1){
      onde++;
    }else if(destination == 2){
      onds++;
    }else if(destination == 3){
      ondw++;
    }
  }else if(origin == 1){
    if(destination == 0){
      oedn++;
    }else if(destination == 2){
      oeds++;
    }else if(destination == 3){
      oedw++;
    }
  }else if(origin == 2){
    if(destination == 1){
      osde++;
    }else if(destination == 0){
      osdn++;
    }else if(destination == 3){
      osdw++;
    }
  }else if(origin == 3){
    if(destination == 1){
      owde++;
    }else if(destination == 2){
      owds++;
    }else if(destination == 0){
      owdn++;
    }
  }
  //KASSERT(intersectionSem != NULL);
  //P(intersectionSem);
  //struct v *x = vcre(origin, destination);
  //array_add(inter,x,NULL);
  lock_release(aaa);
}


/*
 * The simulation driver will call this function each time a vehicle
 * leaves the intersection.
 *
 * parameters:
 *    * origin: the Direction from which the vehicle arrived
 *    * destination: the Direction in which the vehicle is going
 *
 * return value: none
 */

void
intersection_after_exit(Direction origin, Direction destination) 
{
  /* replace this default implementation with your own implementation */
  //(void)origin;  /* avoid compiler complaint about unused parameter */
  //(void)destination; /* avoid compiler complaint about unused parameter */
  lock_acquire(aaa);
  /*int n = array_num(inter);
  struct v *x;
  for(int i=0;i<n;i++){
    x = array_get(inter,i);
    if((x->o == origin)&&
       (x->d == destination)){
      array_remove(inter,i);
    break;
    }
  }*/
  if(origin == 0){
    if(destination == 1){
      onde--;
    }else if(destination == 2){
      onds--;
    }else if(destination == 3){
      ondw--;
    }
  }else if(origin == 1){
    if(destination == 0){
      oedn--;
    }else if(destination == 2){
      oeds--;
    }else if(destination == 3){
      oedw--;
    }
  }else if(origin == 2){
    if(destination == 1){
      osde--;
    }else if(destination == 0){
      osdn--;
    }else if(destination == 3){
      osdw--;
    }
  }else if(origin == 3){
    if(destination == 1){
      owde--;
    }else if(destination == 2){
      owds--;
    }else if(destination == 0){
      owdn--;
    }
  }
  if(origin == 0){
      cv_broadcast(oeast,aaa);
      cv_broadcast(osouth,aaa);
      cv_broadcast(owest,aaa);
    }else if(origin == 1){
      cv_broadcast(onorth,aaa);
      cv_broadcast(osouth,aaa);
      cv_broadcast(owest,aaa);
    }else if(origin == 2){
      cv_broadcast(oeast,aaa);
      cv_broadcast(onorth,aaa);
      cv_broadcast(owest,aaa);
    }else if(origin == 3){
      cv_broadcast(oeast,aaa);
      cv_broadcast(osouth,aaa);
      cv_broadcast(onorth,aaa);
    }
  //KASSERT(intersectionSem != NULL);
  //V(intersectionSem);
  lock_release(aaa);
}
