# Introducere in securitate cibernetica - Tema 2

# Message-send:
  --> În cadrul acestui task, am analizat fișierul dat si am realizat ca rezolvarea are legatură cu un port UDP 18.
  --> Am identificat adresa IP și masca de rețea pentru interfața eth0 folosind comanda "ip a s" =>  10.128.21.6/24.
  --> Pentru a descoperi toate hosturile active din rețea, am folosit nmap cu flag-ul -sn (ping scan) "nmap -sn 10.128.21.0/24".
  --> Pentru fiecare IP descoperit, am încercat să trimit mesajul "buna" către portul UDP 18 cu comanda " echo "buna" | nc -u <ip_găsit> 18 ".
  --> Am continuat acest proces pentru fiecare IP din lista până când am primit flag-ul de la serverul corect.

# Ghost-in-the-shell:
  --> Dupa analia fisierului dat, am început prin monitorizarea traficului folosind tcpdum "tcpdump -i eth0 -n tcp".
  --> Pe urma am filtart pachetele pentru a le urmari numai pachetele TCP care au flag-ul SYN setat "tcpdump 'tcp[tcpflags] & tcp-syn != 0' -n &".
  --> Am observat ca exista un pattern de trafic pentru aceste tipuri de pachete pe portul 14956 si am folosit nc, pentru a primi un răspuns de la acel port.
  --> Am identificat pachete SYN si pe portul 13855 și am realizat că trebuie să fac port knocking.
  --> După ce am aplicat secvența corectă de porturi, am folosit o comanda care creeaza un loop infinit si care porneste continuu netcat listeners pe ambele porturi (când o conexiune se termină, pornește automat noi listeners) => " while true; do nc -l -p 14956 & nc -l -p 13855; done".
  --> Astfel , in final dupa ce am asteptat putin am gasit flagul.

# JWT-auth:
  --> Am scanat reteaua folosind comanda "nmap -p- -T4  10.128.4.6/24" si am vazut ca doua porturi sunt open - 5000 si 9090. Vom folosi la acest task portul 5000, air pe cel 9090 il vom retine pentru taskul urmator.
  --> Am început prin identificarea serverului și portului folosind nmap pentru un server web "nmap -p 5000 10.128.15.0/24".
  --> Am observat care este ip ul deschis pentru portul 5000 si am folosit webtunnel.sh "./webtunnel.sh 10.128.15.148 5000" pentru a accesa si analiza serverul local.
  --> Am deobfuscat static/main.js si am descoperit /profil si /login .
  --> Am descoperit app.py dupa ce am dat un post request invalid la sectiunea login si am primit o eroare ce mentiona app.py. Astfel, am accesat codul sursa "http://localhost:8080/app.py".
  --> Dupa ce am analizat codul am vazut ca este nevoie initial sa ne inregistram, astfel , am facut un request POST către /api/register cu toate datele conform conditiilor date.
  --> Am identificat in codul app.py secret_key-ul, functia de generare a tokenului si necesitatea de a seta isAdmin=true. 
  --> Am creat un script Python pentru a genera un token JWT modificat, setând isAdmin la true.
  --> Folosind token-ul generat, am făcut un request POST către /profile.
  --> Am primit flag-ul în răspunsul serverului.

# SQLI-cart (în progres):
  --> Stim de la exercitiul anterior portul 9090, astfel o sa apelam scriptul pe acest port "./webtunnel.sh 10.128.4.49 9090".
  --> Ne conectam in browser pe localhost:8080/shop, stim conform hintului ca trebuie sa facem sql injection.
  --> Daca dam f12, observam ca site ul are un endpoint vulnerabil la /add_to_cart/.
  --> Astfel, observa, daca dam comanda cu add_to_cart si un numar (http://localhost:8080/add_to_cart/1), spre exemplu 1, in cart (http://localhost:8080/cart) o sa avem produsul adaugat. 
  --> Vom da apoi o comanda pentru a afla numele tabelelor din baza de date, unde vedem ca avem un nume cu flagz (pe care il retinem) : flagzbsHAta.
  --> Dupa, observam ca orice comanda am da, cosul ramane acelasi, deci trebuie sa stergem modificarile facute, resetand cookie-ul, pentru a trece la urmatoarea etapa.
  --> Resetam cookie-ul, tot printr-o comanda de tip sql injection, apoi apasam F12 si cautam coockie-ul in application si il stergem.
  --> Apoi, vom trece la urmatoarea comanda cu care vom afla numele coloanei din tabela, iar in cart va aparea un text de tip CREATE TABLE flagzbsHAta (..), iar in paranteza observam ca avem o valoare, pe care o retinem: 5602.
  --> Vom sterge iar cookie-ul.
  --> Vom folosi valoarea sa accesam tabelul ce contine flagul, iar acesta va fi numele unuia dintre produsele afisate in cart.
  

--------------------------------------------------------------
--> Message-send a fost cel mai simplu exercitiu, dar care m-a ajutat la rezolvarea urmatorului.
--> Cel mai interesant task a fost JWT-auth pentru că a combinat multiple lucruri de securitate web și criptografie.
--> Cel mai dificil mi s-a parut ca a fost ultimul task, am avut nevoie sa ma documentez destul de mult in sql injection, pentru a putea parcurge toti pasii pana sa ma prind tot ce trebuia sa fac.
