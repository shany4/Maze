//full assign-ment

/****************************************************************************************
 * Program Assignment : Maze
 * Date: 2014-11-17
 * Author: Chen Ying
 * Description: Task1:
 *              This programe is used for finding the exit1 of a maze, which is read from the
 *              files that the user can type the filename to run it.If the exit1 can be found, 
 *              it could print out the rout, otherwise,it shows: There is no exit1!
 *              Task2:
 *              Generates a maze randomly and the recursive algorithm is already given. When 
 *              user types RANDOM, it replaces filename for argv[1].
 *              Task3:
 *              Using SDL show the output of maze. When user types SDL, it will be as argv[2].
 *
 ****************************************************************************************/
/* Function List:
 *  file_maze     Read a maze from the file. Before read the maze, there is a check in whether
 *                the file can be openned successfully. When read the maze, the size of maze
 *                (width and height)could be read and be printed out in first line.
 *  find_entrance  This function is used for finding the entrance and exit1 of the maze. We know 
 *                 that there are only two opennings. If we can find one is entrance, the
 *                 otheris exit1. And the entrance, its sum value of column and row is less 
 *                 than the exit1.
 *   explore       This is core function of this program,namely, the rout from entrance to exit1
 *                 could be detected by this function. And the recursive method is used in this 
 *                 part. It check the 4 directions (top,down,left,right) whether can make a step
 *                 It moves on step by step, until it finds the exit1, otherwise, it cannot detect
 *                 exit1.
 *  maze_generated This is mainly function  to generate a maze randomly. This function generates
 *                 the borders of maze and initialise the maze. In this function, it calls
 *                 make_maze function.
 *   make_maze     This function is make the walls and the position of walls is randomly. Hence, 
 *                 it is neccessary to get a position randomly firstly, then generates vertical
 *                 walls and horizontal walls by this position , next step is this funtion
 *                 recursive until a conditon it stops, which is the distance between two walls
 *                 is no more than 4.
 *   dig_hole      This function is make 3 ‘ ’(space) in 4 parts of walls randomly. This is a 
 *                 recursive algorithm to generate maze.
 *   print_out     Print out the rout from entrance to exit1 in a maze board.
 ******************************************************************************************/



#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include "neillsdl2.h"


#define N_ENT_exit1 4  //The number of  counting the  entrance in x row and y column, exit1 in x row and y column
#define MINDIS 2  // The mininum distance between two walls
#define NWALLS 4 //The number of walls in every generation wall
#define RECTSIZE 20
#define MILLISECONDDELAY 1000

//store the position of entrance
struct entrance{
	int x;
	int y;
};
typedef struct entrance Ent;
//store the position of exit1
struct exit1{
	int x;
	int y;
};
typedef struct exit1 Exit;
//size of maze
struct size{
	int width;
	int height;
};
typedef struct size Size;


char **file_maze(char* FILENAME, int *x, int *y);
void find_entrance(Size *size,char **maze,Ent *ent, Exit *exit1);
int explore(int i,int j, char **maze, int endx,int endy, Size size,int is_SDL,SDL_Simplewin *sw);
void print_rout(char **maze, Size size,int is_SDL,SDL_Simplewin *sw);
char **maze_generated(Size *size, Ent *ent, Exit *exit1);
void make_maze(char **maze,int startx ,int starty,int hx,int wy);
void dig_hole(int startx,int starty, char **maze, int hx, int wy,int x,int y);



int main(int argc, char *argv[]){

	int width, height;// These variables stand for the width and height of maze.
	char *FILENAME = argv[1];// *FILENAME stores the string the user input
	Size size;       // The type of variable size is defined in struct size.
	Ent ent;         // The type of variable ent is entrance.
	Exit exit1;       // The type of variable exit1 is exit1.
	char **maze=0;   // maze is a 2 dimension array to print the maze board.
	int expl;        // The return value of function explore is stored in this variable.
	int is_SDL =0;      //0 is no SDL and 1 is SDL print out
	char *SDL = NULL; //store the second user input 
	SDL_Simplewin sw; 
	SDL = argv[2];

	if (argv[1] == NULL)
	{
		printf("no input of file name,exit1\n");
		exit(1);
	}
	
	/* User can choose either genetate a maze randomly or read a maze from file*/
	if((strcmp(argv[1],"RANDOM"))==0){

		maze=maze_generated(&size,&ent,&exit1);//Generate the maze randomly
		exit1.x=size.height-1;                //defining the exit1 position is at maze[size.height-1][size.width-2].
		exit1.y=size.width-2;
		ent.x=0;                             //defining the entrance position is at maze[0][1].
		ent.y=1;

	}else {

		maze=file_maze(FILENAME,&width, &height); //Get the maze from file

		size.width=width;          //Store the width and height of maze to size, which are from the file
		size.height=height;

		find_entrance(&size,maze,&ent,&exit1);// find the entrance and exit1 of the maze read from file.
	}
	if ( SDL != NULL)
	{
		if (strcmp(argv[2],"SDL") == 0)
		{
			Neill_SDL_Init(&sw);
			is_SDL = 1;
		} else
		{
			printf("wrong input of SDL,exit1\n");
			exit(1);
		}
	}

	
	expl=explore(ent.x,ent.y,maze,exit1.x,exit1.y,size,is_SDL,&sw);  //Find the rout in maze


	/* Free the poiinters */ 

	for(int i=0;i<size.height;i++){
		free(maze[i]);

	}
	free(maze);


	/* if the rout cannot be found, print out no exit1 and return 1; if there is existing a rout ,return 0 */
	if(expl==0){   
		printf("no exit!\n");
		return 1;
	}
	return 0;
}


/****************************************************************************************
 * Function Name: file_maze
 * Purpose :      Read a maze from the file. Before read the maze, there is a check in whether
 *                the file can be openned successfully. When read the maze, the size of maze
 *                (width and height)could be read and be printed out in first line.

 * Params  :
 *   @int   *x,*y      pass the address of width value to the parameter
 *   @char  *FILENAME  *FILENAME stores the string the user input
 * Return   char **maze get the maze information.
 *****************************************************************************************/
char **file_maze(char *FILENAME, int *x, int *y){

	FILE *rfp; // rfp is a file pointer to find the file.
	int width,height; // the sise of maze.
	char **maze;  // Maze is the 2D array to store the value information of maze.

	/* Checking whether the file can be openned.*/
	if((rfp=fopen(FILENAME,"r"))==NULL){      //Open the file FILENAME.

		printf("Cannot open file\n");
		exit(1);
	
	}else{
		printf("The file is openned\n");
		
	}


	fscanf(rfp,"%d",&width);             //Get the width of maze from the file
	fscanf(rfp,"%d ",&height);           //Get the height of maze from the file

	*x = width;                          //pass the address of width value to the parameter
	*y = height;                         //pass the address of height value to the parameter

	/*Allocate a two dimension dynamical arrary maze to store values of maze.*/
	maze=(char**)malloc(sizeof(char*)*height);
	for(int i = 0; i < height ; i++)
		maze[i] = (char*)malloc(sizeof(char)*(width+1));  //width+1 is because the enter is at the end of every row.

	printf("%d %d\n",width,height);

	/* Read and print out the value of every element of 2D array maze */
	/* for(int i=0;i<height;i++){
	   for(int j=0;j<width+1;j++){

	   fscanf(rfp,"%c",&maze[i][j]);
	   printf("%2c",maze[i][j]);


	   }
	   }
	 */
	for(int i=0;i<height;i++){

		fgets(maze[i], width+2+1,rfp);     // The string is end of '\0', so there is a need to add i when getting characters from file
		for(int j=0; j<width+2; j++){
			if(maze[i][j]!=' '&& maze[i][j]!='#'){   // If it is the enter or other characters, we turn it into space' ' 
				maze[i][j]=' ';
			}
		}
	}

	/* Print out the maze board we read from file*/
	for(int i=0;i<height;i++){                      
		for(int j=0;j<width+1;j++){
			printf("%2c",maze[i][j]);       
		}
		printf("\n");
	}

	/*for(int i=0;i<width+1;i++){
	  free(maze[i]);

	  }
	  free(maze);*/



	fclose(rfp);    // After use  a file must be closed.

	return maze;
}


/****************************************************************************************
 * Function Name: find_entrance
 * Purpose :       This function is used for finding the entrance and exit1 of the maze. We know
 *                 that there are only two opennings. If we can find one is entrance, the
 *                 otheris exit1. And the entrance, its sum value of column and row is less
 *                 than the exit1.
 * Params  :
 *   @exit1 exit1      the struct ent stands for entrance of maze, the member variable is x,y
 *   @char **maze    the 2D array maze[][] stores  the information of maze.
 *   @Ent  ent       the struct ent stands for entrance of maze, the member variable is x,y
 *   @Size  size     the struct size represents the size of maze,
 *                   its member variables are width and height.
 * Return        void
 *****************************************************************************************/
/*  Please note that, it is already defined that there are only two opennings in this maze*/
void find_entrance(Size *size,char **maze,Ent *ent, Exit *exit1){

	int openning[4]={0};// The oppenning[] array stores only 4 elements, separately,
	//entrance in x row, entrance in y column, exit1 in x row, exit1 in column
	int n=0;            // n is the index in array openning, from 0 to 3;


	/* Find the ' ', from the [height-1] row to the first row in the first colunm and last colunm*/
	for(int i=(size->height)-1;i>=0;i--){
		if(maze[i][0]==' '){      //openning[i][0] is one of possible entrance or exit1

			openning[n]=i;
			openning[n+1]=0;
			n=n+2;
		}
		if(maze[i][(size->width)-1]==' '){    ////openning[i][size->width-1] is one of possible entrance or exit1
			openning[n]=i;
			openning[n+1]=(size->width)-1;
			n=n+2;
		}
	}

	/* Find the ' ', from the [width-1] colunm to the first column in the first row and last row */
	for(int j=(size->width)-1;j>=0;j--){

		if(maze[0][j]==' '){   //openning[0][j] is one of possible entrance or exit1

			openning[n]=0;
			openning[n+1]=j;
			n=n+2;
		}
		if(maze[(size->height)-1][j]==' '){  //openning[j][size->height-1] is one of possible entrance or exit1

			openning[n]=(size->height)-1;
			openning[n+1]=j;
			n=n+2;
		}
	}

	/* the entrance of [x][y], ent->x,ent->y, (ent->x + ent->y)<(exit1->x+exit1->y)
	   otherwise, it is exit1[x][y]*/
	if((openning[0]+openning[1])<=(openning[2]+openning[3])){
		ent->x=openning[0];
		ent->y=openning[1];
		exit1->x=openning[2];
		exit1->y=openning[3];

	}else{
		ent->x=openning[2];
		ent->y=openning[3];
		exit1->x=openning[0];
		exit1->y=openning[1];
	}
//	printf("%d %d %d %d\n",ent->x,ent->y,exit1->x,exit1->y);
//	printf("%d %d\n",size->width,size->height);
}


/****************************************************************************************
 * Function Name: explore
 * Purpose :       The rout from entrance to exit1 could be detected by this function. And
 *                 the recursive method is used in this part. 
 *                 It check the 4 directions (top,down,left,right) whether can make a step
 *                 It moves on step by step, until it finds the exit1, otherwise, it cannot detect
 *                 exit1.
 * Params  :
 *   @int   i, j     i represents the step in which row, the j reprents the step in which column.
 *   @char **maze    the 2D array maze[][] stores  the information of maze.
 *   @int  endx,endy the exit1
 *   @Size  size     the struct size represents the size of maze, 
 *                   its member variables are width and height.
 * Return        1   the rout is found
 *               0    do not find the rout
 *****************************************************************************************/
int explore(int i,int j, char **maze, int endx,int endy, Size size,int is_SDL,SDL_Simplewin *sw){
	
	maze[i][j]='.'; // find the space as the step ,marking '.'

	// find the exit1, if success
	if(i==endx && j==endy){

		

		print_rout(maze, size,is_SDL,sw);// print out the maze board
		return 1;
	}

	// if we can go right
	if(j+1 < size.width && maze[i][j+1]==' '){  //The condition it can move on is not overstep the border
		//and the in right direction, it is space.
		if(explore(i,j+1,maze,endx,endy,size,is_SDL,sw))
			return 1;
	}
	// if we can go down
	if(i+1< size.height && maze[i+1][j]==' '){

		if(explore(i+1,j,maze,endx,endy,size,is_SDL,sw))
			return 1;
	}
	// if we can go up
	if(i-1>=0 && maze[i-1][j]==' '){

		if(explore(i-1,j,maze,endx,endy,size,is_SDL,sw))
			return 1;
	}
	// if we can go left
	if(j-1>=0 && maze[i][j-1]==' '){
		if(explore(i,j-1,maze,endx,endy,size,is_SDL,sw))
			return 1;
	}
	// find the step, but it doesn't succeed
	maze[i][j]=' ';
	return 0;
}


/****************************************************************************************
 * Function Name: print_out
 * Purpose :         print out the maze board
 * Params  :
 *   @char **maze    the 2D array maze[][] stores  the information of maze.
 *   @Size  size     the struct size represents the size of maze,
 *                   its member variables are width and height.
 * Return        void
 *****************************************************************************************/
void print_rout(char **maze, Size size,int is_SDL,SDL_Simplewin *sw){

	SDL_Rect rectangle1;
	SDL_Rect rectangle2;
	rectangle1.w = RECTSIZE;
	rectangle1.h = RECTSIZE;
	rectangle2.w = RECTSIZE-2;
	rectangle2.h = RECTSIZE-2;
	if (is_SDL ==0)
	{
		printf("the rout is:\n");
	}
	for(int i=0;i<size.height;i++){
		for(int j=0;j<size.width;j++){
			if (is_SDL == 0)
			{        
				printf("%2c",maze[i][j]);
			} else
			{
				if (maze[i][j] == '.')
				{
					Neill_SDL_SetDrawColour(sw,255,255,0);
					rectangle2.x = j * RECTSIZE + 1;
					rectangle2.y = i * RECTSIZE + 1;
					SDL_RenderFillRect(sw->renderer, &rectangle2);
				}
				if (maze[i][j] == '#')
				{
					Neill_SDL_SetDrawColour(sw,255,0,255);
					rectangle1.x = j * RECTSIZE;
					rectangle1.y = i * RECTSIZE;
					SDL_RenderFillRect(sw->renderer, &rectangle1);
				}	
			}
		}
		if (is_SDL == 0) {printf("\n");}
	}
	if (is_SDL == 1) 
	{
		SDL_RenderPresent(sw->renderer);
		SDL_UpdateWindowSurface(sw->win); 
		SDL_Delay(MILLISECONDDELAY);
	}
}



/****************************************************************************************
 * Function Name: maze_generated
 * Purpose :      This is mainly function  to generate a maze randomly. This function generates
 *                the borders of maze and initialise the maze. In this function, it calls
 *                make_maze function.
 * Params  :
 *   @exit1 exit1      the struct ent stands for entrance of maze, the member variable is x,y
 *   @Ent  ent       the struct ent stands for entrance of maze, the member variable is x,y
 *   @Size  size     the struct size represents the size of maze,
 *                   its member variables are width and height.
 * Return        char **maze
 *****************************************************************************************/
char **maze_generated(Size *size, Ent *ent, Exit *exit1){

	srand(time(NULL)); //sead of random number

	int  maze_size=rand()%1+20;   // Defining the random size of maze is from 25*25 to 50*50,
	//because this is the appropriate scale to observe maze.
	size->width=maze_size;
	size->height=maze_size;

	//Allocate a two dimension dynamical arrary maze to store values of maze.
	char **maze;
	maze=(char**)malloc(sizeof(char*)*(size->height));
	for(int i = 0; i < size->height ; i++)
		maze[i] = (char*)malloc(sizeof(char)*(size->width));

	//	Initialise the value of every element of maze is ' '.
	for(int i=0;i<size->width;i++)
		for(int j=0;j<size->height;j++){

			maze[i][j]=' ';
		}


	/*Make the the borders of wall */
	/*Those are the elements of maze[0][],maze[size.height-1][],maze[][0]. maze[][width-1] are '#', '#'stands for wall*/
	for(int i=0;i<size->height;i++){
		maze[i][0]='#';
		maze[i][size->width-1]='#';

	}
	for(int j=0;j<size->width;j++){
		maze[0][j]='#';
		maze[size->height-1][j]='#';
	}

	//The entrance of this random maze is at maze[0][1],the exit1 is at [size->height-1][size->width-2]
	maze[0][1]=' ';
	maze[size->height-1][size->width-2]=' ';

	/*printf the boarder of wall	for(int i=0;i<size.width;i++){
	  for(int j=0;j<size.height;j++){


	  printf("%c",maze[i][j]);


	  }
	  printf("\n");

	  }

	 */


	/*Now we can generate the maze following the recursive method */
	/* The entrance of this random maze is at maze[0][1],the exit1 is at [size->height-1][size->width-2]*/
	ent->x=0;
	ent->y=1;
	exit1->x=size->height-1;
	exit1->y=size->width-2;
//	printf("%d %d\n",size->width,size->height);

	make_maze(maze,ent->x,ent->y,exit1->x,exit1->y);//Generate the inner walls in maze following the recursive method

	for(int i=0;i<size->width;i++){
		for(int j=0;j<size->height;j++){


			printf("%2c",maze[i][j]);


		}
		printf("\n");
	}


	return maze;
}


/****************************************************************************************
 * Function Name: make_maze
 * Purpose :       make the inner walls and the position of walls is randomly. Hence,
 *                 it is neccessary to get a position randomly firstly, then generates vertical
 *                 walls and horizontal walls by this position , next step is this funtion
 *                 recursive until a conditon it stops, which is the distance between two walls
 *                 is no more than 4.
 * Params  :
 *   @char **maze           the 2D array maze[][] stores  the information of maze.
 *   @int  startx,satrty    maze[startx][starty] is at the top left vertex
 *   @int  heightx,widthy    maze[heightx][widthy] is at the right bottom vertex
 * Return        void
 *****************************************************************************************/
void make_maze(char **maze,int startx ,int starty,int heightx,int widthy){


	//There is the scale of next wall in a new space, because two '#' cannot adjoin.
	int scalex=heightx-startx-MINDIS-MINDIS;
	int scaley=widthy-starty-MINDIS-MINDIS;

	//Randomized a position [currenty][currentx] to make vertical and horizontal walls
	int currenty=rand()%(scaley+1)+starty+MINDIS;
	int currentx=rand()%(scalex+1)+startx+MINDIS;

	/*At the position[currenty][currentx],make the vertical and horizontal walls */
	/* When generates a wall ,check the elements next to the start and the end of wall whether it is #
	   avoiding the new # get in the way. */
	for(int i=startx+1;i<heightx;i++){
		maze[currenty][i]='#';
	}
	if(maze[currenty][startx]==' '){

		maze[currenty][startx+1]=' ';

	}
	if(maze[currenty][heightx]==' '){


		maze[currenty][heightx-1]=' ';
	}


	for(int j=starty+1;j<widthy;j++){
		maze[j][currentx]='#';
	}
	if(maze[starty][currentx]==' '){

		maze[starty+1][currentx]=' ';

	}
	if(maze[widthy][currentx]==' '){


		maze[widthy-1][currentx]=' ';
	}



	// Now we can dig three holes on these 4 walls
	dig_hole(startx,starty, maze,heightx,widthy,currentx,currenty);

	//The distance between # and # is more than 4, otherwise cannot make the next wall
	if((currentx-startx)>=2*MINDIS && (currenty-starty)>=2*MINDIS){
		make_maze(maze,startx,starty,currentx,currenty);
	}
	if((currentx-startx)>=2*MINDIS && (widthy-currenty)>=2*MINDIS){
		make_maze(maze,startx,currenty,currentx,widthy);
	}
	if((heightx-currentx)>=2*MINDIS && (currenty-starty)>=2*MINDIS){
		make_maze(maze,currentx,starty,heightx,currenty);
	}
	if((heightx-currentx)>=2*MINDIS && (widthy-currenty)>=2*MINDIS){
		make_maze(maze,currentx,currenty,heightx,widthy);
	}


}



/****************************************************************************************
 * Function Name: dig_hole
 * Purpose :       This function is make 3 ‘ ’(space) in 4 parts of walls randomly. This is a
 *                 recursive algorithm to generate maze.
 * Params  :
 *   @char **maze           the 2D array maze[][] stores  the information of maze.
 *   @int  startx,satrty    maze[startx][starty] is at the top left vertex
 *   @int  heightx,widthy    maze[heightx][widthy] is at the right bottom vertex
 *   @int  currentx,currenty    maze[currentx][cyrrenty] is the position making the wall
 * Return        void
 *****************************************************************************************/
/*The position of hole can be calculated by these 6 parameters  */
void dig_hole(int startx,int starty, char **maze, int heightx, int widthy,int currentx,int currenty){

	int number=rand()% NWALLS; //the number 0 stands top wall,1 stands for left wall, 2 stands for right wall, 3 stands for bottom wall

	//The possible positions of hole on every wall
	// top wall: [startx,y]--[x,y], the scale  is x-startx-1
	// left wall:(x,starty)--(x,y), the scale  is y-starty-1
	// right wall:(x,y)--(x,wy), the scale  is wy-y-1
	// bottom wall:(x,y)--(hx,y), the scale  is hx-x-1

	/*Random a number and dig three holes on the other three walls , the position of holes are random*/
	if(number==0){

		//on left wall
		int digly=rand()%(currenty-starty-1)+1+starty;
		int diglx=currentx;
		maze[digly][diglx]=' ';

		//on right wall
		int digry=rand()%(widthy-currenty-1)+1+currenty;
		int digrx=currentx;
		maze[digry][digrx]=' ';

		//on bottom wall
		int digby=currenty;
		int digbx=rand()%(heightx-currentx-1)+1+currentx;
		maze[digby][digbx]=' ';

	}


	if(number==1){

		//on right wall
		int digry=rand()%(widthy-currenty-1)+1+currenty;
		int digrx=currentx;
		maze[digry][digrx]=' ';

		//on bottom wall
		int digby=currenty;
		int digbx=rand()%(heightx-currentx-1)+1+currentx;
		maze[digby][digbx]=' ';

		// on top wall
		int digty=currenty;
		int digtx=rand()%(currentx-startx-1)+1+startx;
		maze[digty][digtx]=' ';

	}

	if(number==2){

		//on left wall
		int digry=rand()%(currenty-starty-1)+1+starty;
		int digrx=currentx;
		maze[digry][digrx]=' ';


		//on bottom wall
		int digby=currenty;
		int digbx=rand()%(heightx-currentx-1)+1+currentx;
		maze[digby][digbx]=' ';


		// on top wall
		int digty=currenty;
		int digtx=rand()%(currentx-startx-1)+1+startx;
		maze[digty][digtx]=' ';


	}

	if(number==3){
		//on right wall
		int digry=rand()%(widthy-currenty-1)+1+currenty;
		int digrx=currentx;
		maze[digry][digrx]=' ';

		//on left wall
		int digly=rand()%(currenty-starty-1)+1+starty;
		int diglx=currentx;
		maze[digly][diglx]=' ';

		// on top wall
		int digty=currenty;
		int digtx=rand()%(currentx-startx-1)+1+startx;
		maze[digty][digtx]=' ';


	}

}

