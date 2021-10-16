#include "MicroBit.h"
#include "nrf_delay.h"
#include "font.h"

//MicroBit uBit; 

void VideoActive(void);
void VideoHsync(void);
void VideoVsync(void);
void VideoITS(void);

char *FrameBuffer;

#define HorizontalResolution 128 //128
#define VerticalResolution 240 //240


unsigned int CurrentLine=1;
unsigned int SyncPin = 1 << 2;
unsigned int VideoPin = 1 << 3;//this is used for spi too

//1<<3=PIN 0
//1<<2=PIN 1
//1<< =PIN 3    

volatile unsigned int *dirset           = (unsigned int *) (0x50000000UL + 0x518);
volatile unsigned int *outset           = (unsigned int *) (0x50000000UL + 0x508);
volatile unsigned int *outclr           = (unsigned int *) (0x50000000UL + 0x50C);

volatile unsigned int *SPI_ENABLE       = (unsigned int *) (0x40004000UL + 0x500);
volatile unsigned int *SPI_INTEN        = (unsigned int *) (0x40004000UL + 0x300);
volatile unsigned int *SPI_INTENSET     = (unsigned int *) (0x40004000UL + 0x304);
volatile unsigned int *SPI_INTENCLR     = (unsigned int *) (0x40004000UL + 0x308);
volatile unsigned int *SPI_READY        = (unsigned int *) (0x40004000UL + 0x108);

volatile unsigned int *SPI_PSELSCK      = (unsigned int *) (0x40004000UL + 0x508);
volatile unsigned int *SPI_PSELMOSI     = (unsigned int *) (0x40004000UL + 0x50C);
volatile unsigned int *SPI_PSELMISO     = (unsigned int *) (0x40004000UL + 0x510);

volatile unsigned int *SPI_RXD          = (unsigned int *) (0x40004000UL + 0x518);
volatile unsigned int *SPI_TXD          = (unsigned int *) (0x40004000UL + 0x51C);

volatile unsigned int *SPI_FREQUENCY    = (unsigned int *) (0x40004000UL + 0x524);
volatile unsigned int *SPI_CONFIG       = (unsigned int *) (0x40004000UL + 0x554);

volatile unsigned int *IER              = (unsigned int *) (0xE000E100UL);

volatile unsigned int *CLOCK_HFCLKSTAT  = (unsigned int *) (0x40000000UL + 0x40C);
volatile unsigned int *CLOCK_HFCLKRUN   = (unsigned int *) (0x40000000UL + 0x408);
volatile unsigned int *CLOCK_XTALFREQ   = (unsigned int *) (0x40000000UL + 0x550);


extern "C" void SPI1_TWI1_IRQHandler(void) {         
    //unsigned int trash;
    //*SPI_READY=0;
    //*SPI_RXD;        
    //if(buf_pos+2 >= sizeof(buf)) buf_pos=0;
    //*SPI_TXD = buf[buf_pos];
    //*SPI_TXD = buf[buf_pos+1];
    //buf_pos+=2;
}


void Spi_Configuration(void) {        
    *SPI_PSELSCK = SCK;
    *SPI_PSELMISO=0xFFFFFFFF;
    *SPI_PSELMOSI=MICROBIT_PIN_P0;
    *SPI_FREQUENCY = SPI_FREQUENCY_FREQUENCY_M4;                      
    //*SPI_INTEN=1<<2;            
    //*SPI_INTENSET=1<<2;           
    //NVIC_SetPriority(SPI1_TWI1_IRQn, 0);       /* highest priority */    
    //NVIC_ClearPendingIRQ(SPI1_TWI1_IRQn);         
    //NVIC_EnableIRQ(SPI1_TWI1_IRQn);    
}
void Port_Configuration(void) {            
    *dirset=VideoPin;
    *dirset=SyncPin;                
    *outclr=VideoPin;
    *outset=SyncPin;
}
void Timer_Configuration(void) {    
    NRF_TIMER0->TASKS_STOP     = 1;                             // stop the task first
    NRF_TIMER0->MODE           = TIMER_MODE_MODE_Timer;        // Set the timer in Timer Mode.
    NRF_TIMER0->PRESCALER      = 0;                            // 
    NRF_TIMER0->BITMODE        = TIMER_BITMODE_BITMODE_16Bit;  // 32 bit mode.
    NRF_TIMER0->TASKS_CLEAR    = 1;                            // clear the task first to be usable for later.
    NRF_TIMER0->CC[0]          = 1017; // ~63.5uS      
    NRF_TIMER0->INTENCLR       = 0xFFFFFFFFUL;       
    NRF_TIMER0->INTENSET  = ( TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos );    
    NRF_TIMER0->SHORTS = (TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos);
    NRF_TIMER0->TASKS_START    = 1;                    // Start timer.  
    NVIC_ClearPendingIRQ(TIMER0_IRQn);
    NVIC_SetPriority(TIMER0_IRQn, 0);
    NVIC_EnableIRQ(TIMER0_IRQn);               
}


void VideoHsync(void) {
       __disable_irq();
        //nrf_delay_us(1);//Front Porch (1.5uS)
        *outclr = SyncPin;
        nrf_delay_us(4);//Sync Tip (4.7uS)
        *outset = SyncPin;
        nrf_delay_us(4); //(Breezeway 0.6us + 2.5us Colour Burst + 1.6us Back Porch)= 4.7uS            
        __enable_irq();             
        //VideoITS();        
        VideoActive();                    
}
void VideoVsync(void) {   
        __disable_irq();     
        switch(CurrentLine) {
            case 1:
            case 2:
            case 3:   
            case 7:
            case 8:
            case 9:                
                *outclr = SyncPin;
                nrf_delay_us(2);//2
                *outset = SyncPin;
                nrf_delay_us(29);//29
                *outclr = SyncPin;
                nrf_delay_us(2);//2
                *outset = SyncPin;                            
                break;
            case 4:
            case 5:
            case 6:            
                *outclr = SyncPin;
                nrf_delay_us(27);//27
                *outset = SyncPin;                
                nrf_delay_us(4);//4
                *outclr = SyncPin;
                nrf_delay_us(27);//27
                *outset = SyncPin;                                                                        
                break;
            default://Hsync but no Data                            
                //nrf_delay_us(1);//Front Porch (1.5uS)
                *outclr = SyncPin;
                nrf_delay_us(4);//Sync Tip (4.7uS)
                *outset = SyncPin;
                nrf_delay_us(4); //(Breezeway 0.6us + 2.5us Colour Burst + 1.6us Back Porch)= 4.7uS                            
        }        
        __enable_irq();
}

void VideoITS(void) {            
    for(int i=0;i<16;i++) {                 
        *SPI_TXD=0xFF;                      
        if(i==1) {
            *SPI_ENABLE=1;    
            nrf_delay_us(1);
        }
        if(i%2) nrf_delay_us(1);
    }        
    nrf_delay_us(1);        
    *SPI_ENABLE=0;    
    *outclr=VideoPin;    
}
void VideoActive(void) {                
    char *p;    
    if(CurrentLine < 20) return;//CHECK WHY WE GET HERE
    p=FrameBuffer + ((CurrentLine-20) * HorizontalResolution/8);    
    for(int i=0;i<HorizontalResolution/8;i++) {
        *SPI_TXD=*p;        
        p++;
        if(i==1) {
            *SPI_ENABLE=1;    
            nrf_delay_us(1);
        }
        if(i%2) nrf_delay_us(1);
    }        
    nrf_delay_us(1);        
    *SPI_ENABLE=0;    
    *outclr=VideoPin;    
}

extern "C" void TIMER0_IRQHandler(void) {  
    if (NRF_TIMER0->EVENTS_COMPARE[0] != 0) {
        NRF_TIMER0->EVENTS_COMPARE[0]=0;
        if(CurrentLine > 262) {            
                CurrentLine=1;                                
        }
        if(CurrentLine <= 9) {
            VideoVsync();
        }
        else {
            VideoHsync();
        }
        CurrentLine++;                            
    }                          
}

void ClearFrameBuffer(void) {
    char *p;
    p=FrameBuffer;
    for(int x=0;x<HorizontalResolution/8;x++) {
        for(int y=0;y<VerticalResolution;y++) {            
            *p=0;            
            p++;        
        }
    }    
}
void DrawDot(unsigned int x,unsigned int y,unsigned int color) {
    if(x >= HorizontalResolution) return;
    if(y >= VerticalResolution) return;            
    char *p;
    p=FrameBuffer;
    p += y * HorizontalResolution/8;
    p += x/8;        
    if(color) {        
        *p |=0x80 >> (x-((x/8)*8));    
    }
    else {
        *p &=~(0x80 >> (x-((x/8)*8)));    
    }
    
}

void DrawChar(unsigned int c,unsigned int x, unsigned int y) {
    int i,q;
    int tmp,tmpa;
    if(c < 32 || c > 127) c=32;
    for(i=0;i<5;i++) {//Columns
        tmp=Font5x7[((c-32)*5)+i];
        for(q=0;q<7;q++) {//Rows
            tmpa = tmp >> q & 1;
            if(tmpa) {
                DrawDot(i+x,q+y,1);
            }
            else {
                DrawDot(i+x,q+y,0);
            }
        }
    }
}
void DrawString(unsigned int x,unsigned int y,char *Message) {    
    for(int i=0;i<strlen(Message);i++) {        
        DrawChar(Message[i],x+(8*i),y);
    }      
}


void DrawLine(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2) {
    if(x2 < x1) return;
    if(y2 < y1) return;
    for(unsigned int x=x1;x<=x2;x++) {
        for(unsigned int y=y1;y<=y2;y++) {
            DrawDot(x,y,1);
        }
    }
}
   
void DumpFrameBuffer(void) {
    char *p;    
    printf("START:\n\r");
    for(int cl=0;cl<VerticalResolution;cl++) {                
        p=FrameBuffer + ((cl) * HorizontalResolution/8);    
        printf("LINE %03u: ",cl);
        for(int i=0;i<HorizontalResolution/8;i++) {
            printf("%02x ",*p);
            p++;
        }        
        printf("\n\r");                
    }            
    printf("DONE.\n\r");    
}
   
   

int main() {           
    MicroBitSerial serial(USBTX, USBRX);       
    printf("\n\rStarted\n\r");       
    Spi_Configuration();
    Port_Configuration();   
    Timer_Configuration();                      
    //uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_EVT_ANY, onButton);            
    printf("Requesting: %u Bytes for frame buffer...",VerticalResolution * HorizontalResolution / 8);    
    FrameBuffer=(char*)malloc(VerticalResolution * HorizontalResolution / 8);
    printf("\n\rFrameBuffer: %u\n\r",FrameBuffer);     
    wait(2);    
    ClearFrameBuffer();  
    DrawLine(20,20,20,50);
    DrawLine(20,20,50,20);
    
    DrawLine(20,50,50,50);
    DrawLine(50,20,50,50);
    
    DrawLine(55,20,105,50);
    DrawLine(105,20,55,50);
                
    //DrawLine(20,20,100,100);        
    DrawString(5,80,"micro:bit NTSC");
    DrawString(5,90,"video by");
    DrawString(5,100,"Kevin Moonlight");
    DrawString(5,130,"rebuilt by");
    DrawString(5,140,"@blogmywiki");
    DrawString(5,160,"abcdefghihjklmn");
    DrawString(5,170,"opqrstuvwxyz!@$");
    
                
    char Message[32];
    unsigned int i;     
    while(1) {              
                
        i++;
        sprintf((char *)&Message,"%u",i);
        DrawString(5,200,Message);
       // printf(".\n\r");
        //printf("Enter Line:");        
        //scanf("%d", &DrawLine);                           
        //printf("%u\n\r",DrawLine);         
    }                   
}


