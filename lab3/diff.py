from sys import argv

def comparar_arquivos(arquivo1, arquivo2):
    with open(arquivo1, 'r') as f1, open(arquivo2, 'r') as f2:
        for linha1, linha2 in zip(f1, f2):
            if linha1 != linha2:
                return False
        return True
    
if comparar_arquivos(argv[1], argv[2]):
    None
else:
    print("As matrizes não são iguais.")