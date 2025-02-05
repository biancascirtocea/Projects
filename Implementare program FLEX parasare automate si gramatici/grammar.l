%{
    /* Scirtocea Bianca 331CB */
#include <string>
#include <vector>
#include <set>
#include <map>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

/* Structuri de date principale */

// Defineste o tranziție în automatul finit
struct Transition {
    string fromState;
    string symbol;   
    string toState;  
};

// Defineste structura unui automat finit complet
struct FiniteAutomaton {
    string name;                 
    set<string> alphabet;       
    set<string> states;         
    set<string> finalStates;    
    string initialState;        
    vector<Transition> transitions; 
};

// Defineste o producție în gramatică
struct Production {
    string left;    // partea stângă (non-terminal)
    string right;   // partea dreaptă (secvență de simboluri)
};

// Defineste structura unei gramatici complete
struct Grammar {
    string name;                         
    set<string> nonterminals;           
    set<string> alphabet;               
    string startSymbol;                 
    vector<pair<string, vector<string>>> productions; 
};

// Defineste o variabilă cu domeniu
struct Variable {
    string name;          
    set<string> domain;   
    bool isLocal;         
};

// Flag pentru prima gramatică procesată (formatare output)
static bool firstGrammar = true;         

// Variabile pentru procesarea producțiilor
string currentLeftSide;                  
string currentRightSide;                 

// Pointeri la structurile curente în procesare
Grammar* currentGrammar = nullptr;       
FiniteAutomaton* currentAutomaton = nullptr; 
Variable* currentVariable = nullptr;     
vector<Variable> globalVariables;        

// Flag-uri pentru contextul curent de procesare
bool insideGrammar = false;
bool insideAutomaton = false;

// Variabile temporare pentru procesarea tranzițiilor
string currentState;
string currentSymbol;

/* Declaratii anticipate ale funcțiilor principale */
void printGrammarInfo();
string determineGrammarType(const Grammar& g);
bool isDeterministic(const FiniteAutomaton& fa);
void printAutomatonInfo();
bool isLocalVariable(const string& symbol);

/* Configurare Flex */
%}

%option noyywrap
%option nounput
%option noinput

%x IN_GRAMMAR
%x IN_AUTOMATON
%x IN_NONTERMINALS
%x IN_ALPHABET_AUTOMATON
%x IN_ALPHABET_GRAMMAR
%x IN_STATES
%x IN_FINAL_STATES
%x IN_PRODUCTION
%x IN_TRANSITION
%x IN_VARIABLE_DOMAIN
%x IN_VARIABLE_LOCAL
%x READ_AUTOMATON_NAME
%x READ_VARIABLE_NAME
%x READ_GRAMMAR_NAME 
%x COMMENT

WS          [ \t\n\r]
IDENTIFIER  [a-zA-Z][a-zA-Z0-9_]*
NONTERMINAL [A-Z][0-9]*
TERMINAL    [a-z0-9]
STATE       q[0-9]+
EPSILON     e

%%

    /* Reguli pentru procesarea comentariilor în cod */
"%".*\n              /* Ignoră comentariile care încep cu % până la sfârșitul liniei */
"/*"                 { BEGIN(COMMENT); }  /* Începe procesarea comentariului multi-line */
<COMMENT>"*/"        { BEGIN(INITIAL); } /* Termină procesarea comentariului multi-line */
<COMMENT>.|\n        /* Ignoră orice caracter din interiorul comentariului */

    /* Definire variabila - formatul ::= {domain} */
"variable"{WS}+ { 
    BEGIN(READ_VARIABLE_NAME); 
}

    /* Citeste numele variabilei pana la ::= */
<READ_VARIABLE_NAME>{IDENTIFIER} {
    currentVariable = new Variable();
    currentVariable->name = yytext;
    BEGIN(IN_VARIABLE_DOMAIN);
}

<READ_VARIABLE_NAME>{WS}* /* ignora whitespace */
<READ_VARIABLE_NAME>"::=" { BEGIN(IN_VARIABLE_DOMAIN); }

    /* Domeniu variabilă - Formatul: {val1, val2, ..., valn} */
<IN_VARIABLE_DOMAIN>"{"{WS}* /* Ignora acolada si whitespace */
<IN_VARIABLE_DOMAIN>{TERMINAL} {
    if(currentVariable) currentVariable->domain.insert(yytext);
}
<IN_VARIABLE_DOMAIN>{WS}*[,;]{WS}* /* Ignora separatorii si whitespace-ul asociat */
<IN_VARIABLE_DOMAIN>{WS}*"}"{WS}* {
    /* Afișează informații despre variabila globală dacă nu suntem în contextul unei gramatici */
    if(currentVariable) {
        if(!insideGrammar) {
            cout << endl;
            cout << "Variabila globala:" << endl;
            cout << "    Identificator: " << currentVariable->name << endl;
            cout << "    Domeniu: {";
            bool first = true;
            for(const auto& elem : currentVariable->domain) {
                if(!first) cout << ", ";
                cout << elem;
                first = false;
            }
            cout << "}" << endl;  
        }
        delete currentVariable;
        currentVariable = nullptr;
    }
    BEGIN(INITIAL);
}

    /* Definire automat - Formatul: identifier ::= FiniteAutomaton(...) */
{IDENTIFIER}{WS}* { 
    currentAutomaton = new FiniteAutomaton();
    currentAutomaton->name = yytext;
    BEGIN(READ_AUTOMATON_NAME);
}

<READ_AUTOMATON_NAME>"::=" { /* continuă procesarea */ }
<READ_AUTOMATON_NAME>"FiniteAutomaton" { BEGIN(IN_AUTOMATON); }

    /* Stari - Formatul: states :: {q0, q1, ..., qn} */
<IN_AUTOMATON>"states"{WS}*"::"{WS}*"{" { BEGIN(IN_STATES); }
<IN_STATES>{STATE} { currentAutomaton->states.insert(yytext); }
<IN_STATES>{WS}*"}"{WS}* { BEGIN(IN_AUTOMATON); }
<IN_STATES>{WS}*[,;]{WS}* /* Procesează separatorii dintre stări */

    /* Stari finale - Formatul: final_states :: {q1, q2, ..., qn} */
<IN_AUTOMATON>"final_states"{WS}*"::"{WS}*"{" { BEGIN(IN_FINAL_STATES); }
<IN_FINAL_STATES>{STATE} { currentAutomaton->finalStates.insert(yytext); }
<IN_FINAL_STATES>{WS}*"}"{WS}* { BEGIN(IN_AUTOMATON); }
<IN_FINAL_STATES>{WS}*[,;]{WS}* /* Procesează separatorii */

    /* Stare initiala - Formatul: initial_state :: q0 */
<IN_AUTOMATON>"initial_state"{WS}*"::"{WS}*{STATE} {
    string temp = yytext;
    size_t pos = temp.find("::");
    currentAutomaton->initialState = temp.substr(pos + 2);
}

    /* Alfabet pentru automat */
<IN_AUTOMATON>"alphabet"{WS}*"::"{WS}*"{" { BEGIN(IN_ALPHABET_AUTOMATON); }
<IN_ALPHABET_AUTOMATON>{TERMINAL} { 
    if(currentAutomaton) {
        currentAutomaton->alphabet.insert(yytext);
    }
}
<IN_ALPHABET_AUTOMATON>{WS}*"}"{WS}* { BEGIN(IN_AUTOMATON); }
<IN_ALPHABET_AUTOMATON>{WS}*[,;]{WS}* /* Proceseaza separtorii */

    /* Variabilelor locale pentru automat */
<IN_AUTOMATON>"variable"{WS}+{IDENTIFIER}{WS}*"::=" {
    currentVariable = new Variable();
    string temp = yytext;

    // Salt peste cuvantul variable
    size_t start = temp.find_first_not_of(" \t\n\r", 8);
    size_t end = temp.find("::=");
    currentVariable->name = temp.substr(start, end-start);
    
    currentVariable->isLocal = true;  // Marchează ca variabilă locală
    BEGIN(IN_VARIABLE_LOCAL);
}

    /* Procesarea domeniului variabilelor locale */
<IN_VARIABLE_LOCAL>"{"{WS}* /* Procesează începutul domeniului */

<IN_VARIABLE_LOCAL>{TERMINAL} {
    if(currentVariable) currentVariable->domain.insert(yytext);
}
<IN_VARIABLE_LOCAL>{WS}*[,;]{WS}* /* Procesează separatorii */
<IN_VARIABLE_LOCAL>{WS}*"}"{WS}* {
    BEGIN(IN_AUTOMATON);  
}

    /* Tranzitii automat - Format: stare_curenta -> (simbol -> stare_destinatie) */
<IN_AUTOMATON>{STATE}{WS}*"->"{WS}*"(" {
    currentState = yytext;
    size_t arrowPos = currentState.find("->");
    currentState = currentState.substr(0, arrowPos);
    while(isspace(currentState.back()))
        currentState.pop_back();
    BEGIN(IN_TRANSITION);
}
    /* Procesarea simbolului de tranziție */
<IN_TRANSITION>{IDENTIFIER}|{TERMINAL}{WS}*"->" {
    string symbol = yytext;
    size_t arrowPos = symbol.find("->");
    symbol = symbol.substr(0, arrowPos);
    while(isspace(symbol.back()))
        symbol.pop_back();
    
    if(isLocalVariable(symbol)) {
        currentSymbol = "*VARIABLE*";
    } else {
        currentSymbol = symbol;
        currentAutomaton->alphabet.insert(symbol);
    }
}

    /* Procesarea stării destinație în tranziții */
<IN_TRANSITION>{STATE}{WS}*";" {
    string targetState = yytext;
    targetState.pop_back(); // Elimină caracterul ';'
    while(isspace(targetState.back()))
        targetState.pop_back();
    
    /* Cazul special când simbolul este o variabilă:
     * Expandăm variabila în toate valorile din domeniul ei
     * și creăm câte o tranziție pentru fiecare valoare
     */
    if(currentSymbol == "*VARIABLE*" && currentVariable) {
        for(const auto& sym : currentVariable->domain) {
            Transition t;
            t.fromState = currentState;
            t.symbol = sym;
            t.toState = targetState;
            currentAutomaton->transitions.push_back(t);
            currentAutomaton->alphabet.insert(sym);
        }
    } else {
        /* Cazul normal: o singură tranziție cu simbolul dat */
        Transition t;
        t.fromState = currentState;
        t.symbol = currentSymbol;
        t.toState = targetState;
        currentAutomaton->transitions.push_back(t);
    }
}

    /* Finalizarea unei tranziții */
<IN_TRANSITION>")" {
    BEGIN(IN_AUTOMATON);
}

    /* Finalizarea definiției automatului */
<IN_AUTOMATON>")"{WS}*";"{WS}*";"{WS}* {
    if(currentAutomaton) {
        printAutomatonInfo();
        delete currentAutomaton;
        currentAutomaton = nullptr;
    }
    // Resetam variabila curenta dupa printare
    if(currentVariable) {
        delete currentVariable;
        currentVariable = nullptr;
    }
    insideAutomaton = false;
    BEGIN(INITIAL);
}

    /* Definire gramatică - Format: identifier ::= Grammar(...) */
{IDENTIFIER}{WS}*"::="{WS}*"Grammar"{WS}*"(" {
    if(currentGrammar != nullptr) 
        delete currentGrammar;
    
    currentGrammar = new Grammar();
    currentGrammar->name = yytext;
    size_t endPos = currentGrammar->name.find("::=");
    currentGrammar->name = currentGrammar->name.substr(0, endPos);

    while(isspace(currentGrammar->name.back()))
        currentGrammar->name.pop_back();

    insideGrammar = true;
    BEGIN(IN_GRAMMAR);
}

    /* Neterminale */
<IN_GRAMMAR>"nonterminals"{WS}*"::"{WS}*"{" { BEGIN(IN_NONTERMINALS); }
<IN_NONTERMINALS>{NONTERMINAL} { currentGrammar->nonterminals.insert(yytext); }
<IN_NONTERMINALS>{WS}*"}"{WS}* { BEGIN(IN_GRAMMAR); }
<IN_NONTERMINALS>{WS}*[,;]{WS}* /* Ignoră separatorii și whitespace-ul asociat */

    /* Alfabet gramatica*/
<IN_GRAMMAR>"alphabet"{WS}*"::"{WS}*"{" { BEGIN(IN_ALPHABET_GRAMMAR); }
<IN_ALPHABET_GRAMMAR>{TERMINAL} { currentGrammar->alphabet.insert(yytext); }
<IN_ALPHABET_GRAMMAR>{WS}*"}"{WS}* { BEGIN(IN_GRAMMAR); }
<IN_ALPHABET_GRAMMAR>{WS}*[,;]{WS}* /* Ignoră separatorii și whitespace-ul asociat */

    /* Simbol start gramatica */
<IN_GRAMMAR>"start_symbol"{WS}*"::"{WS}*{NONTERMINAL} {
    string temp = yytext;
    size_t pos = temp.find("::");
    currentGrammar->startSymbol = temp.substr(pos + 2);
}

  /* Producții gramatica partea stanga */
<IN_GRAMMAR>({NONTERMINAL}|{TERMINAL})"&"({NONTERMINAL}|{TERMINAL})|{NONTERMINAL}{WS}*"->"{WS}* {
    currentLeftSide = string(yytext);
    // Dacă conține "->", păstrăm doar partea din stânga
    size_t arrowPos = currentLeftSide.find("->");

    // Daca gasim sageata
    if (arrowPos != string::npos) {
        currentLeftSide = currentLeftSide.substr(0, arrowPos);
        while(isspace(currentLeftSide.back()))
            currentLeftSide.pop_back();
    }
    currentRightSide = "";
    BEGIN(IN_PRODUCTION);
}

    /* Procesarea părții drepte a unei producții - Formatul: simbol simbol ... | simbol simbol ... | ... */
<IN_PRODUCTION>({NONTERMINAL}|{TERMINAL}|{EPSILON}) {
    /* Adaugă & ca separator între simboluri, doar dacă nu e primul simbol */
    if(!currentRightSide.empty()) 
        currentRightSide += "&";
    currentRightSide += yytext;
}

<IN_PRODUCTION>"&" /* Ignoră & pentru că îl adăugăm automat mai sus */

<IN_PRODUCTION>"|" {
    /* Convertește partea dreaptă într-un vector de string-uri */
    vector<string> rightParts;
    size_t pos = 0;
    string rightSide = currentRightSide;

    /* Separă string-ul la & și adaugă fiecare parte în vector */
    while ((pos = rightSide.find("&")) != string::npos) {
        rightParts.push_back(rightSide.substr(0, pos));
        rightSide.erase(0, pos + 1);
    }

    /* Adaugă și ultima parte dacă există */
    if (!rightSide.empty()) {
        rightParts.push_back(rightSide);
    }

    /* Adaugă producția în gramatică și resetează buffer-ul */
    currentGrammar->productions.push_back({currentLeftSide, rightParts});
    currentRightSide = "";
}

    /* Procesează ; care marchează sfârșitul unei producții complete */
<IN_PRODUCTION>";" {
    // Verifică dacă există conținut în currentRightSide înainte de a procesa
    if (!currentRightSide.empty()) {
        vector<string> rightParts;
        size_t pos = 0;
        string rightSide = currentRightSide;

        // Caută și procesează toate părțile separate prin '&' din partea dreaptă a producției
        while ((pos = rightSide.find("&")) != string::npos) {

            // Extrage subșirul până la '&' și îl adaugă în vectorul de părți
            rightParts.push_back(rightSide.substr(0, pos));

            // Elimină partea procesată și separatorul '&' din șirul original
            rightSide.erase(0, pos + 1);
        }

        // Verifică dacă a mai rămas ceva în rightSide după procesarea tuturor '&'
        // Acest caz apare pentru ultima parte care nu are un '&' după ea
        if (!rightSide.empty()) {
            // Adaugă ultima parte rămasă în vectorul de părți
            rightParts.push_back(rightSide);
        }

        // Verifică dacă s-au găsit părți valide pentru a crea o producție pentru a fi adaugata in gramatica
        if (!rightParts.empty()) {
            currentGrammar->productions.push_back({currentLeftSide, rightParts});
        }
        currentRightSide = "";
        BEGIN(IN_GRAMMAR);
    }
}

    /* Procesează încheierea definiției gramaticii */
<IN_GRAMMAR>")" {
    if(currentGrammar  && !currentGrammar->productions.empty()) {
        /* Adaugă linie goală între gramatici, exceptând prima */
        if(!firstGrammar) 
            cout << endl;
        
        /* Afișează informațiile despre gramatică și eliberează memoria */
        printGrammarInfo();
        delete currentGrammar;
        currentGrammar = nullptr;
    }

    /* Resetează starea la procesarea normală */
    insideGrammar = false;
    BEGIN(INITIAL);
}

{WS}+    /* Ignora spatiile multiple */ ;
<*>.|\n    /* Ignră orice caracter nerecunoscut în orice stare */ 

%%

    /* Verifica daca este sau nu variabila locala a automatului*/
bool isLocalVariable(const string& symbol) {
    if(currentVariable && currentVariable->name == symbol) {
        return true;
    }
    return false;
}

string determineGrammarType(const Grammar& g) {
   // Flaguri pentru fiecare tip de gramatică, pornind cu asumpția că sunt toate adevărate
   bool isGR = true; 
   bool isGDC = true;  
   bool isGIC = true;  
   
   // Parcurgem fiecare producție pentru a verifica restricțiile
   for(const auto& prod : g.productions) {
        const string& leftStr = prod.first;              // Partea stângă a producției
        const vector<string>& rightParts = prod.second;  // Partea dreaptă, împărțită în bucati
       
        // Verificăm întâi cazul special pentru producția vidă (S -> ε)
        // În GDC, doar simbolul de start poate produce ε
        bool isEpsilonRule = (rightParts.size() == 1 && rightParts[0] == "e");
        if(isEpsilonRule) {
            if(leftStr != g.startSymbol) {
               isGDC = false;  // Dacă alt simbol produce ε, nu e GDC
            }
            continue;  // Trecem la următoarea producție
        }

        // Verificări pentru Gramatică Regulată (GR)
        // Prima condiție: partea stângă trebuie să fie un singur neterminal si la GIC la fel
        if(g.nonterminals.find(leftStr) == g.nonterminals.end()) {
           isGR = false;
           isGIC = false;
        }

        // Pentru producțiile non-epsilon, verificăm formatul specific GR
        if(!isEpsilonRule) {
            if(rightParts.size() == 1) {
                // Cazul 1 pentru GR: A -> a (un singur terminal)
                if(g.alphabet.find(rightParts[0]) == g.alphabet.end()) {
                   isGR = false;
                }
            } else if(rightParts.size() == 2) {
                // Cazul 2 pentru GR: A -> aB (terminal urmat de neterminal)
                bool firstIsTerminal = g.alphabet.find(rightParts[0]) != g.alphabet.end();
                bool secondIsNonTerminal = g.nonterminals.find(rightParts[1]) != g.nonterminals.end();
                if(!firstIsTerminal || !secondIsNonTerminal) {
                   isGR = false;
                }
            } else {
                isGR = false;  // Mai mult de 2 simboluri
            }
        }

       // Verificare pentru GDC: |α| ≤ |β| (lungimea părții stângi ≤ lungimea părții drepte)
       // Mai întâi separăm partea stângă în tokeni individuali
       vector<string> leftTokens;
       size_t pos = 0;
       string temp = leftStr;
       while ((pos = temp.find("&")) != string::npos) {
           leftTokens.push_back(temp.substr(0, pos));
           temp.erase(0, pos + 1);
       }
       if (!temp.empty()) {
           leftTokens.push_back(temp);
       }

       // Verificăm condiția de lungime pentru GDC
       if(leftTokens.size() > rightParts.size() && !isEpsilonRule) {
           isGDC = false;
       }
   }

   // Returnăm tipul cel mai restrictiv care se potrivește
   if(isGR) return "GR";
   if(isGIC) return "GIC";          
   if(isGDC) return "GDC";                 
   return "GFR";                   
}

void printGrammarInfo() {
    if(!currentGrammar  || currentGrammar->productions.empty()) 
        return;

    cout << "Numele gramaticii: " << currentGrammar->name << endl;
    cout << "Tipul gramaticii: " << determineGrammarType(*currentGrammar) << endl;
    cout << "Numarul de reguli de productie: " << currentGrammar->productions.size() << endl;
    cout << "Alfabetul gramaticii: {";
    bool first = true;
    for(const auto& symbol : currentGrammar->alphabet) {
        if(!first) cout << ", ";
        cout << symbol;
        first = false;
    }
    cout << "}" << endl<<endl;

    firstGrammar = false;
}

bool isDeterministic(const FiniteAutomaton& fa) {
    /* Mapam fiecare pereche (stare, simbol) la mulțimea starilor destinatie
     * Pentru ca automatul sa fie determinist:
     * 1. Nu trebuie să existe tranziții epsilon
     * 2. Pentru fiecare pereche (stare, simbol), trebuie sa existe exact o stare destinatie */
    
    map<pair<string, string>, set<string>> transitionMap;
    
    // Construim un map de tranziții
    for(const auto& t : fa.transitions) {
        transitionMap[{t.fromState, t.symbol}].insert(t.toState);
    }
    
    // Verificăm dacă există tranziții nedeterministe
    for(const auto& transition : transitionMap) {
        if(transition.second.size() > 1 || (transition.first.second == "e")) {
            return false;
        }
    }
    
    return true;
}

void printAutomatonInfo() {
    if(!currentAutomaton) return;
    
    cout << "Numele automatului: " << currentAutomaton->name << endl;
    cout << "Tipul automatului: " << (isDeterministic(*currentAutomaton) ? "Automat finit determinist" : "Automat finit nedeterminist") << endl;
    
    // Filtrăm doar epsilon din alfabet
    set<string> printAlphabet = currentAutomaton->alphabet;
    printAlphabet.erase("e");

    cout << "Alfabetul de intrare: {";
    bool first = true;
    for(const auto& symbol : printAlphabet) {
        if(!first) cout << ", ";
        cout << symbol;
        first = false;
    }
    cout << "}" << endl;
    
    cout << "Numarul de stari: " << currentAutomaton->states.size() << endl;
    cout << "Starea initiala: " << currentAutomaton->initialState << endl;
    
    cout << "Starile finale: {";
    first = true;
    for(const auto& state : currentAutomaton->finalStates) {
        if(!first) cout << ", ";
        cout << state;
        first = false;
    }
    cout << "}" << endl;
    
    cout << "Functia de tranzitie:" << endl;
    for(const auto& t : currentAutomaton->transitions) {
        cout << "m(" << t.fromState << ", " << t.symbol << ") = " << t.toState << endl;
    }
    cout << endl;

    // Printam variabila locala gasita daca apartine automatului actual
    if(currentVariable && currentVariable->isLocal && insideAutomaton) {
        cout << "Variabila locala:" << endl;
        cout << "    Identificator: " << currentVariable->name << endl;
        cout << "    Domeniu: {";
        first = true;
        for(const auto& elem : currentVariable->domain) {
            if(!first) cout << ", ";
            cout << elem;
            first = false;
        }
        cout << "}" << endl << endl;
    }
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        return 1;
    }
    
    FILE* file = fopen(argv[1], "r");
    if(!file) {
        return 1;
    }
    
    yyin = file;
    yylex();
    
    fclose(file);
    return 0;
}