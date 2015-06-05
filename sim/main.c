#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <SDL/SDL.h>

#include "main.h"
#include <string.h>
#include<sys/time.h>

#include "keyboard.h"

#ifdef serial
#include "libftdi1/ftdi.h"
static struct ftdi_context *ftdi;
#endif


int sdlpause = 0;

#define MAX_ANIMATIONS 200
#define MAX_APPS 200

int animationcount = 0;
int appcount = 0;
int current_animation = 0;


unsigned long long int get_clock(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (unsigned long long int)tv.tv_usec + 1000000*tv.tv_sec;
}

struct animation {
	init_fun init_fp;
	tick_fun tick_fp;
	deinit_fun deinit_fp;
	unsigned int duration;
	unsigned int min_delay;
} animations[MAX_ANIMATIONS];


struct app {
	init_fun init_fp;
	tick_fun tick_fp;
	deinit_fun deinit_fp;
	unsigned int min_delay;
} apps[MAX_APPS];


SDL_Surface* screen;
void Delay(uint16_t t)
{
	
}

int leds[LED_HEIGHT][LED_WIDTH][4];
void setLedXY(uint8_t x, uint8_t y, uint8_t red,uint8_t green,uint8_t blue) {
	if (x >= LED_WIDTH) return;
	if (y >= LED_HEIGHT) return;
	leds[y][x][0] = red;
	leds[y][x][1] = green;
	leds[y][x][2] = blue;
	leds[y][x][3] = 1;
}

void setLedAll(uint8_t red,uint8_t green,uint8_t blue) 
{
	int x, y;
	for(x = 0; x < LED_WIDTH; x++) {
		for(y = 0; y < LED_HEIGHT; y++) {
			leds[y][x][0] = red;
			leds[y][x][1] = green;
			leds[y][x][2] = blue;
			leds[y][x][3] = 1;
		}
	}
}

void invLedXY(uint8_t x, uint8_t y) {
	if (x >= LED_WIDTH) return;
	if (y >= LED_HEIGHT) return;
	leds[y][x][0] = 255 - leds[y][x][0];
	leds[y][x][1] = 255 - leds[y][x][1];
	leds[y][x][2] = 255 - leds[y][x][2];
	leds[y][x][3] = 1;
}
void getLedXY(uint8_t x, uint8_t y,uint8_t* red, uint8_t* green, uint8_t* blue) {
	if (x >= LED_WIDTH) return;
	if (y >= LED_HEIGHT) return;
	*red = leds[y][x][0];
	*green = leds[y][x][1];
	*blue = leds[y][x][2];
}

void setDelay(unsigned int t)
{
	animations[current_animation].min_delay = t;
}

void registerAnimation(init_fun init,tick_fun tick, deinit_fun deinit,uint16_t t, uint16_t count)
{
	if(animationcount == MAX_ANIMATIONS)
		return;
	animations[animationcount].init_fp = init;
	animations[animationcount].tick_fp = tick;
	animations[animationcount].deinit_fp = deinit;
	animations[animationcount].duration = count;
	animations[animationcount].min_delay = t;

	animationcount++;

}

static uint16_t key_press;
uint16_t get_key_press( uint16_t key_mask )
{
	key_mask &= key_press;                          // read key(s)
	key_press ^= key_mask;                          // clear key(s)
	return key_mask;
}


void fillRGB(uint8_t r,uint8_t g, uint8_t b)
{
	int x, y;

	for(x = 0; x < LED_WIDTH; x++) {
		for(y = 0; y < LED_HEIGHT; y++) {
			leds[y][x][0]=r;
			leds[y][x][1]=g;
			leds[y][x][2]=b;
			leds[y][x][3]=1;
		}
	}
}
void lcdFillRGB(uint8_t r,uint8_t g , uint8_t b)
{
	int x, y;

	for(x = 0; x < LED_WIDTH; x++) {
		for(y = 0; y < LED_HEIGHT; y++) {
			leds[y][x][0]=r;
			leds[y][x][1]=g;
			leds[y][x][2]=b;
			leds[y][x][3]=1;
		}
	}
}
void button(uint8_t nr)
{
	printf("button %i\n",nr);
	if(nr < animationcount)
	{
		animations[current_animation].deinit_fp();
		current_animation = nr;
		animations[current_animation].init_fp();
	}
}

void pollKeyboard(void)
{
	KeyboardEvent e;
    
	// controller ranges:
	const int chan_begin= 0, chan_end= 7;
	const int chana_begin= 16, chana_end= 23;

	while(keyboard_poll(&e)) 
	{

		
		printf("%d %d %d\n", e.x, e.y, e.type);
//		fflush(stdout);
		
		if(e.type>>4 == 0x0b)	// control change
		{
			if( (e.x>=chan_begin) && (e.x<=chan_end) )
				chan[e.x-chan_begin]= e.y;
			else if( (e.x>=chana_begin) && (e.x<=chana_end) )
				chana[e.x-chana_begin]= e.y;
			else if((e.x >= 32)&&(e.x <= 39)&&(e.y == 127))
			{
		
				if(e.x < 32+animationcount)
				{
					for(uint8_t i = 32;i <= 39;i++)
					{
						keyboard_send(176,i,0);
					}
					for(uint8_t i = 48;i <= 55;i++)
					{
						keyboard_send(176,i,0);
					}
					for(uint8_t i = 64;i <= 71;i++)
					{
						keyboard_send(176,i,0);
					}
					keyboard_send(176,e.x,127);
				}
				
				
				button(e.x-32);
			}
			else if((e.x >= 48)&&(e.x <= 55)&&(e.y == 127))
			{
		
				if(e.x < 40+animationcount)
				{
					for(uint8_t i = 32;i <= 39;i++)
					{
						keyboard_send(176,i,0);
					}
					for(uint8_t i = 48;i <= 55;i++)
					{
						keyboard_send(176,i,0);
					}
					for(uint8_t i = 64;i <= 71;i++)
					{
						keyboard_send(176,i,0);
					}
					keyboard_send(176,e.x,127);
				}
				
				
				button(e.x-40);
			}
			else if((e.x >= 64)&&(e.x <= 71)&&(e.y == 127))
			{
		
				if(e.x < 48+animationcount)
				{
					for(uint8_t i = 32;i <= 39;i++)
					{
						keyboard_send(176,i,0);
					}
					for(uint8_t i = 48;i <= 55;i++)
					{
						keyboard_send(176,i,0);
					}
					for(uint8_t i = 64;i <= 71;i++)
					{
						keyboard_send(176,i,0);
					}
					keyboard_send(176,e.x,127);
				}
				
				
				button(e.x-48);
			}
		}
	}
}

static int hatposition=SDL_HAT_CENTERED;


int joy_is_up(void)
{
	if(hatposition&SDL_HAT_UP)
	{
		return 1;
	}
	return 0;
}
int joy_is_down(void)
{
	if(hatposition&SDL_HAT_DOWN)
	{
		return 1;
	}
	return 0;
}
int joy_is_left(void)
{
	if(hatposition&SDL_HAT_LEFT)
	{
		return 1;
	}
	return 0;
}
int joy_is_right(void)
{
	if(hatposition&SDL_HAT_RIGHT)
	{
		return 1;
	}
	return 0;
}


#ifdef serial
void write_frame(void)
{

	uint32_t pixel=0;
	
	unsigned char buf[LED_HEIGHT*LED_WIDTH*3*2+1];
				
	buf[pixel++] = 0x23;

	for(uint8_t y = 0;y<LED_HEIGHT;y++)
	{
		for(uint8_t x = 0;x<LED_WIDTH;x++)
		{
			for(int i=0;i<3;i++)
			{
				int pix = leds[y][x][i];

				if(pix == 0x23)
				{
					buf[pixel++] = 0x65;
					buf[pixel++] = 1;
				}
				else if(pix == 0x42)
				{
					buf[pixel++] = 0x65;
					buf[pixel++] = 2;
				}
				else if(pix == 0x65)
				{
					buf[pixel++] = 0x65;
					buf[pixel++] = 3;
				}
				else if(pix == 0x66)
				{
					buf[pixel++] = 0x65;
					buf[pixel++] = 4;
				}
				else
				{
					buf[pixel++] = pix;
				}
			}
		}
	}

	int ret = ftdi_write_data(ftdi, buf, pixel);
	if (ret < 0)
	{
		fprintf(stderr,"write failed , error %d (%s)\n",ret, ftdi_get_error_string(ftdi));
	}
		
	usleep(2000);

}
#endif



int main(int argc __attribute__((__unused__)), char *argv[] __attribute__((__unused__))) {

	fillRGB(0,0,0);

	srand(time(NULL));

	keyboard_init();
	for(uint8_t i = 32;i <= 39;i++)
	{
		keyboard_send(176,i,0);
	}
	keyboard_send(176,32,127);

	chana[7]=127;
	chana[6]=127;
	chana[5]=127;
	chana[4]=127;


#ifdef serial

	
	
	
	int ret;
	struct ftdi_version_info version;
	if ((ftdi = ftdi_new()) == 0)
	{
		fprintf(stderr, "ftdi_new failed\n");
		return EXIT_FAILURE;
	}
	version = ftdi_get_library_version();
	printf("Initialized libftdi %s (major: %d, minor: %d, micro: %d, snapshot ver: %s)\n",
			version.version_str, version.major, version.minor, version.micro,
			version.snapshot_str);
	if ((ret = ftdi_usb_open(ftdi, 0x0403, 0x6001)) < 0)
	{
		fprintf(stderr, "unable to open ftdi device: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
		ftdi_free(ftdi);
		return EXIT_FAILURE;
	}
	// Read out FTDIChip-ID of R type chips
	if (ftdi->type == TYPE_R)
	{
		unsigned int chipid;
		printf("ftdi_read_chipid: %d\n", ftdi_read_chipid(ftdi, &chipid));
		printf("FTDI chipid: %X\n", chipid);
	}
	ret = ftdi_set_line_property(ftdi, 8, STOP_BIT_1, NONE);
	if (ret < 0)
	{
		fprintf(stderr, "unable to set line parameters: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
		exit(-1);
	}
	ret = ftdi_set_baudrate(ftdi, 500000);
	if (ret < 0)
	{
		fprintf(stderr, "unable to set baudrate: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
		exit(-1);
	}
	
		
	unsigned char c=66;
	ret = ftdi_write_data(ftdi, &c,1);
	c=0;
	ret = ftdi_write_data(ftdi, &c,1);
	ret = ftdi_write_data(ftdi, &c,1);
	ret = ftdi_write_data(ftdi, &c,1);
	if (ret < 0)
	{
		fprintf(stderr,"write failed , error %d (%s)\n",ret, ftdi_get_error_string(ftdi));
	}
	usleep(200);
#endif
	
	
	SDL_Init(SDL_INIT_JOYSTICK);

	printf("joysticks: %i",SDL_NumJoysticks());
	SDL_JoystickOpen(0);
	SDL_JoystickEventState(SDL_ENABLE);

	screen = SDL_SetVideoMode(LED_WIDTH*ZOOM,LED_HEIGHT*ZOOM,32, SDL_SWSURFACE | SDL_DOUBLEBUF);


	animations[current_animation].init_fp();
	
	unsigned int tick_count = 0;
	unsigned int running = 1;
	//unsigned long long int startTime = get_clock();
	Uint32 lastFrame = SDL_GetTicks(); 

	while(running) {
		SDL_Event ev;
		while(SDL_PollEvent(&ev)) {
			switch(ev.type) {
				case SDL_JOYHATMOTION:
					hatposition = ev.jhat.value;
					break;
				case SDL_QUIT:
					running = 0;
					break;
				case SDL_KEYUP:
					break;
				case SDL_KEYDOWN:
					switch(ev.key.keysym.sym) {
						case SDLK_ESCAPE:
							running = 0;
							break;
						case SDLK_SPACE:
							if(sdlpause == 0)
							{
								sdlpause = 1;
							}
							else{
								sdlpause = 0;
							}
							break;
						case SDLK_1:
							key_press |= KEY_ESC;
							break;
						case SDLK_2:
							key_press |= KEY_B;
							break;
						case SDLK_3:
							key_press |= KEY_A;
							break;
						case SDLK_4:
							key_press |= KEY_STICK;
							break;
							
						default: break;
					}
				default: break;
			}
		}

		pollKeyboard();
		animations[current_animation].tick_fp();


	


		
		int x, y;
		for(x = 0; x < LED_WIDTH; x++) {
			for(y = 0; y < LED_HEIGHT; y++) {

				if(leds[y][x][3] == 1)
				{
					//SDL_Rect rect = { ZOOM*x, ZOOM*y, ZOOM,ZOOM };
					SDL_Rect rect = { ZOOM*x+(ZOOM/15), ZOOM*(LED_HEIGHT - y - 1)+(ZOOM/15), ZOOM-(ZOOM/15), ZOOM-(ZOOM/15) };
					SDL_FillRect(
						screen, 
						&rect, 
						SDL_MapRGB(screen->format, leds[y][x][0]*(chana[7]/127.0f)*(chana[4]/127.0f),leds[y][x][1]*(chana[7]/127.0f)*(chana[5]/127.0f),leds[y][x][2]*(chana[7]/127.0f)*(chana[6]/127.0f))
					);

					leds[y][x][3] = 0;

				}

			}
		}

		
		SDL_Flip(screen);
#ifdef serial
		write_frame();
#endif


		Uint32 now = SDL_GetTicks(); 

		if( (now - lastFrame) < animations[current_animation].min_delay )
		{
			SDL_Delay(animations[current_animation].min_delay - (now - lastFrame));
		}
		lastFrame = SDL_GetTicks();

		
		tick_count++;


		if(tick_count == animations[current_animation].duration)
		{
			animations[current_animation].deinit_fp();

			current_animation++;
			if(current_animation == animationcount)
			{
				current_animation = 0;
			}
			tick_count=0;

			SDL_Rect rect = { 0, 0, LED_WIDTH*ZOOM,LED_HEIGHT*ZOOM };
			SDL_FillRect(
				screen, 
				&rect, 
				SDL_MapRGB(screen->format,0,0,0)
			);
			SDL_Flip(screen);
			SDL_Delay(300);


			animations[current_animation].init_fp();


		}
	}

	SDL_Quit();
	return 0;
}

