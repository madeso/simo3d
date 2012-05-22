import simocore

def importfile():
	file = simocore.openfile("Select file to import", "All files|*.*")
	if file != "":
		simocore.currentfile().importfile(file)