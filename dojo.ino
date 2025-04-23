// numero di aggiornamenti dei led in una rotazione
#define aggiornamentiLed 60
// array dei valori di accensione dei led
int valoriLed[aggiornamentiLed] = {1552, 1560, 2032, 480, 0, 48, 2032, 1680, 1680, 48, 0, 112, 2032, 1216, 2032, 944, 16, 0, 0, 0, 448, 2032, 1584, 1584, 1008, 0, 48, 4080, 6144, 0, 992, 1584, 1584, 2032, 480, 0, 992, 1584, 1072, 2032, 48, 0, 0, 0, 64, 1008, 1592, 1560, 1584, 48, 0, 480, 1904, 1560, 1592, 1008, 224, 0, 48, 2032};

unsigned long tempoMassimoRotazione = 10000;
// tempo necessario per una rotazione completa
unsigned long tempoRotazione = tempoMassimoRotazione;
// tempo registrato nella rotazione precedente
unsigned long tempoRegistrato;

// frazione di rotazione compiuta dall'elica
int passoElica = 0;
// sfasamento iniziale per anticipare l'accensione dei led
int sfasamentoElica = 3;

bool statoMagnete;  // false = fuori dal campo del magnete, true = dentro il campo del magnete

// Schema pin (L = led, M = magnete)
// Registro B:
// bit    7   6   5   4   3   2   1   0
// pin    x   x   13  12  11  10  9   8
// uso    x   x   x   L11 L10 L9  L8  L7
// Registro C:
// bit    7   6   5   4   3   2   1   0
// pin    A7  A6  A5  A4  A3  A2  A1  A0
// uso    x   x   L16 L15 L14 L13 L12 M
// Registro D:
// bit    7   6   5   4   3   2   1   0
// pin    7   6   5   4   3   2   1   0
// uso    L6  L5  L4  L3  L2  L1  x   x

void setup() {
  // 0 = input, 1 = output
  DDRB = 0b00011111;
  DDRC = 0b00111110;
  DDRD = 0b11111100;
}

void loop() {
  if (controllaPassaggioMagnete()) {
    passoElica = 0;
    aggiornaTempoRotazione();
  }

  if (tempoRotazione >= tempoMassimoRotazione) {
    // non accendere i led se l'elica è ferma o si sta fermando
    impostaLed(0);
    return; 
  }

  if (passoElica < aggiornamentiLed) {
    int indice = (passoElica + sfasamentoElica) % aggiornamentiLed;
    impostaLed(valoriLed[indice]);

    float compensazioneCalcoli = 0.92;  // compensa il tempo impiegato a svolgere i calcoli
    unsigned long tempoAttesa = tempoRotazione / aggiornamentiLed * compensazioneCalcoli;
    delayMicroseconds(tempoAttesa);

    passoElica++;  // incrementa il passo dell'elica
  }
}

bool controllaPassaggioMagnete() {
  if (bit_is_set(PINC, 0)) {  // sensore nel campo del magnete
    statoMagnete = true;
  } else if (statoMagnete) {  // istante in cui il sensore sorpassa il magnete
    statoMagnete = false;
    return true;
  }
  return false;
}

void aggiornaTempoRotazione() {
  unsigned long tempoAttuale = micros();
  tempoRotazione = tempoAttuale - tempoRegistrato;
  tempoRegistrato = tempoAttuale;
}

void impostaLed(int bits) {
  // imposta 16 bit sui registri CCCCCBBBBBDDDDDD
  // i registri DDR hanno bit 1 in corrispondenza dei led
  PORTB = bits >> 6  & DDRB;
  PORTC = bits >> 10 & DDRC;
  PORTD = bits << 2  & DDRD;
}
