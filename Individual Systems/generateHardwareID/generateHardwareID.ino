/*
  The S.A.R.I.B.O. Prototype Functions
  Systematic and Automated Regulation of Irrigation systems for Backyard farming Operations
  
  SARIBO Generate Hardware ID Prototype Function
  Version 1.1 Revision March 27, 2020
  
  BSD 3-Clause License
  Copyright (c) 2020, Roy Joseph Argumido (rjargumido@outlook.com)
  All rights reserved.
  
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  
  1. Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.
  
  2. Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.
  
  3. Neither the name of the copyright holder nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.
  
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

String generateHID() {
  /*
   * This generates a random, 8-alphanumeric string.
   * 
   * Algorithm:
   * [1] To create a psuedo-random number, the randomSeed() in the setup is placed
   * [2] Randomizes 8 characters in the loop
   * [3] Randomize between 0 & 1 in the if(), which randomizes a alphabet character
   *     if the value is 1, otherwise a numeric character
   * [4] Writes the terminating character '\0' at the last position of the array
   * [5] Return the character array as a String
   */
  char HID[8]; //Container for the generated character
  for(int i = 0; i < 8; i++){
    if(random(2) == 1)  //Randomizes between 0 & 1
      HID[i] = random(65, 90);  //1 means alphabet character
    else  //0 means a numeric character
      HID[i] = random(48, 57);
  }
  //Adds the terminating character in the last position of the char array
  HID[8] = '\0';
  
  return HID;
}

void setup() {
   Serial.begin(115200);
   /*
    * If the analog input pin 0 is unconnected, random analog
    * noise will cause the call to randomSeed() to generate
    * different seed numbers each time the sketch runs.
    * randomSeed() will then shuffle the random function.
    */
   randomSeed(analogRead(0));
}

void loop() {
  //Generates a Hardware ID every 1 second
  Serial.print("Hardware ID: ");  
  Serial.println(generateHID());
  delay(1000);
}
