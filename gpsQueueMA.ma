[top]
components : gpsQueue@GpsQueue 
out : turnRequest 
in : gpsInstructionIn 
in : speedIn 
Link : gpsInstructionIn gpsInstructionIn@GpsQueue
Link : speedIn speedIn@GpsQueue
Link : turnRequest@GpsQueue turnRequest

[gpsQueue]
