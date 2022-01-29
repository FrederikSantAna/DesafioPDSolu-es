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

void EEPROMConcatStr(int address, String value) {
int nL;
char readByte;
bool nullFound = false;
int strPos = 0;

   for (nL=0; nL < 1000; nL++)  {

      if (!nullFound) {
         readByte = EEPROM.read(address + nL);

         if (readByte == (char)0) {
            nullFound = true;
         }
      } 
      
      if (nullFound) {
         EEPROM.write(address + nL, value.charAt(strPos) );          
         strPos++;

         if ((strPos + 1) > value.length()) {
            EEPROM.write(address + nL + 1, (char)0 );
            break;
         }
      }
   }     
}
