i will list all the commands needs to be optimized for NLP 
(enga mention pannaada commands ah ignore pannidunga)


1.
2. CMD:RBG=(r,g,b) // we can choose any values of red,green,blue to create endless colors 
ex: CMD:RBG(20,40,100) //

3. 
4. CMD:RGBN=R1,G1,B1;R2,G2,B2;R3,G3,B3;...
   // It’s used to send multiple raw RGB values at once (instead of single color names).
   .Each color = R,G,B (0–255 each)
   .Multiple colors are separated by ;
   .Max = 10 colors (since multiColors[10][3] is allocated in colors.h).
   Examples:
     Two colors (Red + Blue alternating):
     CMD:RGBN=255,0,0;0,0,255
     Four colors (Yellow → Green → Cyan → Purple):
     CMD:RGBN=255,255,0;0,255,0;0,255,255;160,0,160  //
5.
6. 
7. CMD:STOP //stops any effects 
8. CMD:CONTINUE // if stop command is pause and it is play 
