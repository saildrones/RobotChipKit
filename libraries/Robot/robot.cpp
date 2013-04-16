
#include <robot.h>
#include <motor.h>
#include <GP2Y0A21YK.h> // IR sensor
#include <CMPS03.h>     // Compas
#include <Servo.h>      // Servo
#include <TiltPan.h>   // Tilt&Pan
#include <LSY201.h>     // Camera


int motor_state = STATE_STOP;
long nb_go = 0;
long nb_obstacle = 0;

JPEGCameraClass JPEGCamera;  // The Camera class  
int no_picture = 0;          // Picture number

int robot_begin()
{
  int ret = SUCCESS;

  // initialize the camera
  ret=JPEGCamera.begin();
  if (ret != SUCCESS)
  {  
        Serial.print("Error Init Camera, error: ");
        Serial.println(ret);
  }  	  	
  else
  {
        Serial.println("Init Camera OK");
  }  
       
 
  Serial.println("End Robot Init");
  Serial.println("");
  return SUCCESS;
  
} 


int CmdRobot (uint8_t cmd [3], uint8_t *resp, int *presp_len)
{    
 CMPS03Class CMPS03;   // The Compass class 
 int resp_len = 0;
 int ret = SUCCESS;

 switch (cmd[0]) {
 
 case CMD_STOP:
     Serial.println("CMD_STOP"); 
     stop();
     motor_state = STATE_STOP;
     break; 
  
 case CMD_START:
     if (cmd[1] == 0)
     {  
           Serial.println("CMD_START");
           start_forward();
     }
     else
     {       
           Serial.print("CMD_START_TEST motor: ");
           Serial.println((int)cmd[1]);
           start_forward_test(cmd[1]);           
     }                      
     motor_state = STATE_GO;
     break; 
 
 case CMD_CHECK_AROUND:
     Serial.println("CMD_CHECK_AROUND"); 
     ret = check_around();
     // byte 0: direction
     resp[0] = ret;
     resp_len = 1;
     break; 

 case CMD_MOVE_TILT_PAN:
     Serial.print("CMD_MOVE_TILT_PAN, X Y: "); 
     Serial.print((int)cmd[1]);
     Serial.println((int)cmd[2]);
     TiltPan_move(cmd[1], cmd[2]);
     break; 
                    
 case CMD_TURN_RIGHT:
     if (motor_state == STATE_GO)
     { 
           Serial.print("CMD_TURN_RIGHT, alpha: ");
           Serial.println((int)cmd[1]);
           ret = turn ((double)cmd[1], 100);
           if (ret != SUCCESS){  Serial.print("CMD_TURN_RIGHT error"); Serial.print(ret);}
    }
    break;        

 case CMD_TURN_LEFT:
     if (motor_state == STATE_GO)
     { 
           Serial.print("CMD_TURN_LEFT, alpha: ");
           Serial.println((int)cmd[1]);
           ret = turn (-(double)cmd[1], 100);
           if (ret != SUCCESS){  Serial.print("CMD_TURN_LEFT error"); Serial.print(ret);}
     }
     break;        
     
 case CMD_INFOS:    
     Serial.println("CMD_INFOS");
     // byte 0: motor_state
     resp[0] = motor_state;
     // byte 1: SpeedMotorRight
     resp[1] = get_SpeedMotorRight();
     // byte 2: SpeedMotorLeft
     resp[2] = get_SpeedMotorLeft();
     // byte 3: nb go
     resp[3] = nb_go;
     // byte 4: nb obstacle
     resp[4] = nb_obstacle;
     // byte 5: direction
     //resp[5] = CMPS03.CMPS03_read();
          resp[5] = 0;

     // byte 6: distance
     resp[6] = GP2Y0A21YK_getDistanceCentimeter(GP2Y0A21YK_Pin);
     resp_len = 7;
     break; 

 case CMD_PICTURE: 
     Serial.print("CMD_PICTURE,no_picture: ");
     no_picture++;
     Serial.println(no_picture);
     ret = JPEGCamera.makePicture (no_picture);
     if (ret == SUCCESS)
     { 
           resp[0] = no_picture;
           // byte 1: picture number
           resp_len = 1;
     }
     else
     {
        Serial.print("makePicture error: ");
        Serial.println(ret);
     }
     break;
     
 default:
    Serial.println("invalid command");
    break;                     
 
 } //end switch
    
    *presp_len = resp_len;                 
    return ret;
}