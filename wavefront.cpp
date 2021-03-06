//.cpp file for function wavefront algorithm
//@author Vanessa Hadlock
//@date June 22nd, 2020
//@brief Project 4


#include "wavefront.h"
#include "maze.h"


/* Global function wavefront() which implements the Wavefront algorithm with the
 * objective to find  asingle-pair paths in a given map
 * @param       
 * @return      none
 */	

void wavefront() { 

//Orientation constants

#define NORTH 10
#define SOUTH 100
#define EAST 1000
#define WEST 10000

//Variable for orientation of robot, default value is north

int orientation = NORTH;

//Variable to contain the directions to move to
int direction[MAP_AREA];
//Next best neighbor
Cell next;
//Neighbors of a cell
Cell northNeigh;
Cell southNeigh;
Cell eastNeigh;
Cell westNeigh;

//Values of neighbors
int northVal;
int southVal;
int eastVal;
int westVal;

//Given a cell, find out which neighbors are valid
void computeNeighbor(Cell &currCell);

//Compute path for the map from finish to start
void computePath(int *map,int numCols,int numRows,int startX, int startY);

//Check to see if coordinates are non negative and therefore valid

bool checkCoordinate(int x, int y);

//Move robot forward the length of 1 square
void move1Square();

//Move robot back the length of 1 square
void moveBack1Square();
void turn90Clockwise();
void turn90AntiClockwise();

//Move to particular cell represented by i, after next minimum neighbor is computed
void followPlan(int i);

//void execFollowPlan();

/************************ COMPUTE NEIGHBOR *************************/
void computeNeighbor(Cell &currCell)
{
  northNeigh.x = currCell.x;
  northNeigh.y = currCell.y-1;
  //If coordinates of north neighbor are valid, northVal is the value of north neighbor, else it's invalid
  checkCoordinate(northNeigh.x,northNeigh.y)==true ? northVal = world_map[northNeigh.y][northNeigh.x]:northVal = -1 ;
  writeDebugStreamLine("north neighbor is: %d,%d |Value is: %d",northNeigh.x,northNeigh.y,northVal);

  southNeigh.x = currCell.x;
  southNeigh.y = currCell.y+1;
  checkCoordinate(southNeigh.x,southNeigh.y)==true ? southVal = world_map[southNeigh.y][southNeigh.x]:southVal = -1;
  writeDebugStreamLine("south neighbor is: %d,%d |Value is: %d",southNeigh.x,southNeigh.y,southVal);

  eastNeigh.x = currCell.x+1;
  eastNeigh.y = currCell.y;
  checkCoordinate(eastNeigh.x,eastNeigh.y)==true ? eastVal = world_map[eastNeigh.y][eastNeigh.x]:eastVal = -1;
  writeDebugStreamLine("east neighbor is: %d,%d |Value is: %d",eastNeigh.x,eastNeigh.y,eastVal);

  westNeigh.x = currCell.x-1;
  westNeigh.y = currCell.y;
  checkCoordinate(westNeigh.x,westNeigh.y)==true ? westVal = world_map[westNeigh.y][westNeigh.x]:westVal = -1;
  writeDebugStreamLine("west neighbor is: %d,%d |Value is: %d",westNeigh.x,westNeigh.y,westVal);
 }

/******************** COMPUTE PATH ************************************/
void computePath(int *map, int numCols, int numRows, int startX, int startY)
{
  //Initialize start positions
  Cell currCell;
  currCell.x = startX;
  currCell.y = startY;

  int smallestCell;


  //compare all neighbors to find the smallest which is not OBST

  int i = 0;
  while(smallestCell != world_map[goal_y][goal_x])
  {
    if(i > sizeof(direction))
     {
       //Create new array with bigger size
       int newDirections[sizeof(direction)*2];
       for(int i=0;i<sizeof(direction);i++)
       {
         newDirections[i] = direction[i];
       }
       direction = newDirections;
     }
    computeNeighbor(currCell);//Compute and set all possible neighbors
    //Check to make sure the neighbors are valid
    //Assign arbitrary smallest cell for comparison
    smallestCell = world_map[currCell.y][currCell.x];
    //writeDebugStreamLine("smallestCell is:%d",smallestCell);

    if(northVal < smallestCell && (northVal != 1 && northVal != -1))
     {
       smallestCell = northVal;
       //found best cell...set current cell to that cexl
       currCell.x = northNeigh.x;
       currCell.y = northNeigh.y;
       direction[i] = 1;
     }
     if(southVal < smallestCell && (southVal != 1 && southVal != -1))
     {
       smallestCell = southVal;
       //found best cell...set current cell to that cell
       currCell.x = southNeigh.x;
       currCell.y = southNeigh.y;
       direction[i] = 2;
     }
     if(eastVal < smallestCell && (eastVal != 1&& eastVal != -1))
     {
       smallestCell = eastVal;
       //found best cell...set current cell to that cell
       currCell.x = eastNeigh.x;
       currCell.y = eastNeigh.y;
       direction[i] = 3;
     }
     if(westVal < smallestCell && (westVal != 1 && westVal != -1))
     {
       smallestCell = westVal;
       //found best cell...set current cell to thay cell
       currCell.x = westNeigh.x;
       currCell.y = westNeigh.y;
       direction[i] = 4;
     }
     //Print
     writeDebugStreamLine("Moving to cell: %d,%d | Value: %d",currCell.x,currCell.y, world_map[currCell.y][currCell.x]);
     followPlan(i);
     i++;
  }
}
/******************************** END COMPUTE PATH *********************************/


/******************** CHECK COORDINATES *************************************/
bool checkCoordinate(int x, int y)
{
  bool returnValue;
  int dummy = 0;int dummy2 = 0;

  x==-1 ? returnValue = false:dummy += 1;//If x is invalid, return value is false and increment dummy by 1
  y==-1 ? returnValue = false:dummy += 2;//If y is invalid return value is false and increment dummy by `

  x>(X_SIZE-1) ? returnValue = false:dummy2 += 3;//If x exceeds boundary return value is false, increment dummy2 by 3
  y>(Y_SIZE-1) ? returnValue = false:dummy2 += 4;//If y exceeds boundary return value is false, increment dummy2 by 4

  //If dummy is 3 and dumm2 is 7, then both tests have passed, coordinates are valid
  if(dummy==3 && dummy2==7)
  {
    return true;
  }
  else //Otherwise tests failed, return false
  {
    return returnValue;
  }
}
/*********************** END CHECK COORDINATES ******************************/


/******************** MOVE 1 SQUARE **************************************/
void move1Square()
{
  nSyncedMotors = synchAB; //motor  A is the master, motor B is the slave
  nSyncedTurnRatio = 100; //motors move in same directions

  nMotorEncoder[motorA] = 0; // clear the value of motorA's encoder

  nMotorEncoderTarget[motorA] = 900; // sets a target of 900 degrees

  motor[motorA] = 30; //turns the motor on at 30% power

  while(nMotorRunState[motorA] != runStateIdle) //while motorA is not in an idle state
  {
    //continue to power motorA until the motor nMotorEncoderTarget position is reached
  }

  motor[motorA] = 0;  // turn the motors off.
}
/******************** END MOVE 1 SQUARE ****************************/


/********************* MOVE BACK 1 SQUARE ***************************/
void moveBack1Square()
{
  nSyncedMotors = synchAB; //motor  A is the master, motor A is the slave
  nSyncedTurnRatio = 100;

  nMotorEncoder[motorA] = 0; // clears the value of motorA's encoder

  nMotorEncoderTarget[motorA] = 900; // sets a target of 900 degrees

  motor[motorA] = -30; //turns the motor on at -30% power

  while(nMotorRunState[motorA] != runStateIdle) //while motorA is not in an idle state
  {
    //continue to power motorA until the motor nMotorEncoderTarget position is reached
  }

  motor[motorA] = 0;  // turn the motors off.
}
/**************** END MOVE BACK 1 SQUARE *************************/



/**************************** TURN 90 CLOCKWISE **************************************/
void turn90Clockwise()
{
  nSyncedMotors = synchAB; //motor  B is the master, motor A is the slave
  nSyncedTurnRatio = -100; //motors move in opposite directions of one another

  nMotorEncoder[motorA] = 0; // clears the value of motorB's encoder

  nMotorEncoderTarget[motorA] = 180; // sets a target of 900 degrees

  motor[motorA] = -30; //turns the motor on at 30% power

  while(nMotorRunState[motorA] != runStateIdle) //while motorA is not in an idle state
  {
    //continue to power motorA until the motor nMotorEncoderTarget position is reached
  }

  motor[motorA] = 0;  // turn the motors off.
}
//***************************** END TURN 90 CLOCKWISE ******************************//



//***************************** TURN 90 ANTI CLOCKWISE ******************************//
void turn90AntiClockwise()
{
  nSyncedMotors = synchAB; //motor  B is the master, motor A is the slave
  nSyncedTurnRatio = -100; //motors move in opposite directions of one another

  nMotorEncoder[motorA] = 0; // clears the value of motorB's encoder

  nMotorEncoderTarget[motorA] = 180; // sets a target of 900 degrees

  motor[motorA] = 30; //turns the motor on at 30% power

  while(nMotorRunState[motorA] != runStateIdle) //while motorA is not in an idle state
  {
    //continue to power motorA until the motor nMotorEncoderTarget position is reached
  }

  motor[motorA] = 0;  // turn the motors off.

  //wait1Msec(3000);
}
//***************************** END TURN 90 ANTI CLOCKWISE ******************************//




/*********************** FOLLOW PLAN *************************************************/
void followPlan(int i)
{
    //********************   Direction to move to: NORTH    ***************//
    //Orientation: NORTH
    if(direction[i] == 1 && orientation == NORTH)
    {
      move1Square();
    }
    //Orientation: SOUTH
    else if(direction[i] == 1 && orientation == SOUTH)
    {
      turn90Clockwise();
      turn90Clockwise();
      move1Square();
      orientation = NORTH;//Reset Orientation
    }
    //Orientation: EAST
    else if(direction[i] == 1 && orientation == EAST)
    {
      turn90AntiClockwise();
      move1Square();
      orientation = NORTH;//Reset Orientation
    }
    //Orientation: WEST
    else if(direction[i] == 1 && orientation == WEST)
    {
      turn90Clockwise();
      move1Square();
      orientation = NORTH;//Reset orientation
    }

    //********************  Direction to move to: SOUTH    ***************//
    //Orientation: SOUTH
    if(direction[i] == 2 && orientation == SOUTH)
    {
      move1Square();
    }
    //Orientation: NORTH
    else if(direction[i] == 2 && orientation == NORTH)
    {
      turn90Clockwise();
      turn90Clockwise();
      move1Square();
      orientation = SOUTH;//Reset Orientation
    }
    //Orientation: EAST
    else if(direction[i] == 2 && orientation == EAST)
    {
      turn90Clockwise();
      move1Square();
      orientation = SOUTH;//Reset orientation
    }
    //Orientation: WEST
    else if(direction[i] == 2 && orientation == WEST)
    {
      turn90AntiClockwise();
      move1Square();
      orientation = SOUTH;//Reset orientation
    }

    //******************** Direction to move to: EAST     ***************//
    //Orientation: EAST
    if(direction[i] == 3 && orientation == EAST)
    {
      move1Square();
    }
    //Orientation: NORTH
    else if(direction[i] == 3 && orientation == NORTH)
    {
      turn90Clockwise();
      move1Square();
      orientation = EAST;//Reset Orientation
    }
    //ORIENTATION: SOUTH
    else if(direction[i] == 3 && orientation == SOUTH)
    {
      turn90AntiClockwise();
      move1Square();
      orientation = EAST;//Reset orientation
    }
    //ORIENTATION: WEST
    else if(direction[i] == 3 && orientation == WEST)
    {
      turn90Clockwise();
      turn90Clockwise();
      move1Square();
      orientation = EAST;//Reset Orientation
    }

    //********************  Direction to move to: WEST     ***************//
    //Orientation: WEST
    if(direction[i] == 4 && orientation == WEST)
    {
      move1Square();
    }
    //Orientation: NORTH
    else if(direction[i] == 4 && orientation == NORTH)
    {
      turn90AntiClockwise();
      move1Square();
      orientation = WEST;//Reset Orientation
    }
    //Orientation: WEST
    else if(direction[i] == 4 && orientation == SOUTH)
    {
      turn90Clockwise();
      move1Square();
      orientation = WEST;//Reset Orientation
    }
    //Orientation: EAST
    else if(direction[i] == 4 && orientation == EAST)
    {
      turn90AntiClockwise();
      turn90AntiClockwise();
      move1Square();
      orientation = WEST;//Reset Orientation
    }
}    




}
