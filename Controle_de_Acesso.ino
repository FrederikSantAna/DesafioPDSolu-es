enum {
    STATE_MENU, //0
    STATE_1,    //1    
    STATE_2,    //2
    STATE_3,    //3    
    STATE_4,    //4            
    STATE_5     //5
};

char state;

// obtem o número(ponto) da máquina de estado
char getState(void) {
    return state;
}

// é usado para setar o número(ponto) da máquina de estado
void setState(char newState) {
    state = newState;
}

void setup() {
  //Definindo duas saídas
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  
  Serial.begin(9600);

  state = 0;
}

void loop() {
  switch (getState()) 
    {      
      case STATE_MENU: 
        Serial.println("1 - Cadastro do usuario:");
        Serial.println("2 - Listagem dos nomes dos usuarios cadastrados;");
        Serial.println("4 - Liberacao da porta 1:");
        Serial.println("5 - Liberacao da porta 2:");

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
      case STATE_1:   
        Serial.println("Estado 1!");        
      break;
      case STATE_2:   
        Serial.println("Estado 2!");          
      break;
      case STATE_3:   
        Serial.println("Estado 3!");          
      break;
      case STATE_4:   
        Serial.println("Estado 4!");          
      break;
      case STATE_5:   
        Serial.println("Estado 5!");          
      break;
    }
}
