#include "shell.h"
#include <kernel/gs_io.h>
#include <kernel/user.h>
#include <kernel/screen_manager.h>
#include <drivers/fat64/fat64.h>

uint32_t scr_x = 0,
		 scr_y = 0;

const char cmd_ls[] = "ls";
const char cmd_touch[] = "touch";
const char cmd_rm[] = "rm";

const char cmd_send[] = "send";

const char cmd_home[] = "home";
const char cmd_clear[] = "clear";
const char cmd_manager[] = "manager";

fat64_file_t cd;
handle_t cur_dir = (handle_t)&cd;

void get_input(char * buf);
void new_line();

void _cmd_ls();
void _cmd_touch();
void _cmd_rm();

void shell() {
	//get a handle to a screen
	handle_t scr = openscreen();
	char buf[1024] = {0};
	int input;
	uint8_t done = 0;
	
	//check for failure to grab screen
	if(scr & 0x80000000) {
		c_printf("****ERROR**** SHELL: could not get a screen");
		//if failed, return
		exit(X_FAILURE);
	}

	//open the root file
	fat64_open(cur_dir, 0);

	//set this processes screen and switch to it
	setscreen(scr);
	switchscreen(scr);
	gs_clear();

	while(!done) {
		//get the input
		gs_puts_at(FONT_CHAR_WIDTH*scr_x,FONT_CHAR_HEIGHT*scr_y, " >", GS_DRAW_MODE_FLAT);
		scr_x = 2;
		memset(buf, 0, 1024);
		get_input(buf);

		//file system commands
		if(!strcmp(buf,cmd_ls)) {
			_cmd_ls();
		} else if(!strcmp(buf, cmd_touch)) {
			_cmd_touch();
		} else if(!strcmp(buf, cmd_rm)) {
			_cmd_rm();

		//network commands
		} else if(!strcmp(buf, cmd_send)) {
			sendmsg(NULL);

		//other commands
		} else if(!strcmp(buf, cmd_home)) {
			switchscreen(0);
		} else if(!strcmp(buf, cmd_clear)) {
			scr_x = scr_y = 0;
			gs_clear();
		} else if(!strcmp(buf, cmd_manager)) {
			if(!fork(NULL))
				exec(PRIO_STANDARD, screen_manager);
		} else
			gs_puts_at(FONT_CHAR_WIDTH*scr_x,FONT_CHAR_HEIGHT*scr_y," - Command not found", GS_DRAW_MODE_FLAT);
		new_line();
	}

	while(1);
}

void get_input(char * buf) {
	uint32_t loc = 0;
	int input;

	while(1) {
		input = c_getchar();
		if((input == 0x8)) { //check backspace
			if(loc == 0)
				continue;
			input = buf[--loc] = 0;
			--scr_x;
			gs_putc_at(FONT_CHAR_WIDTH*scr_x,FONT_CHAR_HEIGHT*scr_y,input, GS_DRAW_MODE_FLAT);
		} else if(input == 0xD || input == '\n') { //check new line saying we are done
			break;
		} else {
			buf[loc] = input;
			gs_putc_at(FONT_CHAR_WIDTH*scr_x,FONT_CHAR_HEIGHT*scr_y,buf[loc], GS_DRAW_MODE_FLAT);
			++loc;
			++scr_x;
		}
	}

	new_line();
}

void new_line() {
	//move to next line
	++scr_y;
	if(scr_y == LEGACY_SCREEN_HEIGHT) {
		scr_y = 0;
		gs_clear();
	}
	scr_x = 0;
}

void _cmd_ls() {
	uint32_t i;
	fat64_file_t f;
	handle_t file = (handle_t)&f;
	char num[] = "0: ";

	for(i = 0; i < 8; ++i) {
		if(!fat64_dir_entry(cur_dir, i, file)) {
			num[0] += (uint8_t)f.entry.dir_entry_num;
			gs_puts_at(FONT_CHAR_WIDTH*scr_x,FONT_CHAR_HEIGHT*scr_y,num, GS_DRAW_MODE_FLAT);
			scr_x += 3;
			gs_puts_at(FONT_CHAR_WIDTH*scr_x,FONT_CHAR_HEIGHT*scr_y,f.entry.name, GS_DRAW_MODE_FLAT);
			new_line();
		}
	}
}

void _cmd_touch() {
	char file_name[255] = {0};
	gs_puts_at(FONT_CHAR_WIDTH*scr_x,FONT_CHAR_HEIGHT*scr_y,"Filename: ", GS_DRAW_MODE_FLAT);
	scr_x = 10;
	get_input(file_name);
	fat64_touch(cur_dir, file_name);
}

void _cmd_rm() {
	char entry[255] = {0};
	uint64_t entry_num;
	fat64_file_t f;
	handle_t file = (handle_t)&f;

	gs_puts_at(FONT_CHAR_WIDTH*scr_x,FONT_CHAR_HEIGHT*scr_y,"Entry: ", GS_DRAW_MODE_FLAT);
	scr_x = 7;
	get_input(entry);
	entry_num = entry[0]-'0';
	if(!fat64_dir_entry(cur_dir, entry_num, file)) {
		fat64_rm(file);
	}
}
