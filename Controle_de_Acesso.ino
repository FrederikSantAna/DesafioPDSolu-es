enum {
    STATE_MENU, //0
    STATE_1,    //1    
    STATE_2,    //2
    STATE_3,    //3    
    STATE_4,    //4            
    STATE_5     //5
};

struct Usuario {
  String nome,senha; 
  char administrador; 
};

struct Usuario usuario[10];
char state;
int i, cadastroAtual, j, usuarioAtual, ativaTempo = 0, buttonState = 0;
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

void setup() {
  //Definindo duas saídas
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,INPUT);
  pinMode(5,INPUT);
  
  Serial.begin(9600);
  Serial.setTimeout(50);
  state = 0;
  cadastroAtual = 0;
  usuarioAtual = 0;
  validacaoAcesso = false;
  contagemTempo = false;
  validacaoAdm = false;
}

void loop() {
  currentMillis = millis();
  switch (getState()) 
    {      
      case STATE_MENU:
        if(i == 0) { 
          Serial.println("Selecione uma opcao abaixo");
          Serial.println("1 - Cadastro do usuario:");
          Serial.println("2 - Listagem dos nomes dos usuarios cadastrados;");
          Serial.println("3 - Listagem dos eventos:");
          Serial.println("4 - Liberacao da porta 1:");
          Serial.println("5 - Liberacao da porta 2:");
          i = 1;
        }
        //Verifica se chegou informação na Serial
        if(Serial.available() > 0) {
          switch(Serial.read() - 48) {
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
        if(i == 0){
          Serial.print("Nome:");
          i = 1;
        }
        if(Serial.available() > 0) {
          i++;
          if(i == 2) {
            usuario[cadastroAtual].nome = Serial.readString();
            Serial.println(usuario[cadastroAtual].nome);
            Serial.print("Senha:");
          }
          else if(i == 3) {
            usuario[cadastroAtual].senha = Serial.readString();
            Serial.println(usuario[cadastroAtual].senha);
            Serial.print("Administrador(Sim - S ou Nao - N):");
          }
          else if(i == 4) {
            usuario[cadastroAtual].administrador = Serial.read();
            Serial.println(usuario[cadastroAtual].administrador);
            Serial.println("Usuario Cadastrado!");
            cadastroAtual++;
            setState(STATE_MENU);
          }
        }
        //Serial.println("Saiu!");                   
      break;
      case STATE_2: //Listagem dos nomes dos usuários cadastrados
        if(i == 0) {
          Serial.println("Usuarios Cadastrados:");  
          for(j = 0; j < 10; j++) {  
            Serial.println(usuario[j].nome);     
          }
          Serial.println("Digite \"Voltar\" para retornar ao Menu");
          i = 1;
        }
        if(Serial.available() > 0) {
            if(Serial.readString() == "Voltar") {
             setState(STATE_MENU);     
            }
        }
      break;
      case STATE_3: //Listagem dos eventos   
        if(i == 0){
          Serial.print("Nome:");
          i = 1;
        }
        if(Serial.available() > 0 && !validacaoAcesso) {
          dadoAtual = Serial.readString();
          for(j = 0; j < 10; j++) {
            if(dadoAtual == usuario[j].nome) {
              usuarioAtual = j;
              validacaoAcesso = true;
              Serial.println(dadoAtual); 
              Serial.print("Senha:");
            }
          }
          if(!validacaoAcesso) {
            Serial.println(dadoAtual); 
            Serial.println("Usuario nao encontrado!"); 
            i = 0;
          }
        }
        if(Serial.available() > 0 && validacaoAcesso) {
            dadoAtual = Serial.readString();
            if(dadoAtual == usuario[usuarioAtual].senha) {
              Serial.println(dadoAtual); 
              Serial.println("Senha Correta!"); 
              validacaoAdm = true;            
            }
            else {
              Serial.println(dadoAtual);
              Serial.println("Senha Incorreta,Tente Novamente!");
              Serial.print("Senha:");
            }
          if(validacaoAcesso && (usuario[usuarioAtual].administrador == 'S')) {
            
          }
          else {
            Serial.println("Esse usuario nao tem permissao!");
            setState(STATE_MENU); 
          }
        }
      break;
      case STATE_4: //Liberação da porta 1    
        if(i == 0){
          Serial.print("Nome:");
          i = 1;
        }
        if(Serial.available() > 0 && !validacaoAcesso) {
          dadoAtual = Serial.readString();
          for(j = 0; j < 10; j++) {
            if(dadoAtual == usuario[j].nome) {
              usuarioAtual = j;
              validacaoAcesso = true;
              Serial.println(dadoAtual); 
              Serial.print("Senha:");
            }
          }
          if(!validacaoAcesso) {
            Serial.println(dadoAtual); 
            Serial.println("Usuario nao encontrado!"); 
            i = 0;
          }
        }
        if(Serial.available() > 0 && validacaoAcesso) {
            dadoAtual = Serial.readString();
            if(dadoAtual == usuario[usuarioAtual].senha) {
              Serial.println(dadoAtual); 
              Serial.println("Senha Correta!");
              digitalWrite(2, HIGH); 
              contagemTempo = true; 
              previousMillis = currentMillis;        
            }
            else {
              Serial.println(dadoAtual);
              Serial.println("Senha Incorreta,Tente Novamente!");
              Serial.print("Senha:");
            }
        } 
        buttonState = digitalRead(4);
        if ((buttonState == HIGH || (currentMillis == previousMillis + 5000)) && contagemTempo) {
          digitalWrite(2, LOW);
          setState(STATE_MENU); 
        }         
      break;
      case STATE_5: //Liberação da porta 2   
        if(i == 0){
          Serial.print("Nome:");
          i = 1;
        }
        if(Serial.available() > 0 && !validacaoAcesso) {
          dadoAtual = Serial.readString();
          for(j = 0; j < 10; j++) {
            if(dadoAtual == usuario[j].nome) {
              usuarioAtual = j;
              validacaoAcesso = true;
              Serial.println(dadoAtual); 
              Serial.print("Senha:");
            }
          }
          if(!validacaoAcesso) {
            Serial.println(dadoAtual); 
            Serial.println("Usuario nao encontrado!"); 
            i = 0;
          }
        }
        if(Serial.available() > 0 && validacaoAcesso) {
            dadoAtual = Serial.readString();
            if(dadoAtual == usuario[usuarioAtual].senha) {
              Serial.println(dadoAtual); 
              Serial.println("Senha Correta!");
              digitalWrite(3, HIGH); 
              contagemTempo = true; 
              previousMillis = currentMillis;        
            }
            else {
              Serial.println(dadoAtual);
              Serial.println("Senha Incorreta,Tente Novamente!");
              Serial.print("Senha:");
            }
        } 
        buttonState = digitalRead(5);
        if ((buttonState == HIGH || (currentMillis == previousMillis + 5000)) && contagemTempo) {
          digitalWrite(3, LOW);
          setState(STATE_MENU); 
        }          
      break;
    }
}
