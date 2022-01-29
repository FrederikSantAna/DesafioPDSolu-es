#include <EEPROM.h>
#include "EEPROMString.h"
enum {
  STATE_MENU, //0
  STATE_1,    //1
  STATE_2,    //2
  STATE_3,    //3
  STATE_4,    //4
  STATE_5     //5
};

#define Porta1 2
#define Porta2 3
#define Button1 4
#define Button2 5

struct Usuario {
  String nome, senha, administrador;
};

Usuario usuario[10] = {};
String eventos[10];
char state;
int i, cadastroAtual = 0, usuarioAtual, ativaTempo = 0, buttonState = 0, eventoAtual, endereco = 0, enderecosSalvos = 0;
String dadoAtual;
bool validacaoAcesso, contagemTempo, validacaoAdm;
unsigned long previousMillis = 0;
unsigned long currentMillis;

// obtem o número(ponto) da máquina de estado
char getState(void) {
  return state;
}

// é usado para setar o número(ponto) da máquina de estado
void setState(char newState) {
  state = newState;
  i = 0;
  validacaoAcesso = false;
  contagemTempo = false;
  validacaoAdm = false;
}

void CarregaInfoEEPROM() {
  //Reservando a posicao 0 e 1 da memoria para o numero do cadastro atual, um int consome 2 bytes
  cadastroAtual = EEPROM.read(0);

  String texto = readStringEEPROM(endereco);

  //Serial.println("-----");
  //Serial.println("CarregaInfoEEPROM()");
  //Serial.println(cadastroAtual);
  //Serial.println(texto);
  //Serial.println("-----");

  int inicioN, inicioS, inicioA, fimN, fimS, fimA, controle = 0, nUsuario = 0;
  for (int i = 0; i < texto.length(); i++) {
    //Acresta dado na struct
    if (texto[i] == '/') {
      fimA = i;
      usuario[nUsuario].nome = texto.substring(inicioN, fimN);
      usuario[nUsuario].senha = texto.substring(inicioS, fimS);
      usuario[nUsuario].administrador = texto.substring(inicioA, fimA);
      nUsuario++;
      //Serial.println("Fim!");
    }

    //Inicio de uma informação
    if (texto[i] == '-') {
      //Serial.println("Inicio!");
      if (controle == 0) {
        inicioN = i + 1;
        //Serial.println(inicioN);
        controle = 1;
      }
      else if (controle == 1) {
        fimN = i;
        inicioS = i + 1;
        //Serial.println(inicioS);
        controle = 2;
      }
      else if (controle == 2) {
        fimS = i;
        inicioA = i + 1;
        //Serial.println(inicioA);
        controle = 0;
      }
    }
  }
}

void setup() {
  //Definindo duas saídas
  pinMode(Porta1, OUTPUT);
  pinMode(Porta2, OUTPUT);
  pinMode(Button1, INPUT);
  pinMode(Button2, INPUT);

  Serial.begin(9600);
  Serial.setTimeout(50);
  state = 0;
  usuarioAtual = 0;
  eventoAtual = 0;
  validacaoAcesso = false;
  contagemTempo = false;
  validacaoAdm = false;

  //A linha abaixo serve para limpar o numero de cadastros da memoria
  /*
    cadastroAtual = 0;
    EEPROM.write(0,cadastroAtual);
    for (int nL = 0; nL < 1000; nL++) {
      EEPROM.write(nL, 0);
    }
  */
  //Reservando a posicao 20 em diante da memoria para as strings
  endereco = 20;

  CarregaInfoEEPROM();
}

void loop() {
  currentMillis = millis();
  switch (getState())
  {
    case STATE_MENU:
      if (i == 0) {
        Serial.println("Selecione uma opcao abaixo");
        Serial.println("1 - Cadastro do usuario:");
        Serial.println("2 - Listagem dos nomes dos usuarios cadastrados;");
        Serial.println("3 - Listagem dos eventos:");
        Serial.println("4 - Liberacao da porta 1:");
        Serial.println("5 - Liberacao da porta 2:");
        i = 1;
      }
      //Verifica se chegou informação na Serial
      if (Serial.available() > 0) {
        switch (Serial.read() - 48) {
          case 1:
            setState(STATE_1);
            break;
          case 2:
            setState(STATE_2);
            break;
          case 3:
            setState(STATE_3);
            break;
          case 4:
            setState(STATE_4);
            break;
          case 5:
            setState(STATE_5);
            break;
        }
      }
      break;
    case STATE_1: //Cadastro do usuário
      if (i == 0) {
        Serial.print("Nome:");
        i = 1;
      }
      if (Serial.available() > 0) {
        i++;
        if (i == 2) {
          usuario[cadastroAtual].nome = Serial.readString();
          Serial.println(usuario[cadastroAtual].nome);
          Serial.print("Senha:");
        }
        else if (i == 3) {
          usuario[cadastroAtual].senha = Serial.readString();
          Serial.println(usuario[cadastroAtual].senha);
          Serial.print("Administrador(S ou N):");
        }
        else if (i == 4) {
          usuario[cadastroAtual].administrador = Serial.readString();
          Serial.println(usuario[cadastroAtual].administrador);
          if ((usuario[cadastroAtual].administrador == "S") || (usuario[cadastroAtual].administrador == "N")) {
            Serial.println("Usuario Cadastrado!");

            String texto1 = readStringEEPROM(endereco);
            String texto2 = '-' + usuario[cadastroAtual].nome + '-' + usuario[cadastroAtual].senha + '-' + usuario[cadastroAtual].administrador + '/';
            writeStringEEPROM(endereco, texto1 + texto2);

            cadastroAtual++;
            //Salva na memoria o ultimo cadastro e seu endereco final
            EEPROM.write(0, cadastroAtual);
            setState(STATE_MENU);
          }
          else {
            i--;
            Serial.println("Incorreto!");
            Serial.print("Administrador(S ou N):");
          }
        }
      }
      break;
    case STATE_2: //Listagem dos nomes dos usuários cadastrados
      Serial.println("Usuarios Cadastrados:");

      for (int i = 0; i < cadastroAtual; i++) {
        Serial.println(usuario[i].nome);
        //Serial.println(usuario[i].senha);
        //Serial.println(usuario[i].administrador);
      }

      setState(STATE_MENU);
      break;
    case STATE_3: //Listagem dos eventos
      if (i == 0) {
        Serial.print("Nome:");
        i = 1;
      }
      if (Serial.available() > 0 && !validacaoAcesso) {
        dadoAtual = Serial.readString();
        for (int i = 0; i < 10; i++) {
          if (dadoAtual == usuario[i].nome) {
            usuarioAtual = i;
            validacaoAcesso = true;
            Serial.println(dadoAtual);
            Serial.print("Senha:");
          }
        }
        if (!validacaoAcesso) {
          Serial.println(dadoAtual);
          Serial.println("Usuario nao encontrado!");
          i = 0;
        }
      }
      if (Serial.available() > 0 && validacaoAcesso) {
        dadoAtual = Serial.readString();
        if (dadoAtual == usuario[usuarioAtual].senha) {
          Serial.println(dadoAtual);
          Serial.println("Senha Correta!");
          validacaoAdm = true;
        }
        else {
          Serial.println(dadoAtual);
          Serial.println("Senha Incorreta,Tente Novamente!");
          Serial.print("Senha:");
        }
        if (validacaoAcesso && (usuario[usuarioAtual].administrador == "S")) {
          for (int i = 0; i < 10; i++) {
            Serial.println(eventos[i]);
            setState(STATE_MENU);
          }
        }
        else {
          Serial.println("Esse usuario nao tem permissao!");
          setState(STATE_MENU);
        }
      }
      break;
    case STATE_4: //Liberação da porta 1
      LiberaPorta(Porta1, Button1);
      break;
    case STATE_5: //Liberação da porta 2
      LiberaPorta(Porta2, Button2);
      break;
  }
}

void LiberaPorta(int numeroPorta, int buttonPorta) {
  if (i == 0) {
    Serial.print("Nome:");
    i = 1;
  }
  if (Serial.available() > 0 && !validacaoAcesso) {
    dadoAtual = Serial.readString();
    for (int i = 0; i < 10; i++) {
      if (dadoAtual == usuario[i].nome) {
        usuarioAtual = i;
        validacaoAcesso = true;
        Serial.println(dadoAtual);
        Serial.print("Senha:");
      }
    }
    if (!validacaoAcesso) {
      Serial.println(dadoAtual);
      Serial.println("Usuario nao encontrado!");
      i = 0;
    }
  }
  if (Serial.available() > 0 && validacaoAcesso) {
    dadoAtual = Serial.readString();
    if (dadoAtual == usuario[usuarioAtual].senha) {
      Serial.println(dadoAtual);
      Serial.println("Senha Correta!");
      digitalWrite(numeroPorta, HIGH);
      contagemTempo = true;
      previousMillis = currentMillis;
      eventos[eventoAtual] = usuario[usuarioAtual].nome + " - " + usuario[usuarioAtual].senha + " - Acesso a porta 2";
      eventoAtual++;
    }
    else {
      Serial.println(dadoAtual);
      Serial.println("Senha Incorreta,Tente Novamente!");
      Serial.print("Senha:");
    }
  }
  buttonState = digitalRead(buttonPorta);
  if ((buttonState == HIGH || (currentMillis == previousMillis + 5000)) && contagemTempo) {
    digitalWrite(numeroPorta, LOW);
    setState(STATE_MENU);
  }
}
