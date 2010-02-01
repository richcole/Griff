require 'ds'

def write_stats_as_ds_filename(filename, stats)
  File.open(filename, "w") { |file| 
    write_stats_as_ds_file(file, stats) 
  }
end

def write_stats_as_ds_file(file, stats)
  for f, v in stats.fields do
    file.puts f.to_s + "=" + v.to_s
  end
end

def write_stats_as_ds_xml(file, filename, stats)
  file.write "<" + filename + ">"
  write_stats_as_ds_file(file, stats)
  file.write "</" + filename + ">"
end
