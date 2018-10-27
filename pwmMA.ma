[top]
components : pwm@Pwm 
out : pwmStateOut 
in : dutyCycleIn 
Link : dutyCycleIn dutyCycleIn@pwm
Link : pwmStateOut@pwm pwmStateOut

[pwm]
period : 10

