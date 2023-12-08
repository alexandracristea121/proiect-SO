#!/bin/bash
# Verifica daca script-ul primeste un singur argument
if [ "$#" -ne 1 ]; then
    echo "Utilizare: $0 <c>"
    exit 1
fi
 
# Caracterul primit ca argument
character=$1
 
# Initializare contor
counter=0
 
# Citeste linii pana la sfarsitul fisierului (CTRL+D)
while IFS= read -r line; do
    # Verifica daca linia incepe cu o litera mare
    if [[ $line =~ ^[[:upper:]] ]]; then
        # Verifica daca linia respecta conditiile date
        if [[ $line =~ ^[[:upper:][:lower:][:digit:][:space:],.!?]+[.!?]$ && ! $line =~ ,\s*si ]]; 
        then
            counter=$((counter + 1))
        fi
    fi
done
 
# Afiseaza rezultatul
echo $counter
