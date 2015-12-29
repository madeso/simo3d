function importfile()
	file = openfile("Select file to import", "All files|*.*")
	if file ~= "" then
		currentfile().importfile(file)
	end
end
