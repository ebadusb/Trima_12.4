

# This function will take a hexadecimal "chunk" and convert it to a number: e.g.
# "\xde\x3b" -> long (de3b)

function convert_chunk_to_number(orig_chunk) {

   if ((substr(orig_chunk,1,2) != "\\x") || (substr(orig_chunk,5,2) != "\\x")) {
      printf("// ??? INVALID HEX \"CHUNK\" DURING CHUNK_TO_NUMBER CONVERSION: %s \n", orig_chunk);
      bad_literal_lines[bad_literal_line_index] = "!!! BAD HEX CHUNK - "  orig_chunk;
      bad_literal_line_number[bad_literal_line_index] = NR ;
      bad_literal_line_index++;
      return(0);
   }

   number = 0 ;
   hex = substr(orig_chunk, 3, 2) substr(orig_chunk, 7, 2) ;

   for (i = 1; i <= (length(hex)); i++) {

      number = number * 16 ;
      intnum = index ("0123456789abcdef", substr (hex, i, 1)) - 1 ;

      if (intnum == -1) {
         printf("// ??? INVALID HEX \"NUMBER\" DURING CHUNK_TO_NUMBER CONVERSION: %s \n", orig_chunk);
         bad_literal_lines[bad_literal_line_index] = "!!! BAD HEX NUMBER - "  orig_chunk;
         bad_literal_line_number[bad_literal_line_index] = NR ;
         bad_literal_line_index++;
         return(0);
      }

      number = number + intnum ;
   } # END:  for (i = 1; i < (length...


   return (number) ;

} # end function - convert_chunk_to_number (orig_chunk)






# This function will take a number and convert it to a hexadecimal "chunk".
# e.g. "\xef\xa5".
function convert_number_to_chunk(number) {
  rawchunk = "" ;

  for (i = 1; i <= 4; i++) {
    next_digit = (number % 16) + 1 ; # s.b. between 1 - 16
    number = int(number / 16) ; # shift number to the right by 1 hex digit.
    rawchunk = substr("0123456789abcdef", next_digit, 1) rawchunk ; # add digit to the least sig. pos.
  }

  chunk = "\\x" substr (rawchunk, 1, 2) "\\x" substr (rawchunk, 3, 2) ;
  return (chunk) ;

} # end function - convert_number_to_chunk (number)



# This function will take a hexadecimal "chunk" and determine whether it is
# a "full width" latin character (between ff01 and ff5e mapped with katakana).
# If it is, it will remap the UCS2 code into real Latin 1 space (0021 to 007e).

function convert_to_latin1(orig_chunk) {

   #### DEBUG - search for specific UCS2 ranges

      #### See what CJK "symbols" were used by ILE - possibly convert them

      #if (substr(orig_chunk,1,4) == "\\x30") {
      #   if (convert_chunk_to_number(orig_chunk) < 12352) {  # less than x'3040'
      #      bad_literal_lines[bad_literal_line_index] = "!!! CJK Symbol used - "  orig_chunk;
      #      bad_literal_line_number[bad_literal_line_index] = NR ;
      #      bad_literal_line_index++;
      #   }
      #}

   #### END_DEBUG


   if ((substr(orig_chunk,1,4) == "\\xff"))
   {

      # make a number from the lower hex digit only
      lowdigit = "\\x00" substr(orig_chunk, 5, 4) ;
      value = convert_chunk_to_number(lowdigit) ;

      if (value > 0 && value < 95) {  # 95 = x'5f'... I'm looking for a byte between 1 and 5e inclusive.
         # replace this chunk with Latin 1 value: (chunk & x'00ff') + x'20'
         latin_value = value + 32 ;
         #### DEBUG
         #### printf("\n orig = %s, conv = %s \n", orig_chunk, convert_number_to_chunk(latin_value));
         #### END_DEBUG
         return (convert_number_to_chunk(latin_value));
      }
      else
      {
         return (orig_chunk) ;
      }

   }
   else
   {
      return (orig_chunk) ;
   }

} # end function - convert_to_latin1(orig_chunk)






BEGIN {
        # Initialize the error message array and the segment size manifest.
        bad_literal_line_index = 0;
        MAX_SEG_SIZE = 40 ;
        SOURCE_HEX_CHUNK_SIZE = 8 ; ### 8 = \xhh\xhh, a UCS2 doublebyte
        LAST_ORIG_CHUNK = "========" ;
        LAST_TRANSLATED_CHUNK = "========" ;
        INTERJECT_CHARACTER = "" ;

      } #### End of BEGIN block






      ################################# MAINLINE BLOCK #########################################

      {

        #This MAINLINE reads in the Japanese Stringja from ILE scans the
        #   hexadecimal literal by UCS2 "chunk" representations, e.g. \xeb\x40 and
        #   performs the following functions:
        #
        # 1. breaks it into MAX_SEG_SIZE byte segments so that the Watcom Compiler
        #    can scan the tokens and intialize the string arrays.
        # 2. Converts any katakana fullwidth Latin UCS2 into Latin1 UCS2.
        # 3. Swaps the two bytes of UCS2 so that the QNX UCS2->UTF8 conversions can handle
        #    a byte array as if it was a "wchar" array. This requires Intel "endian" swapping.
        # 4. Converts any mathematically mapped minus signs to Latin1 minus sign (i.e. \x22\x12 -> \x00\x2d)
        # 5. Add a NULL byte onto the end of the array to provide a double byte NULL terminator.
        # 6. Convert any Wide Ideographic spaces to Latin1 spaces (i.e. \x3000 to \x0020).
        # 7. Add a Latin1 space after any combination of "#:", where "#" is a number from 0 to 9.



        #### DEBUG PRINT OF INPUT
        #### printf("!!! %s \n", $0);

        DCL_index = index($0,"DCL_TEXT") ;
        CHAR_index = index($0,"char ") ;

        # If the line is not a comment and it starts with a column-agile-combo of "char ... DCL_TEXT"
        #  then we want to process it as a string literal. Otherwise just copy it out to the output file.
        if ((index($1,"//") == 0) && DCL_index > 0 && CHAR_index > 0 && DCL_index > CHAR_index) {
          #{
           # Find the start of the literal
           litstart = index ($0, "\"") ;
           if (litstart == 0) {
              printf("// ??? DID NOT FIND LITERAL IN DECLARE STRING: %s \n", $0);
              bad_literal_lines[bad_literal_line_index] = "!!! NO LITERAL FOUND - " $0 ;
              bad_literal_line_number[bad_literal_line_index] = NR ;
              bad_literal_line_index++;
           } else
           {
              # Print out the beginning of the line; lose the first quote
              printf("%s", substr($0,1,litstart-1));

              # Build the Source Hex string - drop off the ending quote and semi-colon.
              source_hex = substr($0, litstart+1) ;
              shindex = index(source_hex,"\"");
              if (shindex == 0) {
             #{
                 printf("// ??? DID NOT FIND ENDING QUOTE FOR LITERAL IN DECLARE STRING: %s \n", $0);
                 bad_literal_lines[bad_literal_line_index] = "!!! NO ENDING QUOTE FOUND - " $0 ;
                 bad_literal_line_number[bad_literal_line_index] = NR ;
                 bad_literal_line_index++;
              } else
              {
                 # Trim off the trailing quote and semi-colon
                 source_hex = substr(source_hex, 1, shindex-1);

                 # Now chunk out the literal in regular string literals
                 source_size = length(source_hex) ;

                 # if the text literal is null, be sure we print it out in the segment clean up below
                 if (source_size == 0) {
                    null_source = 1;
                 } else
                 {
                    null_source = 0;
                 }

                 target_size = 0;
                 target_string = "";
                 target_segment = "" ;

                 # Do until the source is exhausted
                 for (source_hex_index = 1; source_hex_index <= source_size; source_hex_index = source_hex_index+SOURCE_HEX_CHUNK_SIZE) {
                #{

                     if (length(target_segment) >= MAX_SEG_SIZE) {
                        target_string = target_string " \"" target_segment "\"" ;
                        target_segment = "" ;
                     }
                     target_chunk =  substr(source_hex, source_hex_index, SOURCE_HEX_CHUNK_SIZE) ;

                     # Make sure that we've remained on a chunkette boundary
                     if (substr(target_chunk,1,2) != "\\x" || substr(target_chunk,5,2) != "\\x" || length(target_chunk) != SOURCE_HEX_CHUNK_SIZE) {
                    #{
                        printf("// ??? TARGET CHUNK NOT ON A \\x BOUNDARY: %s %s \n", target_chunk, $0) ;
                        bad_literal_lines[bad_literal_line_index] = "!!! TARGET CHUNK UNSYNCED - " $0 ;
                        bad_literal_line_number[bad_literal_line_index] = NR ;
                        bad_literal_line_index++;
                        # Leave the FOR loop
                        break ;
                     }

                     #save off copy of last original chunk
                     lchunk = target_chunk ;


                     # Convert any UCS2 fullwidth Latin to Latin1
                     target_chunk = convert_to_latin1(target_chunk) ;


                     ################# PER CHARACTER CONVERSIONS ##################
                     ##############################################################

                     # if the chunk is using a mathematical minus sign (which won't display)
                     # change it to a Latin1 minus sign.
                     if (target_chunk == "\\x22\\x12") {
                        target_chunk = "\\x00\\x2d" ;
                     }


                     # if the chunk is using a CJK wide ideographic space (which is too wide for Latin)
                     # change it to a Latin1 space.
                     if (target_chunk == "\\x30\\x00") {
                        target_chunk = "\\x00\\x20" ;
                     }

                     # if a full colon is preceded by a number (many of the prompt lists and headings e.g. 1: hang), inject a space afterward.
                     #
                     if (target_chunk == "\\x00\\x3a" && substr(LAST_TRANSLATED_CHUNK,7,2) == "00" && substr(LAST_TRANSLATED_CHUNK,3,2) >= "30" && substr(LAST_TRANSLATED_CHUNK,3,2) <= "39") {
                        INTERJECT_CHARACTER = "\\x20\\x00" ; 
                        # interjection chars must be pre-swizzled cause they're just concatenated to the segment.
                     }

                     ##############################################################


                     # Record the last original chunk and the last translated chunk
                     # for previous character comparisons above used above.

                     LAST_ORIG_CHUNK = lchunk ;
                     LAST_TRANSLATED_CHUNK = substr(target_chunk,5,4) substr(target_chunk,1,4) ;


                     # Swap the two bytes in the chunkette so that the QNX UCS2 to UTF8 conversion
                     # routine will work properly, then concatenate them to the segment buffer.
                     target_segment = target_segment substr(target_chunk,5,4) substr(target_chunk,1,4) INTERJECT_CHARACTER ;

                     #reset the interjection string
                     INTERJECT_CHARACTER = "" ;

 ;

                     #### STANDARD (non-swizzled) CHUNK CONCAT: target_segment = target_segment target_chunk ;

                 } # END OF: for (source_hex_index = 1; source_he...

                 # Add any partial segments to target buffer OR if the text was a NULL string (add closing quote);
                 #  then add on a superfluous NULL so that the UCS2 scanner recognizes end of string.
                 if (length(target_segment) > 0 || null_source == 1) {
                #{
                    target_string = target_string " \"" target_segment "\\x00\"" ;
                 }
                 else
                 {
                    #Add a stand-alone NULL byte for string termination
                    target_string = target_string " \"\\x00\"" ;
                 }

                 # Print out the segmented literal line plus the C++ line terminator.
                 printf("%s%s\n", target_string, " ;");

              }  # END OF ELSE: if (shindex == 0) {

            } # END OF ELSE: if ((index($0,"DCL_TEXT") >...

        } else
        {
          if (index($0, "BOLD,") > 0) {
             gsub("BOLD,", "NTS,");
          }

          # Just print out the non string literal lines
          printf("%s\n", $0);
        }

      } #### End of Main block


      ################################# END OF MAINLINE BLOCK #####################################



END   {
        # Print out any of the error messages
        printf("\n\n\n// *** ERROR MESSAGES ***\n\n");

        for (i = 0; i < bad_literal_line_index; i++ ) {
       #{
           printf("// %d  %s \n",bad_literal_line_number[i], bad_literal_lines[i]);
        }

      } #### End of END block


