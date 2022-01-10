const fs = require("fs");
const path = require("path");
const process = require("process");

const origPath = "C:\\Users\\Michael\\Downloads\\bullet3-3.21\\bullet3-3.21\\src";
const newPath = "C:\\Users\\Michael\\Downloads\\Redshift\\Vendor\\Include\\bullet";

const getFilesRecursive = (dirPath, arrayOfFiles) =>
{
	arrayOfFiles = arrayOfFiles || [];
	files = fs.readdirSync(dirPath);
	files.forEach((file) =>
	{
		if (fs.statSync(dirPath + "/" + file).isDirectory())
		{
			arrayOfFiles = getFilesRecursive(dirPath + "/" + file, arrayOfFiles);
		}
		else
		{
			arrayOfFiles.push(path.join(dirPath, "/", file));
		}
	});
	return arrayOfFiles
}
const getFiles = (dirPath, arrayOfFiles) =>
{
	arrayOfFiles = arrayOfFiles || [];
	files = fs.readdirSync(dirPath);
	files.forEach((file) =>
	{
		arrayOfFiles.push(file);
	});
	return arrayOfFiles;
}

const allFiles = getFilesRecursive(origPath);
const allFilesOrigDir = getFiles(origPath);

for (let i = 0; i < allFiles.length; ++i)
{
	const file = allFiles[i];
	const dir = path.dirname(file);
	const name = path.basename(file);
	const ext = path.extname(file);
	if (ext == ".h" || ext == ".c" || ext == ".hpp" || ext == ".cpp")
	{
		let data = fs.readFileSync(file, 'utf8');
		var regex = /(?<=(#include.?\")).+(?=(\"))/g
		var matches = [];
		var match = regex.exec(data);
		while (match != null)
		{
			matches.push(match[0]);
			match = regex.exec(data);
		}
		for (let j = 0; j < matches.length; ++j)
		{
			process.chdir(dir);
			const allFilesCurrDir = getFiles(dir);
			let relativePath = "";
			if (allFilesCurrDir.includes(matches[j].replace(dir, "")))
			{
				relativePath = path.resolve(dir).replace(origPath, "").replaceAll("\\", "/") + "/" + matches[j];
				data = data.replace("\"" + matches[j] + "\"", "<bullet" + relativePath + ">");
			}
			else if (allFilesOrigDir.includes(matches[j].substr(0, matches[j].indexOf("/"))))
			{
				data = data.replace("\"" + matches[j] + "\"", "<bullet/" + matches[j] + ">");
			}
			else
			{
				relativePath = path.resolve(matches[j]).replace(origPath, "").replaceAll("\\", "/");	
				data = data.replace("\"" + matches[j] + "\"", "<bullet" + relativePath + ">");
			}
		}
		if (name == "btPolyhedralConvexShape.h")
		{
			console.log(data);
		}
		const newDir = newPath + dir.replace(origPath, "") + "\\";
		const newFile = newPath + dir.replace(origPath, "") + "\\" + name;
		fs.mkdir(newDir, { recursive: true }, (err) => {});
		fs.writeFile(newFile, data, (err) => {});
	}
}