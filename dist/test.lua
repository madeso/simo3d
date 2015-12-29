function importfile()
	f = openfile("Select file to import", "All files|*.*")
	if f ~= "" then
		print(f)
		fileimport(f)
	end
end
