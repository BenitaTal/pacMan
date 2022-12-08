//Bar Mizrahi 316080365
//Tal Benita 316013804

#include "glut.h"
#include <time.h>
#include <vector>
#include <iostream>
#include "Cell.h"
#include "AStar.h"
#include <chrono>
#include<algorithm>
#include <thread>
#include "main.h"


using namespace std;

const int MSZ = 30;  //map size
const int WALL = 0;
const int SPACE = -5;
const int PATH = -10;

const int GHOST_PINK = 1;
const int GHOST_RED = 2;
const int GHOST_ORANGE = 3;
const int PACMAN = 4;

const int BlankCell = 5;
const int WIN = 6;

//i am trying
AStar::Vec2i map;
AStar::Vec2i* pacMan_ptr;
AStar::Vec2i* temp_pacMan_ptr;
AStar::Vec2i* temp_ghost_ptr;
AStar::Vec2i* pink_gohst_ptr;
AStar::Vec2i* red_gohst_ptr;
AStar::Vec2i* orange_gohst_ptr;

AStar::Generator gen;
AStar::Vec2i* ptr;
AStar::Vec2i* collisionPtr;
AStar::CoordinateList list;
AStar::CoordinateList collisionList;
AStar::CoordinateList phaths;

AStar::CoordinateList pacMan_path;
AStar::CoordinateList temp_pacMan_path;
AStar::CoordinateList ghosts_path;
AStar::CoordinateList temp_ghost_path;
AStar::CoordinateList pink_path;
AStar::CoordinateList red_path;
AStar::CoordinateList orange_path;


int pacManArrDir[4]{ 0 };
int lastPosOfPac[4]{ -1,-1,-1,-1 };
int maze[MSZ][MSZ] = { 0 };
int matrix[MSZ][MSZ] = { 0 };
int indexOfLastPosOfPac = 0;
int switchModeTimer = 0;
int Score = 0;
int tryLeft = 0, tryRight = 0, tryUp = 0, tryDown = 0;
bool flag = true;
bool showPink = true;
bool showRed = true;
bool showOrange = true;
bool pacman_is_runnig = true;
bool pacman_is_catching = false;



int num_of_polygon = 9;

bool InitHelper(int i, int j, int randNum, int randNum2)
{
	int temp = 0;
	for (int k = 0; k < randNum; k++)
	{
		for (int g = 0; g < randNum2; g++)
		{
			if (maze[i + k][j + g] == PATH)
			{
				if (temp > 0)
				{
					maze[i + k][j + g] == SPACE;
					
				}
				temp++;
			}

		}
	}

	if (temp == 1)
		return true;
	else
	{
		for (int k = 0; k < randNum; k++)
		{
			for (int g = 0; g < randNum2; g++)
			{
				if (maze[i + k][j + g] == SPACE)
				{
					maze[i + k][j + g] == WALL;
				}

			}
		}
		return false;
	}

}

bool InitFrame(int i, int j, int randNum, int randNum2)
{
	int temp_x = 0;
	int temp_y = 0;

	for (int k = 0; k < randNum; k++)
	{
		for (int g = 0; g < randNum2; g++)
		{
			if ((i - 1 > 0) && (j - 1 > 0) && (i + k < MSZ) && (j + g < MSZ) )
			{
				maze[i][j + g] = WALL; // upper row
				maze[i + randNum][j + g] = WALL; // lower row
				maze[i + k][j] = WALL; // left col
				maze[i + k ][j + randNum2 ] = WALL; // right col

			}

		}
	}

	// we store in gen all the center cell of the rooms 
	ptr = new AStar::Vec2i();
	ptr->x = i + randNum / 2 ;
	ptr->y = j + randNum2 / 2;



	while (maze[ptr->x][ptr->y] == WALL)
	{
		if (maze[ptr->x][ptr->y] == WALL)
		{
			temp_y++;
			temp_x++;
		}

		ptr->x -= temp_x;
		ptr->y -= temp_y;
	}

	if (maze[ptr->x][ptr->y] == SPACE)
	{
		maze[ptr->x][ptr->y] = PATH;
		ptr->wight = maze[ptr->x][ptr->y];
	}
		

	if (InitHelper(i, j, randNum, randNum2))
	{
		ptr->wight = maze[ptr->x][ptr->y];
		list.push_back(*ptr);
		return true;
	}
	else
		return false;

}

void InitRoom(int i, int j, int randNum, int randNum2)
{
	for (int k = 0; k < randNum; k++)
	{
		for (int g = 0; g < randNum2; g++)
		{
				maze[i + k][j + g] = SPACE;
		}
	}
}

void InitMaze() {
	int i, j ;
	int randNum ;
	int randNum2;
	// set map size to AStar algo
	map.x = MSZ;
	map.y = MSZ;
	gen.setWorldSize(map);

	for (i = 1; i < MSZ - 1; i++)
	{
		for (j = 1; j < MSZ - 1; j++)
		{
			randNum = rand() % (8 - 6 + 1) + 6;
			randNum2 = rand() % (8 - 6 + 1) + 6;

			if ((num_of_polygon > 0) && (rand() % MSZ == 25) && (maze[i][j] == WALL) &&  (i + randNum <= MSZ - 1) && (j + randNum2 <= MSZ - 1) &&
				(maze[i+randNum][j] == WALL) && (maze[i][j + randNum2] == WALL) && (maze[i + randNum][j + randNum2] == WALL) &&
				(maze[i + randNum + 1][j] == WALL) && (maze[i][j + randNum2 + 1] == WALL) && (maze[i + randNum + 1][j + randNum2 + 1] == WALL) && 
				(maze[i + randNum - 1][j] == WALL) && (maze[i][j + randNum2 - 1] == WALL) && (maze[i + randNum - 1][j + randNum2 - 1] == WALL))
			{
	
				InitRoom(i,j, randNum, randNum2);
				if(InitFrame(i, j, randNum , randNum2))
					num_of_polygon--;
			}
			
		}
	}
	
}

void collisionHelper()
{
	collisionPtr = new AStar::Vec2i();
	
	for (int i = 0; i < MSZ; i++)
	{
		for (int j = 0; j < MSZ; j++)
		{
			collisionPtr->x = i;
			collisionPtr->y = j;

			//add collision
			if (maze[i][j] == WALL)
			{
	
				collisionPtr->wight = WALL;
				gen.addCollision(*collisionPtr);
			}
		}
	}
}

void InitPacManAndGhost()
{
	collisionHelper();

	int randRow = rand() % MSZ;
	int randCol = rand() % MSZ;

	while (maze[randRow][randCol] != SPACE)
	{
		randRow = rand() % MSZ;
		randCol = rand() % MSZ;
	}
	
	maze[randRow][randCol] = PACMAN;
	
	pacMan_ptr = new AStar::Vec2i();
	pacMan_ptr->x = randRow;
	pacMan_ptr->y = randCol;
	pacMan_ptr->wight = SPACE;


	int tLocation_pink_Row = rand() % MSZ;
	int tLocation_red_Row = rand() % MSZ;
	int tLocation_orange_Row = rand() % MSZ;
	int tLocation_pink_Col = rand() % MSZ;
	int tLocation_red_Col = rand() % MSZ;
	int tLocation_orange_Col = rand() % MSZ;

	
	while (maze[tLocation_pink_Row][tLocation_pink_Col] != SPACE)
	{
		tLocation_pink_Row = rand() % MSZ;
		tLocation_pink_Col = rand() % MSZ;
	}
	maze[tLocation_pink_Row][tLocation_pink_Col] = GHOST_PINK;

	while (maze[tLocation_red_Row][tLocation_red_Col] != SPACE)
	{
		tLocation_red_Row = rand() % MSZ;
		tLocation_red_Col = rand() % MSZ;
	}
	maze[tLocation_red_Row][tLocation_red_Col] = GHOST_RED;

	while (maze[tLocation_orange_Row][tLocation_orange_Col] != SPACE)
	{
		tLocation_orange_Row = rand() % MSZ;
		tLocation_orange_Col = rand() % MSZ;
	}
	maze[tLocation_orange_Row][tLocation_orange_Col] = GHOST_ORANGE;

	pink_gohst_ptr = new AStar::Vec2i();
	pink_gohst_ptr->x = tLocation_pink_Row;
	pink_gohst_ptr->y = tLocation_pink_Col;
	pink_gohst_ptr->wight = SPACE;

	red_gohst_ptr = new AStar::Vec2i();
	red_gohst_ptr->x = tLocation_red_Row;
	red_gohst_ptr->y = tLocation_red_Col;
	red_gohst_ptr->wight = SPACE;

	orange_gohst_ptr = new AStar::Vec2i();
	orange_gohst_ptr->x = tLocation_orange_Row;
	orange_gohst_ptr->y = tLocation_orange_Col;
	orange_gohst_ptr->wight = SPACE;

}

void InitPath()
{
	int x;
	int y;

	for (int i = 0; i < list.size() - 1; i++)
	{
		for (int j = 1; j < list.size(); j++)
		{
			phaths = gen.findPath(list[i], list[j]);
			
			for (int k = 0; k < phaths.size(); k++)
			{
				if(maze[phaths[k].x][phaths[k].y]== WALL)
					maze[phaths[k].x][phaths[k].y] = PATH;
			
			}
		}
	}
}

void DrawMaze() {
	int i, j;
	for (i = 1; i < MSZ - 1; i++) 
	{
		for (j = 1; j < MSZ - 1; j++) 
		{
			//set color
			switch (maze[i][j])
			{
			case WALL:
				glColor3d(0, 0, 1);//blue
				break;
			case SPACE:
				glColor3d(1, 1, 1);//white
				break;
			case PACMAN:
				glColor3d(1, 1, 0);//yellow
				break;
			case GHOST_PINK:
				glColor3d(1, 0.3, 1);//pink
				break;
			case GHOST_RED:
				glColor3d(1, 0, 0);//red
				break;
			case GHOST_ORANGE:
				glColor3d(1, 0.5, 0);//orange
				break;
			case PATH:
				glColor3d(1, 1, 1);// light gray
				break;
			case WIN:
				glColor3d(0, 1, 0); //green
				break;
			}
			

			glBegin(GL_POLYGON);
			glVertex2d(j, i);
			glVertex2d(j, i+1);
			glVertex2d(j+1, i+1);
			glVertex2d(j+1, i);
			glEnd();
			
		}
	}
}

void matrixInitHelper()
{
	for (int i = 0; i < MSZ; i++)
	{
		for (int j = 0; j < MSZ; j++)
		{

			if (maze[i][j] == SPACE || maze[i][j] == PATH)
			{
				matrix[i][j] = BlankCell;

			}
			else if (maze[i][j] == GHOST_PINK)
			{
				matrix[i][j] = GHOST_PINK;
			}
			else if (maze[i][j] == GHOST_RED)
			{
				matrix[i][j] = GHOST_RED;
			}
			else if (maze[i][j] == GHOST_ORANGE)
			{
				matrix[i][j] = GHOST_ORANGE;
			}
			else if (maze[i][j] == PACMAN)
			{
				matrix[i][j] = PACMAN;
			}

		}
	}
}

bool isSafe(int i, int j, int matrix[][MSZ])
{
	if (i >= 0 && i < MSZ && j >= 0 && j < MSZ)
		return true;
	return false;
}

bool isaPath(int matrix[][MSZ], int i, int j, bool visited[][MSZ])
{
	// Checking the boundaries, walls and
	// whether the cell is unvisited
	if (isSafe(i, j, matrix) && matrix[i][j] != 0
		&& !visited[i][j]) {
		// Make the cell visited
		visited[i][j] = true;

		// if the cell is the required
		// destination then return true
		if (matrix[i][j] == GHOST_PINK )  // 
			return true;

		// traverse up
		bool up = isaPath(matrix, i - 1, j, visited);
		
		// if path is found in up
		// direction return true
		if (up) 
			return true;

		// traverse left
		bool left = isaPath(matrix, i, j - 1, visited);

		// if path is found in left
		// direction return true
		if (left)
			return true;

		// traverse down
		bool down = isaPath(matrix, i + 1, j, visited);

		// if path is found in down
		// direction return true
		if (down)
			return true;

		// traverse right
		bool right = isaPath(matrix, i, j + 1, visited);
		
		// if path is found in right
		// direction return true
		if (right)
			return true;
	}

	// no path has been found
	return false;
}

// Method for finding and printing
// whether the path exists or not
void isPath(int matrix[][MSZ])
{

	// Defining visited array to keep
	// track of already visited indexes
	bool visited[MSZ][MSZ];
	memset(visited, 0, sizeof(visited));

	// Flag to indicate whether the
	// path exists or not
	bool flag = false;
	for (int i = 0; i < MSZ; i++) {
		for (int j = 0; j < MSZ; j++) {
			// if matrix[i][j] is source
			// and it is not visited
			if (matrix[i][j] == PACMAN && !visited[i][j])
				
				// Starting from i, j and
				// then finding the path
				if (isaPath(matrix, i, j, visited)) {

					// if path exists
					flag = true;
					break;
				}
		}
	}
}


void init()
{
	srand(time(0)); //seed random numbers
	glClearColor(0, 0, 1, 0);// color of window background
	glOrtho(0, MSZ, MSZ, 0, -1, 1);//setup coordiante system


	InitMaze();
	InitPath();
	InitPacManAndGhost();

}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer
	DrawMaze();
	glutSwapBuffers(); // show all
}

bool ThereIsACollisionBetweenPanManAndGhost(AStar::Vec2i pacMan_ptr, AStar::Vec2i ghost)
{
	if (pacman_is_runnig) //pac man runnig from ghost
	{
		if (pacMan_ptr.x == ghost.x && pacMan_ptr.y == ghost.y)
		{
			maze[pacMan_ptr.x][pacMan_ptr.y] = WIN;
			display();
			char s [100];
			sprintf_s(s, "Game Over, The Score Is : %d \nDo You Want To Try Again ? ", Score);
				int msgboxID = MessageBox(
					NULL, s, "",
					MB_ICONWARNING | MB_OKCANCEL | MB_DEFBUTTON2
				);
				
				switch (msgboxID)
				{
				case IDCANCEL:
					exit(0);
					break;
				case IDOK:
					exit(0);
					break;
				}
			
			exit(0);
			return true;
		}
		return false;
	}
	else if (pacman_is_catching)
	{
		if (pacMan_ptr.x == ghost.x && pacMan_ptr.y == ghost.y)
		{
			//reallocate ghost
			char s[100];
			sprintf_s(s, "You Win!, The Score Is : %d \n ", Score);
			int msgboxID = MessageBox(
				NULL, s, "",
				MB_ICONWARNING | MB_OKCANCEL | MB_DEFBUTTON2
			);
			switch (msgboxID)
			{
			case IDCANCEL:
				exit(0);
				break;
			case IDOK:
				exit(0);
				break;
			}
			return true;
		}
		return false;
	}
	else
		return false;
}
void pinkGhostsChasePacMan()
{
	pink_path = gen.findPath(*pink_gohst_ptr, *pacMan_ptr);

	maze[pink_gohst_ptr->x][pink_gohst_ptr->y] = PATH;
	ThereIsACollisionBetweenPanManAndGhost(*pink_gohst_ptr, *pacMan_ptr);

	if ((pink_path.size() - 2))
	{
		if (pink_gohst_ptr->x > pink_path[pink_path.size() - 2].x)
		{
			pink_gohst_ptr->x--; 
			
		}
		else if (pink_gohst_ptr->x < pink_path[pink_path.size() - 2].x)
		{
			pink_gohst_ptr->x++;
			
		}
		else
		{
			if (pink_gohst_ptr->y > pink_path[pink_path.size() - 2].y)
			{
				pink_gohst_ptr->y--;
				
			}
			else if (pink_gohst_ptr->y < pink_path[pink_path.size() - 2].y)
			{
				pink_gohst_ptr->y++;
				
			}

		}

		maze[pink_gohst_ptr->x][pink_gohst_ptr->y] = GHOST_PINK;

	}
}
void redGhostsChasePacMan()
{

	red_path = gen.findPath(*red_gohst_ptr, *pacMan_ptr);
	maze[red_gohst_ptr->x][red_gohst_ptr->y] = PATH;

	ThereIsACollisionBetweenPanManAndGhost(*red_gohst_ptr, *pacMan_ptr);
	if ((red_path.size() - 2))
	{
		if (red_gohst_ptr->x > red_path[red_path.size() - 2].x)
		{
			red_gohst_ptr->x--;
			
		}
		else if (red_gohst_ptr->x < red_path[red_path.size() - 2].x)
		{
			red_gohst_ptr->x++;
			
		}
		else
		{
			if (red_gohst_ptr->y > red_path[red_path.size() - 2].y)
			{
				red_gohst_ptr->y--;
				
			}
			else if (red_gohst_ptr->y < red_path[red_path.size() - 2].y)
			{
				red_gohst_ptr->y++;
				
			}

		}

		maze[red_gohst_ptr->x][red_gohst_ptr->y] = GHOST_RED;

	}
}

void orangeGhostsChasePacMan()
{

	orange_path = gen.findPath(*orange_gohst_ptr, *pacMan_ptr);
	maze[orange_gohst_ptr->x][orange_gohst_ptr->y] = PATH;
	ThereIsACollisionBetweenPanManAndGhost(*orange_gohst_ptr, *pacMan_ptr);
	if ((orange_path.size() - 2))
	{
		if (orange_gohst_ptr->x > orange_path[orange_path.size() - 2].x)
		{
			orange_gohst_ptr->x--;
			
		}
		else if (orange_gohst_ptr->x < orange_path[orange_path.size() - 2].x)
		{
			orange_gohst_ptr->x++;
			
		}
		else
		{
			if (orange_gohst_ptr->y > orange_path[orange_path.size() - 2].y)
			{
				orange_gohst_ptr->y--;
				
			}
			else if (orange_gohst_ptr->y < orange_path[orange_path.size() - 2].y)
			{
				orange_gohst_ptr->y++;
				
			}

		}

		maze[orange_gohst_ptr->x][orange_gohst_ptr->y] = GHOST_ORANGE;
	}
}



bool checkRandomNumber(int dir)
{
	//pacManArrDir[0] left
	//pacManArrDir[1] right
	//pacManArrDir[2] up
	//pacManArrDir[3] down
	switch (dir)
	{
	case 0:
		if (maze[pacMan_ptr->x][pacMan_ptr->y-1] == WALL) //go left
			return true;
		break;
	case 1:
		if (maze[pacMan_ptr->x][pacMan_ptr->y + 1] == WALL) // go right
			return true;
		break;
	case 2:
		if (maze[pacMan_ptr->x - 1][pacMan_ptr->y] == WALL)  // go up
			return true;
		break;
	case 3:
		if (maze[pacMan_ptr->x + 1][pacMan_ptr->y] == WALL)  // go down
			return true;
		break;
	default:
		return false;
	}
	return false;
}

void helperGhostRunFromPacman(int dir, AStar::Vec2i* ghost , int GhostColor)
{
	//pacManArrDir[0] left
	//pacManArrDir[1] right
	//pacManArrDir[2] up
	//pacManArrDir[3] down
	switch (dir)
	{
	case 0:
		maze[ghost->x][ghost->y - 1] = GhostColor; // go left
		ghost->y -= 1;
		break;
	case 1:
		maze[ghost->x][ghost->y + 1] = GhostColor; // go right
		ghost->y += 1;
		break;
	case 2:
		maze[ghost->x - 1][ghost->y] = GhostColor;  // go up
		ghost->x -= 1;
		break;
	case 3:
		maze[ghost->x + 1][ghost->y] = GhostColor;  // go down
		ghost->x += 1;
		break;
	default:
		break;
	}
}

void helperRunFromGhost(int dir)
{
	
	switch (dir)
	{
	case 0:	
		maze[pacMan_ptr->x][pacMan_ptr->y-1] = PACMAN; // go left
		pacMan_ptr->y -= 1;
		break;
	case 1:
		maze[pacMan_ptr->x][pacMan_ptr->y+1] = PACMAN; // go right
		pacMan_ptr->y += 1;
		break;
	case 2:
		maze[pacMan_ptr->x-1][pacMan_ptr->y] = PACMAN;  // go up
		pacMan_ptr->x -= 1;
		break;
	case 3:
		maze[pacMan_ptr->x+1][pacMan_ptr->y] = PACMAN;  // go down
		pacMan_ptr->x += 1;
		break;
	default:
		break;
	}

}


bool checkFourLastMovesOfPacMan(AStar::Vec2i* pacMan_ptr, int dir)
{
	lastPosOfPac[indexOfLastPosOfPac] = dir; // insert 0\1\2\3 dir
	indexOfLastPosOfPac++;
	if (indexOfLastPosOfPac == 4) //every 4 moves of pac man check if he is in loop
	{
		indexOfLastPosOfPac = 0;
		if (lastPosOfPac[0] == 0 && lastPosOfPac[2] == 0 && lastPosOfPac[1] == 1 && lastPosOfPac[3] == 1) //LRLR
			return true;

		if (lastPosOfPac[0] == 1 && lastPosOfPac[2] == 1 && lastPosOfPac[1] == 0 && lastPosOfPac[3] == 0) //RLRL
			return true;

		if (lastPosOfPac[0] == 2 && lastPosOfPac[2] == 2 && lastPosOfPac[1] == 3 && lastPosOfPac[3] == 3) //UDUD
			return true;

		if (lastPosOfPac[0] == 3 && lastPosOfPac[2] == 3 && lastPosOfPac[1] == 2 && lastPosOfPac[3] == 2) //DUDU
			return true;
	}
	return false;

}

int ChasingPacManFromAllGhost(AStar::Vec2i* GhostPtr)
{
	int sizeIfGoUp = 0;
	int sizeIfGoDown = 0;
	int sizeIfGoLeft = 0;
	int sizeIfGoRight = 0;
	int random = rand() % MSZ;
	int maxPath = 0;
	int maxPath_x = 0;
	int maxPath_y = 0;
	//pacManArrDir[0] left
	//pacManArrDir[1] right
	//pacManArrDir[2] up
	//pacManArrDir[3] down
	temp_pacMan_ptr = new AStar::Vec2i();

	pacMan_path = gen.findPath(*pacMan_ptr, *GhostPtr);
	maze[pacMan_ptr->x][pacMan_ptr->y] = PATH;
	for (int i = 0; i < 4; i++) {
		pacManArrDir[i] = 0;
	}
	if (((maze[pacMan_ptr->x][pacMan_ptr->y - 1] == SPACE) || (maze[pacMan_ptr->x][pacMan_ptr->y - 1] == PATH)) &&
		((pacMan_ptr->y - 1) > 0)) // left
	{
		temp_pacMan_ptr->x = pacMan_ptr->x;
		temp_pacMan_ptr->y = pacMan_ptr->y - 1;
		temp_pacMan_ptr->wight = SPACE;

		temp_pacMan_path = gen.findPath(*temp_pacMan_ptr, *GhostPtr);
		sizeIfGoLeft = temp_pacMan_path.size();
		pacManArrDir[0] = sizeIfGoLeft;
	}
	else {
		sizeIfGoLeft = 1000; //very big number
		pacManArrDir[0] = sizeIfGoLeft;
	}

	if (((maze[pacMan_ptr->x][pacMan_ptr->y + 1] == SPACE) || (maze[pacMan_ptr->x][pacMan_ptr->y + 1] == PATH)) &&
		((pacMan_ptr->y + 1) < MSZ)) // right
	{
		temp_pacMan_ptr->x = pacMan_ptr->x;
		temp_pacMan_ptr->y = pacMan_ptr->y + 1;
		temp_pacMan_ptr->wight = SPACE;

		temp_pacMan_path = gen.findPath(*temp_pacMan_ptr, *GhostPtr);
		sizeIfGoRight = temp_pacMan_path.size();
		pacManArrDir[1] = sizeIfGoRight;
	}
	else
	{
		sizeIfGoRight = 1000;
		pacManArrDir[1] = sizeIfGoRight;
	}


	if (((maze[pacMan_ptr->x + 1][pacMan_ptr->y] == SPACE) || (maze[pacMan_ptr->x + 1][pacMan_ptr->y] == PATH)) &&
		((pacMan_ptr->x + 1) < MSZ)) // down
	{
		temp_pacMan_ptr->x = pacMan_ptr->x + 1;
		temp_pacMan_ptr->y = pacMan_ptr->y;
		temp_pacMan_ptr->wight = SPACE;

		temp_pacMan_path = gen.findPath(*temp_pacMan_ptr, *GhostPtr);
		sizeIfGoDown = temp_pacMan_path.size();
		pacManArrDir[3] = sizeIfGoDown;
	}
	else
	{
		sizeIfGoDown = 1000;
		pacManArrDir[3] = sizeIfGoDown;
	}
	if (((maze[pacMan_ptr->x - 1][pacMan_ptr->y] == SPACE) || (maze[pacMan_ptr->x - 1][pacMan_ptr->y] == PATH)) &&
		((pacMan_ptr->x - 1) > 0)) // up
	{
		temp_pacMan_ptr->x = pacMan_ptr->x - 1;
		temp_pacMan_ptr->y = pacMan_ptr->y;
		temp_pacMan_ptr->wight = SPACE;

		temp_pacMan_path = gen.findPath(*temp_pacMan_ptr, *GhostPtr);
		sizeIfGoUp = temp_pacMan_path.size();
		pacManArrDir[2] = sizeIfGoUp;
	}
	else
	{
		sizeIfGoUp = 1000;
		pacManArrDir[2] = sizeIfGoUp;
	}

	int minn = min({ sizeIfGoDown, sizeIfGoLeft, sizeIfGoRight, sizeIfGoUp });
	if (pacManArrDir[0] == minn)
		return 0;
	if (pacManArrDir[1] == minn)
		return 1;
	if (pacManArrDir[2] == minn)
		return 2;
	if (pacManArrDir[3] == minn)
		return 3;

}

void PacManChase()
{
	int pinkIndexDir, redIndexDir, orangeIndexDir;
	int pinkMaxDis, redMaxDis, orangeMaxDis;
	pinkIndexDir = ChasingPacManFromAllGhost(pink_gohst_ptr); //index
	pinkMaxDis = pacManArrDir[pinkIndexDir]; //size

	redIndexDir = ChasingPacManFromAllGhost(red_gohst_ptr); //index
	redMaxDis = pacManArrDir[redIndexDir]; //size

	orangeIndexDir = ChasingPacManFromAllGhost(orange_gohst_ptr); //index
	orangeMaxDis = pacManArrDir[pinkIndexDir]; //size

	int minDir = min({ pinkMaxDis, redMaxDis,orangeMaxDis });
	int randIndex = rand() % (3) + 1;

	if (minDir == pinkMaxDis)
	{
		if (!checkFourLastMovesOfPacMan(pacMan_ptr, pinkIndexDir))
		{
			helperRunFromGhost(pinkIndexDir); //pac man runs from pinkDis dir
			ThereIsACollisionBetweenPanManAndGhost(*pink_gohst_ptr, *pacMan_ptr);
		}
		else
		{
			while (randIndex == pinkIndexDir || checkRandomNumber(randIndex))
				randIndex = rand() % (3) + 1;
			helperRunFromGhost(randIndex);
			ThereIsACollisionBetweenPanManAndGhost(*pink_gohst_ptr, *pacMan_ptr);
		}

	}
	else if (minDir == redMaxDis)
	{
		if (!checkFourLastMovesOfPacMan(pacMan_ptr, redIndexDir))
		{
			helperRunFromGhost(redIndexDir);
			ThereIsACollisionBetweenPanManAndGhost(*red_gohst_ptr, *pacMan_ptr);
		}
		else
		{
			while (randIndex == redMaxDis || checkRandomNumber(randIndex))
				randIndex = rand() % (3) + 1;
			helperRunFromGhost(randIndex);
			ThereIsACollisionBetweenPanManAndGhost(*red_gohst_ptr, *pacMan_ptr);
		}
	}
	else
	{
		if (!checkFourLastMovesOfPacMan(pacMan_ptr, orangeIndexDir))
		{
			helperRunFromGhost(orangeIndexDir);
			ThereIsACollisionBetweenPanManAndGhost(*orange_gohst_ptr, *pacMan_ptr);
		}
		else
		{
			while (randIndex == orangeIndexDir || checkRandomNumber(randIndex))
				randIndex = rand() % (3) + 1;
			helperRunFromGhost(randIndex);
			ThereIsACollisionBetweenPanManAndGhost(*orange_gohst_ptr, *pacMan_ptr);
		}
	}
}


int AllGhostRunningFromPacMan(AStar::Vec2i* GhostPtr)
{
	int sizeIfGoUp = 0;
	int sizeIfGoDown = 0;
	int sizeIfGoLeft = 0;
	int sizeIfGoRight = 0;
	int random = rand() % MSZ;
	int maxPath = 0;
	int maxPath_x = 0;
	int maxPath_y = 0;
	//pacManArrDir[0] left
	//pacManArrDir[1] right
	//pacManArrDir[2] up
	//pacManArrDir[3] down
	
	temp_ghost_ptr = new AStar::Vec2i();
	ghosts_path = gen.findPath(*GhostPtr , *pacMan_ptr);
	maze[GhostPtr->x][GhostPtr->y] = PATH;
	for (int i = 0; i < 4; i++) {
		pacManArrDir[i] = 0;
	}
	if (((maze[GhostPtr->x][GhostPtr->y - 1] == SPACE) || (maze[GhostPtr->x][GhostPtr->y - 1] == PATH)) &&
		((GhostPtr->y - 1) > 0)) // left
	{
		temp_ghost_ptr->x = GhostPtr->x;
		temp_ghost_ptr->y = GhostPtr->y - 1;
		temp_ghost_ptr->wight = SPACE;

		temp_ghost_path = gen.findPath(*temp_ghost_ptr, *pacMan_ptr);
		sizeIfGoLeft = temp_ghost_path.size();
		pacManArrDir[0] = sizeIfGoLeft;
	}

	if (((maze[GhostPtr->x][GhostPtr->y + 1] == SPACE) || (maze[GhostPtr->x][GhostPtr->y + 1] == PATH)) &&
		((GhostPtr->y + 1) < MSZ)) // right
	{
		temp_ghost_ptr->x = GhostPtr->x;
		temp_ghost_ptr->y = GhostPtr->y + 1;
		temp_ghost_ptr->wight = SPACE;

		temp_ghost_path = gen.findPath(*temp_ghost_ptr, *pacMan_ptr);
		sizeIfGoRight = temp_ghost_path.size();
		pacManArrDir[1] = sizeIfGoRight;
	}

	if (((maze[GhostPtr->x + 1][GhostPtr->y] == SPACE) || (maze[GhostPtr->x + 1][GhostPtr->y] == PATH)) &&
		((GhostPtr->x + 1) < MSZ)) // down
	{
		temp_ghost_ptr->x = GhostPtr->x + 1;
		temp_ghost_ptr->y = GhostPtr->y;
		temp_ghost_ptr->wight = SPACE;

		temp_ghost_path = gen.findPath(*temp_ghost_ptr, *pacMan_ptr);
		sizeIfGoDown = temp_ghost_path.size();
		pacManArrDir[3] = sizeIfGoDown;
	}
	if (((maze[GhostPtr->x - 1][GhostPtr->y] == SPACE) || (maze[GhostPtr->x - 1][GhostPtr->y] == PATH)) &&
		((GhostPtr->x - 1) > 0)) // up
	{
		temp_ghost_ptr->x = GhostPtr->x - 1;
		temp_ghost_ptr->y = GhostPtr->y;
		temp_ghost_ptr->wight = SPACE;

		temp_ghost_path = gen.findPath(*temp_ghost_ptr, *pacMan_ptr);
		sizeIfGoUp = temp_ghost_path.size();
		pacManArrDir[2] = sizeIfGoUp;
	}

	int maxx = max({ sizeIfGoDown, sizeIfGoLeft, sizeIfGoRight, sizeIfGoUp });
	if (pacManArrDir[0] == maxx)
		return 0;
	if (pacManArrDir[1] == maxx)
		return 1;
	if (pacManArrDir[2] == maxx)
		return 2;
	if (pacManArrDir[3] == maxx)
		return 3;
}
void pacManChaseRedGhost()
{
	int dir = AllGhostRunningFromPacMan(red_gohst_ptr);
	helperGhostRunFromPacman(dir, red_gohst_ptr, GHOST_RED);
}
void pacManChasePinkGhost()
{
	int dir = AllGhostRunningFromPacMan(pink_gohst_ptr);
	helperGhostRunFromPacman(dir, pink_gohst_ptr, GHOST_PINK);
}
void pacManChaseOrangeGhost()
{
	int dir = AllGhostRunningFromPacMan(orange_gohst_ptr);
	helperGhostRunFromPacman(dir, orange_gohst_ptr, GHOST_ORANGE);
}
int RunningPacManFromAllGhost(AStar::Vec2i* GhostPtr)
{
	int sizeIfGoUp = 0;
	int sizeIfGoDown = 0;
	int sizeIfGoLeft = 0;
	int sizeIfGoRight = 0;
	int random = rand() % MSZ;
	int maxPath = 0;
	int maxPath_x = 0;
	int maxPath_y = 0;
	//pacManArrDir[0] left
	//pacManArrDir[1] right
	//pacManArrDir[2] up
	//pacManArrDir[3] down
	temp_pacMan_ptr = new AStar::Vec2i();

	pacMan_path = gen.findPath(*pacMan_ptr, *GhostPtr);
	maze[pacMan_ptr->x][pacMan_ptr->y] = PATH;
	for (int i = 0; i < 4; i++) {
		pacManArrDir[i] = 0;
	}
	if (((maze[pacMan_ptr->x][pacMan_ptr->y-1] == SPACE) || (maze[pacMan_ptr->x][pacMan_ptr->y - 1] == PATH)) &&
		((pacMan_ptr->y - 1) > 0)) // left
	{
		temp_pacMan_ptr->x = pacMan_ptr->x;
		temp_pacMan_ptr->y = pacMan_ptr->y-1;
		temp_pacMan_ptr->wight = SPACE;

		temp_pacMan_path = gen.findPath(*temp_pacMan_ptr, *GhostPtr);
		sizeIfGoLeft = temp_pacMan_path.size();
		pacManArrDir[0] = sizeIfGoLeft;
	}

	if (((maze[pacMan_ptr->x][pacMan_ptr->y+1] == SPACE) || (maze[pacMan_ptr->x][pacMan_ptr->y + 1] == PATH)) &&
		((pacMan_ptr->y + 1) < MSZ)) // right
	{
		temp_pacMan_ptr->x = pacMan_ptr->x;
		temp_pacMan_ptr->y = pacMan_ptr->y + 1;
		temp_pacMan_ptr->wight = SPACE;

		temp_pacMan_path = gen.findPath(*temp_pacMan_ptr, *GhostPtr);
		sizeIfGoRight = temp_pacMan_path.size();
		pacManArrDir[1] = sizeIfGoRight;
	}

	if (((maze[pacMan_ptr->x + 1][pacMan_ptr->y] == SPACE) || (maze[pacMan_ptr->x + 1][pacMan_ptr->y] == PATH)) &&
		((pacMan_ptr->x + 1) < MSZ)) // down
	{
		temp_pacMan_ptr->x = pacMan_ptr->x + 1;
		temp_pacMan_ptr->y = pacMan_ptr->y;
		temp_pacMan_ptr->wight = SPACE;

		temp_pacMan_path = gen.findPath(*temp_pacMan_ptr, *GhostPtr);
		sizeIfGoDown = temp_pacMan_path.size();
		pacManArrDir[3] = sizeIfGoDown;
	}
	if (((maze[pacMan_ptr->x-1][pacMan_ptr->y] == SPACE) || (maze[pacMan_ptr->x - 1][pacMan_ptr->y] == PATH)) &&
		((pacMan_ptr->x - 1) > 0)) // up
	{
		temp_pacMan_ptr->x = pacMan_ptr->x-1;
		temp_pacMan_ptr->y = pacMan_ptr->y;
		temp_pacMan_ptr->wight = SPACE;

		temp_pacMan_path = gen.findPath(*temp_pacMan_ptr, *GhostPtr);
		sizeIfGoUp = temp_pacMan_path.size();
		pacManArrDir[2] = sizeIfGoUp;
	}
	
	int maxx = max({ sizeIfGoDown, sizeIfGoLeft, sizeIfGoRight, sizeIfGoUp });
	if (pacManArrDir[0] == maxx)
		return 0;
	if (pacManArrDir[1] == maxx)
		return 1;
	if (pacManArrDir[2] == maxx)
		return 2;
	if (pacManArrDir[3] == maxx)
		return 3;

}

void PacManRun()
{
	int pinkIndexDir, redIndexDir, orangeIndexDir;
	int pinkMaxDis, redMaxDis,orangeMaxDis;
	pinkIndexDir = RunningPacManFromAllGhost(pink_gohst_ptr); //index
	pinkMaxDis = pacManArrDir[pinkIndexDir]; //size

	redIndexDir = RunningPacManFromAllGhost(red_gohst_ptr); //index
	redMaxDis = pacManArrDir[redIndexDir]; //size

	orangeIndexDir = RunningPacManFromAllGhost(orange_gohst_ptr); //index
	orangeMaxDis = pacManArrDir[pinkIndexDir]; //size
	int maxDir = min({ pinkMaxDis, redMaxDis ,orangeMaxDis });
	int randIndex = rand() % (3) + 1;
	int counterLoop = 0;

	if (maxDir == pinkMaxDis)
	{
		if (!checkFourLastMovesOfPacMan(pacMan_ptr, pinkIndexDir))
		{
			helperRunFromGhost(pinkIndexDir); //pac man runs from pinkDis dir
		}
		else
		{
			while ((randIndex == pinkIndexDir || checkRandomNumber(randIndex)) && counterLoop < 2) {
				randIndex = rand() % (3) + 1;
				counterLoop++;
			}
			if(counterLoop!=2)
				helperRunFromGhost(randIndex);
		}
		
	}
	else if (maxDir == redMaxDis)
	{
		if(!checkFourLastMovesOfPacMan(pacMan_ptr, redIndexDir))
			helperRunFromGhost(redIndexDir);
		else
		{
			while ((randIndex == redMaxDis || checkRandomNumber(randIndex)) && counterLoop < 2) {
				randIndex = rand() % (3) + 1;
				counterLoop++;
			}
			if (counterLoop != 2)
				helperRunFromGhost(randIndex);
		}
			
	}
	else
	{
		if(!checkFourLastMovesOfPacMan(pacMan_ptr, orangeIndexDir))
			helperRunFromGhost(orangeIndexDir);
		else
		{
			while ((randIndex == orangeIndexDir || checkRandomNumber(randIndex)) && counterLoop < 2) {
				randIndex = rand() % (3) + 1;
				counterLoop++;
			}
			if (counterLoop != 2)
				helperRunFromGhost(randIndex);
		}
			
	}
	
}
void changeMode()
{
	if (switchModeTimer >= 0 && switchModeTimer < 15 && pacman_is_runnig)
	{
		pacman_is_runnig = true;
		pacman_is_catching = false;
	}
	else if(pacman_is_runnig)
	{
		switchModeTimer = 0;
		pacman_is_runnig = false;
		pacman_is_catching = true;
	}
	if (switchModeTimer >= 0 && switchModeTimer < 8 && pacman_is_catching)
	{
		pacman_is_runnig = false;
		pacman_is_catching = true;
	}
	else if(pacman_is_catching)
	{
		switchModeTimer = 0;
		pacman_is_runnig = true;
		pacman_is_catching = false;
	}
	switchModeTimer++;
}
void idle()
{
	changeMode();
	
	if (pacman_is_runnig)
	{
		redGhostsChasePacMan();
		orangeGhostsChasePacMan();
		pinkGhostsChasePacMan(); 		
		PacManRun();
		Score++;	
		// wait 1 sec
		using namespace std::this_thread; // sleep_for, sleep_until
		using namespace std::chrono; // nanoseconds, system_clock, seconds

		sleep_for(nanoseconds(10));
		sleep_until(system_clock::now() + nanoseconds(100000));
		// wait 1 sec
	}
	if (pacman_is_catching)
	{
		PacManChase();
		pacManChaseRedGhost();
		pacManChasePinkGhost();
		pacManChaseOrangeGhost();
		Score++;
		// wait 1 sec
		using namespace std::this_thread; // sleep_for, sleep_until
		using namespace std::chrono; // nanoseconds, system_clock, seconds

		sleep_for(nanoseconds(10));
		sleep_until(system_clock::now() + nanoseconds(100000));
		// wait 1 sec
	}
		
	glutPostRedisplay();//calls indirectly to display
}



void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);// double means its uses double buffer we print in the buffer
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("Pac Man");

	glutDisplayFunc(display);//refresh function
	glutIdleFunc(idle); //runs all the time in the background


	init();

	glutMainLoop();
}
