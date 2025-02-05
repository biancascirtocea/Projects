# Introducere in securitate cibernetica - Tema 1

# Binary-Database:
  --> In cadrul acestui task, am creat un fișier in python overflow.py care face buffer overflow, pentru a găsi flagul.
  --> Initial, am încărcat proiectul in ghidra, pentru a găsi vreun hint in server legat de task. Astfel, avem vazut ca eu trebuie sa creez un payload pe care sa il pun pe stiva pana ajung la eip (instrucțiunea curenta).
  --> Astfel, de la adresa de la stack preluam mărimea payload-ului pe care trebuie sa il adăugăm in stiva.
  --> Acum, unde avem eip-ul punem db_first_flag, pentru a determina apelarea funcției (astfel adăugăm valoarea lui db_first_flag aflata din gdb, prin comanda info address db_first_flag, scrisa in format little-endian). Iar, după ii adăugăm paddingul specific de 4, pentru a adaugă si argumentele funcției (care se afla la eip + 4).
  --> Acum am ajuns la parametrii funcției, astfel ii dam valoarea lui global_magix tot in format little-endien, preluata din ghidra.
  --> Pana sa trimitem payloadul explicat mai sus, trebuie sa trimitem o valoare negativa pentru a trece de un if din process_loop (0x41 < len). Astfel, din lungimea payload-ului crat scădem 256, numar specific tipului char.  
  --> După, trimitem payload-ul nostru si după trimitem un zero, pentru a trece si de if-ul care verifica len==0. 
  --> Astfel, funcția se termină, ajunge la return cu tot cu suprascrierea noastră pentru db_first_flag. In final, printam rsapunsul primit de la server decodificat.


# Unexpected CEO:
  --> Pe openstack, am modificat permisiunile fișierului id_rsa, pentru a putea utiliza cheia salvata acolo asociata serverului găsit in id_rsa.pub la final, si anume hacker@isc2024.1337.cx .
  --> M-am conectat prin ssh cu comanda: "ssh -i id_rsa hacker@isc2024.1337.cx".
  --> Am verificat toate fișierele din acest director si am găsit hintul legat de formatul de tip "Uppercase + Lowercase + Digit + Digit" si am facut legatura cu pasul următor de la cuvântul crunch.
  --> Am verificat ce se afla in /etc/shadow cu comanda cat, observând ca numai un user ("s3xy_assistant") avea hash ul parolei vizibil (salvându-l într-un fișier pentru mai târziu).
  --> Pe urma, am generat o mulțime de parole conform hintului cu comanda: "crunch 4 4 -t ,@%% -o password_list.txt".
  --> I-am aplicat acestei liste de parole John the Ripper cu hashu-ul găsit anterior, comanda "john --format=crypt --wordlist=password_list.txt ssh.txt" ("john --format=crypt --wordlist=<my_wordlist> <hash_file.txt>" - structura comenzii). Am descoperit parola Pa08.
  --> M-am conectat pe s3xy_assistant si am găsit mesajul text, astfel am înțeles ca trebuie sa fac ceva din poziția lui l33t_ceo cu generate_report.sh
  --> Am trecut pe l33t_ceo si am rulat scriptul pentru el astfel: "sudo -u l33t_ceo ./generate_report.sh".
  --> Acum, am folosit o metoda numita command injection si am reușit prin mai multe comenzi de tipul "`comanda`" sa descopăr ca in directorul .pleasedont se afla un fișier xxflag ce avea flagul.
Outputul comenzilor utilizate in acest punct a fost redirectat in "/tmp/output.txt", pentru a avea acces la ele. 
Ex: "`ls -la /home/l33t_ceo > /tmp/output.txt`" sau "`cat /home/l33t_ceo/.pleasedont/xxflag > /tmp/flag.txt`".

# LCG - OTP:
   --> In cadrul acestui task, am creat un fișier python decrypt.py care implementează un sistem de decriptare (procesul invers utilizat in marios.py) pentru un text criptat folosind un generator congruential liniar (LCG) (funcțiile XOR si LCG sunt preluate din marios.py). 
   --> Programul încearcă să spargă cifrarea prin forță brută, testând diferite valori pentru parametrii 'a' și 'b' ai LCG-ului (între 1337 și 10000), folosind procesare paralelă pentru eficiență. 
   --> Stim ca textul criptat este inițial codificat în Base64, astfel il decriptam. După, este decriptat folosind operația XOR între textul criptat și o cheie generată de LCG. 
   --> Pentru a verifica daca am găsit parametrii buni pentru LCG si am decriptat corect mesajul, programul caută în textul decriptat secvența "SpeishFlag", astfel, afișează valorile parametrilor corecți găsiți și mesajul decriptat.
   --> Pentru a optimiza procesul de căutare, codul utilizează biblioteca multiprocessing pentru a distribui calculele pe mai multe nuclee CPU, împărțind intervalul de căutare în funcție de numărul de procesoare disponibile.


--------------------------------------------------------------
--> Cel mai mult mi-a placut Unexpected_CEO, pentru ca a fost un fel de treasure hunt cu indicii pe care le primeai pe parcurs. 
--> Cel mai greut mi s-a parut Binary-database mai mult pentru ca mi-a luat pana m-am obisnuit cu reutilizarea lui ghidra, si totusi tot am folosit si gdb-ul...
--> Iar, legat de LCG, el a fost primul task facut, a durat putin pana m-am prins de paralelilzarea programului pentru a nu mai sta atat de mult si sa-mi dau seama cum functioneaza exact LCG-ul. In schimb, procesul invers din marios nu a fost greu de implementat, pentru decodificare.