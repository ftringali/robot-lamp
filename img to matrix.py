import math

path = "dojo.bmp" # percorso dell'immagine
res = 512 # risoluzione dell'immagine (quadrata)
sogliaBianco = 127 * 3 # 127 su 255 per 3 colori

matrice = [] # matrice immagine
riga = [] # riga della matrice
pixel = 0 # conta i pixel letti

img = open(path, "rb") # apri file immagine
img.seek(54) # salta l'header BMP

while True:
    strR = img.read(1) # leggi 1 byte (rosso)
    if len(strR) == 0: break # se è vuoto fermati
    strG = img.read(1) # leggi 1 byte (verde)
    strB = img.read(1) # leggi 1 byte (blu)
    
    # converti da stringa a int
    r = ord(strR) 
    g = ord(strG)
    b = ord(strB)

    # se la somma dei valori rgb supera la soglia il pixel sarà bianco, altrimenti nero
    riga.append(1 if r + b + g > sogliaBianco else 0)
    
    pixel += 1
    # aggiungi riga alla matrice quando necessario
    if pixel == res:
        matrice.insert(0, riga)
        riga = []
        pixel = 0

img.close() # chiudi file immagine

# mostra immagine
#for riga in matrice:
#    for bit in riga:
#        print('■ ' if bit == 0 else '· ', end = '')
#    print()

# effettua una interpolazione lineare tra i valori a e b
def interpolazioneLineare(a, b, t):
    return a + t * (b - a)

def interpolazioneBilineare(ox, oy, ex, ey, t):
    x = interpolazioneLineare(ox, ex, t)
    y = interpolazioneLineare(oy, ey, t)
    u1 = int(x)
    v1 = int(y)
    deltaU = x - u1
    deltaV = y - v1
    u2 = u1 + 1 if deltaU > 0 else u1
    v2 = v1 + 1 if deltaV > 0 else v1
    c1 = interpolazioneLineare(matrice[v1][u1], matrice[v1][u2], deltaU)
    c2 = interpolazioneLineare(matrice[v2][u1], matrice[v2][u2], deltaU)
    return round(interpolazioneLineare(c1, c2, deltaV))

def vicinoProssimo(ox, oy, ex, ey, t):
    u = round(interpolazioneLineare(ox, ex, t))
    v = round(interpolazioneLineare(oy, ey, t))
    return matrice[v][u]

# mappa i led sulla matrice
def calcolaValoriLed(passo, aggiornamenti):
    valoriLed = 0
    zonaMorta = 0.25 # sfasamento dovuto alla zona non coperta da led 
    
    ox = oy = (res - 1) / 2.0 # coordinate x, y dell'origine
    # coordinate x, y dell'estremo in funzione dell'angolo
    incrementoAngolare = 2 * math.pi / aggiornamenti
    angoloElica = incrementoAngolare * passo
    ex = ox * (1 + math.sin(angoloElica))
    ey = res - 1 - oy * (1 + math.cos(angoloElica))
    # mappatura dei valori lungo la linea che collega l'origine con l'estremo
    for i in range(16):
        t = (i + 1) / 16 * (1 - zonaMorta) + zonaMorta
        valore = interpolazioneBilineare(ox, oy, ex, ey, t)
        valoriLed |= valore << i
    return valoriLed

# stampa un array per arduino
print("int valoriLed[aggiornamentiLed] = {", end = '')
aggiornamentiLed = 60 # numero di aggiornamenti dei led in una rotazione
for i in range(aggiornamentiLed):
    print(calcolaValoriLed(i, aggiornamentiLed), end = '')
    if i < aggiornamentiLed - 1:
        print(', ', end = '')
print("};")