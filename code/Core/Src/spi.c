/*
 * spi.c
 *
 *  Created on: Nov 11, 2022
 *      Author: Administrator
 */


// STM32F103 SPI1

// PA4  - (OUT)	SPI1_NSS
// PA5  - (OUT)	SPI1_SCK
// PA6  - (IN)	SPI1_MISO (Master In)
// PA7  - (OUT)	SPI1_MOSI (Master Out)


#include "spi.h"

void initSPI1(void) {
	RCC->APB2ENR |= (RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_SPI1EN);

	// âûâîä óïðàâëåíèÿ NSS: âûõîä äâóõòàêòíûé, îáùåãî íàçíà÷åíèÿ,50MHz
	GPIOA->CRL   |=  GPIO_CRL_MODE4;
	GPIOA->CRL   &= ~GPIO_CRL_CNF4;
	GPIOA->BSRR   =  GPIO_BSRR_BS4;

	// âûâîä SCK: âûõîä äâóõòàêòíûé, àëüòåðíàòèâíàÿ ôóíêöèÿ, 50MHz
	GPIOA->CRL   |=  GPIO_CRL_MODE5;
	GPIOA->CRL   &= ~GPIO_CRL_CNF5;
	GPIOA->CRL   |=  GPIO_CRL_CNF5_1;

	// âûâîä MISO: âõîä öèôðîâîé ñ ïîäòÿãèâàþùèì ðåçèñòîðîì, ïîäòÿæêà ê ïëþñó
	GPIOA->CRL   &= ~GPIO_CRL_MODE6;
	GPIOA->CRL   &= ~GPIO_CRL_CNF6;
	GPIOA->CRL   |=  GPIO_CRL_CNF6_1;
	GPIOA->BSRR   =  GPIO_BSRR_BS6;

	// âûâîä MOSI: âûõîä äâóõòàêòíûé, àëüòåðíàòèâíàÿ ôóíêöèÿ, 50MHz
	GPIOA->CRL   |=  GPIO_CRL_MODE7;
	GPIOA->CRL   &= ~GPIO_CRL_CNF7;
	GPIOA->CRL   |=  GPIO_CRL_CNF7_1;

	// Íàñòðîéêà SPI1 (STM32F103)
	SPI1->CR2     = 0x0000;
	SPI1->CR1     = SPI_CR1_MSTR;	// Ìàñòåð
	SPI1->CR1    |= SPI_CR1_BR;		// Ìàëåíüêóÿ ñêîðîñòü SPI Baud rate = Fpclk/256	(2,4,8,16,32,64,128,256)
	SPI1->CR1    |= SPI_CR1_SSM;	// Ïðîãðàììíûé ðåæèì NSS
	SPI1->CR1    |= SPI_CR1_SSI;	// Àíàëîãè÷íî ñîñòîÿíèþ, êîãäà íà âõîäå NSS âûñîêèé óðîâåíü
	SPI1->CR1    |= SPI_CR1_SPE;	// Ðàçðåøèòü ðàáîòó ìîäóëÿ SPI
//	SPI1->CR1 &= ~SPI_CR1_CPOL; 	// Ïîëÿðíîñòü òàêòîâîãî ñèãíàëà (CK to 0 when idle)
//	SPI1->CR1 &= ~SPI_CR1_CPHA; 	// Ôàçà òàêòîâîãî ñèãíàëà (|= SPI_CR1_CPHA - ïî âòîðîìó ôðîíòó)
//	SPI1->CR1 |= SPI_CR1_DFF; 		// 16 áèò äàííûõ
//	SPI1->CR1 &= ~SPI_CR1_LSBFIRST;	// Ìëàäøèé áèò (MSB) ïåðåäàåòñÿ ïåðâûì
//	SPI1->CR2 |= SPI_CR2_SSOE; 		// NSS - èñïîëüçóåòñÿ êàê âûõîä óïðàâëåíèÿ slave select
}

uint8_t SPI1SendByte(uint8_t data) {
	while (!(SPI1->SR & SPI_SR_TXE));      				// óáåäèòüñÿ, ÷òî ïðåäûäóùàÿ ïåðåäà÷à çàâåðøåíà (STM32F103)
	SPI1->DR=data;										// âûâîä â SPI1
	while (!(SPI1->SR & SPI_SR_RXNE));     				// æäåì îêîí÷àíèÿ îáìåíà (STM32F103)
	return SPI1->DR;		         					// ÷èòàåì ïðèíÿòûå äàííûå
}

void SPI1_WriteReg(uint8_t address, uint8_t value) {
	SPI1_NSS_ON();										// CS_Low
	SPI1SendByte(address);
	SPI1SendByte(value);
	SPI1_NSS_OFF();										// CS_HIGH
}

uint8_t SPI1_ReadReg(uint8_t address) {
	uint8_t	val;

	SPI1_NSS_ON();										// CS_Low
	SPI1SendByte(address);
	val = SPI1SendByte(0x00);
	SPI1_NSS_OFF();										// CS_HIGH
	return val;
}
