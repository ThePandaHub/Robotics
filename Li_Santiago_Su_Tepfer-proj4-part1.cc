/**
 * local-roomba.cc
 * 
 * Sample code for a roomba-like robot that has two front bumpers and
 * magically knows where it is. 
 *
 * Written by: Simon Parsons
 * Date:       24th October 2011
 *  
 **/


#include <iostream>
#include <libplayerc++/playerc++.h>
#include <cmath>
using namespace PlayerCc;  

/**
 * Function headers
 *
 **/

player_pose2d_t readPosition(LocalizeProxy& lp);
void printRobotData(BumperProxy& bp, player_pose2d_t pose);

/**
 * main()
 *
 **/

int main(int argc, char *argv[])
{  


  // Variables
  const double X_DEST = 5.0;
  const double Y_DEST = -3.5;
  const double MAX_TURNRATE = 3;
  int counter = 0;
  double speed;            // How fast do we want the robot to go forwards?
  double turnrate;         // How fast do we want the robot to turn?
  player_pose2d_t  pose;   // For handling localization data

  // Set up proxies. These are the names we will use to connect to 
  // the interface to the robot.
  PlayerClient    robot("localhost");  
  BumperProxy     bp(&robot,0);  
  Position2dProxy pp(&robot,0);
  LocalizeProxy   lp (&robot, 0);

  // Allow the program to take charge of the motors (take care now)
  pp.SetMotorEnable(true);

  // Main control loop
  while(true) 
    {    
      // Update information from the robot.
      robot.Read();
      // Read new information about position
      pose = readPosition(lp);

      // Print data on the robot to the terminal
      printRobotData(bp, pose);

      if(bp[0] || bp[1]) { // If either bumper is pressed, stop.
	speed= 0;
	turnrate= 0;
      } 
      else { // bumper not pressed
        double xDist = X_DEST - pose.px;
        double yDist = Y_DEST - pose.py;
        double angle = atan(yDist/xDist);
        double angleDif = angle - pose.pa;
        turnrate = angleDif / 3.14 * MAX_TURNRATE;

        double distance = sqrt(pow(xDist,2) + pow(yDist,2));
        if(abs(distance) < .1) { // reached destination
             std::cout << std::endl << "Reached destination" << std::endl;
             return 0;
        } // end of if reached destination
        speed = distance/5;
      }     

      // What are we doing?
      std::cout << "Speed: " << speed << std::endl;      
      std::cout << "Turn rate: " << turnrate << std::endl << std::endl;

      // Send the commands to the robot
      pp.SetSpeed(speed, turnrate);  
      // Count how many times we do this
      counter++;
    }
  
} // end of main()


/**
 * readPosition()
 *
 * Read the position of the robot from the localization proxy. 
 *
 * The localization proxy gives us a hypothesis, and from that we extract
 * the mean, which is a pose. 
 *
 **/

player_pose2d_t readPosition(LocalizeProxy& lp)
{

  player_localize_hypoth_t hypothesis;
  player_pose2d_t          pose;
  uint32_t                 hCount;

  // Need some messing around to avoid a crash when the proxy is
  // starting up.

  hCount = lp.GetHypothCount();

  if(hCount > 0){
    hypothesis = lp.GetHypoth(0);
    pose       = hypothesis.mean;
  }

  return pose;
} // End of readPosition()


/**
 *  printRobotData
 *
 * Print out data on the state of the bumpers and the current location
 * of the robot.
 *
 **/

void printRobotData(BumperProxy& bp, player_pose2d_t pose)
{

  // Print out what the bumpers tell us:
  std::cout << "Left  bumper: " << bp[0] << std::endl;
  std::cout << "Right bumper: " << bp[1] << std::endl;

  // Print out where we are
  std::cout << "We are at" << std::endl;
  std::cout << "X: " << pose.px << std::endl;
  std::cout << "Y: " << pose.py << std::endl;
  std::cout << "A: " << pose.pa << std::endl;

  
} // End of printRobotData()
