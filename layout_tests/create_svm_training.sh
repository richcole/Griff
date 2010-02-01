rm -f /tmp/diagrams.train
rm -f /tmp/diagrams.test
make svm_training

sed -e 's/-simple//' sans_basties.txt | sort > sans_basties_svm.txt

ruby ../cass_browser/sample_lines.rb sans_basties_svm.txt 22 | \
  sort > sans_basties_svm_training.txt

comm -23 \
  sans_basties_svm.txt sans_basties_svm_training.txt > \
  sans_basties_svm_testing.txt

cat `cat sans_basties_svm_training.txt` > /tmp/diagrams.train
cat `cat sans_basties_svm_testing.txt` > /tmp/diagrams.test

