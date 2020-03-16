#include "esp_camera.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "fb_gfx.h"
#include "fd_forward.h"
#include "fr_forward.h"
#include "FS.h"                // SD Card ESP32
#include "SD_MMC.h"            // SD Card ESP32
#include "soc/soc.h"           // Desativar problemas
#include "soc/rtc_cntl_reg.h"  // Desativar problemas
#include "driver/rtc_io.h"
#include <EEPROM.h>            // salvar dados permanentes na memória flash
#define EEPROM_SIZE 1          // define o número de bytes que você deseja acessar, um byte nos permite gerar até 256 números de imagens.
 
RTC_DATA_ATTR int bootCount = 0;

// Definição de pinos para CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
 
int pictureNumber = 0;// variável que gerará o nome da foto: picture1.jpg, picture2.jpg e assim por diante

#define uS_TO_S_FACTOR 1000000
  
void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);// desativar detector de queda de energia
  Serial.begin(115200);
 
  Serial.setDebugOutput(true);
 
  camera_config_t config; //configurações da câmera
  config.ledc_channel = LEDC_CHANNEL_0; //configurações da câmera
  config.ledc_timer = LEDC_TIMER_0; //configurações da câmera
  config.pin_d0 = Y2_GPIO_NUM; //configurações da câmera
  config.pin_d1 = Y3_GPIO_NUM; //configurações da câmera
  config.pin_d2 = Y4_GPIO_NUM; 
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; //configurações da câmera
  
  pinMode(4, INPUT);
  digitalWrite(4, LOW);
  rtc_gpio_hold_dis(GPIO_NUM_4);
 
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA(TAMANHO DO QUADRO)
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
 
  // Iniciar Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("O início da câmera falhou com o erro 0x% x", err);
    return;
  }
 
  Serial.println("Iniciando o cartão SD");
 
  delay(500);
  if(!SD_MMC.begin()){
    Serial.println("Falha na montagem do cartão SD");
    //return;
  }
 
  uint8_t cardType = SD_MMC.cardType();
  if(cardType == CARD_NONE){
    Serial.println("Nenhum cartão SD conectado");
    return;
  }
   
  camera_fb_t * fb = NULL;
 
  // Tire uma foto com a câmera
  fb = esp_camera_fb_get();  
  if(!fb) {
    Serial.println("Falha na captura da câmera");
    return;
  }
  // inicialize a EEPROM com tamanho predefinido
  EEPROM.begin(EEPROM_SIZE);
  pictureNumber = EEPROM.read(0) + 1;
 
  // Caminho onde a nova imagem será salva no cartão SD
  String path = "/picture" + String(pictureNumber) +".jpg";
 
  fs::FS &fs = SD_MMC;
  Serial.printf("Picture file name: %s\n", path.c_str());
 
  File file = fs.open(path.c_str(), FILE_WRITE);
  if(!file){
    Serial.println("Falha ao abrir o arquivo no modo de gravação");
  }
  else {
    file.write(fb->buf, fb->len); // payload (image), payload length(carga útil (imagem), comprimento da carga útil)
    Serial.printf("Arquivo salvo no caminho: %s\n", path.c_str());
    EEPROM.write(0, pictureNumber);
    EEPROM.commit();
  }
  file.close();
  esp_camera_fb_return(fb);
  
  delay(1000);
  
  // Desliga o LED de bordo branco ESP32-CAM (flash) conectado ao GPIO 4
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  rtc_gpio_hold_en(GPIO_NUM_4);

  esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, 0);
 
  Serial.println("entrando no modo espera");
  delay(1000);
  esp_deep_sleep_start();
  Serial.println("não será impresso");
} 
 
void loop() {
 
}
