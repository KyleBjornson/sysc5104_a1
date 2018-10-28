[top]
components : steerDriver@SteerDriver 
out : servoDutyCylce 
in : wheelDirectionIn 
in : speedIn 
Link : wheelDirectionIn wheelDirectionIn@SteerDriver
Link : speedIn speedIn@SteerDriver
Link : servoDutyCycle@SteerDriver servoDutyCycle

[steerDriver]
