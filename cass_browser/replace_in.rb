
ARGV.each { |filename|
  content = File.read(filename)
  content.gsub!('clarrif','clarrif')
  File.open(filename, "w") { |file| file.write(content) }
}
