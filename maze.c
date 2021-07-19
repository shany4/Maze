/**********************************************************************************************
 * Program Assignment : Maze
 * Date: 2014-11-17
 * Author: Ying Shan 1446842
 * Description: Task1:
 *              Find and Read a maze file that already exit in computer. User is able to type
 *              the filename to run it. If the exit1 can be found, it will print out the rout,
 *              otherwise, maze will not come out.
 *
 *              Task2:
 *              Randomly generates a maze and the recursive algorithm is already given. When
 *              user types RANDOM, it replaces filename for argv[1], and display the maze, with
 *              in route the scree.
 *
 *              Task3:
 *              Using SDL show the output of maze. When user types SDL, it will be as argv[2].
 *
 **********************************************************************************************/
/* Functions:
 *
 *  read_maze      Read a maze from the file. Before read the maze, there is a check in whether
 *                 the file can be openned successfully.
 *
 *  find_entry     finding the entrance and exit of the maze. Each maze has two gaps, and each gap
 *                 has two coordinates x, and y. Since the entrance is always at left of exit, the
 *                 sum value of entrance will be less than the sum value of exit.
 *
 *  explore_maze   This function will detect the rout from entrance to exit of particular maze.
 *                 It will check the 4 directions (top,down,left,right) whether can make a step
 *                 It moves on step by step, until it finds the exit.
 *
 *  set_maze       This function will set the size of the RANDOM maze and draw the board of it.
 *
 *  inner_wall     This function will create walls in side the maze board for the RANDOM maze. It
 *                 will randomly divide the RANDOM maze. The disctance between two can not larger
 *                 than 4.
 *
 *  dig_gap        This function is make 3 gaps in 4 parts of walls randomly. This is a
 *                 recursive algorithm to generate maze.
 *
 *
 *******************************************************************************************************
 *  Finished:      Maze + Maze Generation + SDL
 *                 The RANDOM maze may take a bit time to load in lab's mechine, it work perfect in my own mechine though
 *                 SUBMITTED LATE
 *
 *****************************************************************************************************/
/*   Compile: gcc neillsdl2.c maze.c -o maze `sdl2-config --cflag -Wall -pedantic -std=c99 -lm `sdl2-config-libs`
 *
 ******************************************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include"neillsdl2.h"

/*used in the main function, give me array number that store 20 elements (wall) for maze, that connect with inner_wall_maze function.*/
#define START_MAZE 0
#define END_MAZE 19

/*used in find_entry function, set the x and y coordinates for the entrance and exit of the maze*/
#define MAZE_GAP 4

/*used in set_maze function, set the fix size for the generation maze*/
#define SIZE_MAZE 20

/*used in the inner_wall_maze function. in order to give enough space for dig gap and create route, the minimum size of the wall is four*/
#define MIN_WALL 4

/*used in DispSDL function, set the color (0-255) for the maze that used in SDL*/
#define EXP_R 86
#define EXP_G 124
#define EXP_B 255
#define WALL_R 186
#define WALL_G 224
#define WALL_B 12
#define BORDER_R 52
#define BORDER_G 52
#define BORDER_B 52
#define ROUTE_R 255
#define ROUTE_G 255
#define ROUTE_B 255
/*used in DispSDL function, set size for the square which build wall of maze*/
#define RECTSIZE 40


char **read_maze(char *read, int *rows, int *cols);
void find_entry(int rows, int cols, char **maze, int  *in_x, int *in_y, int *out_x, int  *out_y);
int explore_maze(char **maze,int rows, int cols,int in_x,int in_y,int out_x,int out_y,char *sdl);

char **set_maze(int *rows, int *cols);
void inner_wall_maze(char **maze,int start_x,int start_y,int end_x,int end_y);
void dig_gap_maze(char **maze,int start_x,int start_y,int end_x,int end_y,int mid_x,int mid_y);

int DispSDL(int cols, int rows, char **maze);



int main(int argc, char **argv)
{
  int rows, cols, in_x=0, in_y=0, out_x=0, out_y=0;
  char **maze;
  char *read,*sdl = NULL;
  read = argv[1];
  sdl =  argv[2];
  srand(time(NULL));
/*User can choose either genetate a maze randomly or read the maze files  (argv[1] argv[2])*/
  if(strcmp(read,"RANDOM") == 0)
  {
    maze = set_maze(&rows, &cols);
    inner_wall_maze(maze,START_MAZE,START_MAZE,END_MAZE,END_MAZE);//
  }
  else
  {
    maze = read_maze(argv[1], &rows, &cols);
  }
  find_entry(rows, cols, maze, &in_x, &in_y, &out_x, &out_y);

  if(explore_maze(maze,rows,cols,in_x,in_y,out_x,out_y,sdl) == 0)
  {
    printf("get lost\n"); //if not route to get exit gap, print get lost
    return 1;
  }

  return 0;
}



char **read_maze(char *read, int *rows, int *cols)
{

  char **maze;
  int i, y, nx, ny;
  FILE *fp;//use fp pointer to find the file.
  fp = fopen(read, "r");
  if (fp == NULL)
  {
    printf("the file name is not correct");
  }

  fscanf(fp, "%d %d ", &nx, &ny);
  *rows = ny;
  *cols = nx;
/*allocate a memorey space , set a pointer of pointer, which is 2 level redirection as a 2d array, in order to store the maze*/
  maze = (char**)malloc(ny* sizeof(char*));

  for(i=0;i<ny;i++)
  {
    maze[i] = (char*)malloc(nx+2* sizeof(char*));//+2 here so that the space been allocate is big enough to store some invisiable signs.
  }

  for(y=0;y<ny;y++)
  {

    for(i=0;i<ny;i++)
    {
      maze[i] = (char*)malloc(nx+2* sizeof(char*));
    }
  }

  for(y=0;y<ny;y++)
  {
      fgets(maze[y],nx+3,fp);//+3 here so that the program can read all invisable signs in the file (enter key)

  }
  fclose(fp);

  return maze;
}



void find_entry(int rows, int cols, char **maze,int  *in_x, int *in_y, int *out_x, int  *out_y){

/*It be defined that there are two gaps in this maze ,so the gap_coordinate array stored only 4 elements.which already hash defined at the top*/
  int gap_coordinate[MAZE_GAP];
  int i, j;
  int n = 0; //we set an array for size of 4 for saving the coordinates of entrance and exit gaps, and the array start at 0.
  int a, b;
/*detect everything in rows of the maze*/
  for(i= 0; i<=rows-1; i++){
    if(maze[i][0]==' '){
      gap_coordinate[n] = i;  //The coordinates(i,0) is one of gap
      gap_coordinate[n+1] = 0;
      n = 2;//this means the array has store 2 elements (x and y coordinates)
    }

    if(maze[i][cols-1]==' '){
      gap_coordinate[n] = i;  // the other position(i,cols-1) is another opennings.
      gap_coordinate[n+1] = cols-1;
      n = 2;
    }
  }
/*detect everthing in cols of the maze*/
  for(j=0; j<=cols-1; j++){
    if(maze[0][j]==' '){
      gap_coordinate[n] = 0;//coordinate(0,j)
      gap_coordinate[n+1] = j;
      n = 2;
    }
    if(maze[rows-1][j]==' '){
      gap_coordinate[n] = rows-1;//coordinate)(row-1, j)
      gap_coordinate[n+1] = j;
      n = 2;
    }
  }
/*get sum of coordinates*/
  a = gap_coordinate[0]+gap_coordinate[1];
  b = gap_coordinate[2]+gap_coordinate[3];

/*the gap with smaller coordinate sum nummber is in the left side, which is the entrance*/
  if(a < b){
    *in_y=gap_coordinate[0];
    *in_x=gap_coordinate[1];

    *out_y=gap_coordinate[2];
    *out_x=gap_coordinate[3];
  }
  else{
    *in_y=gap_coordinate[0];
    *in_x=gap_coordinate[1];

    *out_y=gap_coordinate[2];
    *out_x=gap_coordinate[3];

  }
}


int explore_maze(char **maze,int rows,int cols,int in_x,int in_y,int out_x,int out_y,char *sdl)
{
  maze[in_y][in_x] = '.';
  if(in_y==out_y && in_x==out_x)
  {
    if(sdl != NULL)//if the sdl pointer is not 0, which means the SDL file is pointed by *sdl
    {
      if(strcmp(sdl,"SDL") == 0)
      {
	DispSDL(cols, rows, maze);//recieve SDL function here, achieve the SDL effect
      }
      else
      {
        printf("NO SDL\n");//if unable to find SDL file, print NO SDL
      }
    }

    for(int i=0;i<rows;i++)
    {
      printf("\n");
      for(int j=0;j<cols;j++)
      {
        printf("%c",maze[i][j]);
      }
    }
    return 1;
  }
/*detect all direction of the momevent, which use the Recursive method, if a wall been detected then change direction step by step, until the exit is been found*/
  if(maze[in_y][in_x+1] == ' '
     && explore_maze(maze,rows,cols,in_x+1,in_y,out_x,out_y,sdl))
  {
    return 1;
  }

  if(maze[in_y+1][in_x] == ' '
     && explore_maze(maze,rows,cols,in_x,in_y+1,out_x,out_y,sdl))
  {
    return 1;
  }

  if(maze[in_y][in_x-1] == ' '
     && explore_maze(maze,rows,cols,in_x-1,in_y,out_x,out_y,sdl))
  {
    return 1;
  }

  if(maze[in_y-1][in_x] == ' '
     && explore_maze(maze,rows,cols,in_x,in_y-1,out_x,out_y,sdl))
  {
    return 1;
  }

  maze[in_y][in_x] = ' ';
  return 0;
}


char **set_maze(int *rows,int *cols)
{
  int i, y, x, ny,nx;
  char **maze;

  ny = SIZE_MAZE;
  nx = SIZE_MAZE;

  *rows = ny;
  *cols = nx;
/*allocate a memory space, same purpose with the one in read_maze funtion*/
/*set the border of the maze here*/
  maze = (char**)malloc(sizeof(char*)*ny);
  for(i=0;i<ny;i++)
  {
    maze[i] = (char*)malloc(sizeof(char*)*nx);
  }
  for(y=0;y<ny;y++)
  {
    for(x=0;x<nx;x++)
    {
      maze[y][x] = ' ';
    }
  }
  for(y=0;y<ny;y++)
  {
    maze[y][0] = '#';
    maze[y][nx-1] = '#';
  }
  for(x=0;x<nx;x++)
  {
    maze[0][x] = '#';
    maze[ny-1][x] = '#';
  }
/*set the entrance and exit for the random maze, which is left top and right bottom.*/
  maze[1][0] = ' ';
  maze[ny-2][nx-1] = ' ';

  return maze;
}



void inner_wall_maze(char **maze,int start_x,int start_y,int end_x,int end_y)
{
/* based on the maze generation algorithm, divide the maze in many parst randomly*/
  int mid_x=0,mid_y=0,i;
  mid_x = rand()%(end_x-start_x-MIN_WALL)+2+start_x;
  mid_y = rand()%(end_y-start_y-MIN_WALL)+2+start_y;
  for(i=start_x;i<end_x;i++)
  {
    maze[mid_y][i] = '#';//same as maze in file, use # represent wall
  }
  for(i=start_y;i<end_y;i++)
  {
    maze[i][mid_x] = '#';
  }
/*dig gap for the random maze, make sure that the size of the maze in maze always larger in minmum limited in all situation*/
  dig_gap_maze(maze,start_x,start_y,end_x,end_y,mid_x,mid_y);

  if((mid_y-start_y)>MIN_WALL && (mid_x-start_x)>MIN_WALL)
  {
    inner_wall_maze(maze,start_x,start_y,mid_x,mid_y);
  }

  if((end_y-mid_y)>MIN_WALL && (end_x-mid_x)>MIN_WALL)
  {
    inner_wall_maze(maze,mid_x,mid_y,end_x,end_y);
  }

  if((end_y-mid_y)>MIN_WALL && (mid_x-start_x)>MIN_WALL)
  {
    inner_wall_maze(maze,start_x,mid_y,mid_x,end_y);
  }

  if((mid_y-start_y)>MIN_WALL && (end_x-mid_x)>MIN_WALL)
  {
    inner_wall_maze(maze,mid_x,start_y,end_x,mid_y);
  }
}


void dig_gap_maze(char **maze,int start_x,int start_y,int end_x,int end_y,int mid_x,int mid_y)
{
/* based on the maze generation algorithm, randomly dig gap on the inner wall that just found in the function above*/
/*There are four situations(left, right, bottom; right, bottom, top; left, bottom, top; right, left, top), for each side of inner wall, each time dig 3 gas in different direction*/
  int t = rand()%4;
  int a[4];
  a[0] = rand()%(mid_y-start_y-1)+1+start_y;
  a[1] = rand()%(end_x-mid_x-1)+1+mid_x;
  a[2] = rand()%(end_y-mid_y-1)+1+mid_y;
  a[3] = rand()%(mid_x-start_x-1)+1+start_x;

   if(t == 0)
  {
    maze[mid_y][a[1]] = ' ';
    maze[a[2]][mid_x] = ' ';
    maze[mid_y][a[3]] = ' ';
  }
  else if(t == 1)
  {
    maze[a[0]][mid_x] = ' ';
    maze[a[2]][mid_x] = ' ';
    maze[mid_y][a[3]] = ' ';
  }
  else if(t == 2)
  {
    maze[a[0]][mid_x] = ' ';
    maze[mid_y][a[1]] = ' ';
    maze[mid_y][a[3]] = ' ';
  }
  else if(t == 3)
  {
    maze[a[0]][mid_x] = ' ';
    maze[a[2]][mid_x] = ' ';
    maze[mid_y][a[1]] = ' ';
  }
}


/***************************************************************************************************************
* SDL:
*
*     This is Neill's SDL function, I just simply adjusted it: set the wall of maze as square
*     (can set as other shape too)shape, and set wall, route, explored wall and border of each
*     square in different color (0-255)
*
*     The function elplord_maze function call it under the SDL parts, and main function recieve
*     the parameter and set it as argv[2]
*
*
***************************************************************************************************************/
int DispSDL(int cols, int rows, char **maze)

{

  int y, x;

  SDL_Simplewin sw;

  SDL_Rect rectangle;


  rectangle.w = RECTSIZE;

  rectangle.h = RECTSIZE;

  Neill_SDL_Init(&sw);


  for (y = 0; y <(rows ); y++)

  {

    rectangle.y = RECTSIZE * y;

    for (x = 0; x < (cols ); x++)

    {

      rectangle.x = RECTSIZE* x;

      if (maze[y][x]==' ')

      {   Neill_SDL_SetDrawColour(&sw,ROUTE_R , ROUTE_G, ROUTE_B);

        SDL_RenderFillRect(sw.renderer, &rectangle);

      }

      if (maze[y][x]=='.')

      {Neill_SDL_SetDrawColour(&sw, EXP_R, EXP_G, EXP_B);

        SDL_RenderFillRect(sw.renderer, &rectangle);

      }

      if  (maze[y][x]=='#')

      {Neill_SDL_SetDrawColour(&sw, WALL_R, WALL_G, WALL_B);

        SDL_RenderFillRect(sw.renderer, &rectangle);

      }



      Neill_SDL_SetDrawColour(&sw, BORDER_R, BORDER_G, BORDER_B);

      SDL_RenderDrawRect(sw.renderer, &rectangle);

    }

  }


  while (!sw.finished){

    SDL_RenderPresent(sw.renderer);

    SDL_UpdateWindowSurface(sw.win);

    Neill_SDL_Events(&sw);

  }
  atexit(SDL_Quit);

  return 0;
}
