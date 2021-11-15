// The text encryption program in C++ and ASM with a very simple example encryption method - it simply adds 1 to the character.
// The encryption method is written in ASM. You will replace this with your allocated version for the assignment.
// In this version parameters are passed via registers (see 'encrypt' for details).
//
// Original Author: A.Oram - 2012
// Last revised: A.Hamilton - Sep 204832758273793275790327599475437583475632858362895689600000091


#include <string>     // for std::string
#include <chrono>     // for date & time functions
#include <ctime>      // for date & time string functions
#include <iostream>   // for std::cout <<
#include <fstream>    // for file I/O
#include <iomanip>    // for fancy output
#include <functional> // for std::reference_wrapper
#include <vector>     // for std::vector container




constexpr char const * STUDENT_NAME = "Finn Moorhouse";      // Replace with your full name
constexpr int ENCRYPTION_ROUTINE_ID = 17;                    // Replace -1 with your encryption id
constexpr char ENCRYPTION_KEY = 'B';                         // Replace '?' with your encryption key
constexpr int MAX_CHARS = 6;                                 // feel free to alter this, but must be 6 when submitting!

constexpr char STRING_TERMINATOR = '$';                      // custom string terminator
constexpr char LINE_FEED_CHARACTER = '\n';                   
constexpr char CARRIAGE_RETURN_CHARACTER = '\r';             // carriage return character

char original_chars[MAX_CHARS];                              // Original character string
char encrypted_chars[MAX_CHARS];                             // Encrypted character string
char decrypted_chars[MAX_CHARS];                             // Decrypted character string


//---------------------------------------------------------------------------------------------------------------
//----------------- C++ FUNCTIONS -------------------------------------------------------------------------------

/// <summary>
/// get a single character from the user via Windows function _getwche
/// </summary>
/// <param name="a_character">the resultant character, pass by reference</param>
void get_char (char& a_character)
{

   a_character = (char)_getwche(); // https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/getche-getwche

   __asm {
	   OR BYTE PTR[ecx], 020h // ORS the ascii value stored in the address at ecx (which happens to be the referenced variable) to keep or add the 32 to make a number lower case
   }

   
   if (a_character == STRING_TERMINATOR) // skip further checks if user entered string terminating character
  {
    return;
  }
  if (a_character == LINE_FEED_CHARACTER || a_character == CARRIAGE_RETURN_CHARACTER)  // treat all 'new line' characters as terminating character
  {
    a_character = STRING_TERMINATOR;
    return;
  }

}

//---------------------------------------------------------------------------------------------------------------
//----------------- ENCRYPTION ROUTINE --------------------------------------------------------------------------

/// <summary>
/// 'encrypt' the characters in original_chars, up to length
/// output 'encrypted' characters in to encrypted_chars
/// </summary>
/// <param name="length">length of string to encrypt, pass by value</param>
/// <param name="EKey">encryption key to use during encryption, pass by value</param>
void encrypt_chars (int length, char EKey)
{
  char temp_char;                    // Temporary character store

  for (int i = 0; i < length; ++i)   // Encrypt characters one at a time
  {
    temp_char = original_chars [i];  // Get the next char from original_chars array
    
                                     
                                     
    __asm
    {


      push   eax                     // pushes eax, ecx & edx into the stack to recall later
      push   ecx                     // **
      push   edx                     // **

      lea    eax, EKey               // loads the address of EKEY which is B initially into the eax register
      movzx  ecx, temp_char          // moves the 4 bit value of temp char into ecx with zero extend so 32 bits
      call   encrypt_17              // subroutine encrypt 17 is called
      mov    temp_char, dl           // moves the ascii value into temp_char

      pop    edx                     // pops (restores) eax, acx & edx from the stack
      pop    ecx                     // **
      pop    eax                     // **
    
    
    }

    encrypted_chars [i] = temp_char; // Store encrypted char in the encrypted_chars array
  }

  return;

  // Inputs: register EAX = 32-bit address of Ekey
  //                  ECX = the character to be encrypted (in the low 8-bit field, CL) (it actually serves more as a counter for the loop)
  // Output: register EDX = the encrypted value of the source character (in the low 8-bit field, DL)

  __asm
  {
  encrypt_17:

	      push  ebp                             // push base pointer to return to later blah blah standard stuff
		  mov   ebp, esp                        // set new base pointer 
	     
          push  esi                             //push values onto stack
          push  ecx                             // **

          mov   esi, eax                        // routine to mess up the EKEY and create a mangled one in ecx (this is always the same no matter what the chars entered 1st pass: 91, 2nd: 65, 3rd: 5A, 5th: e6, 6th: BA)
          and   dword ptr[esi], 0xFF            // 
          ror   byte ptr[esi], 1
          ror   byte ptr[esi], 1
          add   byte ptr[esi], 0x01
          mov   ecx, [esi]                      
          pop   edx                             // puts the ascii of the char to be encrypted in edx

          x17 : ror   dl, 1                     // rotates dl by 1 until the mangled EKEY = 0 
          dec   ecx                             //do{ ror dl,1
          jnz   x17                             //   --ecx
												//} while(ecx > 0)
          mov   eax, edx
          add   eax, 0x20                       // whats the point of moving into eax
          xor   eax, 0xAA
          mov   edx, eax                        // eax then gets put into edx after its 'mangled'

		  pop   ecx                             // everything gets restored
          pop   esi

		  mov   esp,ebp
		  pop   ebp
		  

          ret
  }
}
//*** end of encrypt_chars function
//---------------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------------
//----------------- DECRYPTION ROUTINE --------------------------------------------------------------------------

/// <summary>
/// 'decrypt' the characters in encrypted_chars, up to length
/// output 'decrypted' characters in to decrypted_chars
/// </summary>
/// <param name="length">length of string to decrypt, pass by value</param>
/// <param name="EKey">encryption key to used during the encryption process, pass by value</param>
void decrypt_chars (int length, char EKey)
{

	char temp_char;       // Temporary character store


	for (int i = 0; i < length; ++i)   // Encrypt characters one at a time
	{
		temp_char = encrypted_chars[i];  // Get the next char from encrypted_chars array
 									 
		__asm
		{


			push   eax                     // pushes eax, ecx & edx into the stack to recall later
			push   ecx                     // **
			push   edx                     // **

			lea    eax, EKey               // loads the address of EKEY which is B initially into the eax register
			movzx  ecx, temp_char          // moves the 4 bit value of temp char into ecx with zero extend so 32 bits
			call   decrypt_17              // subroutine decrypt 17 is called
			mov    temp_char, dl           // moves the ascii value into temp_char

			pop    edx                     // pops (restores) eax, ecx & edx from the stack
			pop    ecx                     // **
			pop    eax                     // **



		}

		decrypted_chars[i] = temp_char; // Store encrypted char in the encrypted_chars array
	}

	return;


	// --------------------------------------------------------------------------------------- //
	// -------------------------------DECRYPTION ALGORITHM------------------------------------ //
	// PRACTICALLY ALL THE ENCRYPTION DOES IS TAKE THE INITAL HEX VALUE OF THE ASCII LETTER 'B'//
	// MANGLES IT WITH THE SAME OPERATION AND CONTINUES TO MANGLE THAT ON EACH PASS THE SAME   //
	// EXACT WAY. THE EKEY IS ALSO BASICALLY JUST A COUNTER... SO ALL WE REALLY NEED TO DO TO  //
	// 'DECRYPT' IS IMPLEMENT THE SAME COUNTER BUT REVERSE THE LOOP AND THE FINAL ADD AND XOR  //
	// --------------------------------------------------------------------------------------- //
	// --------------------------------------------------------------------------------------- //


	__asm
	{
	decrypt_17:

			push  ebp                             // push base pointer to return to later blah blah standard stuff
			mov   ebp, esp                        // set new base pointer 

			push  esi                             //push values onto stack
			push  ecx                             // **

			mov   edx, ecx                        // puts the encrypted char into edx

			mov   esi, eax                        // routine to mess up the EKEY and create a mangled one in ecx (i reused this as its needed for the inverse of the 'while loop' and the counter stays the same)
			and   dword ptr[esi], 0xFF            // 
			ror   byte ptr[esi], 1
			ror   byte ptr[esi], 1
			add   byte ptr[esi], 0x01
			mov   ecx, [esi]                      // put the mangled ekey into ecx

			xor   edx, 0xAA                       // undo the last two 'encryption' operations before the loop
			sub   edx, 0x20

			d17 : rol dl, 1                       // inverse of the loop to encrypt, gets the final value (using the same counter)
			dec   ecx                             // do { rol dl, 1
			jnz   d17                             //   --ecx
			                                      // } while(ecx > 0);
			pop   ecx                             // everything gets restored
			pop   esi

			mov   esp, ebp
			pop   ebp

			ret

	}

}
//*** end of decrypt_chars function
//---------------------------------------------------------------------------------------------------------------


//************ MAIN *********************************************************************************************
//************ YOU DO NOT NEED TO EDIT ANYTHING BELOW THIS LINE *************************************************
//************ BUT FEEL FREE TO HAVE A LOOK *********************************************************************

void get_original_chars (int& length)
{
  length = 0;

  char next_char;
  do
  {
    next_char = 0;
    get_char (next_char);

    if (next_char != STRING_TERMINATOR)
    {
      original_chars [length++] = next_char;
    }
  }
  while ((length < MAX_CHARS) && (next_char != STRING_TERMINATOR));
}

std::string get_date ()
{
  std::time_t now = std::chrono::system_clock::to_time_t (std::chrono::system_clock::now ());
  char buf[16] = { 0 };
  tm time_data;
  localtime_s (&time_data, &now);
  std::strftime (buf, sizeof(buf), "%d/%m/%Y", &time_data);
  return std::string{ buf };
}

std::string get_time ()
{
  std::time_t now = std::chrono::system_clock::to_time_t (std::chrono::system_clock::now ());
  char buf[16] = { 0 };
  tm time_data;
  localtime_s (&time_data, &now);
  std::strftime (buf, sizeof (buf), "%H:%M:%S", &time_data);
  return std::string{ buf };
}

// support class to help output to multiple streams at the same time
struct multi_outstream
{
  void add_stream (std::ostream& stream)
  {
    streams.push_back (stream);
  }

  template <class T>
  multi_outstream& operator<<(const T& data)
  {
    for (auto& stream : streams)
    {
      stream.get () << data;
    }
    return *this;
  }

private:
  std::vector <std::reference_wrapper <std::ostream>> streams;
};

int main ()
{
  int char_count = 0;  // The number of actual characters entered (upto MAX_CHARS limit)

  std::cout << "Please enter upto " << MAX_CHARS << " alphabetic characters: ";
  get_original_chars (char_count);	// Input the character string to be encrypted


  //*****************************************************
  // Open a file to store results (you can view and edit this file in Visual Studio)

  std::ofstream file_stream;
  file_stream.open ("log.txt", std::ios::app);
  file_stream << "Date: " << get_date () << " Time: " << get_time () << "\n";
  file_stream << "Name:                  " << STUDENT_NAME << "\n";
  file_stream << "Encryption Routine ID: '" << ENCRYPTION_ROUTINE_ID << "'" << "\n";
  file_stream << "Encryption Key:        '" << ENCRYPTION_KEY;

  multi_outstream output;
  output.add_stream (file_stream);
  output.add_stream (std::cout);


  //*****************************************************
  // Display and save to the log file the string just input

  output << "\n\nOriginal string  = ";
  output << std::right << std::setw (MAX_CHARS) << std::setfill (' ') << original_chars;

  // output each original char's hex value
  output << " Hex = ";
  for (int i = 0; i < char_count; ++i)
  {
    int const original_char = static_cast <int> (original_chars [i]) & 0xFF; // ANDing with 0xFF prevents static_cast padding 8 bit value with 1s
    output << std::hex << std::right << std::setw (2) << std::setfill ('0') << original_char << " ";
  }


  //*****************************************************
  // Encrypt the string and display/save the result

  encrypt_chars (char_count, ENCRYPTION_KEY);

  output << "\n\nEncrypted string = ";
  output << std::right << std::setw (MAX_CHARS) << std::setfill (' ') << encrypted_chars;

  // output each encrypted char's hex value
  output << " Hex = ";
  for (int i = 0; i < char_count; ++i)
  {
    int const encrypted_char = static_cast <int> (encrypted_chars [i]) & 0xFF; // ANDing with 0xFF prevents static_cast padding 8 bit value with 1s
    output << std::hex << std::right << std::setw (2) << std::setfill ('0') << encrypted_char << " ";
  }


  //*****************************************************
  // Decrypt the encrypted string and display/save the result

  decrypt_chars (char_count, ENCRYPTION_KEY);

  output << "\n\nDecrypted string = ";
  output << std::right << std::setw (MAX_CHARS) << std::setfill (' ') << decrypted_chars;

  // output each decrypted char's hex value
  output << " Hex = ";
  for (int i = 0; i < char_count; ++i)
  {
    int const decrypted_char = static_cast <int> (decrypted_chars [i]) & 0xFF; // ANDing with 0xFF prevents static_cast padding 8 bit value with 1s
    output << std::hex << std::right << std::setw (2) << std::setfill ('0') << decrypted_char << " ";
  }


  //*****************************************************
  // End program

  output << "\n\n";
  file_stream << "-------------------------------------------------------------\n\n";
  file_stream.close ();

  system ("PAUSE"); // do not use in your other programs! just a hack to pause the program before exiting

  return 0;
}

//**************************************************************************************************************
