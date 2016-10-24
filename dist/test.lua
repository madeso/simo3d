function importfile()
	local f = openfile("Select file to import", "All files|*.*")
	if f ~= "" then
		print(f)
		fileimport(f)
		scalemesh(100)
	end
end

function debug()
  for _, m in ipairs(file().meshes) do
    print(m.name)
  end
end

function scalemesh(s)
  for _, m in ipairs(file().meshes) do
    for _, old in ipairs(m.vertices) do
      old.x = old.x * s
      old.y = old.y * s
      old.z = old.z * s
    end
  end
end

function render_face_and_normals()
  render.faces_shaded()
  -- render.normals()
  -- render.edges(Rgba.Black(), 5)
  -- render.edges(Rgba.White(), 1)
  -- render.points(Rgba.new(1,0,0), 10)
end

render_func = render_face_and_normals
