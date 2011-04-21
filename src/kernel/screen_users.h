/**
 * file: screen_users.h
 * @author Benjamin Mayes
 * @description Declares some user functions to test the screen functionality 
 * I wrote and provide simple demos on the usage of the various system calls
 * comprising the screen system.
 */

#ifndef SCREEN_USERS_H_
#define SCREEN_USERS_H_

// Screen user defines, comment these out to not spawn that screen user

// opens and closes a screen
/*#define SPAWN_SCREEN_USER_A 

// gets its screen, sets its screen, gets it again, then sets back to default
#define SPAWN_SCREEN_USER_B 

// opens all the screens that it can, closes one, opens another, then closes 
// all the screens
#define SPAWN_SCREEN_USER_C

// opens a screen, writes to it, switches screen in for 1 second intervals every 10 seconds
#define SPAWN_SCREEN_USER_D*/ 

// function called in the init routine to spawn the screen users
extern void spawn_screen_users(void);
#endif
