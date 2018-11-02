[top]
components : DirectionController
out : turnRequest servoDutyCycle
in : laneDetection gps speedIn

Link : gps gps@DirectionController
Link : speedIn speedIn@DirectionController 
Link : turnRequest@DirectionController turnRequest 
Link : laneDetection laneDetection@DirectionController 
Link : servoDutyCycle@DirectionController servoDutyCycle

[DirectionController]
components : gpsQueue@GpsQueue
components : steerDriver@SteerDriver 
out :  turnRequest servoDutyCycle
in : laneDetection gps speedIn
Link : gps@DirectionController gpsInstructionIn@gpsQueue
Link : speedIn@DirectionController speedIn@gpsQueue
Link : turnRequest@gpsQueue turnRequest@DirectionController 
Link : laneDetection@DirectionController wheelDirectionIn@steerDriver
Link : speedIn@DirectionController speedIn@steerDriver
Link : servoDutyCycle@SteerDriver servoDutyCycle@DirectionController
