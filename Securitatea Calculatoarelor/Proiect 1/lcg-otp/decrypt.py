import base64
import multiprocessing  # Bibliotecă pentru procesare paralelă
from itertools import product
import time
import subprocess  # Permite rularea altor comenzi/scripturi

# Funcție pentru aplicația operației XOR între două șiruri de bytes
def xor(a, b):
    return bytes([x ^ y for x, y in zip(a, b)])

# Clasă pentru implementarea generatorului congruențial liniar (LCG)
class LCG:
    def __init__(self, a, b):
        # Inițializează generatorul cu parametrii a și b
        self.a = a
        self.b = b
        self.mod = 2 ** 16  # Modulul LCG este fixat la 2^16
        self.state = 0  # Starea inițială este 0

    def next(self):
        # Generează următorul număr pseudo-aleator folosind formula LCG
        self.state = (self.a * self.state + self.b) % self.mod
        return self.state

# Funcție care încearcă să decripteze textul pentru o gamă de parametri a și un b fix
def try_parameters(args):
    start_a, end_a, b, ciphertext = args
    for a in range(start_a, end_a):
        lcg = LCG(a, b) 
        states = []
        # Generăm secvența de stări ale LCG pentru cheia necesară
        while len(states) * 2 < len(ciphertext): # Generăm suficiente stări pentru a genera cheia
            states.append(lcg.next())
        # Convertim secvența în bytes pentru a crea cheia
        key = b"".join([state.to_bytes(2, "little") for state in states])
        # Aplicăm XOR pentru a decripta textul
        decrypted = xor(ciphertext, key)
        # Verificăm dacă textul decriptat începe cu o secvență cunoscută
        if b"SpeishFlag" in decrypted:  # Caută flag-ul oriunde în textul decriptat
            return (a, b, decrypted)
    return None  # Dacă nu găsim nimic, returnăm None

def main():
    # Extragem textul criptat returnat de script
    encoded = "mnDYc0662LPoGYhVxNkD44l08LTjImKyIei6vdgJiS49wu3TsxrSmVFoqhtoUzltKjUS9SebOCuYSdyo+aSnfld3P00TMsh/uXrXjZ94VIDZ3C7AmckeGvQD4X9Cke7UxA=="
    # Decodăm textul criptat din format Base64 în bytes
    ciphertext = base64.b64decode(encoded)

    # Obținem numărul de nuclee CPU disponibile pentru procesare paralelă
    num_processes = multiprocessing.cpu_count()
    # Calculăm dimensiunea fiecărui interval pentru a
    range_size = (10000 - 1337) // num_processes
    start_time = time.time()  # Pornim cronometrul pentru a măsura timpul total

    print(f"Starting search with {num_processes} processes...") 

    # Creăm un pool de procese pentru paralelizare
    with multiprocessing.Pool(processes=num_processes) as pool:
        # Iterăm peste toate valorile posibile ale lui b
        for b in range(1337, 10001):
            # Generăm sarcinile pentru procese, împărțind intervalele de a
            tasks = []
            for i in range(num_processes):
                start_a = 1337 + i * range_size  # Începutul intervalului pentru procesul i
                if i < num_processes - 1:
                    end_a = start_a + range_size  # Sfârșitul intervalului pentru procesele care nu sunt ultimul
                else:
                    end_a = 10001  # Ultimul proces include tot intervalul rămas până la 10001
                # Adăugăm sarcina în lista de sarcini
                tasks.append((start_a, end_a, b, ciphertext))

            # Rulăm toate sarcinile în paralel
            results = pool.map(try_parameters, tasks)
            
            # Verificăm rezultatele returnate de procese
            for result in results:
                if result is not None:  # Dacă un proces a găsit o soluție
                    a, b, decrypted = result  # Extragem parametrii și textul decriptat
                    elapsed_time = time.time() - start_time  # Calculăm timpul total
                    # Afișăm rezultatele
                    print(f"\nFound parameters in {elapsed_time:.2f} seconds!")
                    print(f"a = {a}, b = {b}")
                    print(f"Decrypted message:\n{decrypted.decode('ASCII')}")
                    return

            # Afișăm progresul la fiecare 100 de valori ale lui b
            if b % 100 == 0:
                print(f"Trying b={b}...", end='\r')

    print("\nNo solution found!")  

if __name__ == "__main__":
    main()  


