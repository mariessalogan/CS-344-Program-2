/*******************************************************************************************
  Name: Mariessa Logan
  Date: 5/8/2019
  Description: This is the build rooms file.  It will build a series of several rooms and
  connect them together. That way you can play the "ADVENTURE GAME!".  I used the code
  outline provided by the instructor.  I used stack overflow and geeks for geeks to find
  examples of C code that I could use.
*******************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
//This is to create a bool variable for my code since C89 does not have this variable
typedef enum
{
  false,
  true
} bool;

//this is to create a room type variable
typedef enum
{
  startHere,
  keepGoing,
  endHere
} roomType;

//Create the room struct
struct Room
{

  int id;
  char* name;
  int connections;
  //this array will hold an int that holds the id of any room it's connected to
  int connectArray[6];
  char typeOfRoom[10];

};

// Returns true if all rooms have 3 to 6 outbound connections, false otherwise
bool IsGraphFull(struct Room *array[])  
{
  int i;
  for(i=0; i < 7; i++)
  {
    if(array[i]->connections < 3 )
      return false;
  }
  return true;
}

// Returns a random Room, does NOT validate if connection can be added
int GetRandomRoom(struct Room *array[])
{
  //intitialize randomization
  
  int index = rand() % 7;
  //send back the room at that index
  return index;
}

// Returns true if a connection can be added from Room x (< 6 outbound connections), false otherwise
bool CanAddConnectionFrom(struct Room *x) 
{

  if(x->connections < 6)
  {

    return true;
  }
  return false;
}
// Returns true if a connection from Room x to Room y already exists, false otherwise
bool ConnectionAlreadyExists(struct Room *x, struct Room *y)
{
  int i;
  bool connected;
  for(i =0; i <= 6; i++)
  {
    if(x->connectArray[i] == y->id)
    {
      connected = true;
      break;
    }
  }
  if(connected == true)
    return true;
  else
    return false;
}

// Connects Rooms x and y together, does not check if this connection is valid
void ConnectRoom(struct Room *x, struct Room *y) 
{
  //Add the id's to the arrays and increment
  x->connectArray[x->connections] = y->id;
  y->connectArray[y->connections] = x->id;
  x->connections++;
  y->connections++;

}

// Returns true if Rooms x and y are the same Room, false otherwise
bool IsSameRoom(struct Room *x, struct Room *y) 
{
  if(x->id == y->id)
    return true;
  else
    return false;
}

// Adds a random, valid outbound connection from a Room to another Room
void AddRandomConnection(struct Room *array[])  
{
  int A;  // Maybe a struct, maybe global arrays of ints
  int B;
  
  while(true)
  {
    A = GetRandomRoom(array);

    if (CanAddConnectionFrom(array[A]) == true)
    {
     
      break;
    }
  }
  
  B = GetRandomRoom(array);
  int i = 0;
  do
  {
   
     //I didn't want to call randomRoom 7 times, so I figured I could just increment
     B++;
     //don't go over 7!
    if(B == 7)
    {  
      B = 0;
    }
    // variable i is here to break the loop if you can't find a good room to connect with A
    i++;
    if(i == 7)
      return;
  }
  while(CanAddConnectionFrom(array[B]) == false || IsSameRoom(array[A], array[B]) == true || ConnectionAlreadyExists(array[A], array[B]) == true);
  //you found some rooms you can connect, so go for it!
  ConnectRoom(array[A], array[B]);  
  
}

//This is a function I added to assist with naming rooms
void nameRoom(struct Room *array[], int index, char *name[])
{
  int randomizer;
  int match = -1;
  //While match == 0, that means that there is a room already named that
  do
  {

    int i;
    int compare;
    randomizer = rand() % 10;
    for(i = 0; i < 7; i++){
      //no reason to check anything over the index number, since we're accessing these in order of the array
      if(i > index)
        break;
      //Don't compare a room to itself.
      if(i != index)
      {
        //check to see if the two names match, and break and start the do statement again if it does
        match = strcmp(name[randomizer], array[i]->name);
        if(match == 0)
          break;
      }
    }

  }
  while( match == 0);
  array[index]->name = name[randomizer];
}

//Main function to run the other functions
int main()
{
  int i;
  srand(time(NULL));
  //initialize room names
  char *nameArray[10];
  nameArray[0] = "Taako";
  nameArray[1] = "Magnus";
  nameArray[2] = "Merle";
  nameArray[3] = "Angus";
  nameArray[4] = "Lucretia";
  nameArray[5] = "Killian";
  nameArray[6] = "Carrie";
  nameArray[7] = "Lupe";
  nameArray[8] = "Barry";
  nameArray[9] = "Pan";

  //initialize an array of room structs
  struct Room *roomArray[7];
  for(i = 0; i < 7; i++)
  {
    roomArray[i]=(struct Room *)malloc( sizeof(struct Room));
  }

  //Initialize start room
  roomArray[0]->id = 0;
  roomArray[0]->name = calloc(16, sizeof(char));
  strcpy(roomArray[0]->typeOfRoom, "START_ROOM");
  roomArray[0]->connections = 0;
  nameRoom(roomArray, 0, nameArray);
 
  //Initialize middle rooms
  
  for(i = 1; i < 6; i++)
  {
    roomArray[i]->id = i;
    roomArray[i]->name = calloc(16, sizeof(char));
    strcpy(roomArray[i]->typeOfRoom, "MID_ROOM");
    roomArray[i]->connections = 0;
    nameRoom(roomArray, i, nameArray);
  }

   //initialize end room
  roomArray[6]->id = 6;
  roomArray[6]->name = calloc(16, sizeof(char));
  strcpy(roomArray[6]->typeOfRoom, "END_ROOM");
  roomArray[6]->connections = 0;
  nameRoom(roomArray, 6, nameArray);

  // Create all connections in graph
  int j;
 while (IsGraphFull(roomArray) == false)
  {
    AddRandomConnection(roomArray);
  }
  
  //Time to make the files and the directory
  //Get the process ID for the directory
  int processID = getpid();
  char dirName[32];
  sprintf(dirName,"loganma.rooms.%d/", processID);

  //Create the directory
  int result = mkdir(dirName , 0777);
  char fileName[64];
  FILE *f;
  int k;
  
  //start getting the created
  for(i = 0; i < 7 ; i++)
    {
      char connectionName[8];
      sprintf(fileName, "%s%s_room", dirName, roomArray[i]->name);
      f = fopen(fileName, "w+");
      //Make sure file opened
      if(f == NULL)
        printf("Error, file couldn't open");
      //Print name to file
      fprintf(f,"ROOM NAME: %s\n", roomArray[i]->name);
      //Print the connections
      for(j=0; j < roomArray[i]->connections; j++)
      {
        for(k=0; k < 7; k++)
        {
          if(roomArray[i]->connectArray[j] == roomArray[k]->id)
          {
            strcpy(connectionName, roomArray[k]->name);
            break;
          }
        }
        //print out the connections
        fprintf(f,"CONNECTION %d:  %s \n", j +1, connectionName);

        
      }
      fprintf(f, "ROOM TYPE: %s\n", roomArray[i]->typeOfRoom);
      fclose(f);
    }
      
  for(i = 0; i < 7; i++)
  {
    
    free(roomArray[i]);
  }
  return 0;
}