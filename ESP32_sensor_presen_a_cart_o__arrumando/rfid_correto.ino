//RFID para detectar o código dos carros que estão cadastrados e possuem os tags.

//Bibliotecas utilizadas 
#include <SPI.h>
#include <MFRC522.h>
 
//Pinos conectados na placa
#define LED_VERDE 6
#define LED_VERMELHO 7
#define BUZZER 8
#define SS_PIN 10
#define RST_PIN 9
 
String IDtag = ""; //Variável onde é armazenada as variaveis das tags
bool Permitido = false; //Variável para verificar a função
 
//Vetor onde é armazenados os ID's das Tag's que estão cadastradas
String TagsCadastradas[] = {"ID_1", 
                            "ID_2",
                            "ID_3"};
 
MFRC522 LeitorRFID(SS_PIN, RST_PIN);    // responsàvel por criar uma nova instância para o leitor e passa os pinos como parâmetro estabelecidos
 
 
void setup() {
        Serial.begin(9600);             // comunicação Serial inicializada
        SPI.begin();                    // comunicacao SPI inicializada
        LeitorRFID.PCD_Init();          // leitor RFID inicializado
        pinMode(LED_VERDE, OUTPUT);     // pino do led verde como saída
        pinMode(LED_VERMELHO, OUTPUT);  // pino do led vermelho como saída
        pinMode(BUZZER, OUTPUT);        // pino do buzzer como saída
}

 
void loop() {  
  Leitura();  //função responsável por fazer a leitura das Tag's
}
 
void Leitura(){
 
        IDtag = ""; //Inicialmente IDtag deve estar vazia.
        
        // executa uma verificação para ver se tem uma Tag presente
        if ( !LeitorRFID.PICC_IsNewCardPresent() || !LeitorRFID.PICC_ReadCardSerial() ) {
            delay(50);
            return;
        }
        
        // Pega o ID da Tag através da função LeitorRFID.uid e Armazena o ID na variável IDtag        
        for (byte i = 0; i < LeitorRFID.uid.size; i++) {        
            IDtag.concat(String(LeitorRFID.uid.uidByte[i], HEX));
        }        
        
        //Verificad se os valores lidos das tags são referentes a alguma cadastrada
        for (int i = 0; i < (sizeof(TagsCadastradas)/sizeof(String)); i++) {
          if(  IDtag.equalsIgnoreCase(TagsCadastradas[i])  ){
              Permitido = true; //Variável verdadeiro de tag cadastrada
          }
        }       
        switch (Permitido) {
        case (true):
        acessoLiberado();
        break;
        case (false):
        acessoNegado();
        break;
         
        delay(2000); //aguarda 2 segundos para efetuar uma nova leitura
}
 
void acessoLiberado(){
  Serial.println("Tag Cadastrada: " + IDtag); //Exibe a mensagem "Tag Cadastrada" e o ID da tag não cadastrada
    efeitoPermitido();  //Chama a função efeitoPermitido()
    Permitido = false;  //Seta a variável Permitido como false novamente
}
 
void acessoNegado(){
  Serial.println("Tag NAO Cadastrada: " + IDtag); //Exibe a mensagem "Tag NAO Cadastrada" e o ID da tag cadastrada
  efeitoNegado(); //Chama a função efeitoNegado()
}
 
void efeitoPermitido(){  
  int qtd_bips = 2; //definindo a quantidade de bips
  for(int j=0; j<qtd_bips; j++){
    //Ligando o buzzer com uma frequência de 1500 hz e ligando o led verde.
    tone(BUZZER,1500);
    digitalWrite(LED_VERDE, HIGH);   
    delay(100);   
    
    //Desligando o buzzer e led verde.      
    noTone(BUZZER);
    digitalWrite(LED_VERDE, LOW);
    delay(100);
  }  
}
 
void efeitoNegado(){  
  int qtd_bips = 1;  //definindo a quantidade de bips
  for(int j=0; j<qtd_bips; j++){   
    //Ligando o buzzer com uma frequência de 500 hz e ligando o led vermelho.
    tone(BUZZER,500);
    digitalWrite(LED_VERMELHO, HIGH);   
    delay(500); 
      
    //Desligando o buzzer e o led vermelho.
    noTone(BUZZER);
    digitalWrite(LED_VERMELHO, LOW);
    delay(500);
  }  
}
