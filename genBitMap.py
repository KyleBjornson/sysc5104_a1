import sys
if len(sys.argv) < 3:
	print("Please use inputs in this format:\n")
	print("    -eSpeed <speed> <distance>")
	print("    -dSpeed <bitMappedSpeed>")
	print("    -gSign <type> <value in km/hr> <distance>")
	print("          note: For value, use 0 if sign is not speed.")
	print("                For the sign, use strings none, speed, yield, stop.")
	print("    -dSign <bitMappedSign>")
else:
	if sys.argv[1] == "-gSpeed":
		speed = (int) (sys.argv[2])
		distance = (int) (sys.argv[3])
		speedBitMap = (speed & 0xFF) << 24
		distanceBitMap = distance & 0x00FFFFFF
		x = distanceBitMap + speedBitMap
		print(x)	
	elif sys.argv[1] == "-dSpeed":
		x = (int) (sys.argv[2])
		speed = (x & 0xFF000000) >> 24
		print(speed)
		distance = x & 0x00FFFFFF
		print("speed: ", speed)
		print("distance: ", distance)
	elif sys.argv[1] == "-gSign":
		typeSign = sys.argv[2]
		value = ((int) (sys.argv[3]))/5
		distance = (int) (sys.argv[4])
		if typeSign == "none":
			x = 0
		elif typeSign == "stop":
			x = 1 << 30
		elif typeSign == "yield":
			x = 2 << 30
		elif typeSign == "speed":
			x = 3 << 30
		x += (value & 0x1F) << 25
		x += distance & 0x01FF
		print(x)
	elif sys.argv[1] == "-dSign":
		x = (int) (sys.argv[2])
		typeSign = (x & 0xC0000000) >> 30
		value = ((x & 0x3E000000) >> 25)*5
		distance = x & 0x01FF
		if typeSign == 0 :
			ts = "none"
		elif typeSign == 1:
			ts = "stop"
		elif typeSign == 2:
			ts = "yield"
		elif typeSign == 3:
			ts = "speed"
		print("typeSign: ", ts)
		print("value: ", value)
		print("distance: ", distance)
		