/******************************************************************************************
Name: Mariessa Logan
Date: 5/8/19
Description:  This program reads the directory created by "buildrooms" and plays a game
The player will follow a path between rooms and will try to get to the end of the game.
Sources: TA office hours, stack overflow, man, geeks for geeks
******************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <dirent.h>

//Create the room struct
struct Room
{

  int id;
  char name[10];
  int connections;
  //this array will hold an int that holds the id of any room it's connected to
  char connectArray[6][10];
  char typeOfRoom[10];

};

char *getDirectory()
{
	//declare stat struct
	struct stat buf;
	struct stat recentBuf;
	//reading the current directory was found on stack overflow, but changed to suit my needs
	//declare a dirent structure
  	struct dirent *de;  
  	struct dirent *mostCurrent;
    // opendir() returns a pointer of DIR type that points to current directory
    DIR *dr = opendir("."); 
  	time_t recentTime;
  	time_t loopTime;

  // opendir returns NULL if couldn't open directory 
    if (dr == NULL)  
    { 
        printf("ERROR - Could not open current directory" ); 
        return 0; 
    } 
  	
  	//assign directory entry to the first file entry 
    mostCurrent = readdir(dr);
    stat(mostCurrent->d_name, &recentBuf);

    recentTime = recentBuf.st_mtime;

    //I had to implement this i variable because it kept saying the most recent directory
    //was ".", which means the current directory that we're in.  This causes the if statement
    //to go past that.
    int i = 1;
    while ((de = readdir(dr)) != NULL)
    {
    	//stat the "most recent" and the next directory in the loop

    	stat(de->d_name, &buf);

    	//if this is a directory then check the time
		if(S_ISDIR(buf.st_mode))
		{
			loopTime = buf.st_mtime;
			//if recent time is older, or if for some reason it's not a directory
			if(recentTime < loopTime || i == 1 )
			{
				mostCurrent = de;
				recentTime = loopTime;
			}

		}
		i++;
    }
    closedir(dr); 
    return mostCurrent->d_name;
}
 
//MUTEX TIME FUNCTION
pthread_t tid[2]; 
int counter; 
pthread_mutex_t lock; 
  
void* timeCheck() 
{ 
	//LOCK THE THREAD
	//MUTEX RIGHT HERE TA'S! I HOPE THIS IS EASIER TO SPOT :)
    pthread_mutex_lock(&lock); 
  
    char timeStamp[200];
    time_t t;
    struct tm *tempTime;
    //SET THE TIME AND CHECK THAT TEMPTIME ACTUALLY HAS A VALUE
    t = time(NULL);
    tempTime = localtime(&t);
    if(tempTime == NULL)
    {
    	printf("Error computing time\n");
    	return NULL;
    }
  	//format should be  time(1:03), weekay(word), month(word), date(#), year(#)
  	strftime(timeStamp, sizeof(timeStamp), "%l:%M%P, %A, %B %d, %Y",tempTime);
 
  	printf("\n%s\n", timeStamp);
  	//PRINT TO A FILE NOW
  	FILE *timeFile;
  	timeFile = fopen("currentTime.txt", "w+");
  	if(timeFile == NULL)
        printf("Error, file couldn't open");
      //Print time to file
      fprintf(timeFile, timeStamp);
      fclose(timeFile);

   	//UNLOCK THE THREAD 
  	//MORE MUTEX HERE TA'S! :D
    pthread_mutex_unlock(&lock); 
    return NULL; 
}

//GAME FUNCTION
void *playGame()
{
	//get directory
	char *dirName;
    char roomFileName[25];
	dirName = getDirectory();
	FILE *fp;
	char fileNames[7][64];
	struct dirent *de;  // Pointer for directory entry 
	//had to use two variables for this as there wer issues with dirname getting over written
  	strcpy(roomFileName, dirName);
    // opendir() returns a pointer of DIR type.  
    DIR *dr = opendir(roomFileName); 
  	// opendir returns NULL if couldn't open directory 
    if (dr == NULL)  
    { 
        printf("Error: Could not open most current directory" ); 
        return 0; 
    } 
 	strcat(roomFileName, "/");
 	int i =0;
 	int j = 0;
 	de = readdir(dr);
    while ((de = readdir(dr)) != NULL) 
    {
    	//if j is not 0 that means it's not ".." file type
    	if(j != 0)
    	{
    		//copy the directory name and the file name into the array so we can start reading the files.
            strcpy(fileNames[i], "./");
            strcat(fileNames[i], roomFileName);
            strcat(fileNames[i], de->d_name);
            i++;
        }
    	j++;
    }
  	
    closedir(dr);  
    //Now start copying rooms into the room array
	//room array structure
	struct Room *roomArray[7];
	for(i = 0; i < 7; i++)
  	{
    	roomArray[i]=(struct Room *)malloc( sizeof(struct Room));
  	}
	for(i=0; i < 7; i++)
	{
		
		fp = fopen(fileNames[i], "r");
		if(fp == NULL)
		{
			printf("ERROR - Could not read %s\n", fileNames[i]);
			exit(0);
		}
		char ch = getc(fp);
		int countLines =0;

		while( ch != EOF)
		{
			if(ch == '\n')
			{
				countLines = countLines + 1;
				j = 0;
			}
			
			ch = getc(fp);	
		}
		//go back to the beginning of the file
		rewind(fp);
		char line [128];
		char name[10];
		int j = 0;
		while(fgets(line, sizeof line, fp) != NULL)
		{
			//sscan will skip the first 2 columns then get info from the 3rd
			sscanf(line, "%*s %*s %s", name);
			//j is counting to see if it's 0, which is the first line, therefore the name
			if(j == 0)
			{
				strcpy(roomArray[i]->name, name);
			}
			//or if it's the last line, therefore the type of room
			else if(j == countLines -1)
			{
				strcpy(roomArray[i]->typeOfRoom, name);
			}
			//or a connection
			else
			{
				strcpy(roomArray[i]->connectArray[j-1], name);

			}
			j++;
		}
		roomArray[i]->id = i;
		roomArray[i]->connections = countLines - 2 ;

		fclose(fp);
	}

	struct Room *currentRoom;

	//look for the start room
	for(i =0; i < 7; i++)
	{
		//Find the start room to start the game
		if(! strcmp(roomArray[i]->typeOfRoom, "START_ROOM"))
		{
			currentRoom = roomArray[i];
			break;
		}
	}
	//Declare array to hold path
	char name[50][10];
	int countPath;
	strcpy(name[0], currentRoom->name);
//START THE GAME!!!
	do{
		int checkValid = -2;
		//run a do while until a valid connection is entered
		do{
			printf("CURRENT LOCATION: %s\n", currentRoom->name);
			printf("POSSIBLE LOCATIONS: ");
			for(j=0; j < currentRoom->connections; j++)
			{
				printf("%s", currentRoom->connectArray[j]);
				if(j == currentRoom->connections -1)
					printf(".\n");
				else
					printf(", ");
			}

		
			printf("WHERE TO? >");
			char whereTo[10];
			scanf("%s", whereTo);
			for(j = 0; j <currentRoom->connections; j++)
			{
				if(! strcmp(whereTo, "time"))
				{
					int error;
					error = pthread_create(&(tid[1]), NULL, &timeCheck, NULL); 
     				if (error != 0) 
        				printf("\nThread can't be created :[%s]", strerror(error)); 
   				    pthread_join(tid[1], NULL); 
					printf("\n");
					break;
				}
				//make sure that it is a valid connection
				if(! strcmp(whereTo, currentRoom->connectArray[j]))
				{
					checkValid = 0;
					int k;
					//Set current room to the valid connection
					for(k=0; k < 7; k++)
					{
						if(! strcmp(whereTo, roomArray[k]->name))
						{
							currentRoom = roomArray[k];
							break;
						}
					}
					break;
				}
			}
			//let the user know it's invalid
			if(checkValid !=0 && strcmp(whereTo, "time") != 0)
			{
				printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
			}
			printf("\n");
		}while(checkValid != 0);
		//increment count path
		countPath++;
		strcpy(name[countPath], currentRoom->name);
	//End the while loop when you find the end room, or if you have walked to rooms over 50 times
	}while( strcmp(currentRoom->typeOfRoom, "END_ROOM") && countPath <= 50);
	printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	//print out the path
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", countPath);
	for(i=1; i <= countPath; i++)
	{
		printf("%s\n", name[i]);
	}
	//They took too long to guess, so end the game and save your memory
	if(countPath > 50)
	{
		printf("GAME OVER! YOU TOOK TOO LONG STORMING THE CASTLE!\n\nDon't do so bad next time\n");
		return 0;
	}
}
//Main function
int main()
{
	//error will be a flag that the thread wasn't built
	int error;
	//initiate the thread MUTEX LOCK (HERE'S MUTEX TOO :D)
    if (pthread_mutex_init(&lock, NULL) != 0) 
    { 
        printf("\n mutex init has failed\n"); 
        return 1; 
    } 
    //check for errors
	error = pthread_create(&(tid[0]), NULL, &playGame, NULL); 
	if (error != 0) 
 		printf("\nThread can't be created :[%s]", strerror(error));
 	//Start the thread with the playgame function 
   	pthread_join(tid[0], NULL); 
   	//destroy the thread locks
    pthread_mutex_destroy(&lock); 
	

	return 0;
}
