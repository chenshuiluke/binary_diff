def PyTruncate(fileName,size):
	file = open(fileName,"r+b")
	file.truncate(size)
	file.flush()
	file.close()

