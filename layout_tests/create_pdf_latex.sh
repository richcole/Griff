#!/bin/sh


echo '\documentclass[landscape]{article}'
echo '\usepackage{graphicx}'
echo '\usepackage[left=2cm,top=1cm,right=2cm,bottom=1cm,nohead,nofoot]{geometry}'

echo '\begin{document}'

for pngfile in *.png; do

    echo "\centerline{$pngfile}" | sed -e 's/_/\\_/g'    
    echo "\centerline{\includegraphics[width=0.9 \textwidth]{$pngfile}}\newpage"

done

echo '\end{document}'