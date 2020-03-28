#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <limits.h> 
#include <stdbool.h>
											
#define INF 99999
#define MAX_STAT 305
#define MAX_LINE 15
#define MAX_CHAR 70
#define TIME 1
#define FASTEST 2
#define SHORTEST 3
#define QUIT 4
#define noOfLines 13
#define noOfStations 303
#define noOfConnections 406

int conn_time[MAX_STAT][MAX_STAT];
int conn_stat[MAX_STAT][MAX_STAT];
int conn_line[MAX_STAT][MAX_STAT];

int start;
int stop;

//struct that is used to store details about every line
struct Line
{
	char name[MAX_CHAR];	
}line[MAX_LINE];

//struct that is used to store details about every station
struct Station
{
	int number_of_lines;
	int ln[10];
	char name[MAX_CHAR];
}stat[MAX_STAT];


//all the functions created and used
void run(void);
void displayOptions(void);
void removeNewLine(char *buffer);
void readLines(void);
void readStations(void);
void readConnerctions(void);
bool verifLine(int number, int line);
void justLetters(char* ch);
void toLowerCase(char* ch);
int findID(char* ch);
int startStation(void);
int finalStation(void);
int communLine(int a, int b);
void printPath(int aux[], int n, int conn[MAX_STAT][MAX_STAT]);
void path(int aux[], int n, int dist, int selector, int conn[MAX_STAT][MAX_STAT]);
void calculatePath(int graph[MAX_STAT][MAX_STAT], int selector, int startnode, int target);
void all(int selector);

// reads the optins and, while the option 
// is not QUIT, runs the program
void run()
{
	bool go = true;

	while(go)
	{
		displayOptions();
		int op;
		printf("Your choice: ");
		scanf("%d", &op);
		if(op == QUIT)
		{
			go = false;
		}
		else
		{
			all(op);
		}
	}
}

// displays all the options from the menu
void displayOptions()
{
	printf("TUBE NAVIGATION! - Please select your option!\n");
	printf("1. Calculate time between two stations\n");
	printf("2. Calculate the fastest route between two stations\n");
	printf("3. Calculate the route with the least number of stations\n");
	printf("4. QUIT\n");
}

// remove any unnecessary line
void removeNewLine(char *buffer)
{
	size_t length = strlen(buffer);	

	if (length == 0)
  	{
    	return;
  	}

  	if (buffer[length - 1] == '\n')
 	{
    	buffer[length - 1] = '\0';
  	}
}

// verifies if a line is already added to the array 
// containing the lines og a station 
bool verifLine(int number, int line)
{
	for(int i = 1; i <= stat[number].number_of_lines; i++)
	{
		if(stat[number].ln[i] == line)
		{
			return false;
		}
	}
	return true;
}

//reads the lines from the file and using the line struct 
void readLines()
{
	FILE *fp_lines = fopen("../files/lines_list.txt", "r");
	
	for(int i = 1; i <= noOfLines; i++)
	{
		fgets(line[i].name, MAX_CHAR, fp_lines);
		removeNewLine(line[i].name);
	}
	fclose(fp_lines);
}

//reads the lines from the file and using the station struct 
void readStations()
{
	FILE *fp_stations = fopen("../files/stations_list.txt", "r");
	
	for(int i = 1; i <= noOfStations; i++)
	{
		fgets(stat[i].name, MAX_CHAR, fp_stations);
		removeNewLine(stat[i].name); 
	}
	fclose(fp_stations);
}

// reads the connections, the line on which is the connection and
// time needed to go between 2 stations and adds the values in graphs
void readConnerctions()
{
	FILE *fp_connections = fopen("../files/connections_list.txt", "r");
	
	for(int i = 1; i <= noOfConnections; i++)
	{
		int onLine;
		int time;
		int st_one, st_two;

		fscanf(fp_connections, "%d %d %d %d", &st_one, &st_two, &onLine, &time);

		conn_stat[st_one][st_two] = 1;
		conn_stat[st_two][st_one] = 1;
		conn_time[st_one][st_two] = time;
		conn_time[st_two][st_one] = time;
		conn_line[st_one][st_two] = onLine;
		conn_line[st_two][st_one] = onLine;

		if(verifLine(st_one, onLine) == true)
		{
			stat[st_one].number_of_lines++;
			int temp = stat[st_one].number_of_lines;
			stat[st_one].ln[temp] = onLine;
		}

		if(verifLine(st_two, onLine) == true)
		{
			stat[st_two].number_of_lines++;
			int temp2 = stat[st_two].number_of_lines;
			stat[st_two].ln[temp2] = onLine;
		}
	}
	fclose(fp_connections);
}

// remove any character which is not a letter
void justLetters(char *ch)
{
	char *temp = ch;

	for(int i = 0; i < strlen(temp); i++)
	{
		if((*(temp + i) < 'a' || *(temp + i) > 'z') && (*(temp + i) < 'A' || *(temp + i) > 'Z'))
		{
			strcpy((temp + i), (temp + i + 1));
		}
	}
	strcpy(ch, temp);
}

// sets to lowercase every letter character
void toLowerCase(char *ch)
{
	char *temp = ch;

	justLetters(temp);

	for(int i = 0; i < strlen(temp); i++)
	{
		*(temp + i) = tolower(*(temp + i));
	}
	strcpy(ch, temp);
}

// retirns the id of a certain station or returns -1 if the station couldn't be found
// eliminates all the nonletter characters and sets every char to lowercase
// in order to let the user type in any way he/she wants
int findID(char *ch)
{
	char *copy = ch;
	justLetters(copy);
	toLowerCase(copy);

	for(int i = 1; i <= noOfStations; i++)
	{	
		char new[MAX_CHAR ];
		strcpy(new, stat[i].name);
		char *temp = new;

		justLetters(temp);
		toLowerCase(temp);
		
		if(strcmp(temp, copy) == 0)
		{
			return i;
		}
	}
	return -1;
}

// reads the station from the terminal
// if the station couldn't be recognized, ask the user to try again
int startStation()
{
	char start_station[MAX_CHAR];

	printf("What is your start station?\n");

	fgets(start_station, MAX_CHAR, stdin);

	if(findID(start_station) == -1)
	{
		while(true)
		{
			fgets(start_station, MAX_CHAR, stdin);

			if(findID(start_station) == -1)
			{
				printf("Cannot recognize the station! Try again!\n");
			}
			else
			{
				return findID(start_station);
			}
		}
	}
}

// reads the station from the terminal
// if the station couldn't be recognized, ask the user to try again
int finalStation()
{
	char final_station[MAX_CHAR];

	printf("\nWhat is your final station?\n");

	while(true)
	{
		fgets(final_station, MAX_CHAR, stdin);
		
		if(findID(final_station) == -1)
		{
			printf("Cannot recognize the station! Try again!\n");
		}
		else
		{
			return findID(final_station);
		}
	}
}

// returns the commun line of 2 stations using the conn_line graph
int communLine(int a, int b)
{
	if(conn_line[a][b] != 0)
	{
		return conn_line[a][b];
	}	
	return false;
}

// prints the path and the stations where theuser should change the line
void printPath(int aux[], int n, int conn[MAX_STAT][MAX_STAT])
{
	int temp;
	int v[MAX_STAT];
	for(int i = n; i > 1; i--)
	{
		temp = communLine(aux[i], aux[i - 1]);
		v[i] = temp;
	}

	int prev = v[n];

	for(int i = n; i > 1; i--)
	{
		int x = aux[i];
		printf("-> %s\n", stat[x].name);

		if(i == n)
		{
			printf("!!! Take the %s\n", line[v[i]].name);
		}
			
		if(prev != v[i])
		{
			printf("!!! Change line here! Take the %s\n", line[v[i]].name);
			prev = v[i];
		}

		printf("%*c|\n", 5, ' ');
	}
	printf("-> %s\n\n", stat[aux[1]].name);
}

// selects what path should be printed according to the selector
void path(int aux[], int n, int dist, int selector, int conn[MAX_STAT][MAX_STAT])
{

	if(selector == TIME)
	{
		printf("\nIt takes about %d minutes to go from your start station to your destination!\n\n", dist + n);
	}
	else if(selector == FASTEST)
	{
		printf("\nIt takes about %d minutes to go from your start station to your destination!\n\n", dist + n);
		printPath(aux, n, conn);
	}
	else if(selector == SHORTEST)
	{
		printf("\nYou have to go through %d stations to go from your start station to your destination!\n\n", dist);			
		printPath(aux, n, conn);
	}
}

// Djkstra's algorithm which create an array with the right path
void calculatePath(int graph[MAX_STAT][MAX_STAT], int selector, int startnode, int target)
{
	int cost[MAX_STAT][MAX_STAT];
	int distance[MAX_STAT], pred[MAX_STAT], visited[MAX_STAT];
	int count, mindistance, nextnode;
	
	for(int i = 1; i <= noOfStations; i++)
	{
		for(int j = 1; j <= noOfStations; j++)
		{
			// where the initial graph is 0, we initialize a new graph with a big value(INF)
			if(graph[i][j] == 0)
			{
				cost[i][j] = INF;
			}
			else
			{
				// if the graph is not 0, we initialize the new graph with the value from the initial graph
				cost[i][j] = graph[i][j];
			}
		}
	}
	
	for(int i = 1; i <= noOfStations; i++)
	{
		distance[i] = cost[startnode][i];
		pred[i] = startnode;
		visited[i] = 0;
	}
	
	distance[startnode] = 0;
	visited[startnode] = 1;
	count = 1;
	
	while(count <= noOfStations - 1)
	{
		mindistance = INF;
		
		for(int i = 1; i <= noOfStations; i++)
		{
			if(distance[i] < mindistance && !visited[i])
			{
				mindistance = distance[i];
				nextnode = i;
			}
		}
			
		visited[nextnode] = 1;

		for(int i = 1; i <= noOfStations; i++)
		{
			//it the node is not visited and the new distance is smaller than the the old one, 
			// we change the value of the old value
			if(!visited[i])
			{
				if(mindistance + cost[nextnode][i] < distance[i])
				{
					distance[i] = mindistance + cost[nextnode][i];
					pred[i] = nextnode;
				}
			}
		}
		count++;
	}
 	int aux[noOfStations];
 	int nr_st = 0;

 	// we build a new vector in which we add the path
	if(target != startnode)
	{
		int j = target;
		aux[++nr_st] = j;
		do
		{
			j = pred[j];
			aux[++nr_st] = j;
		} while(j != startnode);
	}
	path(aux, nr_st, distance[target], selector, graph);
}

// all functions are called here(according to the selector) 
void all(int selector)
{
	readLines();
	readStations();
	readConnerctions();

	start = startStation();
	stop = finalStation();

	if(start == stop)
	{
		printf("\nYou are already here!\n\n");
	}
	else
	{
		switch(selector)
		{
			case TIME :	calculatePath(conn_time, TIME, start, stop); break;
			case FASTEST : calculatePath(conn_time, FASTEST, start, stop); break;
			case SHORTEST : calculatePath(conn_stat, SHORTEST, start, stop); break;
			default : printf("\nERROR\n");
		}
	}
}


int main()
{
	run();
	return 0;
}
