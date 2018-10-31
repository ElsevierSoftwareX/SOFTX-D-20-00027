#! /usr/bin/env ruby

PROGNAME      = "Celltracker"
PROGNAME_LONG = "Celltracker – A curation tool for object tracks."
YEAR="2018"
AUTHORS="TU Dresden"

CLEAN_AUTHOR_MAP = {}
CLEAN_AUTHOR_MAP["Enrico"]="Enrico Uhlig"
CLEAN_AUTHOR_MAP["Konstantin"]="Konstantin Thierbach"
CLEAN_AUTHOR_MAP["Konstantin Thierbach"]="Konstantin Thierbach"
CLEAN_AUTHOR_MAP["Sebastian Wagner"]="Sebastian Wagner"

LICENSE_URL="https://www.gnu.org/licenses/lgpl-3.0.txt"
LICENSE_FILE="COPYING"

def getLicense(prog, proglong, year, author)
	lic = "#{proglong}
Copyright (C) #{year} #{author}

#{prog} is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

#{prog} is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with #{prog}.  If not, see <https://www.gnu.org/licenses/>.
"
	return lic

end

COMMENT = {
	".cpp" => {:begin => "/*",   :line => " * ", :end => " */"},
	".h"   => {:begin => "/*",   :line => " * ", :end => " */"},
	".hpp" => {:begin => "/*",   :line => " * ", :end => " */"},
	".svg" => {:begin => "<!--", :line => "",    :end => "-->"},
	".pro" => {:begin => "#",    :line => "# ",  :end => "#"},
	".qml" => {:begin => "/*",   :line => " * ", :end => " */"},
	".md"  => nil,
	".pri" => nil,
	".qrc" => nil,
	".rb"  => nil,
	".sh"  => nil,
	".user" => nil
}

def commentBlock(block, type)
	return block if COMMENT[type].nil?
	out = ""
	begeqend = COMMENT[type][:begin] == COMMENT[type][:end]
	out += COMMENT[type][:begin] + "\n" if not begeqend
	block.each_line {|line|
		out += COMMENT[type][:line] + line
	}
	out += COMMENT[type][:end] + "\n" if not begeqend
	return out
end

def authorsForFile(file)
	authors = `git log --pretty=format:'%an' "#{file}" | sort -u`
	authors = authors.split("\n").map{|x| CLEAN_AUTHOR_MAP[x]}.uniq
	return authors
end

def extForFile(file)
	return File.extname(file)
end

def commentForFile(file)
	ext = extForFile(file)
	return "" if ext.empty? or COMMENT[ext].nil?
	authors = authorsForFile(file).join(", ")
	years = "9999"
	block = getLicense(PROGNAME, PROGNAME_LONG, years, authors)
	return commentBlock(block, ext)
end

def commentFile(file)
	content = File.read(file)
	out = ""
	STDERR.write "Processing #{file}\n"
	return "" if content.empty?
	skip_first = content.lines[0].start_with? "#!"

	comment = commentForFile(file)

	if skip_first
		out += content.lines[0]
		out += comment
		out += content.lines[1..-1].join("")
	else
		out += comment
		out += content
	end

	return out
end

puts commentFile(ARGV[0])

