function importfile() {
	local f = openfile("Select file to import", "All files|*.*")
	if(f ~= "") {
		print(f)
		fileimport(f)
		scalemesh(100)
	}
}

function scalemesh(s) {
	if( file().meshes.size() == 1 ) {
		local m = file().meshes[0]
		for(var vi=0; vi<m.vertices.size(); ++vi) {
			var old := m.vertices[vi]
			old.x *= s
			old.y *= s
			old.z *= s
		}
	}
}
