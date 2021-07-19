# Maze
Program Assignment : Maze

Date: 2014-11-17

Author: Ying Shan 

## Description: 

<b>Task1:</b> Find and Read a maze file that already exit in computer. User is able to type the filename to run it. If the exit1 can be found, it will print out the rout, otherwise, maze will not come out.

<b>Task2:</b> Randomly generates a maze and the recursive algorithm is already given. When user types RANDOM, it replaces filename for argv [1], and display the maze, with in route the scree.

<b>Task3:</b> Using SDL show the output of maze. When user types SDL, it will be as argv [2].


## Functions:
`read_maze` Read a maze from the file. Before read the maze, there is a check in whether the file can be openned successfully.
 
`find_entry` Finding the entrance and exit of the maze. Each maze has two gaps, and each gap has two coordinates x, and y. Since the entrance is always at left of exit, the sum value of entrance will be less than the sum value of exit.
 
`explore_maze` This function will detect the rout from entrance to exit of particular maze. It will check the 4 directions (top,down,left,right) whether can make a step. It moves on step by step, until it finds the exit.

`set_maze` This function will set the size of the RANDOM maze and draw the board of it.

`inner_wall` This function will create walls in side the maze board for the RANDOM maze. It will randomly divide the RANDOM maze. The disctance between two can not larger than 4.

`dig_gap` This function is make 3 gaps in 4 parts of walls randomly. This is a recursive algorithm to generate maze.


## Finished:      
Maze + Maze Generation + SDL
The RANDOM maze may take a bit time to load in lab's mechine, it work perfect in my own mechine though
               
## Compile: 
`gcc neillsdl2.c maze.c -o maze sdl2-config --cflag -Wall -pedantic -std=c99 -lm sdl2-config-libs`
