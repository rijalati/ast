

!cc = |> ^ CC %f^ gcc -c %f -o %o |>


function compile(source, output)
	tup.definerule{ inputs = {source}, command = !cc .. source .. ' -o ' .. output, outputs = {output} }
end
