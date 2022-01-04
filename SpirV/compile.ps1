$cwd = $args[0]
$src = $args[1]
$dst = $args[2]

Get-ChildItem -Path $src -Filter *.vert -File -Name| ForEach-Object {
	$file = [System.IO.Path]::GetFileNameWithoutExtension($_)
	Invoke-Expression "$($cwd)glslangValidator.exe -V '$($src)$($file).vert' -o '$($dst)$($file).vert'"
}

Get-ChildItem -Path $src -Filter *.frag -File -Name| ForEach-Object {
	$file = [System.IO.Path]::GetFileNameWithoutExtension($_)
	Invoke-Expression "$($cwd)glslangValidator.exe -V '$($src)$($file).frag' -o '$($dst)$($file).frag'"
}

Get-ChildItem -Path $src -Filter *.comp -File -Name| ForEach-Object {
	$file = [System.IO.Path]::GetFileNameWithoutExtension($_)
	Invoke-Expression "$($cwd)glslangValidator.exe -V '$($src)$($file).comp' -o '$($dst)$($file).comp'"
}