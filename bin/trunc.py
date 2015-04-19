def PyTruncate(fileName,size):
	import os
	diskSize = os.path.getsize(fileName)
	file = open(fileName,"r+b")
	while(diskSize > size): 
		print("Current File Size: " + str(diskSize))
		print("Desired File Size: " + str(size))
		file.truncate(size)
		file.flush()
		diskSize = os.path.getsize(fileName)
	file.close()

