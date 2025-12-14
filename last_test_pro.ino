#include <Servo.h>

// TCRT5000 센서 핀 정의
#define LEFT_SENSOR 3 // 왼쪽 센서 핀
#define RIGHT_SENSOR 2 // 오른쪽 센서 핀
#define TRIG_PIN 5     // 초음파 센서 Trig 핀
#define ECHO_PIN 4     // 초음파 센서 Echo 핀

// 서보모터 동작 각도 정의
#define STOP_ANGLE 1500    // 서보모터 정지
//#define FORWARD_OFFSET 10  // 속도 조정 계수

// 서보모터 객체 생성
Servo leftServo;  // 왼쪽 서보모터
Servo rightServo; // 오른쪽 서보모터

void setup() {
  // 센서 핀 설정
  pinMode(LEFT_SENSOR, INPUT);
  pinMode(RIGHT_SENSOR, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // 서보모터 핀 연결
  leftServo.attach(12);  // 왼쪽 서보모터
  rightServo.attach(13); // 오른쪽 서보모터

  // 서보모터 초기화
  //leftServo.write(STOP_ANGLE);
  //rightServo.write(STOP_ANGLE);

  // 시리얼 통신 시작
  Serial.begin(9600);
  Serial.println("TCRT5000 Wide Sensor Line Tracing Initialized");
}

void loop() {
  long distance = getDistance(); // 초음파로 거리 측정
  //Serial.print("Distance: ");
  //Serial.print(distance);
  //Serial.println(" cm"); // 디버깅및 거리 테스트용 시리얼 코드 


  if (distance == -1) {
    // 초음파 센서가 신호를 감지하지 못한 경우 라인트레이싱
    lineTracing(1600);
  } else if (distance < 20) {
    // 물체가 20cm 이내로 접근한 경우
    Serial.println("여앞에 뭐있다잉 ");
    stopMotors(); // 정지
    delay(500);   // 잠시 정지
  } else {
    // 이상없음 
    lineTracing(1600); // 라인트래이싱
  }

  delay(10); // 반복 간격
}


// 초음파 센서로 거리 측정
long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

   long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 20ms 동안 신호가 반환이 안되면 값을 날림 
  if (duration == 0) {
    return -1; // 신호 없음
  }

  long distance = duration * 0.034 / 2; // 거리 계산
  return distance > 30 ? -1 : distance; // 30cm 이상은 처리하지 않음 
}

void lineTracing(int speed) {
  
    int leftValue = digitalRead(LEFT_SENSOR);  // 왼쪽 센서 값
    int rightValue = digitalRead(RIGHT_SENSOR); // 오른쪽 센서 값

    // 센서 상태 출력 (디버깅용)
    //Serial.print("Left Sensor: "); Serial.print(leftValue == LOW ? "Detected (Black)" : "Not Detected (White)");
    //Serial.print("Right Sensor: "); Serial.println(rightValue == LOW ? "Detected (Black)" : "Not Detected (White)");
    
    
    // low 가 화이트 안한거 high 가 블랙 감지한거
    // 라인 추적 로직
    if (leftValue == LOW && rightValue == LOW) { // 양쪽 다 하얀색일때 
      moveForward(speed); // 직진
    } else if (leftValue == HIGH && rightValue == LOW) { // 왼쪽만 검은색
      turnLeft(speed); 
    } else if (leftValue == LOW && rightValue == HIGH) { // 오른쪽만 검은색
      turnRight(speed); 
    } else { // 양쪽이 다 검은색일때  정지
      stopMotors();      
    }

    delay(100); 
  }


// 서보모터 제어 함수

// L 12pin R13pin
//L1700 R1300 == 전진
//L1700 R1500 == 주회전 
//L1500 R1700 == 좌회전 
void moveForward(int speed) {
  leftServo.writeMicroseconds(speed);
  rightServo.writeMicroseconds(speed-200);
}

void turnLeft(int speed) {
  leftServo.writeMicroseconds(speed);
  rightServo.writeMicroseconds(speed - 80);
}

void turnRight(int speed) {
  leftServo.writeMicroseconds(speed - 80);
  rightServo.writeMicroseconds(speed);
}

void stopMotors() {
  leftServo.write(STOP_ANGLE);
  rightServo.write(STOP_ANGLE);
}
