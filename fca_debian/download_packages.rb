reps       = [ "http://ftp.de.debian.org/debian unstable main contrib non-free"]

def download_sources(reps, arch)
  for x in reps do
    xs = x.split(" ")
    url_base = xs[0]
    dist = xs[1]
    for dir in xs[2..-1] do
      `wget -q #{url_base}/dists/#{dist}/#{dir}/binary-#{arch}/Packages.gz -O #{dist}-#{dir}-binary-#{arch}-Packages.gz`
    end
  end
end

download_sources(reps, "i386")



