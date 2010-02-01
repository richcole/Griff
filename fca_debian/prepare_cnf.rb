fca_debian = ../fca_debian
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
`zcat *.gz | #{fca_debian}/packages_to_cnf`
`../../sarl3/dump_uninstallable_packages.exe packages.cnf packages.lex 1 30000 > check_installable.txt`
`grep Uninstallable check_installable.txt | sed -e 's/Checking \(.*\): num vars.*/\1/' | sort > uninstallable.txt`
`sed -e 's/\([^ ]*\) .*/\1/' reasons.txt | sort > simply_uninstallable.txt`
`comm -13 simply_uninstallable.txt uninstallable.txt > complex_uninstallable.txt`



