# Scirtocea Bianca 331CB
# TEMA FLEX - LFA

Codul începe prin definirea structurilor de bază - una pentru tranziții în automate (cu stare sursă, simbol și stare destinație), una pentru automate complete (cu nume, alfabet, stări, etc) și una pentru gramatici (cu nume, neterminale, alfabet și producții).
    
În partea de procesare, codul folosește stări diferite pentru a parcurge input-ul. Când întâlnește cuvântul cheie "variable", începe să proceseze o variabilă (care poate fi globală sau locală unui automat) și îi citește numele și domeniul de valori. Când dă peste "FiniteAutomaton", știe că urmează definirea unui automat și începe să-i citească componentele - stările (care încep toate cu 'q'), stările finale, starea inițială și alfabetul. Pentru fiecare tranziție din automat (de forma q0 -> (a -> q1)), codul extrage starea sursă, simbolul și starea destinație și le salvează. Un caz special e când simbolul e de fapt o variabilă - atunci expandează variabila și creează câte o tranziție pentru fiecare valoare din domeniul ei.

Pentru gramatici, procesul e similar. Când întâlnește "Grammar", începe să citească neterminalele (litere mari), alfabetul (litere mici și cifre) și simbolul de start. Partea mai complexă e la citirea producțiilor, unde trebuie să separe partea stângă de partea dreaptă și să gestioneze cazul cu '|' care reprezintă producții alternative. După ce citește toate componentele, codul determină tipul gramaticii verificând diverse condiții - de exemplu, pentru o gramatică regulată verifică să aibă un singur neterminal în stânga și maxim două simboluri în dreapta.

La final, codul afișează informațiile despre structurile citite.

## Probleme întâlnite și soluții

1. **Procesarea variabilelor și domeniilor**
Una dintre primele provocări a fost extragerea corectă a numelor variabilelor din input, deoarece acestea puteau fi atât globale cât și locale pentru automate. A trebuit să implementez un sistem de stări pentru a procesa pas cu pas declarațiile și pentru a face diferența între tipurile de variabile. Pentru rezolvare, am folosit flag-ul isLocal și stări intermediare care au permis o procesare corectă.

2. **Parsarea producțiilor gramaticilor**
Procesarea producțiilor gramaticilor s-a dovedit complexă din cauza necesității de a separa corect partea stângă de partea dreaptă. Am întâmpinat dificultăți în special la tratarea producțiilor vide (ε). Soluția a venit sub forma unui sistem de buffering care permite construirea treptata a prodcțiilor și procesarea separată a fiecărei componente.

3. **Determinarea tipului de gramatică**
Identificarea tipului corect de gramatică a necesitat implementarea unor verificări complexe și tratarea mai multor cazuri speciale. Am implementat un sistem de flags care marchează încălcarea restricțiilor specifice fiecărui tip de gramatică.

4. **Procesarea tranzițiilor automatului**
Parsarea tranzițiilor a fost complicată de formatul complex și de necesitatea de a expanda variabilele în tranziții multiple. Pentru verificarea determinismului, a trebuit să implementez un sistem de mapare care să țină cont de toate tranzițiile posibile dintr-o stare. Soluția a implicat crearea mai multor stări intermediare pentru procesarea componentelor.

5. **Gestionarea whitespace-ului**
Am întâmpinat probleme cu eliminarea whitespace-ului nedorit din tokens și cu păstrarea unei formatări corecte la output. A fost necesară implementarea unor reguli specifice pentru procesarea separatorilor și curățarea explicită a whitespace-ului din tokens. Acest lucru a asigurat un output curat, fara spatii nedorite.

## Platformă și Tehnologii
- Sistem de operare: Linux
- Compilator: g++