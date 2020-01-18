#include <iostream>
#include <cstdlib>
#include <ctime>
#include <iomanip>
using namespace std;
/*This program attempts to display the effects of evolution
A certain amount of robots will be placed on a virtual map
The map contains walls and batteries
The robots will each have 4 power and each turn consumes 1 power
Each battery gives a robot 4 power
If the robot does not have enough power, it shuts down
The robots that lived the longest will have its genes passed on
Map objects will be represented by the following:
Nothing:0
Wall:1
Battery:2
Bot:9
*/

class Evolution{
  private:
//determines the size of the map
  static const int MAP=12;
  int map[MAP][MAP];
//The percent number of bateries on map
  static const int BATTERIES=40;
//determines how much power is given per battery
  static const int P_BATT=4;
  public:
  void make_Map();
  void i_Map(int y,int x,int i){map[y][x]=i;};
  int o_Map(int y,int x){return map[y][x];};
  int o_BATT(){return P_BATT;};
  void s_MAP();
};

//function to build map
void Evolution::make_Map(){
  int batt_To_Place=((MAP-2)*(MAP-2))*BATTERIES/100;
  int x,y;
//fills in the borders for the map
  for(int i=0;i<MAP;i++){
    map[i][0]=1;
    map[0][i]=1;
    map[i][MAP-1]=1;
    map[MAP-1][i]=1;
  }
//fills in the blank spaces
  for(int y=1;y<MAP-1;y++){
    for(int x=1;x<MAP-1;x++){
      map[y][x]=0;
    }
  }
  do{
    x=rand()%10+1;
    y=rand()%10+1;
    if(map[y][x]==0){
      map[y][x]=2;
      batt_To_Place--;
    }
  }while(batt_To_Place>0);
}

//function to show map
void Evolution::s_MAP(){
  cout<<"<MAP>\n"
  <<"=========================================\n"
  <<"   |1|2|3|4|5|6|7|8|9|10|11|12|\n"
  <<"--------------------------------\n";
  for(int y=0;y<MAP;y++){
    cout<<left<<setw(3)<<y+1<<"|";
    for(int x=0;x<MAP;x++){
      cout<<"|"<<map[y][x];
    }
    cout<<"|"<<endl;
  }
  cout<<"--------------------------------\n\n";
}

/*class for robots
Gene has different rows and columns
For each row represents a different condition
Each column except the last one is the condition for the action that the robot will take
The last column is the action
Action:
Up=0
Down=1
Left=2
Right=3
Random=4
Direction:
First column is up, second is down, third is left, forth is right
*/
class Robot:public Evolution{
  private:
//determines the size of the genes and of sensors
  static const int SIZE=4;
  int gene[SIZE*SIZE][SIZE+1];
  int sensor[SIZE];
//stores the location,power and life time of bot
  int y_C,x_C,power,life;
  public:
  int o_Size(){return SIZE;};
  void i_Gene(int y,int x,int i){gene[y][x]=i;};
  int o_Gene(int y,int x){return gene[y][x];};
  int o_Power(){return power;};
  int o_Life(){return life;};
//setting up the genes, adding in extra steps to make the process more randomized
  Robot(){
    for(int y=0;y<SIZE*SIZE;y++){
      for(int x=0;x<SIZE+1;x++){
        if(x<SIZE){
          gene[y][x]=rand()%4;
        }
        else{
          gene[y][x]=rand()%5;
        }
      }
    }
  }
//function to spawn robot on map
  void spawn();
//function for sensor **REQUIRES THE DIRECTIONS TO BE IN JUST 4 DIRECTIONS THEREFORE LIMITS SIZE TO BE ABLE TO HAVE DIFFERENT VALUES, MORE ADJUSTMENTS MAYBE NEEDED IN THE FUTURE
  void blind();
//function to which action to do
  int choose();
//function for bot to move
  void legs();
};

//function to spawn robot on map
void Robot::spawn(){
  power=4;
  life=0;
  bool spawned=false;
  do{
    y_C=rand()%10+1;
    x_C=rand()%10+1;
    if(o_Map(y_C,x_C)==0){
      i_Map(y_C,x_C,9);
      spawned=true;
    }
  }while(spawned==false);
}

//function for sensor
void Robot::blind(){
  sensor[0]=o_Map(y_C-1,x_C);
  sensor[1]=o_Map(y_C+1,x_C);
  sensor[2]=o_Map(y_C,x_C-1);
  sensor[3]=o_Map(y_C,x_C);
}

int Robot::choose(){
//variable to make sure that all conditions are passed
  bool pass=true;
  int x=0;
  for(int y=0;y<SIZE*SIZE;y++){
    x=0;
    pass=true;
    do{
      if(gene[y][x]!=sensor[x]){
        pass=false;
      }
      x++;
    }while((x<SIZE)&&(pass==true));
    if(pass==true){
      return gene[y][SIZE];
    }
  }
//if no conditions apply, allow robot to be random
  return 4;
}

void Robot::legs(){
  bool moved=false;
  int brain;
  i_Map(y_C,x_C,0);
  life++;
  brain=choose();
//do loop in case some weird computer magic happened
  do{
    if(brain==0){
      if(o_Map(y_C-1, x_C)!=1){
        y_C--;
      }
      moved=true;
    }
    else if(brain==1){
      if(o_Map(y_C+1, x_C)!=1){
        y_C++;
      }
      moved=true;
    }
    else if(brain==2){
      if(o_Map(y_C, x_C-1)!=1){
        x_C--;
      }
      moved=true;
    }
    else if(brain==3){
      if(o_Map(y_C, x_C+1)!=1){
        x_C++;
      }
      moved=true;
    }
    else{
      brain=rand()%4;
    }
  }while(moved==false);
  if(o_Map(y_C,x_C)==2){
    power+=o_BATT();
  }
  power--;
  i_Map(y_C,x_C,9);
}

//function prototypes
void sort(Robot bot[],int HANGAR);
void sip_Zap(Robot bot[],int HANGAR);
bool M_Roover(Robot bot[],int HANGAR);

int main() {
  srand(time(0));
//Determine the number of bots participating
  const int HANGAR=200;
//Determine the number of generations
  const int GENERATIONS=100;
  Robot bot[HANGAR];
  int avg_Life=0;
  for(int g=0;g<GENERATIONS;g++){
    for(int i=0;i<HANGAR;i++){
      bot[i].make_Map();
      bot[i].spawn();
    }
    do{
      for(int k=0;k<HANGAR;k++){
        if(bot[k].o_Power()>0){
          bot[k].blind();
          bot[k].choose();
          bot[k].legs();
        }
      }
    }while(M_Roover(bot, HANGAR)==true);
    avg_Life=0;

    for(int k=0;k<HANGAR;k++){
      avg_Life+=bot[k].o_Life();
    }
    avg_Life=avg_Life/HANGAR;
    sort(bot, HANGAR);
    cout<<"Generation: "<<g+1
    <<"\n=========================================\n"
    <<"Average life span: "<<avg_Life<<"  Longest lifespan: "<<bot[0].o_Life()<<endl<<"Lowest life span: "<<bot[HANGAR-1].o_Life()<<endl
    <<"-------------------------------------------\n\n";
    sip_Zap(bot, HANGAR);
  }
}

//function to bubble short
void sort(Robot bot[],int HANGAR){
  bool swapped=false;
  for (int i = 0; i < HANGAR-1; i++){
    swapped = false;
    for (int z = 0; z < HANGAR-i-1; z++){
      if (bot[z].o_Life() < bot[z+1].o_Life()){
        swapped = true;
        Robot swap = bot[z];
        bot[z] = bot[z+1];
        bot[z+1] = swap;
      }
    }
    if (swapped == false){
      return;
    }
  }
}

//function to allow bots to 'evolve' by using the best half of the preforming bots genes and create new robots
//A random percent chance for a certain gene to mutate is set
void sip_Zap(Robot bot[],int HANGAR){
  Robot *proto=new Robot[HANGAR];
  int count=HANGAR/2,num=1,mutate;
  const int SIZE = bot[0].o_Size();
//determines the chance of mutation
  const int MUTATE_CHANCE=5;
//bug testing code
/*
  cout<<"\nBefore\n"<<"============================================================\n";
  for(int k=0;k<HANGAR;k++){
    cout<<"\nBOT: "<<k<<"\n=========================================\n";
    for(int y=0;y<SIZE*SIZE;y++){
      cout<<"\nY GENE: "<<y<<endl;
      for(int x=0;x<SIZE+1;x++){
        cout<<"X GENE: "<<bot[k].o_Gene(y, x)<<" ";
      }
    }
  }
*/
  for(int i=0;i<HANGAR/2;i++){
    proto[i]=bot[i];
  }
  do{
    for(int y=0;y<SIZE*SIZE/2;y++){
      mutate=rand()%100+1;
      if(mutate<=MUTATE_CHANCE){
        for(int x=0;x<SIZE+1;x++){
          if(x<SIZE){
            proto[count].i_Gene(y, x, rand()%rand()%4);
          }
          else{
            proto[count].i_Gene(y, x, rand()%rand()%5);
          }
        }
      }
      else{
        for(int x=0;x<SIZE+1;x++){
          proto[count].i_Gene(y, x, bot[num].o_Gene(y,x));
        }
      }
    }
    for(int y=SIZE*SIZE/2;y<SIZE*SIZE;y++){
      mutate=rand()%100+1;
      if(mutate<=MUTATE_CHANCE){
        for(int x=0;x<SIZE+1;x++){
          if(x<SIZE){
            proto[count].i_Gene(y, x, rand()%rand()%4);
          }
          else{
            proto[count].i_Gene(y, x, rand()%rand()%5);
          }
        }
      }
      else{
        for(int x=0;x<SIZE+1;x++){
          proto[count].i_Gene(y, x, bot[num-1].o_Gene(y,x));
        }
      }
    }
    num++;
    count++;
  }while(count<HANGAR);
  for(int i=0;i<HANGAR;i++){
    bot[i]=proto[i];
  }
//Used for bug testing
/*
  cout<<"\nAFTER\n"<<"============================================================\n";
  for(int k=0;k<HANGAR;k++){
    cout<<"\nBOT: "<<k<<"\n=========================================\n";
    for(int y=0;y<SIZE*SIZE;y++){
      cout<<"\nY GENE: "<<y<<endl;
      for(int x=0;x<SIZE+1;x++){
        cout<<"X GENE: "<<bot[k].o_Gene(y, x)<<" ";
      }
    }
  }
*/
  delete []proto;
}

//function to check if all bots are alive
bool M_Roover(Robot bot[],int HANGAR){
  for(int k=0;k<HANGAR;k++){
    if(bot[k].o_Power()>0){
      return true;
    }
  }
  return false;
}
