#include <EEPROM.h>

void writeStringEEPROM(int endereco, String texto) {
  byte tamanho = texto.length();
  EEPROM.write(endereco, tamanho);

  for(int i = 0; i < tamanho; i++) {
    EEPROM.write(endereco + 1 + i, texto[i]);
  }
}

String readStringEEPROM(int endereco) {
  int tamanho = EEPROM.read(endereco);  
  char data[tamanho + 1];

  for(int i = 0; i < tamanho; i++) {
    data[i] = EEPROM.read(endereco + 1 + i);
  }
  data[tamanho] = '\0';

  return String(data);
}