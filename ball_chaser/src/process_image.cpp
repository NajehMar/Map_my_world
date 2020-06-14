#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
	ROS_INFO_STREAM("Moving the robot towards the ball");
    ROS_INFO("DriveToTarget received - lin:%1.2f, ang:%1.2f", lin_x, ang_z);
  	ball_chaser::DriveToTarget srv;

	srv.request.linear_x = lin_x;
	srv.request.angular_z = ang_z;

    // Call the command_robot service and pass the requested velocities
        // Call the safe_move service and pass the requested joint angles
    if (!client.call(srv))
        ROS_ERROR("Failed to call service safe_move");
       

}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    float ang, lin = 0.0; 
  
	for (int i = 0; i < img.height * img.step; i++)
    {
      int pose = i % img.step;
      
      if ((img.data[i] == white_pixel) && (img.data[i+1] == white_pixel) && (img.data[i+2] == white_pixel))
        {  
        if (pose <= img.step/3){  
           // drive the robot left
		   lin = 0.0; ang = 0.3;
        }
		else if ((img.step/3 < pose) && (pose < img.step*2/3))
        {
           // drive the robot forward
           lin = 0.3; ang = 0.0;
        }
        else if((img.step*2/3 <= pose) && (pose < img.step))
        {
        	// drive the robot right
           lin = 0.0; ang = -0.3; 
        }
       } 
	}
   drive_robot(lin, ang);
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}