leave_out=$1

rm -f /tmp/diagrams.train
rm -f /tmp/diagrams.test
make svm_training

sed -e 's/-simple//' sans_basties.txt | sort > sans_basties_svm.txt
echo "$1-best_diagrams.train" > leave_out.txt

comm -23 sans_basties_svm.txt leave_out.txt > svm_training.txt
cat leave_out.txt > svm_testing.txt

cat `cat svm_training.txt` > /tmp/diagrams.train
cat `cat svm_testing.txt` > /tmp/diagrams.test

