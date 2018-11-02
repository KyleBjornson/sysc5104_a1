[top]
components : gpsQueue@GpsQueue 
out : turnRequest 
in : gpsInstructionIn 
in : speedIn 
Link : gpsInstructionIn gpsInstructionIn@gpsQueue
Link : speedIn speedIn@gpsQueue
Link : turnRequest@gpsQueue turnRequest

[gpsQueue]
