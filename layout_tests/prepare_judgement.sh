sed -e 's/[^0-9]*-\([0-9]*\)\.diagram$/\1/' $1-best_diagrams.txt > $1-judgements.txt
xv $1-best_diagrams.png* &

