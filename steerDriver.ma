[top]
components : steerDriver@SteerDriver 
out : servoDutyCycle 
in : wheelDirectionIn 
in : speedIn 
Link : wheelDirectionIn wheelDirectionIn@SteerDriver
Link : speedIn speedIn@SteerDriver
Link : servoDutyCycle@SteerDriver servoDutyCycle

[steerDriver]
