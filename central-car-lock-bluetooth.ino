#define PERMISSION_RELAY 4
#define POLARITY_CHANGE_RELAY_1 5
#define POLARITY_CHANGE_RELAY_2 6

#define ON HIGH
#define OFF LOW
#define PAUSE_DELAY 2000
/* 
 * оголошення затримки, яка знадобиться для паузи між змінами положень реле
 */
#define SWITCHING_DELAY 100
 
/* 
 * оголошуємо команди, що будуть виконувати зміну полярності на релейних 
 * пристроях та закривати чи відкривати замковий механізм відповідно
 */
void plusMinus();
void minusPlus();

void setup() {
  /* 
   * оголошуємо виходи на піни трьох реле, двоє з яких відповідаються 
   * за зміну полярності на замковому приводі, а третє реагує на подання 
   * струму та пропускає його далі, щоб дозволити зреагувати цим двом реле
   * на цю зміну і змінити свій стан
   */
  pinMode(PERMISSION_RELAY, OUTPUT);
  pinMode(POLARITY_CHANGE_RELAY_1, OUTPUT);
  pinMode(POLARITY_CHANGE_RELAY_2, OUTPUT);
}

/* 
 * для перевірки роботи схеми запускаємо цикл, що буде 
 * із затримкою виконувати відкриття/закриття замка
 */
void loop() {
  plusMinus();
  delay(PAUSE_DELAY);
  minusPlus();
  delay(PAUSE_DELAY);
}

void plusMinus() {
  /* 
   * змінюємо полярність реле на (-) та закриваємо замок
   */
  digitalWrite(PERMISSION_RELAY, OFF);
  delay(SWITCHING_DELAY);
  digitalWrite(POLARITY_CHANGE_RELAY_1, OFF);
  digitalWrite(POLARITY_CHANGE_RELAY_2, OFF);
  delay(SWITCHING_DELAY);
  digitalWrite(PERMISSION_RELAY, ON);
}

void minusPlus() {
  /* 
   * змінюємо полярність реле на (+) та відкриваємо замок
   */
  digitalWrite(PERMISSION_RELAY, OFF);
  delay(SWITCHING_DELAY);
  digitalWrite(POLARITY_CHANGE_RELAY_1, ON);
  digitalWrite(POLARITY_CHANGE_RELAY_2, ON);
  delay(SWITCHING_DELAY);
  digitalWrite(PERMISSION_RELAY, ON);
}
