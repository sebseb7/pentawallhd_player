#include <stdlib.h>
#include "main.h"
#include <stdio.h>


static const char initial_field[21][21] = {
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0 },
	{ 0, 5, 0, 0, 0, 3, 0, 0, 0, 3, 0, 3, 0, 0, 0, 3, 0, 0, 0, 5, 0 },
	{ 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0 },
	{ 0, 3, 0, 0, 0, 3, 0, 3, 0, 0, 0, 0, 0, 3, 0, 3, 0, 0, 0, 3, 0 },
	{ 0, 3, 3, 3, 3, 3, 0, 3, 3, 3, 0, 3, 3, 3, 0, 3, 3, 3, 3, 3, 0 },
	{ 0, 0, 0, 0, 0, 3, 0, 0, 0, 1, 0, 1, 0, 0, 0, 3, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 3, 0, 1, 1, 1, 1, 1, 1, 1, 0, 3, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 3, 0, 1, 0, 0, 1, 0, 0, 1, 0, 3, 0, 0, 0, 0, 0 },
	{ 1, 1, 1, 1, 1, 3, 1, 1, 0, 1, 1, 1, 0, 1, 1, 3, 1, 1, 1, 1, 1 },
	{ 0, 0, 0, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 1, 0, 3, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 3, 0, 1, 1, 1, 1, 1, 1, 1, 0, 3, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 3, 0, 1, 0, 0, 0, 0, 0, 1, 0, 3, 0, 0, 0, 0, 0 },
	{ 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0 },
	{ 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0 },
	{ 0, 5, 3, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 3, 3, 5, 0 },
	{ 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0 },
	{ 0, 3, 3, 3, 3, 3, 0, 3, 3, 3, 0, 3, 3, 3, 0, 3, 3, 3, 3, 3, 0 },
	{ 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0 },
	{ 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};

static char field_dots[21][21];
static char field_pups[21][21];

static const int bot_r[4] = {255,128,255,0};
static const int bot_g[4] = {0,0,128,128};
static const int bot_b[4] = {128,255,0,255};

struct pos_t
{
	int x;
	int y;
};

struct bot_t
{
	struct pos_t pos;
	int orientation;
	int dying;
};

static struct bot_t bots[4];
static struct bot_t player;

#define DOOR_X 8
#define DOOR_Y 10

static int door = 0;
static int pupmode = 0;
static int hurt = 0;
static int lives = 3;

static void init(void)
{
	bots[0].pos.x = 7;
	bots[0].pos.y = 10;
	bots[0].orientation=1;
	bots[1].pos.x = 9;
	bots[1].pos.y = 10;
	bots[1].orientation=1;
	bots[2].pos.x = 9;
	bots[2].pos.y = 9;
	bots[2].orientation=1;
	bots[3].pos.x = 9;
	bots[3].pos.y = 11;
	bots[3].orientation=1;
	player.pos.x = 15;
	player.pos.y = 10;
	player.orientation=1;


	for(int x=0;x<21;x++)
		for(int y=0;y<21;y++)
		{
			if((initial_field[x][y]&2)==2)
				field_dots[x][y]=1;
			else
				field_dots[x][y]=0;
			
			if((initial_field[x][y]&4)==4)
				field_pups[x][y]=1;
			else
				field_pups[x][y]=0;
		}
				
}

static void deinit(void)
{
}
				
static void set_block(int8_t x,int8_t y,int8_t r,int8_t g,int8_t b)
{
			setLedXY(y+1, 23-x, r, g, b);
}

struct pos_t translate(struct pos_t start,int orientation)
{
	struct pos_t target;
	
	if(orientation==0)
	{
		target.x=start.x+1;
		target.y=start.y;
	}
	if(orientation==1)
	{
		target.x=start.x;
		target.y=start.y+1;
	}
	if(orientation==2)
	{
		target.x=start.x-1;
		target.y=start.y;
	}
	if(orientation==3)
	{
		target.x=start.x;
		target.y=start.y-1;
	}

	return target;
}


int check(struct pos_t start,int orientation)
{

	struct pos_t target = translate(start,orientation);

	if((door==0)&&(target.x==DOOR_X)&&(target.y)==DOOR_Y)
	{
		return 0;
	}


	if(( initial_field[target.x][target.y] & 1) == 1)
	{
		return 1;
	}

	return 0;
	
}

void move(struct pos_t start,int orientation,struct pos_t* target)
{
	struct pos_t new = translate(start,orientation);
	target->x=new.x;
	target->y=new.y;
}

static int rotate_left(int orientation)
{
	if(orientation == 0)
	{
		return 3;
	}
	else
	{
		return orientation-1;
	}
}

static int rotate_right(int orientation)
{
	if(orientation == 3)
	{
		return 0;
	}
	else
	{
		return orientation+1;
	}
}
static int rotate_full(int orientation)
{
	if(orientation > 1)
	{
		return orientation - 2;
	}
	else
	{
		return orientation + 2;
	}
}

static int look(struct pos_t pos,int orientation)
{

	struct pos_t target;

	target = translate(pos,orientation);

	if((player.pos.x == target.x)&&(player.pos.y == target.y))
	{
		return 1;
	}
	if(initial_field[target.x][target.y] == 0)
	{
		return 0;
	}
	return look(target,orientation);

}


static void movebot(struct bot_t* bot)
{

	if(bot->dying > 0)
	{
		return;
	}

	struct pos_t target;

	int option_count = 0;
	int options[3];

	if(check(bot->pos,bot->orientation))
	{
		options[option_count]=1;
		option_count++;
	}
	if(check(bot->pos,rotate_left(bot->orientation)))
	{
		options[option_count]=2;
		option_count++;
	}
	if(check(bot->pos,rotate_right(bot->orientation)))
	{
		options[option_count]=3;
		option_count++;
	}

	if(option_count == 0)
	{
		bot->orientation = rotate_full(bot->orientation);
		return;
	}

	if(look(bot->pos,bot->orientation))
	{
		option_count=1;
		options[0]=1;
	}
	else if(look(bot->pos,rotate_left(bot->orientation)))
	{
		option_count=1;
		options[0]=2;
	}
	else if(look(bot->pos,rotate_right(bot->orientation)))
	{
		option_count=1;
		options[0]=3;
	}


	int selected =	rand()%option_count;

	if(options[selected]==1)
	{
		move(bot->pos,bot->orientation,&target);
		bot->pos = target;
	}
	if(options[selected]==2)
	{
		move(bot->pos,rotate_left(bot->orientation),&target);
		bot->orientation = rotate_left(bot->orientation);
		bot->pos = target;
	}
	if(options[selected]==3)
	{
		move(bot->pos,rotate_right(bot->orientation),&target);
		bot->orientation = rotate_right(bot->orientation);
		bot->pos = target;
	}



}

static void eat(struct pos_t pos)
{
	if(field_dots[pos.x][pos.y]==1)
	{
		field_dots[pos.x][pos.y]=0;
	}
	if(field_pups[pos.x][pos.y]==1)
	{
		field_pups[pos.x][pos.y]=0;
		pupmode=100;
		for(int i =0;i<4;i++)
			bots[i].orientation = rotate_full(bots[i].orientation);
	}
}

static int check_collision(struct bot_t *bot,struct bot_t *player)
{
	if((bot->pos.x == player->pos.x)&&(bot->pos.y == player->pos.y))
	{
		return 1;
	}
	return 0;
}



static void collision_check(void)
{
	for(int i =0;i<4;i++)
		if(check_collision(&bots[i],&player))
		{
			if(pupmode)
			{
				if(bots[i].dying==0)
				{
					bots[i].dying=25;
				}
			}
			else
			{
				if(hurt==0)
				{
					lives--;
					hurt = 30;
				}
			}
		}

}

static int x = 0;

static uint8_t tick(void) {

	x++;

	lcdFillRGB(0,0,0);

	for(int x =0;x<21;x++)
		for(int y=0;y<21;y++)
		{
			if(field_dots[x][y]==1)
			{
				set_block(x,y,32,32,32);
			}
			if(field_pups[x][y]==1)
			{
				set_block(x,y,64,64,64);
			}
			if(initial_field[x][y]==0)
			{
				set_block(x,y,0,0,200);
			}
		}

	if(door==0)
		set_block(DOOR_X,DOOR_Y,64,0,96);
	
	for(int i = 0;i<4;i++)
	{
		int r = bot_r[i];
		int g = bot_g[i];
		int b = bot_b[i];

	
		if((bots[i].dying%2)==1)
		{
			r=255;
			g=255;
			b=255;
		}
		else if(pupmode>20)
		{
			if(pupmode%2)
			{
				r = 64;
				g = 32;
				b = 148;
			}
			else
			{
				r=32;
				g=64;
				b=148;
			}
		}
		else if(pupmode)
		{
			if(pupmode%2)
			{
				r = 64;
				g = 32;
				b = 148;
			}
		}
		set_block(bots[i].pos.x,bots[i].pos.y,r,g,b);
	
	
	
		if(bots[i].dying > 1)
		{
			bots[i].dying--;
		}
		if(bots[i].dying == 1)
		{
			bots[i].dying=0;
			bots[i].pos.x=9;
			bots[i].pos.y=9;
		}
	}

	if(hurt%2)
	{
		set_block(player.pos.x,player.pos.y,64,64,0);
	}
	else
	{
		set_block(player.pos.x,player.pos.y,255,255,0);
	}

	for(int i = 0;i<lives;i++)
	{
		set_block(11,1+i,255,255,0);
	}


	if((pupmode)&&(x%3))
	{
	}
	else
	{
		if((x%7) == 0) movebot(&bots[0]);
		if((x%8) == 0) movebot(&bots[1]);
		if((x%9) == 0) movebot(&bots[2]);
		if((x%10) == 0) movebot(&bots[3]);
	}


	collision_check();

	if((x%5) == 0)
	{
		
		if(joy_is_up())
		{
			if(check(player.pos,2))
			{
				struct pos_t target;
				move(player.pos,2,&target);
				player.pos = target;
			}
		}
		if(joy_is_down())
		{
			if(check(player.pos,0))
			{
				struct pos_t target;
				move(player.pos,0,&target);
				player.pos = target;
			}
		}
		if(joy_is_left())
		{
			if(check(player.pos,3))
			{
				struct pos_t target;
				move(player.pos,3,&target);
				player.pos = target;
			}
		}
		if(joy_is_right())
		{
			if(check(player.pos,1))
			{
				struct pos_t target;
				move(player.pos,1,&target);
				player.pos = target;
			}
		}



	}
	//	movebot(&player);

	collision_check();

	if(pupmode > 0)
		if((x%2)==0)	pupmode--;
	if(hurt > 0)
		if((x%2)==0)	hurt--;

	
	if((x%100) == 0) door = 1;
	if((x%100) == 15) door = 0;

	eat(player.pos);


	return 0;
}


static void constructor(void) CONSTRUCTOR_ATTRIBUTES
void constructor(void) {
	registerAnimation(init,tick,deinit, 30, 5000);
}

