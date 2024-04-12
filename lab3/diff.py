def calcular_media(resultado):
    pass

def comparar_arquivos(arquivo1, arquivo2):
    with open(arquivo1, 'r') as f1, open(arquivo2, 'r') as f2:
        for linha1, linha2 in zip(f1, f2):
            if linha1 != linha2:
                return False
        return True
    
if '__name__' == '__main__':
    if comparar_arquivos('output', 'output_sequential'):
        print('Resultado:', None)
    else:
        print("As matrizes não são iguais.")