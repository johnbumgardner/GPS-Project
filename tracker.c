/*
Name: John Bumgardner
Class: ECE 209 - Computer Systems Programming
Program 2: This program simulates the operations of a GPS. Assuming roads exist in the xy coordinate plane, and are given by equations in the form of y=mx+b, and assuming position in the world can be modeled as 2D coordinates, this
program keeps track of your location as you move along a system (2) of roads, storing important information such as the number of times your direction changed, the number of time you changed roads, total distance traveled, what 
road you are currently on, and your maximum speed. To simplify this process, numerous functions are used to perform tasks such as tracking current position and updating global variables, calculating distances, finding the closest roads.
*/


#include<stdio.h>
#include<math.h>

// Put this right after the #includes.
#ifndef SECRET_TESTS
double M1=-1.0, B1=0; // Road 1 slope and intercept
double M2=1.0, B2=0;  // Road 2 slope and intercept
#else 
// This allows me to test your program easily with different roads
double M1=SECRET_M1, B1=SECRET_B1;
double M2=SECRET_M2, B2=SECRET_B2;
#endif


//GLOBAL VARIABLES
int Road=1, First_road=1;
int Changed_direction=0;
int Changed_road=0;
double Total_distance=0.0;
double Peak_speed=0.0; 




/* Function Declarations */
void track(double time, double x, double y); //function that updates the global variables and determines when roads and directions have changed
double distance(double x1, double y1, double x2, double y2); // this function computes the distance between (x1,y1) and (x2,y2)
double nearest_point_on_road(double x, double y, int road); //returns x coordinate from nearest point on road
double distance_to_road(int road, double x, double y); //calculates distance to road
int closest_road(double x, double y, int road); //returns the road that is closest, either 1 or 2
double intersection_point_x(); //returns the x coordinate of where the roads intersect





int main() 
{

  double time, x, y;
  int ret;

  do 
  {
    // get time,x,y coordinate from stdin
    ret = scanf("%lf%lf%lf",&time,&x,&y);
    
    if (ret==3)
      {
        // call into the tracking algorithm
        track(time,x,y);
      }

    // if we didn't get three doubles, time to quit
  } while(ret==3);

  // print out final results
  printf("Total distance = %.2lf\n",Total_distance);
  printf("Peak speed = %.2lf\n",Peak_speed);
  printf("# of changes in direction = %d\n",Changed_direction);
  printf("# of changes in road = %d\n",Changed_road);
  printf("First Road = %d, Last Road = %d\n",First_road,Road);
  
  return 0;
}


void track(double time, double x, double y) //function that updates the global variables and determines when roads and directions have changed
{
    static int count = 0; // global variable - keeps track of the number of inputs passed into the program
    static double prior_x; // global variable - stores the last x position
    static double prior_delta; //global variable - stores the previous direction in position
    static int prior_road; //global variable - stores the road from the last function call
    
    double distanceForTime; //amount of distance accumulated over one change in position
    double current_delta; //stores the current diirection in position

    if(count == 0) //for first movement - no distance is accumulated
    {
        Road = closest_road(x,y,First_road); //assume we are on road one - returns the closest road currently
        First_road = Road; //sets the global First_road to the first road passed into the function
        prior_road = Road; //sets this into the prior road to determine if there was a change in road
        prior_x = nearest_point_on_road(x, y, Road); //sets the first position into prior x
        distanceForTime = 0; //again, no distance was traveled here
    }    
    else //for all other calls of track
    {
        Road = closest_road(x,y,Road); //updates the current road
        if(Road == prior_road) //if no change in road occurs
        {
            if (Road == 1) //for road 1 - important to distinguish because of different variables of data to describe each
            {
                //computes the distance traveled in this time instant between the prior xy coordinates and the current xy coordinates - adjusted for the noise in the inputs
                distanceForTime += distance(prior_x, M1 * prior_x + B1, nearest_point_on_road(x, y, Road), M1 * nearest_point_on_road(x, y, Road) + B1); 
            }
            else if(Road == 2) //for road 2 - important to distinguish because of different variables of data to describe each
            {
                //computes the distance traveled in this time instant between the prior xy coordinates and the current xy coordinates - adjusted for the noise in the inputs
                distanceForTime += distance(prior_x, M2 * prior_x + B2, nearest_point_on_road(x, y, Road), M2 * nearest_point_on_road(x, y, Road) + B2);
            }
        }
        else //if the road does not equal the prior road
        {
            Changed_road++; //increment the Changed road counter by one 
            
            if (Road == 1)//from 2 to road 1 - important to distinguish because of different variables of data to describe each
            {
                //compute the distance between prior x and the intersection point and between the interesection point and the new x and y position
                distanceForTime += distance(prior_x, M2 * prior_x + B2, intersection_point_x(), M2 * intersection_point_x() + B2);
                distanceForTime += distance(intersection_point_x(), M2 * intersection_point_x() + B2, nearest_point_on_road(x, y, Road), M1 * nearest_point_on_road(x, y, Road) + B1);
            }
            else if(Road == 2) //from 1 to road 2 - important to distinguish because of different variables of data to describe each
            {
                //compute the distance between prior x and the intersection point and between the interesection point and the new x and y position
                distanceForTime += distance(prior_x, M1 * prior_x + B1, intersection_point_x(), M2 * intersection_point_x() + B2);
                distanceForTime += distance(intersection_point_x(), M2 * intersection_point_x() + B2, nearest_point_on_road(x, y, Road), M2 * nearest_point_on_road(x, y, Road) + B2);
            }
        }
    }
    //use prior delta to determine whether the direction changed
    if(count == 1) //only if one change in position has occurred - meaning there are no two deltas to compare
    {
        //set prior delta equal to the difference between the prior x and the current x adjusted to the road
        prior_delta = prior_x - nearest_point_on_road(x, y, Road);
        if(Road != prior_road)
        {
            Changed_direction++;
        }
    }
    else if(count > 1) //for the rest of the iterations of the track function call
    {
        current_delta = prior_x - nearest_point_on_road(x, y, Road); //calculate the current change in x_position (direction)
        if((current_delta > 0 && prior_delta < 0) || (current_delta < 0 && prior_delta > 0) || (Road != prior_road)) //if the signs are different
        {
            Changed_direction++; //increment the change in direction
        }
        prior_delta = current_delta; //set the prior delta to the current delta

    }
    prior_x = nearest_point_on_road(x, y, Road); //store the current x as the prior x
    Total_distance += distanceForTime; //sum the distance for this time into the total distance
    prior_road = Road; //set the road to the prior road
    double currentSpeed = distanceForTime / time; //calculate the speed
    if(currentSpeed > Peak_speed) //check to see if the current speed exceeds the stored peak speed
    {
        Peak_speed = currentSpeed; //if it does, store the new maximum speed
    }

    count++; //increment the counter on the function
}    


double distance(double x1, double y1, double x2, double y2) // this function computes the distance between (x1,y1) and (x2,y2)
{
    double d = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2)); //using the distance formula, calculate and return the distance
    return d;
}
double nearest_point_on_road(double x, double y, int road)  //returns x coordinate from nearest point on road
{
    double newX; //x value of the closest point on road
    if (road == 1) //for road 1
    {
        newX = (y + (x / M1) - B1) / (M1 + (1 / M1));
    } 
    else if (road == 2) //for road 2
    {
        newX = (y + (x / M2) - B2) / (M2 + (1 / M2));
    }
    return newX;
}
double distance_to_road(int road, double x, double y) //calculates distance to road
{
    //find points closest to the parameter points on the road
    double xVal = nearest_point_on_road(x, y, road); 
    double yVal;
    if (road == 1) 
    {
        yVal = M1 * xVal + B1;
    } 
    else if (road == 2) 
    {
        yVal = M2 * xVal + B2;
    }
    //return the distance between the point on the road closest and the parameter points
    return distance(xVal, yVal, x, y);
}
int closest_road(double x, double y, int road) //returns the road that is closest, either 1 or 2
{
    int otherRoad = (road == 1) ? 2 : 1; //if road is one, other road is 2, and if road is 2, other road is 1
    double currentRoadDistance = distance_to_road(road, x, y); //calculates the distance to the first road
    double otherRoadDistance = distance_to_road(otherRoad, x, y); //calculates the distance to the other road
    if (otherRoadDistance < currentRoadDistance) //return the closest road
    {
        road = otherRoad;
    }
    return road;
}

double intersection_point_x() //returns the x coordinate of where the roads intersect
{ 
    //returns the x coordinate of the intersection of the two lines, 
    //ASSUMES ROADS ARE NOT PARALLEL
    return (B1 - B2) / (M1 - M2); 

}