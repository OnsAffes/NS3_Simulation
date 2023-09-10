set terminal pdf
set output "nb_des_paquets.pdf"
set title "Variation du débit en fct du nombre maximal des packet"
set xlabel "Nombre des paquets"
set ylabel "Débit en Mbps"
plot "mydata.txt" using 1:2 with lines title "débit/NbrPACK"
