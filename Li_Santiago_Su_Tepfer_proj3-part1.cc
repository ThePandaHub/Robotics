/*
 *  blobs.cc
 * 
 *  The beginnings of a controller that uses the blobfinder.
 *
 *  Created:     Simon Parsons
 *  Date:        7th October 2011 
 *  Edited: Eric Li, Myles Santiago, Oscar Su, Michael Tepfer
 */


#include <iostream>
#include <cstdlib>
#include <libplayerc++/playerc++.h>
#include <stdint.h>
#include <math.h>

int main(int argc, char *argv[])
{  
  using namespace PlayerCc;  

  // Set up proxies
  PlayerClient    robot("localhost");  
  BumperProxy     bp(&robot,0);  
  Position2dProxy pp(&robot,0);
  PlayerClient    blobfinder("localhost");
  BlobfinderProxy bf(&blobfinder,0);

  // Variables

  player_blobfinder_data_t  blobList;   // Structure holding the blobs found
  player_blobfinder_blob_t* theBlobs;   // A point to a list of blobs
  player_blobfinder_blob_t  myBlob;     // A single blob

  // Allow the program to take charge of the motors (take care now)
  pp.SetMotorEnable(true);


  // Control loop
  while(true) 
    {    
      const double DEFAULT_TURNRATE = .3;
      double turnrate, speed;
      bool reachedDest = false;

      // Read from the proxies
      robot.Read();
      blobfinder.Read();

      // We only want to drive if the bumpers are not pressed

      if ((bp[0] == 0)&&(bp[1] == 0))
	{
	  if (bf.GetCount() > 0){

	    // Here the blobfinder has found some color blob
	    

	    std::cout << std::endl;
	    std::cout << bf.GetCount() << " Blobs found!" << std::endl;
	
	    for(int i = 0; i < bf.GetCount(); i++){ // blob loop
	      if((short)bf.GetBlob(i).color == (short)0) { // if red blob found
	        if((bf.GetBlob(i).x < 100 || bf.GetBlob(i).x > 200)) { // if not centered
		  speed = 0;
		  turnrate = DEFAULT_TURNRATE;
	        } // end of if not centered
                else { // not off center
		  if(bf.GetBlob(i).area > 760) { // if reached dest, end program
			speed = 0;
			turnrate = 0;
			reachedDest = true;
		  } // end of if reached dest
		  speed = 2.0/sqrt(bf.GetBlob(i).area);
                  turnrate = 0;
		} // end of not off center
		
		std::cout << "Id: "    << bf.GetBlob(i).id    << std::endl;
	        std::cout << "Color: " << (short)bf.GetBlob(i).color << std::endl;
	        std::cout << "Area: "  << bf.GetBlob(i).area  << std::endl;
	        std::cout << "X: "     << bf.GetBlob(i).x     << std::endl;
	        std::cout << "Y: "     << bf.GetBlob(i).y     << std::endl;
	        std::cout << std::endl;
	      } // end of if red blob found
	      else // not a red blob
	      {
		speed = 0;
		turnrate = DEFAULT_TURNRATE;
	      } // end of not a red blob
		if (reachedDest) return 0;
	    } // end of blob loop
		
	  } // end of blobs found
	  else { // no blobs
	      speed = 0;
	      turnrate = DEFAULT_TURNRATE;
	    } // end of no blobs
	} // end of bumpers not pressed
      else { // If bumpers are pressed, do nothing. 
	speed = 0;
	turnrate = 0;
      } // end of bumpers pressed

      // What are we doing?
      //std::cout << pp << std::endl;      
      std::cout << "Speed: " << speed << std::endl;      
      std::cout << "Turn rate: " << turnrate << std::endl << std::endl;
      
      // For safety, in the absence of a controller, do nothing.
      //speed = 0;
      //turnrate = 0;
      pp.SetSpeed(speed, turnrate);
    } // end of while(true) loop
}



