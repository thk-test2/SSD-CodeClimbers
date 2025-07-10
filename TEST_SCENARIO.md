# SSD Test Shell - 테스트 시나리오
## 기본 명령어 테스트 시나리오

### 1. 기본 Read/Write 테스트
**목적**: SSD의 기본적인 읽기/쓰기 동작 검증

#### 테스트 케이스 1-1: 정상적인 Write/Read 동작
```
write 0 0x12345678
read 0
예상결과: LBA 00 : 0x12345678
```

#### 테스트 케이스 1-2: 여러 LBA에 대한 Write/Read
```
write 5 0xAAAABBBB
write 10 0xCCCCDDDD
read 5
read 10
예상결과: 
LBA 05 : 0xAAAABBBB
LBA 10 : 0xCCCCDDDD
```

### 2. 에러 처리 테스트
**목적**: 잘못된 입력에 대한 에러 처리 검증

#### 테스트 케이스 2-1: 잘못된 LBA 범위
```
write 100 0x12345678
read 100
예상결과: [Read] ERROR 또는 [Write] ERROR
```

#### 테스트 케이스 2-2: 잘못된 인자 개수
```
write 5
read
erase 10
예상결과: INVALID COMMAND
```

#### 테스트 케이스 2-3: 잘못된 데이터 형식
```
write abc 0x12345678
write 5 invalid_hex
예상결과: INVALID COMMAND
```

### 3. Erase 명령어 테스트
**목적**: 데이터 삭제 기능 검증

#### 테스트 케이스 3-1: 기본 Erase 동작
```
write 0 0x12345678
write 1 0x87654321
erase 0 2
read 0
read 1
예상결과: 데이터가 삭제되었는지 확인
```

#### 테스트 케이스 3-2: Erase Range 동작
```
erase_range 10 19
예상결과: [Erase Range] Done
```

### 4. Full Read/Write 테스트
**목적**: 전체 SSD 영역에 대한 읽기/쓰기 검증

#### 테스트 케이스 4-1: Full Write
```
fullwrite 0xDEADBEEF
예상결과: [Full Write] Done
```

#### 테스트 케이스 4-2: Full Read
```
fullread
예상결과: LBA 00부터 LBA 99까지 모든 데이터 출력
```

### 5. Flush 명령어 테스트
**목적**: 버퍼 플러시 기능 검증

#### 테스트 케이스 5-1: 정상적인 Flush
```
write 0 0x12345678
flush
예상결과: [Flush] Done / nand file 반영 확인
```

### 6. Test Script 테스트
**목적**: Test Script 수행 검증
```
1_
2_
예상결과: Pass
* 3_, 4_, Runner는 시간 관계상 스크린샷으로 대체
```

### 7. Logger 테스트
**목적**: Logger 기능 검증
```
fullwrite 0x11111111
fullread
예상결과: 로그 파일 확인
```

### 8. SSD Command Buffer 테스트
**목적**: SSD Command Buffer 기능 검증
```
최초: 1_empty , 2_empty , 3_empty , 4_empty , 5_empty

write 20 0x12345678
결과: 1_W_20_0xABCDABCD , 2_empty , 3_empty , 4_empty , 5_empty

write 20 0x87654321
결과: 1_W_20_0x87654321 , 2_empty , 3_empty , 4_empty , 5_empty

erase 20 1
결과: 1_E_20 , 2_empty , 3_empty , 4_empty , 5_empty
```