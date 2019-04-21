# Remote Connected Health Care System
# Friend node Implementation

Individual Report : https://drive.google.com/drive/u/1/folders/1fS_gp4G-N2RsFurSeZW3kLyLtgJAmFri

Friend Node Implementation

Group Report:  https://drive.google.com/drive/u/1/folders/19hdArP41vl--8bYrzDXRaQdipgCvqJyU

# Project update 1 - April 13, 2019
1) Integrated friend node with sensors
2) Established friendship with a low power node and tested it with on off model

# Project update 2 - April 20, 2019
# Current staus of Checked in Friend Node 
  
    Completed Two way communication between FN and FALL_DETECTION_NODE along with alert system
    Completed unicast publishing from friend node to LPNs using level model
    Completed Alert system
  
  
1) Established friendship with all the three low power nodes
2) Gets data from Fall Detection low power node.
3) Triggers buzzer on Friend node if fall is detected or tap is detected on FALL_DETECTION_NODE
4) When PB1 is pressed,
   - Publishes data to FALL_DETECTION_NODE to stop the local buzzer ringing on FALL_DETECTION_NODE
   - Stops the buzzer ringing on Friend node
5) Tracks a count of the number of available doctors with the help of touch sensors


# Main tasks to be done this week
1) Try using two models in the friend node
2) OOB authentication
3) Two way Communication with two other LPN other than Fall Detection LPN



